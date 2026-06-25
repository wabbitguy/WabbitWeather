// Open-Meteo weather data fetcher / parser for ESP32
// Replaces the OpenWeatherMap version.  All struct field names are unchanged.
//
// Streaming JSON parser:
//   https://github.com/Bodmer/JSON_Decoder

#include <WiFiClient.h>  // plain HTTP — Open-Meteo supports it, saves ~40 KB heap
#include <JSON_Listener.h>
#include <JSON_Decoder.h>

#include "OpenMeteo.h"

// ---------------------------------------------------------------------------
// Open-Meteo API endpoint (no key needed)
// ---------------------------------------------------------------------------
static const char *OM_HOST = "api.open-meteo.com";
static const int OM_PORT = 80;

/***************************************************************************************
** Function name:           isoToUnix
** Description:             Convert "YYYY-MM-DDTHH:MM" or "YYYY-MM-DD" to unix time.
**                          Open-Meteo does not include seconds or timezone suffix in
**                          its time strings; the returned value is UTC epoch seconds.
**                          Uses a simple integer-math approach (no mktime) so it works
**                          without POSIX libc support on the ESP32 Arduino framework.
***************************************************************************************/
uint32_t OW_Weather::isoToUnix(const String &iso) {
  if (iso.length() < 10) return 0;

  int year = iso.substring(0, 4).toInt();
  int month = iso.substring(5, 7).toInt();
  int day = iso.substring(8, 10).toInt();
  int hour = 0, minute = 0;
  if (iso.length() >= 16) {
    hour = iso.substring(11, 13).toInt();
    minute = iso.substring(14, 16).toInt();
  }

  if (month < 3) {
    year--;
    month += 12;
  }
  long A = year / 100;
  long B = 2 - A + A / 4;
  long jd = (long)(365.25 * (year + 4716))
            + (long)(30.6001 * (month + 1))
            + day + B - 1524;
  long days = jd - 2440588;

  return (uint32_t)(days * 86400UL + hour * 3600UL + minute * 60UL);
}

/***************************************************************************************
** Function name:           getForecast
** Description:             Build the Open-Meteo URL and kick off the parse.
***************************************************************************************/
bool OW_Weather::getForecast(OW_current *current, OW_hourly *hourly, OW_daily *daily,
                             String latitude, String longitude, String units) {


  arrayIndex = 0;
  hourly_index = 0;
  daily_index = 0;
  currentSection = "";
  currentKey = "";

  this->current = current;
  this->hourly = hourly;
  this->daily = daily;

  String temp_unit = (units == "imperial") ? "fahrenheit" : "celsius";
  String wind_unit = (units == "imperial") ? "mph" : "ms";

  String currentFields = "";
  String hourlyFields = "";
  String dailyFields = "";

  if (current) {
    if (!partialSet) {
      currentFields = "temperature_2m,apparent_temperature,relative_humidity_2m,"
                      "surface_pressure,cloud_cover,visibility,wind_speed_10m,"
                      "wind_direction_10m,wind_gusts_10m,dew_point_2m,uv_index,"
                      "rain,snowfall,weather_code,is_day";
    } else {
      currentFields = "temperature_2m,relative_humidity_2m,surface_pressure,"
                      "cloud_cover,wind_speed_10m,wind_direction_10m,uv_index,"
                      "weather_code,is_day";
    }
  }

  if (hourly && !partialSet) {
    hourlyFields = "temperature_2m,apparent_temperature,relative_humidity_2m,"
                   "surface_pressure,cloud_cover,wind_speed_10m,wind_direction_10m,"
                   "wind_gusts_10m,dew_point_2m,rain,snowfall,weather_code,"
                   "precipitation_probability";
  }

  if (daily) {
    if (!partialSet) {
      dailyFields = "temperature_2m_max,temperature_2m_min,sunrise,sunset,"
                    "surface_pressure_mean,precipitation_probability_max,"
                    "wind_speed_10m_max,wind_gusts_10m_max,wind_direction_10m_dominant,"
                    "rain_sum,snowfall_sum,weather_code,uv_index_max,"
                    "apparent_temperature_max,apparent_temperature_min,"
                    "cloud_cover_mean,dew_point_2m_mean";
    } else {
      dailyFields = "temperature_2m_max,temperature_2m_min,weather_code";
    }
  }

  String url = "http://";
  url += OM_HOST;
  url += "/v1/forecast?latitude=" + latitude;
  url += "&longitude=" + longitude;
  url += "&timezone=auto";
  url += "&temperature_unit=" + temp_unit;
  url += "&wind_speed_unit=" + wind_unit;
  url += "&forecast_days=8";
  url += "&forecast_hours=8";  // breathing room
  if (currentFields.length()) url += "&current=" + currentFields;
  if (hourlyFields.length()) url += "&hourly=" + hourlyFields;
  if (dailyFields.length()) url += "&daily=" + dailyFields;

 // Serial.println(url); only if we need check the URL contents

  bool result = parseRequest(url);

  // Open-Meteo has no sunrise/sunset in current — copy from day 0
  if (result && current && daily) {
    current->sunrise = daily->sunrise[0];
    current->sunset = daily->sunset[0];
  }

  this->current = nullptr;
  this->hourly = nullptr;
  this->daily = nullptr;

  return result;
}

/***************************************************************************************
** Function name:           partialDataSet
** Description:             Set requested data set to partial (true) or full (false)
***************************************************************************************/
void OW_Weather::partialDataSet(bool partialSet) {
  this->partialSet = partialSet;
}

/***************************************************************************************
** Function name:           parseRequest  (ESP32, plain HTTP)
***************************************************************************************/
bool OW_Weather::parseRequest(String url) {

  uint32_t dt = millis();

  WiFiClient client;

  JSON_Decoder parser;
  parser.setListener(this);

  if (!client.connect(OM_HOST, OM_PORT)) {
    Serial.println("Connection to api.open-meteo.com failed.");
    return false;
  }

  parseOK = false;

  String request = String("GET ") + url + " HTTP/1.0\r\n"
                   + "Host: " + OM_HOST + "\r\n"
                   + "Connection: close\r\n\r\n";

  Serial.println("\nSending GET request to api.open-meteo.com...");
  client.print(request);

  uint32_t timeout = millis();
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Header end found");
      break;
    }
#ifdef SHOW_HEADER
    Serial.println(line);
#endif
    if ((millis() - timeout) > 5000UL) {
      Serial.println("HTTP header timeout");
      client.stop();
      return false;
    }
  }

  // Serial.println("\nParsing JSON");

#ifdef SHOW_JSON
  int ccount = 0;
#endif

  timeout = millis();
  while (client.available() > 0 || client.connected()) {
    while (client.available() > 0) {
      char c = client.read();
      parser.parse(c);

#ifdef SHOW_JSON
      if (c == '{' || c == '[' || c == '}' || c == ']') Serial.println();
      Serial.print(c);
      if (ccount++ > 100 && c == ',') {
        ccount = 0;
        Serial.println();
      }
#endif

      if ((millis() - timeout) > 10000UL) {
        Serial.println("JSON parse timeout");
        parser.reset();
        client.stop();
        return false;
      }
      yield();
    }
  }

  // Serial.println("");
  // Serial.print("Done in ");
  // Serial.print(millis() - dt);
  // Serial.println(" ms\n");

  parser.reset();
  client.stop();

  return parseOK;
}

// ===========================================================================
// Streaming parser callbacks
// ===========================================================================

void OW_Weather::key(const char *key) {
  currentKey = key;
#ifdef SHOW_CALLBACK
  Serial.print("\n>>> Key >>> " + (String)key);
#endif
}

void OW_Weather::startDocument() {
  currentSection = currentKey = currentSet = "";
  objectLevel = arrayLevel = arrayIndex = 0;
  parseOK = true;
#ifdef SHOW_CALLBACK
  Serial.print("\n>>> Start document >>>");
#endif
}

void OW_Weather::endDocument() {
  currentSection = currentKey = "";
  objectLevel = arrayLevel = arrayIndex = 0;
#ifdef SHOW_CALLBACK
  Serial.print("\n<<< End document <<<");
#endif
}

void OW_Weather::startObject() {
  if (objectLevel == 1) currentSection = currentKey;
  objectLevel++;
#ifdef SHOW_CALLBACK
  Serial.print("\n>>> Start object level:" + (String)objectLevel + " section:" + currentSection + " >>>");
#endif
}

void OW_Weather::endObject() {
  objectLevel--;
  if (objectLevel == 0) currentSection = "";
#ifdef SHOW_CALLBACK
  Serial.print("\n<<< End object <<<");
#endif
}

void OW_Weather::startArray() {
  arrayIndex = 0;
  arrayLevel++;
#ifdef SHOW_CALLBACK
  Serial.print("\n>>> Start array section:" + currentSection + " key:" + currentKey + " >>>");
#endif
}

void OW_Weather::endArray() {
  if (arrayLevel > 0) arrayLevel--;
  arrayIndex = 0;
#ifdef SHOW_CALLBACK
  Serial.print("\n<<< End array <<<");
#endif
}

void OW_Weather::whitespace(char c) {}

void OW_Weather::error(const char *message) {
  Serial.print("\nParse error: ");
  Serial.println(message);
  parseOK = false;
}

void OW_Weather::value(const char *val) {
  if (!partialSet) fullDataSet(val);
  else partialDataSet(val);
}

// ===========================================================================
// WMO weather code → main/description strings
// Raw WMO code is stored directly in ->id and used by getMeteoconIcon()
// in the sketch, which maps WMO codes to icon filenames natively.
//
// WMO code reference:
//   0        = Clear sky
//   1,2,3    = Mainly clear, partly cloudy, overcast
//   45,48    = Fog
//   51,53,55 = Drizzle light/moderate/dense
//   56,57    = Freezing drizzle
//   61,63,65 = Rain slight/moderate/heavy
//   66,67    = Freezing rain
//   71,73,75 = Snow slight/moderate/heavy
//   77       = Snow grains
//   80,81,82 = Rain showers slight/moderate/heavy
//   85,86    = Snow showers
//   95       = Thunderstorm
//   96,99    = Thunderstorm with hail
// ===========================================================================

static void wmoToDescription(int code, String &main, String &description) {
  if (code == 0) {
    main = "Clear";
    description = "Clear sky";
  } else if (code == 1) {
    main = "Clear";
    description = "Mainly clear";
  } else if (code == 2) {
    main = "Clouds";
    description = "Partly cloudy";
  } else if (code == 3) {
    main = "Clouds";
    description = "Overcast";
  } else if (code == 45) {
    main = "Fog";
    description = "Fog";
  } else if (code == 48) {
    main = "Fog";
    description = "Icy fog";
  } else if (code == 51) {
    main = "Drizzle";
    description = "Light drizzle";
  } else if (code == 53) {
    main = "Drizzle";
    description = "Moderate drizzle";
  } else if (code == 55) {
    main = "Drizzle";
    description = "Dense drizzle";
  } else if (code == 56) {
    main = "Drizzle";
    description = "Light freezing drizzle";
  } else if (code == 57) {
    main = "Drizzle";
    description = "Heavy freezing drizzle";
  } else if (code == 61) {
    main = "Rain";
    description = "Slight rain";
  } else if (code == 63) {
    main = "Rain";
    description = "Moderate rain";
  } else if (code == 65) {
    main = "Rain";
    description = "Heavy rain";
  } else if (code == 66) {
    main = "Rain";
    description = "Light freezing rain";
  } else if (code == 67) {
    main = "Rain";
    description = "Heavy freezing rain";
  } else if (code == 71) {
    main = "Snow";
    description = "Slight snow";
  } else if (code == 73) {
    main = "Snow";
    description = "Moderate snow";
  } else if (code == 75) {
    main = "Snow";
    description = "Heavy snow";
  } else if (code == 77) {
    main = "Snow";
    description = "Snow grains";
  } else if (code == 80) {
    main = "Rain";
    description = "Slight rain showers";
  } else if (code == 81) {
    main = "Rain";
    description = "Moderate rain showers";
  } else if (code == 82) {
    main = "Rain";
    description = "Violent rain showers";
  } else if (code == 85) {
    main = "Snow";
    description = "Slight snow showers";
  } else if (code == 86) {
    main = "Snow";
    description = "Heavy snow showers";
  } else if (code == 95) {
    main = "Thunderstorm";
    description = "Thunderstorm";
  } else if (code == 96) {
    main = "Thunderstorm";
    description = "Thunderstorm with hail";
  } else if (code == 99) {
    main = "Thunderstorm";
    description = "Thunderstorm with hail";
  } else {
    main = "Unknown";
    description = "Unknown";
  }
}

/***************************************************************************************
** Function name:           fullDataSet
** Description:             Populate structures with the full data set
***************************************************************************************/
void OW_Weather::fullDataSet(const char *val) {

  String value = val;

  // ── Top-level scalars ──────────────────────────────────────────────────
  if (currentSection == "") {
    if (currentKey == "latitude") lat = value.toFloat();
    else if (currentKey == "longitude") lon = value.toFloat();
    else if (currentKey == "timezone") timezone = value;
    return;
  }

  // ── current ────────────────────────────────────────────────────────────
  // Open-Meteo "current" is a flat object (not an array).
  if (currentSection == "current" && current) {
    if (currentKey == "time") current->dt = isoToUnix(value);
    else if (currentKey == "temperature_2m") current->temp = value.toFloat();
    else if (currentKey == "apparent_temperature") current->feels_like = value.toFloat();
    else if (currentKey == "relative_humidity_2m") current->humidity = value.toInt();
    else if (currentKey == "surface_pressure") current->pressure = value.toFloat();
    else if (currentKey == "dew_point_2m") current->dew_point = value.toFloat();
    else if (currentKey == "uv_index") current->uvi = value.toFloat();
    else if (currentKey == "cloud_cover") current->clouds = value.toInt();
    else if (currentKey == "visibility") current->visibility = value.toInt();
    else if (currentKey == "wind_speed_10m") current->wind_speed = value.toFloat();
    else if (currentKey == "wind_gusts_10m") current->wind_gust = value.toFloat();
    else if (currentKey == "wind_direction_10m") current->wind_deg = (uint16_t)value.toInt();
    else if (currentKey == "rain") current->rain = value.toFloat();
    else if (currentKey == "snowfall") current->snow = value.toFloat();
    else if (currentKey == "weather_code") {
      // Store raw WMO code — getMeteoconIcon() uses it directly
      current->id = value.toInt();
      wmoToDescription(current->id, current->main, current->description);
      current->icon = "";  // icon resolved at draw time by getMeteoconIcon()
    }
    // is_day is not needed — getMeteoconIcon() derives day/night from
    // current->dt vs current->sunrise / current->sunset
    return;
  }

  // ── hourly ─────────────────────────────────────────────────────────────
  if (currentSection == "hourly" && hourly) {
    if (arrayIndex >= MAX_HOURS) {
      arrayIndex++;
      return;
    }

    if (currentKey == "time") hourly->dt[arrayIndex] = isoToUnix(value);
    else if (currentKey == "temperature_2m") hourly->temp[arrayIndex] = value.toFloat();
    else if (currentKey == "apparent_temperature") hourly->feels_like[arrayIndex] = value.toFloat();
    else if (currentKey == "relative_humidity_2m") hourly->humidity[arrayIndex] = value.toInt();
    else if (currentKey == "surface_pressure") hourly->pressure[arrayIndex] = value.toFloat();
    else if (currentKey == "dew_point_2m") hourly->dew_point[arrayIndex] = value.toFloat();
    else if (currentKey == "cloud_cover") hourly->clouds[arrayIndex] = value.toInt();
    else if (currentKey == "wind_speed_10m") hourly->wind_speed[arrayIndex] = value.toFloat();
    else if (currentKey == "wind_gusts_10m") hourly->wind_gust[arrayIndex] = value.toFloat();
    else if (currentKey == "wind_direction_10m") hourly->wind_deg[arrayIndex] = (uint16_t)value.toInt();
    else if (currentKey == "rain") hourly->rain[arrayIndex] = value.toFloat();
    else if (currentKey == "snowfall") hourly->snow[arrayIndex] = value.toFloat();
    else if (currentKey == "precipitation_probability") hourly->pop[arrayIndex] = value.toFloat() / 100.0f;
    else if (currentKey == "weather_code") {
      hourly->id[arrayIndex] = value.toInt();
      wmoToDescription(hourly->id[arrayIndex], hourly->main[arrayIndex], hourly->description[arrayIndex]);
      hourly->icon[arrayIndex] = "";
    }

    arrayIndex++;
    return;
  }

  // ── daily ──────────────────────────────────────────────────────────────
  if (currentSection == "daily" && daily) {
    // Serial.print("daily key: ");
    // Serial.print(currentKey);
    // Serial.print(" arrayIndex: ");
    // Serial.print(arrayIndex);
    // Serial.print(" value: ");
    // Serial.println(value);

    if (arrayIndex >= MAX_DAYS) {
      arrayIndex++;
      return;
    }

    if (currentKey == "time") daily->dt[arrayIndex] = isoToUnix(value);
    else if (currentKey == "sunrise") daily->sunrise[arrayIndex] = isoToUnix(value);
    else if (currentKey == "sunset") daily->sunset[arrayIndex] = isoToUnix(value);
    else if (currentKey == "temperature_2m_max") daily->temp_max[arrayIndex] = value.toFloat();
    else if (currentKey == "temperature_2m_min") daily->temp_min[arrayIndex] = value.toFloat();
    else if (currentKey == "apparent_temperature_max") daily->feels_like_day[arrayIndex] = value.toFloat();
    else if (currentKey == "apparent_temperature_min") daily->feels_like_night[arrayIndex] = value.toFloat();
    else if (currentKey == "surface_pressure_mean") daily->pressure[arrayIndex] = value.toFloat();
    else if (currentKey == "cloud_cover_mean") daily->clouds[arrayIndex] = value.toInt();
    else if (currentKey == "wind_speed_10m_max") daily->wind_speed[arrayIndex] = value.toFloat();
    else if (currentKey == "wind_gusts_10m_max") daily->wind_gust[arrayIndex] = value.toFloat();
    else if (currentKey == "wind_direction_10m_dominant") daily->wind_deg[arrayIndex] = (uint16_t)value.toInt();
    else if (currentKey == "rain_sum") daily->rain[arrayIndex] = value.toFloat();
    else if (currentKey == "snowfall_sum") daily->snow[arrayIndex] = value.toFloat();
    else if (currentKey == "precipitation_probability_max") daily->pop[arrayIndex] = value.toFloat() / 100.0f;
    else if (currentKey == "dew_point_2m_mean") daily->dew_point[arrayIndex] = value.toFloat();
    else if (currentKey == "weather_code") {
      daily->id[arrayIndex] = value.toInt();
      wmoToDescription(daily->id[arrayIndex], daily->main[arrayIndex], daily->description[arrayIndex]);
      daily->icon[arrayIndex] = "";
    }

    arrayIndex++;
    return;
  }
}

/***************************************************************************************
** Function name:           partialDataSet
** Description:             Populate structures with minimal data set
***************************************************************************************/
void OW_Weather::partialDataSet(const char *val) {

  String value = val;

  // ── Top-level scalars ──────────────────────────────────────────────────
  if (currentSection == "") {
    if (currentKey == "latitude") lat = value.toFloat();
    else if (currentKey == "longitude") lon = value.toFloat();
    else if (currentKey == "timezone") timezone = value;
    return;
  }

  // ── current (partial) ──────────────────────────────────────────────────
  if (currentSection == "current" && current) {
    if (currentKey == "time") current->dt = isoToUnix(value);
    else if (currentKey == "temperature_2m") current->temp = value.toFloat();
    else if (currentKey == "relative_humidity_2m") current->humidity = value.toInt();
    else if (currentKey == "surface_pressure") current->pressure = value.toFloat();
    else if (currentKey == "cloud_cover") current->clouds = value.toInt();
    else if (currentKey == "wind_speed_10m") current->wind_speed = value.toFloat();
    else if (currentKey == "wind_direction_10m") current->wind_deg = (uint16_t)value.toInt();
    else if (currentKey == "uv_index") current->uvi = value.toFloat();
    else if (currentKey == "weather_code") {
      current->id = value.toInt();
      wmoToDescription(current->id, current->main, current->description);
      current->icon = "";
    }
    return;
  }

  // ── daily (partial) ────────────────────────────────────────────────────
  if (currentSection == "daily" && daily) {
    if (arrayIndex >= MAX_DAYS) {
      arrayIndex++;
      return;
    }

    if (currentKey == "time") daily->dt[arrayIndex] = isoToUnix(value);
    else if (currentKey == "temperature_2m_max") daily->temp_max[arrayIndex] = value.toFloat();
    else if (currentKey == "temperature_2m_min") daily->temp_min[arrayIndex] = value.toFloat();
    else if (currentKey == "weather_code") {
      daily->id[arrayIndex] = value.toInt();
      wmoToDescription(daily->id[arrayIndex], daily->main[arrayIndex], daily->description[arrayIndex]);
      daily->icon[arrayIndex] = "";
    }

    arrayIndex++;
    return;
  }
}