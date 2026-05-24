// WebConfig.cpp — WabbitWeather web configuration portal
// All globals (units, latitude, etc.) are visible because Arduino
// compiles all .ino/.cpp files in the sketch folder together.
// Do NOT include All_Settings.h here — it would cause duplicate definitions.

#include <WebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "WebConfig.h"
#include "Language.h"
#include "Translation.h"

// ── Web server instance ────────────────────────────────────────────────────
static WebServer server(80);

// ── Forward declarations of globals from All_Settings.h ───────────────────
extern String units;
extern String latitude;
extern String longitude;
extern bool   show24Hour;
extern bool   autoDimDusk;
extern bool   showBarometric;
extern bool   showPrecipProbability;
extern bool   showUVindex;
extern bool   show6hrForecast;
extern uint8_t  highTempVal;
extern int8_t   lowTempVal;
extern uint32_t highTempColour;
extern uint32_t midTempColour;
extern uint32_t lowTempColour;
extern uint8_t  highPrecipProb;
extern uint8_t  lowPrecipProb;
extern uint32_t highPOPColour;
extern uint32_t midPOPColour;
extern uint32_t lowPOPColour;
extern uint32_t labelColour;
extern uint32_t astrologyColour;
extern uint8_t blDusk;
extern volatile bool configUpdated;

// ===========================================================================
// Config file: /config.txt  (key=value, one per line, # = comment)
// ===========================================================================

void loadConfig() {
  if (!LittleFS.exists(CONFIG_FILE)) {
    Serial.println("WebConfig: no config file, writing defaults");
    saveConfig();
    return;
  }

  File f = LittleFS.open(CONFIG_FILE, "r");
  if (!f) { Serial.println("WebConfig: failed to open config"); return; }

  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.startsWith("#") || line.length() == 0) continue;
    int sep = line.indexOf('=');
    if (sep < 0) continue;
    String key   = line.substring(0, sep);  key.trim();
    String value = line.substring(sep + 1); value.trim();

    if      (key == "units")                 units                 = value;
    else if (key == "latitude")              latitude              = value;
    else if (key == "longitude")             longitude             = value;
    else if (key == "show24Hour")            show24Hour            = (value == "true");
    else if (key == "autoDimDusk")           autoDimDusk           = (value == "true");
    else if (key == "showBarometric")        showBarometric        = (value == "true");
    else if (key == "showPrecipProbability") showPrecipProbability = (value == "true");
    else if (key == "showUVindex")           showUVindex           = (value == "true");
    else if (key == "show6hrForecast")       show6hrForecast       = (value == "true");
    else if (key == "highTempVal")           highTempVal           = value.toInt();
    else if (key == "lowTempVal")            lowTempVal            = value.toInt();
    else if (key == "highTempColour")        highTempColour        = colourFromName(value);
    else if (key == "midTempColour")         midTempColour         = colourFromName(value);
    else if (key == "lowTempColour")         lowTempColour         = colourFromName(value);
    else if (key == "highPrecipProb")        highPrecipProb        = value.toInt();
    else if (key == "lowPrecipProb")         lowPrecipProb         = value.toInt();
    else if (key == "highPOPColour")         highPOPColour         = colourFromName(value);
    else if (key == "midPOPColour")          midPOPColour          = colourFromName(value);
    else if (key == "lowPOPColour")          lowPOPColour          = colourFromName(value);
    else if (key == "labelColour")           labelColour           = colourFromName(value);
    else if (key == "astrologyColour")       astrologyColour       = colourFromName(value);
    else if (key == "blDusk")               blDusk                = value.toInt();
  }
  f.close();
  Serial.println("WebConfig: loaded " CONFIG_FILE);
}

void saveConfig() {
  File f = LittleFS.open(CONFIG_FILE, "w");
  if (!f) { Serial.println("WebConfig: failed to save config"); return; }

  f.println("# WabbitWeather configuration — edit via http://wabbitweather.local");
  f.println("units="                 + units);
  f.println("latitude="              + latitude);
  f.println("longitude="             + longitude);
  f.println("show24Hour="            + String(show24Hour            ? "true" : "false"));
  f.println("autoDimDusk="           + String(autoDimDusk           ? "true" : "false"));
  f.println("showBarometric="        + String(showBarometric        ? "true" : "false"));
  f.println("showPrecipProbability=" + String(showPrecipProbability ? "true" : "false"));
  f.println("showUVindex="           + String(showUVindex           ? "true" : "false"));
  f.println("show6hrForecast="       + String(show6hrForecast       ? "true" : "false"));
  f.println("highTempVal="           + String(highTempVal));
  f.println("lowTempVal="            + String(lowTempVal));
  f.println("highTempColour="        + String(nameFromColour(highTempColour)));
  f.println("midTempColour="         + String(nameFromColour(midTempColour)));
  f.println("lowTempColour="         + String(nameFromColour(lowTempColour)));
  f.println("highPrecipProb="        + String(highPrecipProb));
  f.println("lowPrecipProb="         + String(lowPrecipProb));
  f.println("highPOPColour="         + String(nameFromColour(highPOPColour)));
  f.println("midPOPColour="          + String(nameFromColour(midPOPColour)));
  f.println("lowPOPColour="          + String(nameFromColour(lowPOPColour)));
  f.println("labelColour="           + String(nameFromColour(labelColour)));
  f.println("astrologyColour="       + String(nameFromColour(astrologyColour)));
  f.println("blDusk="                + String(blDusk));
  f.close();
  Serial.println("WebConfig: saved " CONFIG_FILE);
}

// ===========================================================================
// HTML helpers
// ===========================================================================

static String colourSelect(const String& name, uint32_t currentValue) {
  // Find current colour name and CSS
  String curName = "Unknown";
  String curCss  = "rgb(255,255,255)";
  for (uint8_t i = 0; i < TFT_COLOUR_COUNT; i++) {
    if (TFT_COLOURS[i].value == currentValue) {
      curName = TFT_COLOURS[i].name;
      uint32_t c = TFT_COLOURS[i].value;
      uint8_t r = ((c >> 11) & 0x1F) << 3;
      uint8_t g = ((c >>  5) & 0x3F) << 2;
      uint8_t b = ((c >>  0) & 0x1F) << 3;
      char css[24];
      snprintf(css, sizeof(css), "rgb(%d,%d,%d)", r, g, b);
      curCss = String(css);
      break;
    }
  }

  String id = "cs_" + name;

  // Hidden input carries the value for form submission
  String html = "<div class='cs' id='" + id + "'>"
                "<input type='hidden' name='" + name + "' id='" + id + "_val' value='" + curName + "'>"
                "<div class='cs-sel' onclick='csToggle(\"" + id + "\")'>"
                "<div class='cs-swatch' style='background:" + curCss + "'></div>"
                "<span id='" + id + "_lbl'>" + curName + "</span>"
                "<span style='margin-left:auto;color:var(--muted)'>&#9660;</span>"
                "</div>"
                "<div class='cs-drop' id='" + id + "_drop'>";

  for (uint8_t i = 0; i < TFT_COLOUR_COUNT; i++) {
    uint32_t c = TFT_COLOURS[i].value;
    uint8_t r = ((c >> 11) & 0x1F) << 3;
    uint8_t g = ((c >>  5) & 0x3F) << 2;
    uint8_t b = ((c >>  0) & 0x1F) << 3;
    bool sel = (TFT_COLOURS[i].value == currentValue);
    String cname = TFT_COLOURS[i].name;
    String rs = String(r), gs = String(g), bs = String(b);

    html += "<div class='cs-opt" + String(sel ? " sel" : "") + "' "
            "onclick='csSelect(\"" + id + "\",\"" + cname + "\"," +
            "&quot;rgb(" + rs + "," + gs + "," + bs + ")&quot;)'>"
            "<div class='cs-swatch' style='background:rgb(" + rs + "," + gs + "," + bs + ")'></div>"
            "<span style='color:rgb(" + rs + "," + gs + "," + bs + ");font-weight:600'>" +
            cname + "</span>"
            "</div>";
  }

  html += "</div></div>";
  return html;
}

static String toggle(const String& name, bool value, const String& label) {
  return "<label class='toggle'>"
         "<input type='checkbox' name='" + name + "'" + (value ? " checked" : "") + ">"
         "<span class='slider'></span>"
         "<span class='tl'>" + label + "</span>"
         "</label>";
}

static String section(const String& title) {
  return "<h2>" + title + "</h2>";
}

// ===========================================================================
// Full page HTML
// ===========================================================================

static String buildPage(bool saved = false) {
  String ip = WiFi.localIP().toString();

  String html = F("<!DOCTYPE html><html lang='en'><head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>WabbitWeather</title>"
    "<link rel='stylesheet' href='https://unpkg.com/leaflet@1.9.4/dist/leaflet.css'/>"
    "<script src='https://unpkg.com/leaflet@1.9.4/dist/leaflet.js'></script>"
    "<style>"
    ":root{--bg:#121212;--card:#1e1e1e;--bdr:#333;--txt:#e0e0e0;--muted:#888;"
    "--accent:#4fc3f7;--danger:#ef5350;}"
    "*{box-sizing:border-box;margin:0;padding:0}"
    "body{background:var(--bg);color:var(--txt);font-family:sans-serif;font-size:15px}"
    "header{background:var(--card);border-bottom:1px solid var(--bdr);padding:14px 20px;"
    "display:flex;align-items:center;justify-content:space-between;"
    "position:sticky;top:0;z-index:100}"
    "header h1{font-size:20px;font-weight:600;color:var(--accent);letter-spacing:1px}"
    "header .ip{font-size:11px;color:var(--muted);margin-top:2px}"
    ".wreset{background:none;border:1px solid var(--danger);color:var(--danger);"
    "border-radius:6px;padding:6px 12px;font-size:13px;cursor:pointer}"
    ".wreset:hover{background:rgba(239,83,80,0.12)}"
    "main{max-width:700px;margin:0 auto;padding:20px 16px 50px}"
    ".saved{background:#1b3a1b;border:1px solid #4caf50;color:#81c784;border-radius:8px;"
    "padding:10px 16px;margin-bottom:16px;font-size:14px}"
    "h2{font-size:12px;font-weight:600;text-transform:uppercase;letter-spacing:1px;"
    "color:var(--accent);margin:28px 0 10px;padding-bottom:6px;border-bottom:1px solid var(--bdr)}"
    ".card{background:var(--card);border:1px solid var(--bdr);border-radius:10px;"
    "padding:16px;display:flex;flex-direction:column;gap:14px}"
    ".row{display:flex;align-items:center;justify-content:space-between;gap:12px;flex-wrap:wrap}"
    ".row>label:first-child{font-size:14px;flex:1;min-width:150px}"
    "input[type=text],input[type=number]{background:#1a1a1a;border:1px solid var(--bdr);"
    "border-radius:6px;color:var(--txt);font-size:14px;padding:7px 10px;width:100%;max-width:210px}"
    "input:focus{outline:none;border-color:var(--accent)}"
    ".rg{display:flex;gap:8px}"
    ".rg label{display:flex;align-items:center;gap:6px;background:#1a1a1a;"
    "border:1px solid var(--bdr);border-radius:6px;padding:6px 14px;cursor:pointer;"
    "font-size:14px;min-width:unset;flex:unset}"
    ".rg input{accent-color:var(--accent)}"
    ".rg label:has(input:checked){border-color:var(--accent);color:var(--accent)}"
    ".toggle{display:flex;align-items:center;gap:10px;cursor:pointer;width:100%}"
    ".toggle input{display:none}"
    ".slider{width:44px;height:24px;background:#444;border-radius:12px;"
    "position:relative;flex-shrink:0;transition:background .2s}"
    ".slider::after{content:'';position:absolute;width:18px;height:18px;background:#fff;"
    "border-radius:50%;top:3px;left:3px;transition:left .2s}"
    ".toggle input:checked+.slider{background:var(--accent)}"
    ".toggle input:checked+.slider::after{left:23px}"
    ".tl{font-size:14px}"
    "#map{width:100%;height:280px;border-radius:8px;border:1px solid var(--bdr);margin-bottom:10px}"
    ".cgrid{display:grid;grid-template-columns:1fr 1fr;gap:10px}"
    ".cgrid input{max-width:100%}"
    ".actions{display:flex;gap:12px;margin-top:28px;flex-wrap:wrap}"
    ".btn{padding:10px 28px;border-radius:8px;font-size:15px;font-weight:600;"
    "cursor:pointer;border:2px solid;letter-spacing:.5px}"
    ".bu{background:var(--accent);border-color:var(--accent);color:#000}"
    ".bu:hover{opacity:.85}"
    ".bc{background:transparent;border-color:var(--muted);color:var(--muted)}"
    ".bc:hover{border-color:var(--txt);color:var(--txt)}"
    ".mbg{display:none;position:fixed;inset:0;background:rgba(0,0,0,.75);"
    "z-index:200;align-items:center;justify-content:center}"
    ".mbg.open{display:flex}"
    ".modal{background:var(--card);border:1px solid var(--danger);border-radius:12px;"
    "padding:28px 24px;max-width:340px;width:90%;text-align:center}"
    ".modal h3{color:var(--danger);margin-bottom:12px;font-size:17px}"
    ".modal p{color:var(--muted);font-size:14px;margin-bottom:20px;line-height:1.5}"
    ".mbtns{display:flex;gap:10px;justify-content:center}"
    ".mok{background:var(--danger);border:none;color:#fff;padding:9px 22px;"
    "border-radius:7px;font-size:14px;font-weight:600;cursor:pointer}"
    ".mno{background:transparent;border:1px solid var(--muted);color:var(--muted);"
    "padding:9px 22px;border-radius:7px;font-size:14px;cursor:pointer}"
    ".cs{position:relative;width:100%;max-width:210px}"
    ".cs-sel{background:#1a1a1a;border:1px solid var(--bdr);border-radius:6px;"
    "color:var(--txt);font-size:14px;padding:7px 10px;cursor:pointer;"
    "display:flex;align-items:center;gap:8px;user-select:none}"
    ".cs-sel:hover{border-color:var(--accent)}"
    ".cs-swatch{width:14px;height:14px;border-radius:3px;flex-shrink:0;border:1px solid #444}"
    ".cs-drop{display:none;position:absolute;top:100%;left:0;right:0;"
    "background:#1a1a1a;border:1px solid var(--bdr);border-radius:6px;"
    "max-height:200px;overflow-y:auto;z-index:999;margin-top:4px}"
    ".cs-drop.open{display:block}"
    ".cs-opt{display:flex;align-items:center;gap:8px;padding:7px 10px;"
    "cursor:pointer;font-size:14px}"
    ".cs-opt:hover{background:#2a2a2a}"
    ".cs-opt.sel{background:#0f3460}"
    "@media(max-width:480px){.row{flex-direction:column;align-items:flex-start}"
    "input[type=text],input[type=number]{max-width:100%}.cs{max-width:100%}}"
    "</style></head><body>");

  // Header
  html += "<header><div><h1>&#9928; " + String(wcTitle) + "</h1>"
          "<div class='ip'>" + ip + " &nbsp;|&nbsp; wabbitweather.local</div></div>"
          "<button class='wreset' onclick='document.getElementById(\"wm\").classList.add(\"open\")'>"
          + String(wcWifiResetBtn) + "</button></header>";

  // WiFi reset modal
  html += "<div class='mbg' id='wm'><div class='modal'>"
          "<h3>" + String(wcWifiResetTitle) + "</h3>"
          "<p>" + String(wcWifiResetBody) + "</p>"
          "<div class='mbtns'>"
          "<button class='mok' onclick='window.location=\"/wifireset\"'>" + String(wcWifiResetYes) + "</button>"
          "<button class='mno' onclick='document.getElementById(\"wm\").classList.remove(\"open\")'>" + String(wcWifiResetCancel) + "</button>"
          "</div></div></div>";

  html += "<main>";

  if (saved) html += "<div class='saved'>" + String(wcSaved) + "</div>";

  html += "<form method='POST' action='/save'>";

  // ── Location ──────────────────────────────────────────────────────────────
  html += section(wcSecLocation);
  html += "<div class='card'>"
          "<div id='map'></div>"
          "<div class='cgrid'>"
          "<div><label style='display:block;font-size:12px;color:var(--muted);margin-bottom:4px'>" + String(wcLatLabel) + "</label>"
          "<input type='text' id='lat' name='latitude' value='" + latitude + "'></div>"
          "<div><label style='display:block;font-size:12px;color:var(--muted);margin-bottom:4px'>" + String(wcLonLabel) + "</label>"
          "<input type='text' id='lon' name='longitude' value='" + longitude + "'></div>"
          "</div></div>";

  // ── Display ───────────────────────────────────────────────────────────────
  html += section(wcSecDisplay);
  html += "<div class='card'>";
  html += "<div class='row'><label>" + String(wcUnitsLabel) + "</label>"
          "<div class='rg'>"
          "<label><input type='radio' name='units' value='imperial'" + String(units=="imperial"?" checked":"") + "> " + String(wcImperial) + "</label>"
          "<label><input type='radio' name='units' value='metric'"   + String(units=="metric"  ?" checked":"") + "> " + String(wcMetric)   + "</label>"
          "</div></div>";
  html += "<div class='row'>" + toggle("show24Hour",           show24Hour,           wcClock24)    + "</div>";
  html += "<div class='row'>" + toggle("autoDimDusk",          autoDimDusk,          wcAutoDim)    + "</div>";
  html += "<div class='row'><label>" + String(wcNightBright) + "</label>"
          "<div>"
          "<input type='number' name='blDusk' value='" + String(blDusk) + "' min='0' max='255' style='max-width:90px'>"
          "<div style='font-size:11px;color:var(--muted);margin-top:4px'>" + String(wcNightBrightHint) + "</div>"
          "</div></div>";
  html += "<div class='row'>" + toggle("showBarometric",       showBarometric,       wcShowBaro)   + "</div>";
  html += "<div class='row'>" + toggle("showPrecipProbability",showPrecipProbability,wcShowPrecip) + "</div>";
  html += "<div class='row'>" + toggle("showUVindex",          showUVindex,          wcShowUV)     + "</div>";
  html += "<div class='row'>" + toggle("show6hrForecast",      show6hrForecast,      wcShow6Hr)    + "</div>";
  html += "<div class='row'><label>" + String(wcWeatherHeadings) + "</label>" + colourSelect("labelColour",     labelColour)     + "</div>";
  html += "<div class='row'><label>" + String(wcAstroHeadings)   + "</label>" + colourSelect("astrologyColour", astrologyColour) + "</div>";
  html += "</div>";

  // ── Temperature colours ───────────────────────────────────────────────────
  html += section(wcSecTempColours);
  html += "<div class='card'>";
  html += "<div class='row'><label>" + String(wcHighTempThresh) + "</label>"
          "<input type='number' name='highTempVal' value='" + String(highTempVal) + "' min='-50' max='150' style='max-width:90px'></div>";
  html += "<div class='row'><label>" + String(wcLowTempThresh)  + "</label>"
          "<input type='number' name='lowTempVal'  value='" + String(lowTempVal)  + "' min='-50' max='150' style='max-width:90px'></div>";
  html += "<div class='row'><label>" + String(wcHighTempColour) + "</label>" + colourSelect("highTempColour", highTempColour) + "</div>";
  html += "<div class='row'><label>" + String(wcMidTempColour)  + "</label>" + colourSelect("midTempColour",  midTempColour)  + "</div>";
  html += "<div class='row'><label>" + String(wcLowTempColour)  + "</label>" + colourSelect("lowTempColour",  lowTempColour)  + "</div>";
  html += "</div>";

  // ── Precipitation colours ─────────────────────────────────────────────────
  html += section(wcSecPrecipColours);
  html += "<div class='card'>";
  html += "<div class='row'><label>" + String(wcHighPrecipThresh) + "</label>"
          "<input type='number' name='highPrecipProb' value='" + String(highPrecipProb) + "' min='0' max='100' style='max-width:90px'></div>";
  html += "<div class='row'><label>" + String(wcLowPrecipThresh)  + "</label>"
          "<input type='number' name='lowPrecipProb'  value='" + String(lowPrecipProb)  + "' min='0' max='100' style='max-width:90px'></div>";
  html += "<div class='row'><label>" + String(wcHighPOPColour) + "</label>" + colourSelect("highPOPColour", highPOPColour) + "</div>";
  html += "<div class='row'><label>" + String(wcMidPOPColour)  + "</label>" + colourSelect("midPOPColour",  midPOPColour)  + "</div>";
  html += "<div class='row'><label>" + String(wcLowPOPColour)  + "</label>" + colourSelect("lowPOPColour",  lowPOPColour)  + "</div>";
  html += "</div>";

  // ── Actions ───────────────────────────────────────────────────────────────
  html += "<div class='actions'>"
          "<button type='submit' class='btn bu'>" + String(wcSaveBtn)   + "</button>"
          "<button type='button' class='btn bc' onclick='window.location.reload()'>" + String(wcCancelBtn) + "</button>"
          "</div>";

  html += "</form>";

  // ── Map script ────────────────────────────────────────────────────────────
 html += "<script>"
          "var lt=parseFloat(document.getElementById('lat').value)||49.154;"
          "var ln=parseFloat(document.getElementById('lon').value)||-122.772;"
          "var map=L.map('map').setView([lt,ln],11);"
          "L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',{"
          "attribution:'&copy; OpenStreetMap contributors',maxZoom:18}).addTo(map);"
          "var mk=L.marker([lt,ln],{draggable:true}).addTo(map);"
          "function uc(a,b){b=((b+180)%360+360)%360-180;"
          "document.getElementById('lat').value=a.toFixed(4);"
          "document.getElementById('lon').value=b.toFixed(4);}"
          "mk.on('dragend',function(e){var p=mk.getLatLng();uc(p.lat,p.lng);});"
          "map.on('click',function(e){mk.setLatLng(e.latlng);uc(e.latlng.lat,e.latlng.lng);});"
          "document.getElementById('lat').addEventListener('change',function(){"
          "var a=parseFloat(this.value),b=parseFloat(document.getElementById('lon').value);"
          "if(!isNaN(a)&&!isNaN(b)){mk.setLatLng([a,b]);map.setView([a,b]);}});"
          "document.getElementById('lon').addEventListener('change',function(){"
          "var a=parseFloat(document.getElementById('lat').value),b=parseFloat(this.value);"
          "if(!isNaN(a)&&!isNaN(b)){mk.setLatLng([a,b]);map.setView([a,b]);}});"
          "function csToggle(id){"
          "var d=document.getElementById(id+'_drop');"
          "d.classList.toggle('open');"
          "document.querySelectorAll('.cs-drop').forEach(function(x){"
          "if(x.id!==id+'_drop')x.classList.remove('open');});"
          "}"
          "function csSelect(id,name,css){"
          "document.getElementById(id+'_val').value=name;"
          "document.getElementById(id+'_lbl').textContent=name;"
          "document.querySelector('#'+id+' .cs-swatch').style.background=css;"
          "document.getElementById(id+'_drop').classList.remove('open');"
          "document.querySelectorAll('#'+id+' .cs-opt').forEach(function(x){"
          "x.classList.remove('sel');"
          "if(x.querySelector('span:last-child').textContent===name)x.classList.add('sel');});"
          "}"
          "document.addEventListener('click',function(e){"
          "if(!e.target.closest('.cs')){"
          "document.querySelectorAll('.cs-drop').forEach(function(x){"
          "x.classList.remove('open');});}});"
          "</script>";
          
  html += "</main></body></html>";
  return html;
}

// ===========================================================================
// Request handlers
// ===========================================================================

static void handleRoot() {
  bool saved = server.hasArg("saved");
  server.send(200, "text/html", buildPage(saved));
}

static void handleSave() {
  if (server.hasArg("units"))               units                 = server.arg("units");
  if (server.hasArg("latitude"))            latitude              = server.arg("latitude");
  if (server.hasArg("longitude"))           longitude             = server.arg("longitude");

  show24Hour            = server.hasArg("show24Hour");
  autoDimDusk           = server.hasArg("autoDimDusk");
  showBarometric        = server.hasArg("showBarometric");
  showPrecipProbability = server.hasArg("showPrecipProbability");
  showUVindex           = server.hasArg("showUVindex");
  show6hrForecast       = server.hasArg("show6hrForecast");

  if (server.hasArg("highTempVal"))         highTempVal           = server.arg("highTempVal").toInt();
  if (server.hasArg("lowTempVal"))          lowTempVal            = server.arg("lowTempVal").toInt();
  if (server.hasArg("highTempColour"))      highTempColour        = colourFromName(server.arg("highTempColour"));
  if (server.hasArg("midTempColour"))       midTempColour         = colourFromName(server.arg("midTempColour"));
  if (server.hasArg("lowTempColour"))       lowTempColour         = colourFromName(server.arg("lowTempColour"));
  if (server.hasArg("highPrecipProb"))      highPrecipProb        = server.arg("highPrecipProb").toInt();
  if (server.hasArg("lowPrecipProb"))       lowPrecipProb         = server.arg("lowPrecipProb").toInt();
  if (server.hasArg("highPOPColour"))       highPOPColour         = colourFromName(server.arg("highPOPColour"));
  if (server.hasArg("midPOPColour"))        midPOPColour          = colourFromName(server.arg("midPOPColour"));
  if (server.hasArg("lowPOPColour"))        lowPOPColour          = colourFromName(server.arg("lowPOPColour"));
  if (server.hasArg("labelColour"))         labelColour           = colourFromName(server.arg("labelColour"));
  if (server.hasArg("astrologyColour"))     astrologyColour       = colourFromName(server.arg("astrologyColour"));
  if (server.hasArg("blDusk"))              blDusk                = server.arg("blDusk").toInt();

  saveConfig();
  configUpdated = true;

  server.sendHeader("Location", "/?saved=1");
  server.send(303);
}

static void handleWifiReset() {
  server.send(200, "text/html",
    "<html><body style='background:#121212;color:#e0e0e0;font-family:sans-serif;"
    "display:flex;align-items:center;justify-content:center;height:100vh'>"
    "<div style='text-align:center'><h2 style='color:#ef5350'>"
    + String(wcWifiResetting) +
    "</h2><p>" + String(wcWifiResetHotspot) + "</p>"
    "</div></body></html>");
  delay(1000);
  WiFiManager wm;
  wm.resetSettings();
  ESP.restart();
}

static void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

// ===========================================================================
// Public functions
// ===========================================================================

void startWebConfig() {
  if (MDNS.begin("wabbitweather")) {
    Serial.println("WebConfig: mDNS started — http://wabbitweather.local");
  } else {
    Serial.println("WebConfig: mDNS failed — use IP");
  }
  server.on("/",          HTTP_GET,  handleRoot);
  server.on("/save",      HTTP_POST, handleSave);
  server.on("/wifireset", HTTP_GET,  handleWifiReset);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.print("WebConfig: running at http://");
  Serial.println(WiFi.localIP());
}

void handleWebConfig() {
  server.handleClient();
}
