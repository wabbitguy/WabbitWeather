// City name lookup using Nominatim (OpenStreetMap), free and keyless
// Usage policy (no bulk requests, real User-Agent): https://operations.osmfoundation.org/policies/nominatim/

#include <WiFiClientSecure.h>  // need an HTTPS connection
#include <HTTPClient.h>
#include <JSON_Listener.h>
#include <JSON_Decoder.h>

#include "Nominatim.h"

static const char *NOM_HOST = "nominatim.openstreetmap.org";

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

  // addressdetails=1 = structured address
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
**  Switched from a hand-rolled WiFiClientSecure parser to HTTPClient — the old version
**  had the same busy-wait bug the original Open-Meteo fetch had: the timeout check and
**  yield() only lived inside the inner "while available()" loop, so a connection that
**  stayed open but idle (no more data, no clean close) could spin loop() forever with
**  no yield and no way out. HTTPClient::getString() removes that whole class of bug
**  and correctly handles chunked transfer-encoding if Nominatim ever sends it.
***************************************************************************************/
bool BDC_location::parseBDCRequest(String url) {

  uint32_t dt = millis();

  WiFiClientSecure client;
  client.setInsecure();  // skip cert validation

  HTTPClient http;
  http.setConnectTimeout(8000);  // bounds TCP connect + TLS handshake
  http.setTimeout(12000);        // bounds the read phase

  parseOK = false;

  if (!http.begin(client, url)) {
    Serial.println("Nominatim: HTTPClient begin() failed");
    return false;
  }

  // Nominatim requires a User-Agent header identifying the application
  http.addHeader("User-Agent", NOM_USER_AGENT);

  http.addHeader("Connection", "close");  // no keep-alive socket left lingering

  Serial.println("Sending GET request to nominatim.openstreetmap.org...");
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("Nominatim HTTP error: %d\n", httpCode);
    http.end();
    return false;
  }

  String payload = http.getString();  // HTTPClient dechunks correctly here
  http.end();

  if (payload.length() == 0) {
    Serial.println("Nominatim: empty response body");
    return false;
  }

  JSON_Decoder parser;
  parser.setListener(this);

#ifdef SHOW_BDCJSON
  int ccount = 0;
#endif

  size_t len = payload.length();
  for (size_t i = 0; i < len; i++) {
    char c = payload[i];
    parser.parse(c);
#ifdef SHOW_BDCJSON
    if (c == '{' || c == '[' || c == '}' || c == ']') Serial.println();
    if (ccount++ > 100 && c == ',') {
      ccount = 0;
      Serial.println();
    }
#endif
    if ((i & 0x3F) == 0) yield();  // give the scheduler a turn periodically during the parse
  }

  parser.reset();

  Serial.print("Nominatim done in ");
  Serial.print(millis() - dt);
  Serial.println(" ms");

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
**   "lat": "50.1660",
**   "lon": "-132.7768",
**   "display_name": "Surrey, Metro Vancouver, British Columbia, Canada",
**   "address": {
**     "suburb":       "Whalley",          → locality
**     "city":         "Surrey",           → city
**     "county":       "Metro Vancouver",  → (not stored)
**     "state":        "British Columbia", → principalSubdivision
**     "country":      "Canada",           → countryName
**     "country_code": "ca",               → countryCode
**     "postcode":     "V3Y",              → postcode
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
