# ESP32 2.8" CYD Open-Meteo weather client

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
* Web interface makes user adjustments easily done including wifi reset so it can be a present to someone
* Use Nominatim for location when you pin the map (long/lat)
* Weather icons are from the AURA repository on my GitHub site.
* Added multilingual support for English, French, German, Dutch, Turkish, Spanish and Portuguese
* Updated the moonphase for south of equator
* Added a version number for more accurate tracking of updates
* Fixed the User_Setup.h file for the ST7789 driver
* Web Interface now shows colour swatches plus names for selections

<i>Keep in mind that some of the translations are lengthy and there isn't a lot of TFT real estate to play with on a 2.8" screen.</i>

<h3>Current Version is 1.3</h3>

AllSettings.h has some default user options if you want to set them in advance. Or, easier, just use a web browser after you select your WiFi network.

Requires the JSON parse library here:
https://github.com/Bodmer/JSON_Decoder

Requires the TFT_eSPI library here:
https://github.com/Bodmer/TFT_eSPI

<h2>Arduino Steps:</h2>

<ol>
  <li>Download zip archive and unzip in your folder/directory of choice</li>
  <li>Look in the TFT_eSPI folder to get the User_Setup.h file for the TFT_eSPI library</li>
  <li>User_Setup.h is for ILI9341_2, User_Setup_ST7789 if needed rename to User_Setup.h</li>
  <li>Run the Arduino IDE and open WabbitWeather.ino</li>
  <li>Update any libraries that you are missing in the IDE</li>
  <li>Edit NPT_Time.h to select your TimeZone the time.</li>
  <li>Edit Language.h to select your language</li>
  <li>ESP32 Dev Module - NO OTA (2MB APP/2MB SPIFFS)</li>
  <li>Select port your CYD is connected to, ensure you do NOT have Serial Port enabled for next step</li>
  <li>Ctrl-shift-P or CMD-shift-P "Upload LittleFS..." (this will upload all data folder contents)</li>
  <li>Compile, upload</li>
  <li>Enjoy!</li>
</ol>


Latest screen grabs:

<img width="199" height="350" alt="WabbitWeather_One" src="https://github.com/user-attachments/assets/81bbd92a-b1bf-4342-a476-1c112ba051c6" />

<img width="202" height="350" alt="WabbitWeather_Two" src="https://github.com/user-attachments/assets/21aead63-3476-49a4-ad0f-6fc7adfcf599" />

</br>
<img width="282" height="696" alt="WabbitWeather_SS" src="https://github.com/user-attachments/assets/a7ca682d-38ec-48a5-82ea-de00100a46ba" />



