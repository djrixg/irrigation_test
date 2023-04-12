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
void handleRoot() {
  String html = R"(<html><head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Smart Irrigation</title>)";
  html+=R"(</head><body><center>
  <h1>Smart Irrigation</h1>
  <h2>Water Tank</h2>
  <div class="container">
    <div class="row row-cols-1 row-cols-md-4 g-4">
      <div class="col">
        <div class="card">
          <div class="card-body">
            <h5 class="card-title">Irrigate</h5>
            <p class="card-text">)";

      html+=R"(<button class='btn btn-primary' onclick="disableButton(this)"+String(fogging_duration)+R"();ajax('irrigate');">Manual Start</button>)";
      html+=R"(<label for="irrigate_1" class="form-label">Schedule 1</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="irrigate_1" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <label for="irrigate_2" class="form-label">Schedule 2</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="irrigate_2" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <button class='btn btn-outline-primary' onclick="schedule_this('irrigate')\">Save</button>
            </p>
          </div>
        </div>
      </div>
      <div class="col">
        <div class="card">
          <div class="card-body">
            <h5 class="card-title">Fertilize</h5>
            <p class="card-text">)";

      html+=R"(<button class='btn btn-primary' onclick="disableButton(this, )"+String(fogging_duration)+R"();ajax('fertilize');">Manual Start</button>)";
      html+=R"(<label for="fertilize_1" class="form-label">Schedule 1</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="fertilize_1" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <label for="fertilize_2" class="form-label">Schedule 2</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="fertilize_2" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <button class='btn btn-outline-primary' onclick="schedule_this('fertilize')\">Save</button>
            </p>
          </div>
        </div>
      </div>
      <div class="col">
        <div class="card">
          <div class="card-body">
            <h5 class="card-title">Spray</h5>
            <p class="card-text">)";

      html+=R"(<button class='btn btn-primary' onclick="disableButton(this, )"+String(fogging_duration)+R"();ajax('spray');">Manual Start</button>)";
      html+=R"(<hr class="my-4">
              <label for="spray_1" class="form-label">Schedule 1</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="spray_1" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <label for="spray_2" class="form-label">Schedule 2</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="spray_2" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <button class='btn btn-outline-primary' onclick="schedule_this('spray')\">Save</button>
            </p>
          </div>
        </div>
      </div>
      <div class="col">
        <div class="card">
          <div class="card-body">
            <h5 class="card-title">Fogging</h5>
            <p class="card-text">)";

      html+=R"(<button class='btn btn-primary' onclick="disableButton(this, )"+String(fogging_duration)+R"();ajax('fogging');">Manual Start</button>)";
      html+=R"(<hr class="my-4">
              <label for="fogging_1" class="form-label">Schedule 1</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="fogging_1" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <label for="fogging_2" class="form-label">Schedule 2</label>
              <div class="input-group mb-3">
                <input type="time" class="form-control" id="fogging_2" value="00:00" disabled>
                <button class="btn btn-outline-secondary" type="button" onclick="toggleInput(this);">Enable</button>
              </div>
              <button class='btn btn-outline-primary' onclick="schedule_this('fogging')\">Save</button>
            </p>
          </div>
        </div>
      </div>
    </div>
  </div>
  <script type="text/javascript">
    function schedule_this(text){
      const sched_1 = document.getElementById(text+"_1");
      const sched_2 = document.getElementById(text+"_2");
      // const inputValue = sched_1.value;
      const schedules = [];
      if(!sched_1.disabled){
        schedules[text]=sched_1.value;
      }
      if(!sched_2.disabled){
        schedules[text]=sched_2.value;
      }
      json_schedules = JSON.stringify(schedules);
      const xhr = new XMLHttpRequest();
      xhr.open("POST", "schedules");
      xhr.setRequestHeader("Content-type", "application/json");
      xhr.onload = function() {
        if (xhr.status === 200) {
          console.log("POST request successful!");
        }
      };
      xhr.send(json_schedules);
    }
      function toggleInput(button) {
        var input = button.previousElementSibling;
        if (input.disabled) {
          input.disabled = false;
          button.textContent = 'Disable';
        } else {
          input.disabled = true;
          button.textContent = 'Enable';
        }
      }
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
    </script></center>
</body></html>)";
  server.send(200, "text/html", html);
}