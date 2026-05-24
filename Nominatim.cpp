// Reverse geocoding using Nominatim (OpenStreetMap)
// Replaces BigDataCloud with a completely free, keyless, worldwide service.
// Usage policy: https://operations.osmfoundation.org/policies/nominatim/
// - No bulk requests (we only call once at boot — fine)
// - Requires a descriptive User-Agent header (set below)
//
// Streaming JSON parser:
//   https://github.com/Bodmer/JSON_Decoder
#pragma once

#include <WiFiClientSecure.h>  // need an HTTPS connection
#include <JSON_Listener.h>
#include <JSON_Decoder.h>

#include "Nominatim.h"

static const char *NOM_HOST = "nominatim.openstreetmap.org";
static const int NOM_PORT = 443;

// Identify your app in the User-Agent as required by Nominatim's usage policy
static const char *NOM_USER_AGENT = "ESP32-WeatherDisplay/1.0";

/***************************************************************************************
** Function name:           getBDCLocation
** Description:             Request reverse geocode from Nominatim.
**                          Signature unchanged so sketch call needs no modification.
**                          'language' parameter accepted but unused (Nominatim returns
**                          English names by default for most locations worldwide).
***************************************************************************************/
bool BDC_location::getBDCLocation(BDC_current *BDCcurrent, String latitude, String longitude, String language) {

  BDCdata_set = "";
  this->BDCcurrent = BDCcurrent;

  // Nominatim reverse geocode endpoint
  // zoom=14 returns neighbourhood/suburb level detail
  // addressdetails=1 returns the structured address object we need
  // String url = "https://" + String(NOM_HOST)
  //              + "/reverse?lat=" + latitude
  //              + "&lon=" + longitude
  //              + "&format=json"
  //              + "&addressdetails=1"
  //              + "&zoom=14"
  //              + "&accept-language=en";
String url = "https://" + String(NOM_HOST)
           + "/reverse?lat=" + latitude
           + "&lon=" + longitude
           + "&format=json"
           + "&addressdetails=1"
           + "&zoom=12"          // city level instead of neighbourhood
           + "&accept-language=en";

  bool result = parseBDCRequest(url);

  this->BDCcurrent = nullptr;

  return result;
}

/***************************************************************************************
** Function name:           parseBDCRequest
** Description:             Fetches the Nominatim JSON response and feeds to the parser.
**                          Uses plain HTTP/1.0 to avoid chunked transfer encoding.
***************************************************************************************/
bool BDC_location::parseBDCRequest(String url) {

  uint32_t dt = millis();
//Serial.println("Nominatim URL: " + url);

  // and in parseBDCRequest():
  WiFiClientSecure client;
  client.setInsecure();  // skip cert validation

  JSON_Decoder parser;
  parser.setListener(this);

  if (!client.connect(NOM_HOST, NOM_PORT)) {
    Serial.println("Nominatim connection failed.");
    return false;
  }
  Serial.println("Nominatim connected OK");

  parseOK = false;

  uint32_t timeout = millis();
  char c = 0;

#ifdef SHOW_BDCJSON
  int ccount = 0;
#endif

  // HTTP/1.0 avoids chunked transfer encoding which the streaming parser can't handle
  // Nominatim requires a User-Agent header identifying the application
  client.print(String("GET ") + url + " HTTP/1.0\r\n"
               + "Host: " + NOM_HOST + "\r\n"
               + "User-Agent: " + NOM_USER_AGENT + "\r\n"
               + "Connection: close\r\n\r\n");

  // Skip HTTP response headers
  //   while (client.connected()) {
  //     String line = client.readStringUntil('\n');
  //     if (line == "\r") break;
  // #ifdef SHOW_BDCJSON
  //     Serial.println(line);
  // #endif
  //     if ((millis() - timeout) > 5000UL) {
  //       Serial.println("Nominatim HTTP header timeout");
  //       client.stop();
  //       return false;
  //     }
  //   }

  // while (client.connected()) {
  //   String line = client.readStringUntil('\n');
  // Serial.println("Header line: " + line);  // temporary
  // if (line == "\r") {
  // Serial.println("Header end found");
  //     break;
  //   }
  // }

  // Parse the JSON body
  while (client.available() > 0 || client.connected()) {
    while (client.available() > 0) {
      c = client.read();
      parser.parse(c);
#ifdef SHOW_BDCJSON
      if (c == '{' || c == '[' || c == '}' || c == ']') Serial.println();
      if (ccount++ > 100 && c == ',') {
        ccount = 0;
        Serial.println();
      }
#endif
      if ((millis() - timeout) > 8000UL) {
        Serial.println("Nominatim JSON parse timeout");
        parser.reset();
        client.stop();
        return false;
      }
      yield();
    }
  }

#ifdef SHOW_BDCJSON
  Serial.println("");
  Serial.print("Nominatim done in ");
  Serial.print(millis() - dt);
  Serial.println(" ms");
#endif

  parser.reset();
  client.stop();

  return parseOK;
}

// ===========================================================================
// Streaming parser callbacks
// ===========================================================================

void BDC_location::key(const char *key) {
  BDCcurrentKey = key;
#ifdef SHOW_BDCCALLBACK
  Serial.println("\n>>> Key >>> " + (String)key);
#endif
}

void BDC_location::startDocument() {
  BDCcurrentParent = BDCcurrentKey = BDCcurrentSet = "";
  BDCobjectLevel = BDCarrayLevel = BDCarrayIndex = 0;
  BDCvaluePath = "";
  parseOK = true;
#ifdef SHOW_BDCCALLBACK
  Serial.println("\n>>> Nominatim Start document >>>");
#endif
}

void BDC_location::endDocument() {
  BDCcurrentParent = BDCcurrentKey = "";
  BDCobjectLevel = 0;
  BDCvaluePath = "";
#ifdef SHOW_BDCCALLBACK
  Serial.println("\n<<< Nominatim End document <<<");
#endif
}

void BDC_location::startObject() {
  // Track which sub-object we're in — "address" is the key one
  if (BDCobjectLevel >= 1) BDCcurrentParent = BDCcurrentKey;
  BDCcurrentSet = BDCcurrentKey;
  BDCobjectLevel++;
#ifdef SHOW_BDCCALLBACK
  Serial.println("\n>>> Start object level:" + (String)BDCobjectLevel + " parent:" + BDCcurrentParent);
#endif
}

void BDC_location::endObject() {
  BDCobjectLevel--;
  if (BDCobjectLevel <= 1) BDCcurrentParent = "";
#ifdef SHOW_BDCCALLBACK
  Serial.println("\n<<< End object <<<");
#endif
}

void BDC_location::startArray() {
  BDCarrayLevel++;
}

void BDC_location::endArray() {
  if (BDCarrayLevel > 0) BDCarrayLevel--;
}

void BDC_location::whitespace(char c) {}

void BDC_location::error(const char *message) {
  Serial.print("\nNominatim parse error: ");
  Serial.println(message);
  parseOK = false;
}

void BDC_location::value(const char *val) {
  fullDataSet(val);
}

/***************************************************************************************
** Function name:           fullDataSet
** Description:             Map Nominatim JSON fields to BDC_current struct.
**
** Nominatim response structure (abbreviated):
** {
**   "lat": "49.1660",
**   "lon": "-122.7768",
**   "display_name": "Surrey, Metro Vancouver, British Columbia, Canada",
**   "address": {
**     "suburb":       "Whalley",          → locality
**     "city":         "Surrey",           → city
**     "county":       "Metro Vancouver",  → (not stored)
**     "state":        "British Columbia", → principalSubdivision
**     "country":      "Canada",           → countryName
**     "country_code": "ca",               → countryCode
**     "postcode":     "V3T",              → postcode
**   }
** }
**
** Note: not all fields appear for every location. For towns without "city",
** Nominatim may use "town" or "village" instead — all are checked below.
***************************************************************************************/
void BDC_location::fullDataSet(const char *val) {
  String value = val;

// Serial.print("parent: "); Serial.print(BDCcurrentParent);
// Serial.print(" key: ");   Serial.print(BDCcurrentKey);
// Serial.print(" value: "); Serial.println(value);

  if (BDCcurrentParent == "" || BDCcurrentParent == "reverse") {
    if      (BDCcurrentKey == "lat")  BDCcurrent->latitude  = value.toFloat();
    else if (BDCcurrentKey == "lon")  BDCcurrent->longitude = value.toFloat();
    else if (BDCcurrentKey == "name" && !value.isEmpty()) BDCcurrent->locality = value;
  }

  if (BDCcurrentParent == "address") {
    if      (BDCcurrentKey == "city"    && BDCcurrent->city.isEmpty()) BDCcurrent->city = value;
    else if (BDCcurrentKey == "town"    && BDCcurrent->city.isEmpty()) BDCcurrent->city = value;
    else if (BDCcurrentKey == "village" && BDCcurrent->city.isEmpty()) BDCcurrent->city = value;
    else if (BDCcurrentKey == "hamlet"  && BDCcurrent->city.isEmpty()) BDCcurrent->city = value;
  }
}
