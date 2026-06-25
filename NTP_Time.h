//====================================================================================
//                                  NTP_Time.h
//  WabbitWeather — NTP time sync, no timezone library needed.
//  Timezone offset is stored as a plain int32_t (seconds east of UTC) and
//  fetched automatically from timeapi.io based on lat/long.
//  Replaces the JChristensen Timezone library approach entirely.
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
uint8_t  lastHour       = 0;
uint8_t  lastSecond     = 0;

uint32_t nextSendTime   = 0;
uint32_t newRecvTime    = 0;
uint32_t lastRecvTime   = 0;
uint32_t no_packet_count = 0;

bool     rebooted       = true;

//====================================================================================
//                         Local time helper
//
//  Replaces (*tz).toLocal(t, &tcr) everywhere in the sketch.
//  tzOffset is declared in All_Settings.h as int32_t.
//  Usage:  time_t local = toLocal(now());
//          time_t local = toLocal(current->dt);
//====================================================================================
extern int32_t tzOffset;
extern int32_t tzOffset2;
extern bool    altLocation;

inline time_t toLocal(time_t utcTime) {
  return utcTime + (altLocation ? tzOffset2 : tzOffset);
}

//====================================================================================
//                         Timezone offset fetch from timeapi.io
//
//  Queries https://timeapi.io/api/timezone/coordinate?latitude=xx&longitude=yy
//  Parses "currentUtcOffset":{"seconds": NNN } and returns NNN.
//  Returns the existing offset unchanged on any failure so the clock
//  keeps running even if the network call fails.
//====================================================================================
int32_t fetchTZOffset(const String& lat, const String& lon, int32_t currentOffset) {
  if (WiFi.status() != WL_CONNECTED) return currentOffset;

  HTTPClient http;
  String url = "https://timeapi.io/api/timezone/coordinate?latitude="
               + lat + "&longitude=" + lon;

  Serial.println("TZ fetch: " + url);
  http.begin(url);
  http.setTimeout(5000);   // 5 second timeout — don't block the display loop long
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
//  Call as:  syncTime();   (no tzIndex argument needed anymore)
//====================================================================================
void syncTime() {
  if (nextSendTime < millis()) {
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