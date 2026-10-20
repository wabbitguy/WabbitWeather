//  The weather icons and fonts are in the sketch data folder, press Ctrl+K
//  to view.

//            >>>       IMPORTANT TO PREVENT CRASHES      <<<
//>>>>>>  Set SPIFFS to at least 1.5Mbytes before uploading files  <<<<<<

//                >>>           DON'T FORGET THIS             <<<
//  Upload the fonts and icons to SPIFFS using the "Tools"  "ESP32 Sketch Data Upload"
//  or "ESP8266 Sketch Data Upload" menu option in the IDE.
//  To add this option follow instructions here for the ESP8266:
//  https://github.com/esp8266/arduino-esp8266fs-plugin
//  To add this option follow instructions here for the ESP32:
//  https://github.com/me-no-dev/arduino-esp32fs-plugin

//  Close the IDE and open again to see the new menu option.

#pragma once

/***************************************************************************************
**                          Language selection
**                          Uncomment ONE of the following:
***************************************************************************************/
#define LANG_EN   // English  (default)
//#define LANG_FR   // French
//#define LANG_DE   // German
//#define LANG_ES   // Spanish
//#define LANG_NL   // Dutch
//#define LANG_PT   // Portuguese
//#define LANG_TR   // Turkish

// WebConfig.cpp includes this file with SETTINGS_LANGUAGE_ONLY defined, so the
// web page uses the language above without redefining the settings below.
#ifndef SETTINGS_LANGUAGE_ONLY

/***************************************************************************************
**                          Timing
***************************************************************************************/

// Update every 30 minutes — conditions don't meaningfully change faster than
// that, and it's right in line with what most weather apps/widgets use
// (Apple ~10min, Android widgets floor at 30min by OS policy). Halves our
// Open-Meteo call volume and exposure to their outages vs. the old 15min.
const int UPDATE_INTERVAL_SECS = 30 * 60UL;

// Running more than one station? Give each its own HOSTNAME, e.g. "WabbitWeather2".
// It sets the web address (wabbitweather2.local), the router device name and
// the splash screen title. Use letters, digits and hyphens only.
#define HOSTNAME           "WabbitWeather"
#define DEFAULT_CAPTIVE_SSID "WabbitWeather"

/***************************************************************************************
**                          sunrise/sunset trigger points for auto-dim
***************************************************************************************/
uint32_t sunrise, sunset;

/***************************************************************************************
**                          Primary location
***************************************************************************************/

// Temperature units: "metric" or "imperial"
String units = "imperial";

// Default location: Science World, Vancouver BC
// Users set their own location via the web interface — no need to edit this file.
String latitude  = "49.2734";
String longitude = "-123.1037";

// Timezone offset in seconds east of UTC for the primary location.
// -25200 = UTC-7 (PDT).  Updated automatically at 2am and on web save.
int32_t tzOffset = -25200;

/***************************************************************************************
**                          Alternate location
***************************************************************************************/

// Second location — user sets via web interface.
// Defaults to same as primary so the display shows something sensible on first boot.
String latitude2  = "49.2734";
String longitude2 = "-123.1037";

// Timezone offset for the alternate location.
int32_t tzOffset2 = -25200;

// City name cache — populated by Nominatim, not editable directly
String theCityLocation  = "Unknown";
String theCityLocation2 = "Unknown";

/***************************************************************************************
**                          Display options
***************************************************************************************/

bool show24Hour           = false;  // false = 12hr, true = 24hr clock
bool autoDimDusk          = true;   // dim display between sunset and sunrise
bool showBarometric       = false;  // show barometric pressure
bool showPrecipProbability = true;  // show precip probability in forecast
bool showUVindex          = true;   // show UV index bar
bool show6hrForecast      = true;   // rotate 6hr forecast in lower panel
bool forecastStartToday   = false;  // false = forecast from tomorrow, true = from today

// Weather icon set: 0 = Standard (BMPs in /icon and /icon50 on LittleFS)
//                   1 = Meteocons (built into the firmware, see MeteoconIcons.h)
uint8_t iconSet = 0;

/***************************************************************************************
**                          Meteocons icon colours
**                          Only used when iconSet = 1
***************************************************************************************/

uint16_t meteoconPalette[4] = {
  TFT_BLACK,   // 0 background
  TFT_WHITE,   // 1 cloud outlines and moon
  TFT_YELLOW,  // 2 sun
  0x7E3C       // 3 rain, snow and cloud edges (light blue)
};

/***************************************************************************************
**                          Backlight
***************************************************************************************/

uint8_t blDusk = 30;  // backlight brightness at night (0 = off, 255 = full)

/***************************************************************************************
**                          Temperature threshold colours
**                          Imperial defaults: high=85F, low=40F
**                          Metric equivalents: ~29C, ~4C
***************************************************************************************/

uint8_t  highTempVal    = 85;
uint8_t  lowTempVal     = 40;
uint32_t highTempColour = TFT_RED;
uint32_t midTempColour  = TFT_GREEN;
uint32_t lowTempColour  = TFT_BLUE;

/***************************************************************************************
**                          Precipitation probability threshold colours
***************************************************************************************/

uint8_t  highPrecipProb = 75;
uint8_t  lowPrecipProb  = 30;
uint32_t highPOPColour  = TFT_RED;
uint32_t midPOPColour   = TFT_YELLOW;
uint32_t lowPOPColour   = TFT_GREEN;

/***************************************************************************************
**                          Label colours
***************************************************************************************/

uint32_t labelColour     = TFT_WHITE;   // weather panel headings
uint32_t astrologyColour = TFT_YELLOW;  // astronomy panel headings

// End of settings

#endif  // SETTINGS_LANGUAGE_ONLY
