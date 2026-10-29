//====================================================================================
//                                  NTP_Time.h
//  NTP time sync; the timezone offset (seconds east of UTC) comes from timeapi.io by lat/long
//====================================================================================
#pragma once

// Time library: https://github.com/PaulStoffregen/Time
#include <TimeLib.h>

// Built-in ESP32 libraries
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>

//====================================================================================
//                              NTP Configuration
//====================================================================================

const char* ntpServerName = "pool.ntp.org";
IPAddress   timeServerIP;

//====================================================================================
//                              Variables
//====================================================================================

time_t   utc            = 0;
bool     timeValid      = false;

unsigned int localPort  = 2390;

const int    NTP_PACKET_SIZE = 48;
byte         packetBuffer[NTP_PACKET_SIZE];

WiFiUDP udp;

uint8_t  lastMinute     = 0;
uint8_t  lastHour       = 0;  // still used for the once-an-hour 2am DST check only
uint8_t  lastSecond     = 0;

uint32_t nextSendTime   = 0;
uint32_t newRecvTime    = 0;
uint32_t lastRecvTime   = 0;

//====================================================================================
//                  NTP re-sync timer (millis() from boot)
//  Offset by half a weather interval, so it always lands midway between weather fetches
//====================================================================================
const uint32_t NTP_SYNC_INTERVAL_MS = 4UL * 3600UL * 1000UL;               // 4 hours
const uint32_t NTP_SYNC_OFFSET_MS   = (UPDATE_INTERVAL_SECS * 1000UL) / 2; // half a weather interval
uint32_t nextNTPSync = 0;  // armed once in setup(), after the initial boot-time syncTime()
uint32_t no_packet_count = 0;

bool     rebooted       = true;

//====================================================================================
//                         Local time helper: toLocal(utc) adds tzOffset
//====================================================================================
extern int32_t tzOffset;
extern int32_t tzOffset2;
extern bool    altLocation;

inline time_t toLocal(time_t utcTime) {
  return utcTime + (altLocation ? tzOffset2 : tzOffset);
}

//====================================================================================
//                         Timezone offset fetch from timeapi.io
//  Returns the current offset unchanged if the fetch fails
//====================================================================================
int32_t fetchTZOffset(const String& lat, const String& lon, int32_t currentOffset) {
  if (WiFi.status() != WL_CONNECTED) return currentOffset;

  HTTPClient http;
  String url = "https://timeapi.io/api/timezone/coordinate?latitude="
               + lat + "&longitude=" + lon;

  http.setConnectTimeout(8000);  // bounds TCP connect + TLS handshake — setTimeout() alone does NOT cover this
  http.begin(url);
  http.setTimeout(12000);  // bounds the read phase — don't block the display loop indefinitely

  http.addHeader("Connection", "close");  // no keep-alive socket left lingering

  Serial.println("Sending GET request to timeapi.io...");
  int code = http.GET();

  if (code != 200) {
    Serial.println("TZ fetch failed, HTTP " + String(code));
    http.end();
    return currentOffset;  // keep whatever we had
  }

  String payload = http.getString();
  http.end();

  // Parse  "currentUtcOffset":{"seconds": -25200
  int idx = payload.indexOf("\"currentUtcOffset\":{\"seconds\":");
  if (idx < 0) {
    Serial.println("TZ parse failed — key not found");
    return currentOffset;
  }

  int start = payload.indexOf(':', idx + 29) + 1;  // skip past the second ':'
  int end   = payload.indexOf(',', start);
  if (start < 0 || end < 0) {
    Serial.println("TZ parse failed — bad format");
    return currentOffset;
  }

  int32_t offset = payload.substring(start, end).toInt();
  Serial.printf("TZ offset fetched: %d seconds (%.1f hrs)\n",
                offset, (float)offset / 3600.0f);
  return offset;
}

//====================================================================================
//                         Send NTP packet
//====================================================================================
void sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0]  = 0b11100011;  // LI, Version, Mode
  packetBuffer[1]  = 0;           // Stratum
  packetBuffer[2]  = 6;           // Polling Interval
  packetBuffer[3]  = 0xEC;        // Peer Clock Precision
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  udp.beginPacket(address, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

//====================================================================================
//                         Decode NTP reply
//====================================================================================
void decodeNTP() {
  timeValid = false;
  uint32_t waitTime = millis() + 500;

  while (millis() < waitTime && !timeValid) {
    yield();
    if (udp.parsePacket()) {
      newRecvTime = millis();
      udp.read(packetBuffer, NTP_PACKET_SIZE);
      lastRecvTime = newRecvTime;

      unsigned long highWord    = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord     = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;

      utc = secsSince1900 - 2208988800UL;
      setTime(utc);
      timeValid = true;

      Serial.printf("NTP UTC: %02d:%02d:%02d\n", hour(utc), minute(utc), second(utc));
    }
  }

  if (timeValid) {
    no_packet_count = 0;
  } else {
    Serial.println("No NTP reply, trying again in 1 minute...");
    no_packet_count++;
  }

  if (no_packet_count >= 10) {
    no_packet_count = 0;
    Serial.println("No NTP packet in last 10 minutes");
  }
}

//====================================================================================
//                         Sync time from NTP pool
//====================================================================================
void syncTime() {
  if (nextSendTime < millis()) {
    Serial.println("Sending NTP request to pool.ntp.org...");
    WiFi.hostByName(ntpServerName, timeServerIP);
    nextSendTime = millis() + 5000;
    sendNTPpacket(timeServerIP);
    decodeNTP();
  }
}

//====================================================================================
//                         Debug print (SERIAL_MESSAGES only)
//====================================================================================
void printTime(time_t t) {
#ifdef SERIAL_MESSAGES
  Serial.printf("%02d:%02d:%02d  %d/%d/%d  %s\n",
    hour(t), minute(t), second(t),
    day(t), month(t), year(t),
    dayStr(weekday(t)));
#endif
}

//====================================================================================