#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#define LED_PIN 5
#define NUM_LEDS 8
#define TOUCH_PIN 15
Adafruit_NeoPixel ring(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Preferences prefs;
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
WebServer server(80);
bool ledState = true;
int currentR=255,currentG=255,currentB=255;
int targetR=255,targetG=255,targetB=255;
int currentBrightness=150;
int targetBrightness=150;
bool timerActive=false;
unsigned long timerStartMillis=0;
unsigned long timerDuration=0;
bool lastTouch=LOW;
void saveState()
{
 prefs.begin("led", false);
 prefs.putBool("state", ledState);
 prefs.putInt("r", targetR);
 prefs.putInt("g", targetG);
 prefs.putInt("b", targetB);
 prefs.putInt("bright", targetBrightness);
 prefs.end();
}
void loadState()
{
 prefs.begin("led", true);
 ledState = prefs.getBool("state", true);
 targetR = prefs.getInt("r",255);
 targetG = prefs.getInt("g",255);
 targetB = prefs.getInt("b",255);
 targetBrightness = prefs.getInt("bright",150);
 prefs.end();
 currentR=targetR;
 currentG=targetG;
 currentB=targetB;
 currentBrightness=targetBrightness;
}
void updateLED()
{
 ring.setBrightness(currentBrightness);
 if(ledState)
 {
  for(int i=0;i<NUM_LEDS;i++)
   ring.setPixelColor(i, ring.Color(currentR,currentG,currentB));
 }
 else
 {
  ring.clear();
 }
 ring.show();
}
void smoothColorUpdate()
{
 bool changed=false;
 if(currentR<targetR){currentR++; changed=true;}
 if(currentR>targetR){currentR--; changed=true;}
 if(currentG<targetG){currentG++; changed=true;}
 if(currentG>targetG){currentG--; changed=true;}
 if(currentB<targetB){currentB++; changed=true;}
 if(currentB>targetB){currentB--; changed=true;}
 if(changed) updateLED();
}
String getCountdown()
{
 if(!timerActive) return "No active timer";
 unsigned long remaining =
 timerDuration - (millis() - timerStartMillis);
 if((long)remaining <= 0)
  return "00:00:00";
 int totalSeconds = remaining / 1000;
 int h = totalSeconds / 3600;
 int m = (totalSeconds % 3600) / 60;
 int s = totalSeconds % 60;
 char buffer[20];
 sprintf(buffer,"%02d:%02d:%02d",h,m,s);
 return String(buffer);
}
String webpage()
{
return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width">
<style>
body{
background:#111;
color:white;
font-family:Arial;
text-align:center;
}
button{
padding:15px;
font-size:18px;
margin:5px;
}
#countdown{
font-size:28px;
color:#00ffaa;
}
</style>
</head>
<body>
<h2>ESP32 RGB Smart Controller</h2>
<button onclick="fetch('/on')">ON</button>
<button onclick="fetch('/off')">OFF</button>
<br><br>
Brightness<br>
<input type="range"
min="5"
max="255"
value="150"
oninput="fetch('/brightness?value='+this.value)">
<br><br>
Color<br>
<input type="color"
oninput="fetch('/color?value='+this.value.substring(1))">
<br><br>
Timer<br>
<input id="h" type="number" placeholder="Hours">
<input id="m" type="number" placeholder="Minutes">
<input id="s" type="number" placeholder="Seconds">
<br><br>
<button onclick="startTimer()">Start Timer</button>
<h3>Countdown</h3>
<div id="countdown">Loading...</div>
<script>
function startTimer()
{
let h=document.getElementById('h').value||0;
let m=document.getElementById('m').value||0;
let s=document.getElementById('s').value||0;
fetch(`/timer?h=${h}&m=${m}&s=${s}`);
}
setInterval(()=>{
fetch('/countdown')
.then(res=>res.text())
.then(data=>{
document.getElementById('countdown').innerHTML=data;
});
},1000);
</script>
</body>
</html>
)rawliteral";
}
void handleRoot()
{
 server.send(200,"text/html",webpage());
}
void handleON()
{
 ledState=true;
 updateLED();
 saveState();
 server.send(200,"text/plain","OK");
}
void handleOFF()
{
 ledState=false;
 timerActive=false;
 updateLED();
 saveState();
 server.send(200,"text/plain","OK");
}
void handleBrightness()
{
 targetBrightness = server.arg("value").toInt();
 currentBrightness = targetBrightness;
 updateLED();
 saveState();
 server.send(200,"text/plain","OK");
}
void handleColor()
{
 String hex=server.arg("value");
 targetR=strtol(hex.substring(0,2).c_str(),NULL,16);
 targetG=strtol(hex.substring(2,4).c_str(),NULL,16);
 targetB=strtol(hex.substring(4,6).c_str(),NULL,16);
 saveState();
 server.send(200,"text/plain","OK");
}
void handleTimer()
{
 int h=server.arg("h").toInt();
 int m=server.arg("m").toInt();
 int s=server.arg("s").toInt();
 timerDuration=((unsigned long)h*3600+
                (unsigned long)m*60+
                (unsigned long)s)*1000UL;
 timerStartMillis=millis();
 timerActive=true;
 ledState=true;
 updateLED();
 server.send(200,"text/plain","OK");
}
void handleCountdown()
{
 server.send(200,"text/plain",getCountdown());
}
void setup()
{
 Serial.begin(115200);
 pinMode(TOUCH_PIN,INPUT);
 ring.begin();
 loadState();
 updateLED();
 WiFi.begin(ssid,password);
 while(WiFi.status()!=WL_CONNECTED)
  delay(500);
 Serial.println(WiFi.localIP());
 server.on("/",handleRoot);
 server.on("/on",handleON);
 server.on("/off",handleOFF);
 server.on("/brightness",handleBrightness);
 server.on("/color",handleColor);
 server.on("/timer",handleTimer);
 server.on("/countdown",handleCountdown);
 server.begin();
}
void loop()
{
 server.handleClient();
 smoothColorUpdate();
 if(timerActive && millis()-timerStartMillis>=timerDuration)
 {
  ledState=false;
  timerActive=false;
  updateLED();
  saveState();
 }
 bool touch=digitalRead(TOUCH_PIN);
 if(touch==HIGH && lastTouch==LOW)
 {
  ledState=!ledState;
  updateLED();
  saveState();
  delay(250);
 }
 lastTouch=touch;
}