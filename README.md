# WabbitWeather

A weather display for the ESP32 CYD (Cheap Yellow Display) using [Open-Meteo](https://open-meteo.com) for weather data and [Nominatim](https://nominatim.org) for location lookup. No API keys or subscriptions required.

<img width="285" height="480" alt="IMG_8050" src="https://github.com/user-attachments/assets/2c43670e-5614-47df-a0f6-49a67f651d69" />

Originally based on [Bodmer's DarkSkyWeather](https://github.com/Bodmer/DarkSkyWeather), significantly extended and adapted by WabbitGuy.

---

## Hardware

- **ESP32 CYD** (Cheap Yellow Display) — 2.8" 240×320 TFT with XPT2046 touchscreen
- The board used and tested is the **ST7789**-based CYD. Earlier boards used an ILI9341 driver — check the back of your board before ordering.

---

## Features

- Live weather from [Open-Meteo](https://open-meteo.com) — updates every 15 minutes, no subscription, no API key
- Location lookup via [Nominatim](https://nominatim.org) — city name displayed automatically from coordinates
- 8-day forecast with tap-to-page (days 1–4 or days 5–8), with option to start from today
- 6-hour rotating forecast showing precipitation probability, temperature and dew point
- Current conditions: temperature, wind speed/direction, UV index, cloud cover, humidity
- Moon phase display with correct hemisphere orientation (northern/southern)
- Sunrise/sunset times with automatic backlight dimming at dusk
- **Dual location support** — tap the BIG weather icon area to toggle between two locations
- **Automatic timezone detection** via [timeapi.io](https://timeapi.io) — no hardcoding required
- Timezone offsets self-correct at 2am daily for both locations
- Full web configuration interface — no need to edit code for normal setup
- Map-based location selection with search box (type "London UK" to jump there)
- Multilingual: English, French, German, Spanish, Dutch, Portuguese, Turkish
- Temperature and precipitation threshold colours — fully user configurable
- WiFi management via WiFiManager captive portal

---

## Touch Zones

The display has three touch-sensitive areas:

```
┌─────────────────────────┐
│   Clock / date area     │  Tap → About / splash screen
├─────────────────────────┤
│   Weather icon, temp,   │  Tap → Toggle between TZ1 and TZ2 locations
│   wind, city name       │
├─────────────────────────┤
│   MON TUE WED THU      │  Tap → Toggle forecast page (days 1-4 / days 5-8)
└─────────────────────────┘
```

---

## Web Configuration

Access the configuration page at **http://wabbitweather.local** (or the device IP address) from any browser on the same network.

### Location

Set up to two locations using the interactive map. Click or drag the pin, or type a city name in the search box. The timezone offset is detected automatically from the coordinates — no manual timezone selection needed.

<img width="640" height="426" alt="map" src="https://github.com/user-attachments/assets/c47578f7-f9ef-4088-91c7-09aed7d13437" />

TZ1 is your primary location. TZ2 is an alternate location (family in another city, a vacation property, etc.). Tap the weather icon area on the display to switch between them.

### Display Settings

<img width="640" height="512" alt="Display" src="https://github.com/user-attachments/assets/b2cc595c-fd33-4768-afa2-a0b8abb30452" />

- Imperial (°F) or Metric (°C)
- 12 or 24 hour clock
- Auto dim at dusk (uses sunrise/sunset times for the active location)
- Night brightness (0 = display off, 255 = full brightness)
- Show/hide barometric pressure, precipitation probability, UV index, 6hr forecast
- Forecast start: Today or Tomorrow
- Weather and astrology heading colours

### Temperature Threshold Colours

<img width="640" height="288" alt="Temp" src="https://github.com/user-attachments/assets/091f0a38-46af-410d-a36e-07950b6bdd9f" />

Set high and low temperature thresholds and choose colours for each range. Values are in whichever units you selected (°F or °C).

### Precipitation Threshold Colours

<img width="640" height="287" alt="Precip" src="https://github.com/user-attachments/assets/330a87c7-93ce-41f3-8e0d-4fce30f4b987" />

Set high and low precipitation probability thresholds and choose colours for each range.

---

## Partition Scheme

WabbitWeather uses a **NO OTA 2MB App / 2MB LittleFS** partition scheme to accommodate the fonts and weather icon set. Select this in the Arduino IDE under Tools → Partition Scheme before flashing.

---

## First Time Setup

1. Make your language selection first in the All_Settings.h file (see section below)
2. Flash the firmware and upload the LittleFS data folder (fonts and icons)
3. Power on the device — it will create a WiFi access point called **WabbitWeather**
4. Connect to that access point with your phone or computer
5. Select your home WiFi network and enter the password
6. The device reboots and connects to your network
7. Open **http://wabbitweather.local** in a browser (or http://xxx.xxx.xxx.xxx your routers IP)
8. Click the map to set your location — timezone is detected automatically
9. Hit **Update & Save**
10. Done — the display updates immediately with your local weather

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
