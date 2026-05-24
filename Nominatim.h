// Reverse geocoding using Nominatim (OpenStreetMap)
// Worldwide, free, no API key required.
//
// Usage policy: https://operations.osmfoundation.org/policies/nominatim/

// Debug settings for Nominatim reverse geocoding
//#define SHOW_BDCJSON      // Debug — response header and JSON
//#define SHOW_BDCCALLBACK  // Debug — decode tree

#pragma once
// Location data structure for Nominatim reverse geocoding
// (OpenStreetMap) - worldwide, free, no API key required
// https://nominatim.openstreetmap.org

// typedef struct BDC_current {
//   float  latitude  = 0;
//   float  longitude = 0;
//   String city;                     // Surrey
//   String locality;                 // Whalley (suburb/neighbourhood)
//   String principalSubdivision;     // British Columbia
//   String principalSubdivisionCode; // not returned by Nominatim — kept for compatibility
//   String countryName;              // Canada
//   String countryCode;              // CA
//   String postcode;                 // V3T
//   // Fields below not returned by Nominatim — kept for sketch compatibility
//   String localityLanguageRequested;
//   String continent;
//   String continentCode;
// } BDC_current;

typedef struct BDC_current {
  float  latitude  = 0;
  float  longitude = 0;
  String city;      // city / town / village / hamlet from address object
  String locality;  // top-level "name" field — last-resort fallback for rural areas
} BDC_current;

/***************************************************************************************
** Description:   JSON interface class for Nominatim reverse geocoding
***************************************************************************************/
class BDC_location : public JsonListener {

public:
  bool getBDCLocation(BDC_current *BDCcurrent, String latitude, String longitude, String language);
  bool parseBDCRequest(String url);

private:
  void startDocument();
  void endDocument();
  void startObject();
  void endObject();
  void startArray();
  void endArray();
  void key(const char *key);
  void value(const char *value);
  void whitespace(char c);
  void error(const char *message);
  void fullDataSet(const char *value);

private:
  BDC_current *BDCcurrent;
  String BDCvaluePath;
  String BDCdata_set;
  bool parseOK;
  String BDCcurrentParent;
  uint16_t BDCobjectLevel;
  String BDCcurrentKey;
  String BDCcurrentSet;
  String BDCarrayPath;
  uint16_t BDCarrayIndex;
  uint16_t BDCarrayLevel;
};
