# WabbitWeather

A weather display for the ESP32 CYD (Cheap Yellow Display) using [Open-Meteo](https://open-meteo.com) for weather data and [Nominatim](https://nominatim.org) for location lookup. No API keys or subscriptions required.

<img width="360" height="558" alt="Splash" src="https://github.com/user-attachments/assets/490a29e4-68ee-4f7f-9cf9-07290c2f26a3" />

Originally based on [Bodmer's DarkSkyWeather](https://github.com/Bodmer/DarkSkyWeather), significantly extended and adapted by WabbitGuy.

---
## Hardware

- **ESP32 CYD** (Cheap Yellow Display) — 2.8" 240×320 TFT with XPT2046 touchscreen
- The boards used and tested with are the **ST7789** and **ILI9341** versions
- Both CYD Drivers are included in TFT-eSPI_Configs folder

---

## Features

- Live weather from [Open-Meteo](https://open-meteo.com) — updates every 30 minutes, no subscription, no API key
- Location lookup via [Nominatim](https://nominatim.org) — city name displayed automatically from coordinates, or type your own
- 8-day forecast with tap-to-page (days 1–4 or days 5–8), with option to start from today
- 6-hour rotating forecast showing precipitation probability, temperature and dew point
- Current conditions: temperature, wind speed/direction, UV index, cloud cover, humidity
- Moon phase display with correct hemisphere orientation (northern/southern)
- Sunrise/sunset times, with optional backlight dimming from sunset to sunrise
- **Dual location support** — tap the BIG weather icon area to toggle between two locations
- **Automatic timezone detection** via [timeapi.io](https://timeapi.io) — no hardcoding required
- Timezone offsets self-correct at 2am daily for both locations
- Full web configuration interface — no need to edit code for normal setup
- Map-based location selection with search box (type "London UK" to jump there)
- Multilingual: English, French, German, Spanish, Dutch, Portuguese, Turkish
- Temperature and precipitation threshold colours — fully user configurable
- Barometric pressure selectable sea level or current elevation
- Easy WiFi setup: scan the QR codes on the display with your phone's camera (WiFiManager)

---

## Touch Zones

The display has three touch-sensitive areas:

```
┌─────────────────────────┐
│   Clock / date area     │  Tap → About screen (version and IP address)
├─────────────────────────┤
│   Weather icon, temp,   │  Tap → Toggle between TZ1 and TZ2 locations
│   wind, city name       │
├─────────────────────────┤
│   MON TUE WED THU       │  Tap → Toggle forecast page (days 1-4 / days 5-8)
└─────────────────────────┘
```

---

## Web Configuration

Access the configuration page at **http://wabbitweather.local** (or the device IP address) from any browser on the same network. Some phones and computers (older Android especially) can't open `.local` addresses — tap the clock on the display to see its IP address and use that instead.

### Location

Set up to two locations using the interactive map. Click or drag the pin, or type a city name in the search box. The timezone offset is detected automatically from the coordinates — no manual timezone selection needed.

<img width="640" height="426" alt="map" src="https://github.com/user-attachments/assets/c47578f7-f9ef-4088-91c7-09aed7d13437" />

TZ1 is your primary location. TZ2 is an alternate location (family in another city, a vacation property, etc.). Tap the weather icon area on the display to switch between them.

Under the map, **TZ1 name on display** and **TZ2 name on display** show the name the display uses for each location. Moving a pin fills in the town name from OpenStreetMap; change it to whatever you like. Handy when OpenStreetMap picks the neighbouring town — its town lines don't always match your mailing address.

### Display Settings

<img width="640" height="583" alt="Display_Settings" src="https://github.com/user-attachments/assets/dd57c5b8-7393-4dab-b0e4-a4730a541b99" />

- Imperial (°F) or Metric (°C)
- 12 or 24 hour clock
- Auto dim at sunset (uses sunrise/sunset times for the active location; off by default)
- Night brightness (0 = display off, 255 = full brightness)
- Show/hide barometric pressure, precipitation probability, UV index, 6hr forecast
- Forecast start: Today or Tomorrow
- Weather icon set
- Weather and astrology heading colours

### Temperature Threshold Colours

<img width="640" height="288" alt="Temp" src="https://github.com/user-attachments/assets/091f0a38-46af-410d-a36e-07950b6bdd9f" />

Set high and low temperature thresholds and choose colours for each range. Values are in whichever units you selected (°F or °C).

### Precipitation Threshold Colours

<img width="640" height="287" alt="Precip" src="https://github.com/user-attachments/assets/330a87c7-93ce-41f3-8e0d-4fce30f4b987" />

Set high and low precipitation probability thresholds and choose colours for each range.

---

## 🚀 Quick Install (no Arduino IDE needed)
### Works with both ST7789 and ILI9341 boards

[**Click here to install WabbitWeather from your browser**](https://wabbitguy.github.io/WabbitWeather-Install/)

Works in Chrome, Edge, Opera, or recent Firefox on desktop. Not supported in Safari. Plug in your CYD over USB, pick your display and language, and click Install. Then carry on from step 4 of First Time Setup below.

---
## Partition Scheme (Arduino IDE)

WabbitWeather uses a **NO OTA 2MB App / 2MB LittleFS** partition scheme to accommodate the fonts and weather icon set. Select this in the Arduino IDE under Tools → Partition Scheme before flashing.

---

## First Time Setup (Arduino IDE)

1. Use correct driver from TFT-eSPI_Configs, copy to your TFT_eSPI folder, rename copied driver to User_Setup.h
2. Make your language selection first in the All_Settings.h file (see section below)
3. Flash the firmware and upload the LittleFS data folder (fonts and icons)
4. Power on the device — it creates a WiFi access point called **WabbitWeather** and shows a QR code
5. Point your phone's camera at the QR code and tap **Join**
6. The display then shows a second QR code — scan it to open the setup page. (No camera? Join the **WabbitWeather** WiFi network by hand — it has no password — and browse to **http://192.168.4.1**)
7. Tap **Configure WiFi**, select your home WiFi network and enter the password. Pick a 2.4 GHz network; the ESP32 can't use 5 GHz
8. The device reboots and connects to your network
9. Open **http://wabbitweather.local** in a browser. If that doesn't work, tap the clock on the display to see its IP address and type that in instead
10. Click the map to set your location — timezone is detected automatically
11. Hit **Update & Save**
12. Done — the display updates immediately with your local weather

If you're new to Arduino, read down further for detailed installation steps

---

## Language Selection

Open `All_Settings.h` and uncomment the language you want:

```cpp
#define LANG_EN   // English  (default)
//#define LANG_FR   // French
//#define LANG_DE   // German
//#define LANG_ES   // Spanish
//#define LANG_NL   // Dutch
//#define LANG_PT   // Portuguese
//#define LANG_TR   // Turkish
```

---

## Libraries Required

- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- [XPT2046_Touchscreen](https://github.com/PaulStoffregen/XPT2046_Touchscreen)
- [JSON_Decoder](https://github.com/Bodmer/JSON_Decoder)
- [WiFiManager](https://github.com/tzapu/WiFiManager)
- [Time (TimeLib)](https://github.com/PaulStoffregen/Time)

---

## Installation

### 1. Download and rename the sketch folder

Download the zip from GitHub and extract it. GitHub names the extracted folder `WabbitWeather-master`. **Arduino requires the sketch folder name to exactly match the `.ino` filename**, so you must rename it before opening the sketch.

Rename `WabbitWeather-master` → `WabbitWeather`, then open `WabbitWeather/WabbitWeather.ino` in the Arduino IDE.

If you skip this step the `.cpp` files won't compile and you'll see a wall of "undefined reference" linker errors.

### 2. Select the correct board and partition scheme

In **Tools → Board**, select **ESP32 Dev Module**, then set:

| Setting | Value |
|---|---|
| Partition Scheme | **No OTA (2MB APP/2MB SPIFFS)** |
| CPU Frequency | 240MHz (WiFi/BT) |
| Flash Size | 4MB (32Mb) |
| Upload Speed | 115200 |

> ⚠️ Do **not** select the FATFS variant — WabbitWeather uses LittleFS and the icons and fonts stored on flash will not be found at runtime.
> Upload the data partition **after** uploading the sketch, with the correct port selected.

### 3. Upload the LittleFS data partition

The weather icons and fonts live in the `data` folder and must be uploaded to the ESP32's flash separately — uploading the sketch alone is not enough. The method differs between IDE versions. If you've never uploaded LittleFS data before or get an error while trying to because the uploader is not installed keep reading...

**Arduino IDE 1.x**

Install the ESP32 LittleFS upload plugin:
- Download from: https://github.com/lorol/LITTLEFS_esp32fs-plugin/releases
- Place the `.jar` file in `<Arduino sketchbook>/tools/ESP32LittleFS/tool/`
- Restart the IDE
- Use **Tools → ESP32 LittleFS Data Upload**

**Arduino IDE 2.x**

Install the separate upload tool:
- Download from: https://github.com/earlephilhower/arduino-littlefs-upload/releases
- Place the `.vsix` file in the correct location per the instructions on that page
- Restart the IDE
- On macOS, press [⌘] + [Shift] + [P], Windows: [Ctrl] + [Shift] + [P]
- Type in Upload and you'll see "Upload LittleFS to PICO/ESP8266/ESP32"

> ⚠️ The serial monitor must be **closed** before uploading the data partition.

---

## Version History

### v1.8.2
- WiFi setup by QR code: scan to join the hotspot, then scan a second code to open the setup page
- Your own name for each location on the settings page, filled in from OpenStreetMap when you move the pin
- IP address on the About screen (tap the clock), for phones and computers that can't open wabbitweather.local
- "Auto dim at sunset" is now off by default, and its name matches the sunrise/sunset times on screen

### v1.8.0
- Accented letters on screen and on the web page (French, German, Spanish, Portuguese, Turkish)
- City names shown in the selected language where OpenStreetMap has them (e.g. München)
- Wi-Fi setup screen and WiFi Reset button translated
- One name to set (HOSTNAME) for the hotspot, web address and page title
- ILI9341 boards fixed (HSPI)
- Quick Install from the browser, no Arduino IDE needed

### v1.7.0
- Weather cached per location unless outside the 30 minute refresh, then it's updated
- To force a weather update, touch the large weather icon for 2 seconds. Progress ring top left shows.
- Updated/Saved via web browser settings only refreshes weather if location is changed
- Failed downloads of weather show last good data instead of blank screen
- City names are cached to remove constant checks with Nominatim lookups (fast screen updates)
- AM/PM shown in 12Hr clock display
- Screen optimized for UV and WIFI bars, plus 6hr display area
- Pressure can be sea level or station (your elevation). Web browser settings

### v1.6.0
- Added optional weather icons from the original ThingPulse project
- Added a hostname change in All_Settings.h for multiple stations on same network (slight name changes)
- Removed unused Language.h file, language settings remain in All_Settings.h
- More code cleanup done

### v1.5.8
- Nomination's policy requires cached names so you don't over load them when switch locales
- When looking up the current locale, if it failed (rare) memory wasn't recovered. Fixed.

### v1.5.6
- Change weather requests to 30 minute intervals
- Fixed bitmap draw that wasn't using LittleFS properly
- Changed NTP requests so they don't collide with Open-Meteo requests
- Increased server timeouts to compensate for slow response from Open-Meteo
- Error responses or bad data from Open-Meteo no longer lock up the app (clock still runs)
- In event of Open-Meteo "issue", retries in 1 minute, then waits 30 minutes for next request

### v1.5.4
- Adjusted timeouts on Open-Meteo so it doesn't lock up the fetch
- Added flashing colons to the time display
- If weather fetch fails for over an hour, updated time changes to failed message

### v1.5.1
- Removed search icon from map and placed two search options by TZ lines
- Adjusted number of locations found for easier selection
- Fixed phone display from auto resizing on search
- Search can be by city, address or country

### v1.5.0
- Dual location support — tap the weather icon area to toggle between TZ1 and TZ2
- Automatic timezone detection via timeapi.io — no hardcoding, no dropdowns
- Timezone offsets self-correct at 2am daily for both locations
- Map search box — type a city name to jump there instantly
- Correct local time shown for whichever location is active
- Correct dusk dimming for whichever location is active
- Forecast start today/tomorrow option (up to 8 days from current day)
- JChristensen Timezone library eliminated entirely
- Language selection moved to All_Settings.h
- Fixed daily forecast to show HIGH then LOW (matching worldwide convention)
- Three touch zones (clock / weather / forecast)

### v1.4.0
- 8-day forecast with tap-to-page between day sets
- Auto-return to main forecast after 15 seconds
- Forecast start day option (today or tomorrow)

### Earlier
- Adapted from Bodmer DarkSkyWeather to use Open-Meteo (no subscription)
- Nominatim location lookup (no subscription)
- 6hr rotating forecast (POP%, temperature, dew point)
- Temperature and precipitation threshold colours
- UV index display
- Auto display dimming at dusk/dawn
- Full web configuration interface
- Multilingual support (EN, FR, DE, ES, NL, PT, TR)
- Moon phase icons with southern hemisphere flip

---

## Credits

- Original sketch by [Daniel Eichhorn](https://github.com/squix78)
- Adapted by [Bodmer](https://github.com/Bodmer) for TFT_eSPI and OpenWeather
- Extended by [WabbitGuy](https://github.com/wabbitguy) for Open-Meteo, Nominatim, dual location, web config and more

Weather data: [Open-Meteo](https://open-meteo.com) — free, no API key required  
Location lookup: [Nominatim / OpenStreetMap](https://nominatim.org) — free, no API key required  
Timezone detection: [timeapi.io](https://timeapi.io) — free, no API key required  
Map tiles: [OpenStreetMap](https://www.openstreetmap.org) via Leaflet

---

*WabbitWeather is open source under the MIT License*
