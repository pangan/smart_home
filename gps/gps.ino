#include <SoftwareSerial.h>

#include <TinyGPS.h> 

// This is for display
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 6, /* data=*/ 7, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED


float lat = 0,lon = 0; // create variable for latitude and longitude object  

unsigned long fix_age, time, date, speed, course;
unsigned long chars;
unsigned short sentences, failed_checksum;


SoftwareSerial gpsSerial(5, 4);//rx,tx
//SoftwareSerial gpsSerial(8, 9);//rx,tx

//SoftwareSerial gpsSerial(2, 3);//rx,tx 


//NewSoftSerial nss(2, 3);

TinyGPS gps; // create gps object 

int aa;
int foundSec = 0;
bool searching = true;




void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print();
  Serial.begin(115200); // connect serial 
  //Serial.println("The GPS Received Signal:"); 
  Serial.println();
  Serial.println("Ready..."); 
  gpsSerial.begin(9600); // connect gps sensor 
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.clearBuffer();
  u8g2.setCursor(0,8);
  u8g2.println("Looking for Sattelite");    

    
  
   
   u8g2.sendBuffer();
   pinMode(2, OUTPUT);
   

}

void loop() {
  digitalWrite(2, LOW);
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  

  u8g2.clearBuffer();         // clear the internal memory
  //u8g2.setFontMode(1);
  
  

  
  
  
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    //digitalWrite(2, LOW);
    //delay(1);
    //digitalWrite(2, HIGH);
    //delay(1);
    while(gpsSerial.available()){ // check for gps data 
      
      char c = gpsSerial.read();
      Serial.write(c);

      if(gps.encode(c))// encode gps data 
        { 
           newData = true;
          
        }
    }
  }

  if (newData){
    float flat, flon, latid;
    unsigned long age;
    unsigned long fix_age, time, date, speed, course;
    
    gps.f_get_position(&flat, &flon, &age);
    
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    Serial.print(" Speed(km/h)=");
    Serial.print(gps.f_speed_kmph());
    Serial.print(" Latitude(m)=");
    Serial.print(gps.f_altitude());
    Serial.print(" Date=");
    Serial.print(date);
    Serial.print(" Time=");
    Serial.print(time);
    Serial.print(" Age=");
    Serial.println(age);
  
    u8g2.setCursor(0,8);
    u8g2.print("SAT: "+String(gps.satellites()));
    u8g2.setCursor(0,8+8);
    u8g2.println("Lat: "+String(flat, 6));
    u8g2.setCursor(0,8+8+8);
    u8g2.println("Lon: "+String(flon, 6));
    u8g2.setCursor(0,8+8+8+8);
    u8g2.println("ALT: "+String(gps.f_altitude(), 1)+" SPD: "+String(gps.f_speed_kmph(), 1));
    

    
  
   
    u8g2.sendBuffer();
  
  } 
  digitalWrite(2, HIGH);
  //delay(1);

}
