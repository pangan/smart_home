#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ArduinoOTA.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <ESP8266HTTPClient.h>

ESP8266WebServer server(8090);



String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pin
const int output5 = 5;
const int output4 = 4;
const int resetwifi = 14;  //D5

const char* www_username = "pangan";
const char* www_password = "amir";

const bool RELAY_ON = LOW;
const bool RELAY_OFF = HIGH;

//foto sensor
int val = 0;
const int AN_PORT = A0;

bool sensorActive = true;


int ON_TIME = 1000 * 60 * 60 * 2;  // how long should it be on if it is dark
int OFF_TIME = 2000;  // used to not get feedback




int ON_COUNTER = 0;
int OFF_COUNTER = 0;

bool TURNED_ON = false;
bool REMOTE_ON = false;


const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
         .button_on { background-color: #195B6A; border: none; color: white; padding: 16px 40px;
            text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
          .button_off { background-color:red; border: none; color: white; padding: 16px 40px;
            text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
           
</style>
</head>
<body>
<center>
<h1>Light</h1><br>
<a href="pon" target="myIframe" class="button_on">LIGHT ON</a><br><br><br><br><br>
<a href="poff" target="myIframe" class="button_off">LIGHT OFF</a><br><br><br>
<br>
<br>
<hr>
<a href="son" target="myIframe2" class="button_on">Sensor ON</a><br><br><br><br><br>
<a href="soff" target="myIframe2" class="button_off">Sensor OFF</a><br><br><br>
Light State: <iframe name="myIframe" width="100" height="25" frameBorder="0"></iframe><br>
Sensor Status: <iframe name="myIframe2" width="100" height="25" frameBorder="0"></iframe><br>
<hr>
<br>
<a href="logout">Logout</a>
</center>
 
</body>
</html>
)=====";


void handleRoot() {
 Serial.println("You called root page");
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleLEDon() { 
 Serial.println("LED on page");
  digitalWrite(output5, RELAY_ON);
   //String s = MAIN_pumpoff; //Read HTML contents
 server.send(200, "text/html", "On"); //Send ADC value only to client ajax request
}
 
void handleLEDoff() { 
 Serial.println("LED off page");
 digitalWrite(output5, RELAY_OFF);
 //String s = MAIN_pumpon; //Read HTML contents
 server.send(200, "text/html", "Off"); //Send ADC value only to client ajax request
}

void handleSensoron() { 
 Serial.println("Sensor on page");
sensorActive = true;
   //String s = MAIN_pumpoff; //Read HTML contents
 server.send(200, "text/html", "On"); //Send ADC value only to client ajax request
}
 
void handleSensoroff() { 
 Serial.println("Sensor off page");
sensorActive = false;
 //String s = MAIN_pumpon; //Read HTML contents
 server.send(200, "text/html", "Off"); //Send ADC value only to client ajax request
}


void handleRemoteon(){
  Serial.println("handle on");
  HTTPClient http;
  String serverPath = "http://pangan:amir@192.168.1.192:8090/pon"; //kitchen
  // Your Domain name with URL path or IP address with path
   http.begin(serverPath.c_str());
   // begin(host, port, url)   
   // Send HTTP GET request
   int httpResponseCode = http.GET();
   //String payload = http.getString();
   //Serial.println(payload);
   //server.send(200, "text/html", "Remote ON"); 
}


void handleRemoteoff(){
  HTTPClient http;
  Serial.println("handle off");

  
  String serverPath = "http://pangan:amir@192.168.1.192:8090/poff"; //kitchen
  // Your Domain name with URL path or IP address with path
   http.begin(serverPath.c_str());
      
   // Send HTTP GET request
   int httpResponseCode = http.GET();
   //server.send(200, "text/html", "Remote ON"); 
}



void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);   

     // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(resetwifi, INPUT);
  
  // Set outputs to LOW
  digitalWrite(output5, RELAY_OFF);
  digitalWrite(output4, LOW);
  
   
    Serial.println("Open http://192.168.4.1");
    Serial.println(WiFi.localIP());
    Serial.println("/ in your browser to see it working");
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    if (digitalRead(resetwifi) == LOW){
        digitalWrite(output5, HIGH);
    wifiManager.resetSettings();
    }
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("CanaryAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

   
    //if you get here you have connected to the WiFi
    Serial.println("HTTP SERVER CONNECTED:)");
    server.on("/", []() {
      if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
      }
      handleRoot();
      
    });
    
    server.on("/pon", []() {
      if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
      }
      handleLEDon();
      
    });

    server.on("/poff", []() {
      if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
      }
      handleLEDoff();
      
    });

    server.on("/son", []() {
      if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
      }
      handleSensoron();
      
    });

    server.on("/soff", []() {
      if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
      }
      handleSensoroff();
      
    });

    server.on("/logout", []() {
      server.send(401, "text/html", "Logged out!");
    });

    
    server.begin();
    ArduinoOTA.begin();
}

bool isDark(){
  val = analogRead(AN_PORT);
  delay(3);
  Serial.println(val);
  if (val < 500){
    return true;
  } else {
    return false;
  }
}

void handleRelay(){
  if (ON_COUNTER < ON_TIME) {
    //TURN ON THE LIGHT
    digitalWrite(output5, RELAY_ON);
    if (REMOTE_ON == false){
      handleRemoteon();
      REMOTE_ON = true;
    }
    TURNED_ON = true;
    ON_COUNTER += 1;
  } else{
    if (OFF_COUNTER < OFF_TIME) {
      // TUTN OFF THE LIGHT
      digitalWrite(output5, RELAY_OFF);
      if (REMOTE_ON == true){
          handleRemoteoff();
          REMOTE_ON = false;

      }
      handleRemoteoff();
      OFF_COUNTER += 1; 
    } else{
      TURNED_ON = false;
    }
  }
}

void loop() {
  
 
  ArduinoOTA.handle();  
  
  server.handleClient();
  if (sensorActive == true){
   delay(1);
   if (TURNED_ON != true){
    if (isDark() == false) {
      ON_COUNTER = 0;
      OFF_COUNTER = 0;

      // goto start
    } else {
      handleRelay();
    }
   } else {
    handleRelay();
   }  
    
  }
}
