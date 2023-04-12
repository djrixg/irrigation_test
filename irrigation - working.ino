#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <DS3231.h>

#define watertank D0
#define fertilizertank D1
#define waterpump D2
#define air D3
#define glow D4
#define buzzer D5
#define level_pin D6
int empty = 1;//kung naay sulod ang tank, 1 ang value. kung walay sulod, zero. pwede ichange if bali ang mugawas.
int irrigate_duration = 10000;
int fertilize_duration = 10000;
int fogging_duration = 20000;
const char* ssid = "ESP8266";
const char* password = "12345678";

unsigned long watertank_time = 0;
int watertank_state = 0;
unsigned long fertilizertank_time = 0;
int fertilizertank_state = 0;
unsigned long waterpump_time = 0;
int waterpump_state = 0;
unsigned long air_time = 0;
int air_state = 0;
unsigned long glow_time = 0;
int glow_state = 0;
unsigned long buzzer_time = 0;
unsigned long current_time = 0;
int water_level = 1;

ESP8266WebServer server(80);
RTClib myRTC;
DateTime now = myRTC.now();
String time_now;
void setup() {
  Wire.begin();
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  pinMode(watertank, OUTPUT);
  pinMode(fertilizertank, OUTPUT);
  pinMode(waterpump, OUTPUT);
  pinMode(air, OUTPUT);
  pinMode(glow, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(level_pin, INPUT);
  digitalWrite(watertank, LOW);
  digitalWrite(fertilizertank, LOW);
  digitalWrite(waterpump, LOW);
  digitalWrite(air, LOW);
  digitalWrite(glow, LOW);
  digitalWrite(buzzer, LOW);
  Serial.begin(115200);
  server.on("/", handleRoot);
  server.on("/irrigate", irrigate);
  server.on("/fogging", fogging);
  server.on("/fertilize", fertilize);
  server.begin();
  
  Serial.println();
  Serial.print("time_now: ");
  time_now = get_date();
  Serial.println(time_now);
}
unsigned long view_time = millis();
void loop() {
  server.handleClient();
  current_time = millis();
  if(current_time-view_time>60000){
    Serial.print("time_now: ");
    time_now = get_date();
    Serial.println(time_now);
    view_time = current_time;
  }
  if(watertank_time<current_time){
    digitalWrite(watertank,LOW);
    if(watertank_state == 1){
      Serial.println("watertank off");
      watertank_state = 0;
    }
  }
  if(fertilizertank_time<current_time){
    digitalWrite(fertilizertank,LOW);
    if(fertilizertank_state == 1){
      Serial.println("fertilizertank off");
      fertilizertank_state = 0;
    }
  }
  if(waterpump_time<current_time){
    digitalWrite(waterpump,LOW);
    if(waterpump_state == 1){
      Serial.println("waterpump off");
      waterpump_state = 0;
    }
  }
  if(air_time<current_time){
    digitalWrite(air,LOW);
    if(air_state == 1){
      Serial.println("air off");
      air_state = 0;
    }
  }
  if(glow_time<current_time){
    digitalWrite(glow,LOW);
    if(glow_state == 1){
      Serial.println("glow off");
      glow_state = 0;
    }
  }
  if(fertilizertank_time<current_time){
    digitalWrite(fertilizertank,LOW);
    if(fertilizertank_state == 1){
      Serial.println("fertilizertank off");
      fertilizertank_state = 0;
    }
  }
  if(digitalRead(level_pin)==empty){
    water_level = 0;
  }else{
    water_level = 1;
  }
  if(water_level==1){
    digitalWrite(buzzer,HIGH);
  }else{
    digitalWrite(buzzer,LOW);
  }
}

String get_date() {
  String date_str = "";
  now = myRTC.now();
  date_str += String(now.year(), DEC);
  date_str += '/';
  if (now.month() < 10) {
    date_str += '0';
  }
  date_str += String(now.month(), DEC);
  date_str += '/';
  if (now.day() < 10) {
    date_str += '0';
  }
  date_str += String(now.day(), DEC);
  date_str += ' ';
  if (now.hour() < 10) {
    date_str += '0';
  }
  date_str += String(now.hour(), DEC);
  date_str += ':';
  if (now.minute() < 10) {
    date_str += '0';
  }
  date_str += String(now.minute(), DEC);
  return date_str;
}

void handleRoot() {
  String html = R"(<html><head><style>
.btn,body{line-height:1.5}.btn,.btn-primary{color:#fff;background-color:#007bff}body{font-family:"Helvetica Neue",Helvetica,Arial,sans-serif;font-size:16px;color:#333}h1,h2,h3,h4,h5,h6{font-weight:500;line-height:1.2;margin-top:20px;margin-bottom:10px}h1{font-size:2.5rem}h2{font-size:2rem}h3{font-size:1.75rem}h4{font-size:1.5rem}h5{font-size:1.25rem}.btn,h6{font-size:1rem}p{margin-top:0;margin-bottom:1rem}.btn{display:inline-block;font-weight:400;text-align:center;vertical-align:middle;-webkit-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none;border:1px solid #007bff;padding:.5rem 1rem;border-radius:.25rem;transition:color .15s ease-in-out,background-color .15s ease-in-out,border-color .15s ease-in-out,box-shadow .15s ease-in-out}.btn-primary:hover,.btn:hover{color:#fff;background-color:#0069d9;border-color:#0062cc}.btn-primary.focus,.btn-primary:focus,.btn.focus,.btn:focus{box-shadow:0 0 0 .2rem rgba(0,123,255,.5)}.btn-primary{border-color:#007bff}.btn.disabled,.btn:disabled {opacity: 0.65;pointer-events: none;}
  </style>
  <meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Smart Irrigation</title></head><body><center>)";
  html += "<h1>Smart Irrigation</h1>";
  html += "<h2>Water Tank</h2>";
  html += R"(<button class='btn btn-primary' onclick="disableButton(this, )"+String(irrigate_duration)+R"();ajax('irrigate');">Irrigate</button><br>)";
  html += R"(<button class='btn btn-primary' onclick="disableButton(this, )"+String(fertilize_duration)+R"();ajax('fertilize');">Fertilize</button><br>)";
  html += R"(<button class='btn btn-primary' onclick="disableButton(this, )"+String(fogging_duration)+R"();ajax('fogging');">Fogging</button>)";
  html += R"(<script type="text/javascript">
      function ajax(link){
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            console.log(this.responseText);
          }
        };
        xhttp.open("GET", link, true);
        xhttp.send();
      }
      function disableButton(button, duration) {
        var temp_btn = button.innerHTML;
        var count = duration/1000;
        button.disabled = true;
        button.classList.add("disabled");
        button.innerHTML = count;
        var timer = setInterval(function() {
          count--;
          button.innerHTML = count;
          if (count == 0) {
            button.disabled = false;
            button.classList.remove("disabled");
            button.innerHTML = temp_btn;
            clearInterval(timer);
          }
        }, 1000);
      }
    </script></center></body></html>)";
  server.send(200, "text/html", html);
}

void homepage() {
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}
void irrigate() {//watertank fertilizertank waterpump air glow buzzer
  unsigned long temp_time = millis();
  digitalWrite(watertank, HIGH);
  watertank_state=1;
  Serial.println("watertank on");
  delay(1000);
  digitalWrite(waterpump, HIGH);
  waterpump_state=1;
  Serial.println("waterpump on");
  watertank_time = temp_time+irrigate_duration;
  waterpump_time = temp_time+irrigate_duration;
  server.send(200, "text/plain", "irrigate");// homepage();
}

void fertilize() {//watertank fertilizertank waterpump air glow buzzer
  unsigned long temp_time = millis();
  digitalWrite(fertilizertank, HIGH);
  fertilizertank_state=1;
  Serial.println("fertilizertank on");
  delay(1000);
  digitalWrite(waterpump, HIGH);
  waterpump_state=1;
  Serial.println("waterpump on");
  fertilizertank_time = temp_time+fertilize_duration;
  waterpump_time = temp_time+fertilize_duration;
  server.send(200, "text/plain", "fertilize");// homepage();
}
void fogging() {//watertank fertilizertank waterpump air glow  buzzer
  unsigned long temp_time = millis();
  digitalWrite(glow, HIGH);
  glow_state=1;
  Serial.println("glow on");
  delay(10000);
  digitalWrite(air, HIGH);
  air_state=1;
  Serial.println("air on");
  glow_time = temp_time+fogging_duration;
  air_time = temp_time+fogging_duration;
  server.send(200, "text/plain", "fogging");// 
}