# ESP8266 and ESP32 Open-Meteo weather client

Arduino client library for https://open-Meteo

Collects current weather plus daily forecasts.

* Added signal strength for ESP32 - top right corner of display
* Added current UV index 0 - 9 bar graph under signal strength
* Redid baromatric pressure reading as an option (really needs a history to show rising or falling to be useful)
* Added ESP32 captive portal for setup (removed router network and password from settings)
* Default HOSTNAME is WabbitWeather for captive portal
* Updates NTP once per hour
* Updates Open-Meteo every 15 minutes
* Added POP (probility of precipitation) to future forecast
* Auto dim display at dusk to any brightness you want (including OFF)
* Custom user selected coloured thresholds for POP% and daily temperatures
* Expanded date for last weather update info
* Fixed 12hr midnight clock (reads 12:xx now)...
* Bottom of display rotates between Sunrise/set-Moon-Cloud-Humidity to POP%-Temp-Dew 6hr forecast
  Display rotates every 15 seconds
* Web interface makes user adjustments easily done
* Use Nominatim for location when you pin the map (long/lat)
* Web interface includes WiFi reset so you can gift this to someone already preset
* Weather icons are from the AURA repository on my GitHub site.

AllSettings.h has default user options if you want to set them in advance.

Requires the JSON parse library here:
https://github.com/Bodmer/JSON_Decoder

Requires the TFT_eSPI library here:
https://github.com/Bodmer/TFT_eSPI

The TFT_eSPI_Weather example works with the ESP32 CYD, it displays the weather data on 2.8" TFT screen.  These examples use anti-aliased fonts and newly icons from the AURA project.

Latest screen grabs:

<img width="199" height="350" alt="WabbitWeather_One" src="https://github.com/user-attachments/assets/81bbd92a-b1bf-4342-a476-1c112ba051c6" />

<img width="202" height="350" alt="WabbitWeather_Two" src="https://github.com/user-attachments/assets/21aead63-3476-49a4-ad0f-6fc7adfcf599" />

</br>
<img width="460" height="1024" alt="Wabbit_Weather Web_Interface" src="https://github.com/user-attachments/assets/8dacf061-aa40-4541-8f0a-bf70c9382262" />


