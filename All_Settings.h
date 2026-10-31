// WabbitWeather settings; most can also be changed later on the web page.
// Partition Scheme: "No OTA (2MB APP/2MB SPIFFS)". Upload the data folder (fonts, icons) to LittleFS.

#pragma once

/***************************************************************************************
**                          Language selection
**                          Uncomment ONE of the following:
**  (The ready-made .bin files on GitHub choose their language when they're built.)
***************************************************************************************/
#ifndef LANG_FROM_BUILD
#define LANG_EN   // English  (default)
//#define LANG_FR   // French
//#define LANG_DE   // German
//#define LANG_ES   // Spanish
//#define LANG_NL   // Dutch
//#define LANG_PT   // Portuguese
//#define LANG_TR   // Turkish
//#define LANG_IT   // Italian
#endif

/***************************************************************************************
**                          Device name
**  Used for the Wi-Fi setup hotspot, web address (wabbitweather.local), router device
**  name, splash screen and web page. Running more than one? Give each its own name,
**  e.g. "WabbitWeather2". Letters, digits and hyphens only.
***************************************************************************************/
#define HOSTNAME "WabbitWeather"

#ifndef SETTINGS_LANGUAGE_ONLY  // WebConfig.cpp only needs the language and name above

/***************************************************************************************
**                          Timing
***************************************************************************************/

const int UPDATE_INTERVAL_SECS = 30 * 60UL;  // weather update every 30 minutes

/***************************************************************************************
**                          sunrise/sunset trigger points for auto-dim
***************************************************************************************/
uint32_t sunrise, sunset;

/***************************************************************************************
**                          Primary location
***************************************************************************************/

// Temperature units: "metric" or "imperial"
String units = "imperial";

// Default location: Science World, Vancouver BC. Set your own on the web page.
String latitude  = "49.2734";
String longitude = "-123.1037";

int32_t tzOffset = -25200;  // seconds east of UTC (-25200 = UTC-7); updated at 2am and on Save

/***************************************************************************************
**                          Alternate location
***************************************************************************************/

// Second location, set on the web page (starts the same as the first)
String latitude2  = "49.2734";
String longitude2 = "-123.1037";

// Timezone offset for the alternate location.
int32_t tzOffset2 = -25200;

// City name cache — populated by Nominatim, not editable directly
String theCityLocation  = "Unknown";
String theCityLocation2 = "Unknown";

// Names typed on the web page, shown instead of the looked-up ones ("" = use the lookup)
String locationName  = "";
String locationName2 = "";

/***************************************************************************************
**                          Display options
***************************************************************************************/

bool show24Hour           = false;  // false = 12hr, true = 24hr clock
bool autoDimDusk          = false;   // dim display between sunset and sunrise
bool showBarometric       = false;  // show barometric pressure
bool pressureSeaLevel     = true;   // true = sea level (as weather reports), false = station (actual at your elevation)
bool showPrecipProbability = true;  // show precip probability in forecast
bool showUVindex          = true;   // show UV index bar
bool show6hrForecast      = true;   // rotate 6hr forecast in lower panel
bool forecastStartToday   = false;  // false = forecast from tomorrow, true = from today
bool invertColours        = false;  // true if black shows as white (some CYD panels need it)

uint8_t iconSet = 0;  // 0 = Standard (BMPs on LittleFS), 1 = Meteocons (built into the firmware)

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

int16_t  highTempVal    = 85;  // signed — Celsius thresholds go below zero
int16_t  lowTempVal     = 40;
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
