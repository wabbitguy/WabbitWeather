#pragma once


#define MAX_ICON_INDEX 11 // Maximum for weather icon index
#define ICON_RAIN 1       // Index for the rain icon bitmap (bmp file)
#define NO_VALUE 11       // for precipType default (none)

//#include "OpenMeteo_Setup.h"
// #include "Data_Point_Set.h"
#define MAX_HOURS 8    // Maximum hourly forecast periods (1-30)
#define MAX_DAYS  5    // Maximum daily forecast periods (1-8), must be >= 5

//#define SHOW_HEADER   // Debug — response header via serial
//#define SHOW_JSON     // Debug — JSON message formatting
//#define SHOW_CALLBACK // Debug — decode tree

// Bounds checking
#if (MAX_HOURS > 30) || (MAX_HOURS < 1)
  #undef  MAX_HOURS
  #define MAX_HOURS 30
#endif

#if (MAX_DAYS > 8) || (MAX_DAYS < 1)
  #undef  MAX_DAYS
  #define MAX_DAYS 8
#endif
/***************************************************************************************
** Description:   Structure for current weather
***************************************************************************************/
typedef struct OW_current {

  // current
  uint32_t dt = 0;
  uint32_t sunrise = 0;
  uint32_t sunset = 0;
  float    temp = 0;
  float    feels_like = 0;
  float    pressure = 0;
  uint8_t  humidity = 0;
  float    dew_point = 0;
  uint8_t  clouds = 0;
  float    uvi = 0;
  uint32_t visibility = 0;
  float    wind_speed = 0;
  float    wind_gust = 0;
  uint16_t wind_deg = 0;
  float    rain = 0;
  float    snow = 0;

  // current.weather
  uint16_t id = 0;
  String   main;
  String   description;
  String   icon;

} OW_current;

/***************************************************************************************
** Description:   Structure for hourly weather
***************************************************************************************/
typedef struct OW_hourly {

  // hourly
  uint32_t dt[MAX_HOURS] = { 0 };
  float    temp[MAX_HOURS] = { 0 };
  float    feels_like[MAX_HOURS] = { 0 };
  float    pressure[MAX_HOURS] = { 0 };
  uint8_t  humidity[MAX_HOURS] = { 0 };
  float    dew_point[MAX_HOURS] = { 0 };
  uint8_t  clouds[MAX_HOURS] = { 0 };
  float    wind_speed[MAX_HOURS] = { 0 };
  float    wind_gust[MAX_HOURS] = { 0 };
  uint16_t wind_deg[MAX_HOURS] = { 0 };
  float    rain[MAX_HOURS] = { 0 };
  float    snow[MAX_HOURS] = { 0 };

  // hourly.weather
  uint16_t id[MAX_HOURS] = { 0 };
  String   main[MAX_HOURS];
  String   description[MAX_HOURS];
  String   icon[MAX_HOURS];
  float    pop[MAX_HOURS];
} OW_hourly;

/***************************************************************************************
** Description:   Structure for daily weather
***************************************************************************************/
typedef struct OW_daily {

  // daily
  uint32_t dt[MAX_DAYS] = { 0 };  // dt
  uint32_t sunrise[MAX_DAYS] = { 0 };
  uint32_t sunset[MAX_DAYS] = { 0 };
  
  // daily.temp
  float    temp_morn[MAX_DAYS] = { 0 };
  float    temp_day[MAX_DAYS] = { 0 };
  float    temp_eve[MAX_DAYS] = { 0 };
  float    temp_night[MAX_DAYS] = { 0 };
  float    temp_min[MAX_DAYS] = { 0 };
  float    temp_max[MAX_DAYS] = { 0 };

  // daily.feels_like
  float    feels_like_morn[MAX_DAYS] = { 0 };
  float    feels_like_day[MAX_DAYS] = { 0 };
  float    feels_like_eve[MAX_DAYS] = { 0 };
  float    feels_like_night[MAX_DAYS] = { 0 };

  // daily
  float    pressure[MAX_DAYS] = { 0 };
  uint8_t  humidity[MAX_DAYS] = { 0 };
  float    dew_point[MAX_DAYS] = { 0 };
  float    wind_speed[MAX_DAYS] = { 0 };
  float    wind_gust[MAX_DAYS] = { 0 };
  uint16_t wind_deg[MAX_DAYS] = { 0 };
  uint8_t  clouds[MAX_DAYS] = { 0 };
  float    uvi[MAX_DAYS] = { 0 };
  uint32_t visibility[MAX_DAYS] = { 0 };

  float    rain[MAX_DAYS] = { 0 };
  float    snow[MAX_DAYS] = { 0 };

  // hourly.weather
  uint16_t id[MAX_DAYS] = { 0 };
  String   main[MAX_DAYS];
  String   description[MAX_DAYS];
  String   icon[MAX_DAYS];
  float    pop[MAX_DAYS];

} OW_daily;

/***************************************************************************************
** Description:   JSON interface class for Open-Meteo API (ESP32 only)
**
** Open-Meteo differences from OpenWeatherMap:
**   - No API key required
**   - JSON is columnar: parallel arrays per field, NOT array-of-objects
**   - Field names differ (mapped internally, struct names unchanged)
**   - "time" is ISO-8601 string, converted to unix timestamp internally
**   - Uses plain HTTP (port 80) to save heap vs TLS on 4MB ESP32
***************************************************************************************/
class OW_Weather: public JsonListener {

  public:
    // Sketch calls this forecast request, returns true if no parse errors encountered.
    bool getForecast(OW_current *current, OW_hourly *hourly, OW_daily  *daily, String latitude, String longitude,
                     String units);

    // Called by library (or user sketch), sends a GET request to a http url
    // and parses response, returns true if no parse errors
    bool parseRequest(String url);

    void partialDataSet(bool partialSet);

    float    lat = 0;
    float    lon = 0;
    String   timezone = "";

  private: // Streaming parser callback functions

    void startDocument();
    void endDocument();
    void startObject();
    void endObject();
    void startArray();
    void endArray();

    void key(const char *key);
    void value(const char *value);
    void whitespace(char c);
    void error( const char *message );

    void fullDataSet(const char *value);
    void partialDataSet(const char *value);

    // Convert an Open-Meteo ISO-8601 time string ("2024-01-15T14:00") to a
    // unix timestamp (seconds since epoch). Handles both date-only and
    // date+time variants. Returns 0 on parse failure.
    uint32_t isoToUnix(const String &iso);

  private:

    uint16_t hourly_index;
    uint16_t daily_index;

    OW_current  *current;
    OW_hourly   *hourly;
    OW_daily    *daily;

    // -----------------------------------------------------------------------
    // Open-Meteo columnar tracking
    // -----------------------------------------------------------------------
    // In Open-Meteo the JSON looks like:
    //   "hourly": { "time": [...], "temperature_2m": [...], ... }
    //   "daily":  { "time": [...], "temperature_2m_max": [...], ... }
    //
    // So instead of incrementing arrayIndex when an object ends (as with OWM),
    // we increment it each time endArray() fires for a field array, then reset
    // it to 0 when we start the next field array.  currentKey tells us which
    // field is being filled.

    String   currentSection;    // "current", "hourly", "daily", or ""
    String   currentKey;        // field name inside that section
    String   currentSet;        // sub-object name (unused for Open-Meteo but kept)
    uint16_t arrayIndex;        // position within the current field array
    uint16_t objectLevel;
    uint16_t arrayLevel;

    bool     parseOK;
    bool     partialSet = false;
};