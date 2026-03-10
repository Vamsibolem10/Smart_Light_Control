#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "Vamsi";
const char* password = "Vamsi@3791";

WebServer server(80);

#define RELAY_PIN 23

bool lightState = false;
bool timerActive = false;

unsigned long timerStartMillis = 0;
unsigned long timerDurationMillis = 0;

String html = R"rawliteral(

<!DOCTYPE html>
<html>
<head>
<title>Smart Light Control</title>
<style>
body{
font-family:Arial;
text-align:center;
margin-top:50px;
}
button{
padding:15px;
margin:10px;
font-size:20px;
}
input{
padding:10px;
margin:5px;
font-size:18px;
width:80px;
}
</style>
</head>

<body>

<h2>ESP32 Smart Light</h2>

<button onclick="turnOn()">ON</button>
<button onclick="turnOff()">OFF</button>

<h3>Timer</h3>

<input id="h" placeholder="Hours" type="number">
<input id="m" placeholder="Minutes" type="number">
<input id="s" placeholder="Seconds" type="number">

<br>

<button onclick="startTimer()">Start Timer</button>

<h3 id="status">Status: OFF</h3>

<script>

function turnOn(){
fetch("/on",{method:"POST"});
}

function turnOff(){
fetch("/off",{method:"POST"});
}

function startTimer(){

let h=parseInt(document.getElementById("h").value)||0;
let m=parseInt(document.getElementById("m").value)||0;
let s=parseInt(document.getElementById("s").value)||0;

fetch("/setDuration",
{
method:"POST",
headers:{"Content-Type":"application/json"},
body:JSON.stringify({
hours:h,
minutes:m,
seconds:s
})
});

}

setInterval(async()=>{
let res=await fetch("/status");
let data=await res.json();

document.getElementById("status").innerHTML=
"Status: "+(data.light?"ON":"OFF")+
"<br>Remaining: "+data.remaining+" sec";

},1000);

</script>

</body>
</html>

)rawliteral";


void turnON(){
digitalWrite(RELAY_PIN,LOW);
lightState=true;
}

void turnOFF(){
digitalWrite(RELAY_PIN,HIGH);
lightState=false;
timerActive=false;
}

void handleRoot(){
server.send(200,"text/html",html);
}

void handleON(){
turnON();
server.send(200,"application/json","{\"status\":\"on\"}");
}

void handleOFF(){
turnOFF();
server.send(200,"application/json","{\"status\":\"off\"}");
}

void handleSetDuration(){

String body=server.arg("plain");

StaticJsonDocument<200> doc;
deserializeJson(doc,body);

int h=doc["hours"];
int m=doc["minutes"];
int s=doc["seconds"];

timerDurationMillis=
((h*3600UL)+(m*60UL)+s)*1000UL;

if(timerDurationMillis>0){
turnON();
timerStartMillis=millis();
timerActive=true;
}

server.send(200,"application/json","{\"status\":\"timer_started\"}");
}

void handleStatus(){

StaticJsonDocument<200> doc;

doc["light"]=lightState;

if(timerActive){

unsigned long elapsed=millis()-timerStartMillis;

if(elapsed<timerDurationMillis)
doc["remaining"]=(timerDurationMillis-elapsed)/1000;
else
doc["remaining"]=0;

}
else
doc["remaining"]=0;

String json;
serializeJson(doc,json);

server.send(200,"application/json",json);
}

void setup(){

Serial.begin(115200);

pinMode(RELAY_PIN,OUTPUT);
turnOFF();

WiFi.begin(ssid,password);

Serial.print("Connecting");

while(WiFi.status()!=WL_CONNECTED){
delay(500);
Serial.print(".");
}

Serial.println();
Serial.println(WiFi.localIP());

server.on("/",handleRoot);

server.on("/on",HTTP_POST,handleON);

server.on("/off",HTTP_POST,handleOFF);

server.on("/setDuration",HTTP_POST,handleSetDuration);

server.on("/status",HTTP_GET,handleStatus);

server.begin();

Serial.println("Server started");

}

void loop(){

server.handleClient();

if(timerActive){

if(millis()-timerStartMillis>=timerDurationMillis){

turnOFF();

}

}

}
