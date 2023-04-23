
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>  // LCD I2C module library
//#include <DS3231.h>
#include <Servo.h>              // servo library
#include <LiquidCrystal_I2C.h>  // LCD Library

LiquidCrystal_I2C lcd(0x27, 16, 2);
// if lcd is not print then use this 0x27 or 0x3F..

Servo s1;
Servo s2;

int LED = 16;

int sw1 = D5;  // Select switch
int sw2 = D6;  // Hour switch
int sw3 = D7;  // Min switch
int sw4 = D8;  //Override switch

int s1speed = 110;  //servo1 speed
int s1delay = 400;  //servo1 delay

int s2speed = 99;   //servo2 speed
int s2delay = 400;  //servo2 delay

const char *ssid = "SMH Guest";                  //"Satish iPhone 13 Pro Max";  //
const char *password = "ShavesVelocityDonor30";  //"vikisthebest";          //

//Flags
bool screen = true;
bool counter = true;
int count = 0;
//Flags

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

int OnHour = 10;  //SET TIME TO ON LED (24 HOUR FORMAT)
int OnMin = 10;
String hr = "12";
String _min = "30";
String chr = "00";
String c_min = "00";


void setup() {

  Serial.begin(57600);
  Wire.begin();
  lcd.init();  // Some times lcd.init() works
  lcd.backlight();
  WiFi.begin(ssid, password);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(sw1, INPUT);  // Select switch
  pinMode(sw2, INPUT);  // Hour switch
  pinMode(sw3, INPUT);  // Min switch
  pinMode(sw4, INPUT);  // Override switch

  /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");

  }  // end of while (WiFi.status() != WL_CONNECTED)
*/
  timeClient.begin();
  timeClient.setTimeOffset(-14400);

}  // end of void setup

void loop() {

  if (timeClient.getHours() < 10) {
    chr = "0" + String(timeClient.getHours());
  } else {
    chr = timeClient.getHours();
  }

  if (timeClient.getMinutes() < 10) {
    c_min = "0" + String(timeClient.getMinutes());
  } else {
    c_min = timeClient.getMinutes();
  }
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.setCursor(6, 0);
  lcd.print(chr);
  lcd.setCursor(8, 0);
  lcd.print(":");
  lcd.setCursor(9, 0);
  lcd.print(c_min);
  timeClient.update();

  lcd.setCursor(0, 1);
  lcd.print("Set:");
  lcd.setCursor(6, 1);
  lcd.print(hr);
  lcd.setCursor(8, 1);
  lcd.print(":");
  lcd.setCursor(9, 1);
  lcd.print(_min);

  Serial.println(timeClient.getFormattedTime());
  delay(2000);

  if (digitalRead(sw4) == 1) {
    counter = true;
    runServo();
    count = 0;
    lcd.clear();
    digitalWrite(LED, LOW);
  }

  else {
    // Reading switch to set hours and min.
    if (digitalRead(sw1) == 1) {
      screen = true;
      lcd.clear();
      menuScreen();
    }

    while (String(timeClient.getHours()) == String(OnHour) && String(timeClient.getMinutes()) == String(OnMin)) {
      runServo();
    }
    count = 0;
    lcd.clear();
    digitalWrite(LED, LOW);
  }
  count = 0;
}  // end of void loop

/*function for menuScreen to select time to dispense the pills */

void menuScreen() {

  while (screen == true) {

    lcd.setCursor(0, 0);
    lcd.print("Set ON Time");
    lcd.setCursor(0, 1);
    lcd.print(hr);
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(3, 1);
    lcd.print(_min);

    if (digitalRead(sw2) == 1) {
      lcd.clear();
      OnHour = OnHour + 1;  // Hour incriment
      if (OnHour > 23) {
        OnHour = 0;  // if value exceeds 23 make it back to 0
      }

      lcd.setCursor(0, 1);
      lcd.print(OnHour);  // display hour on lcd
    }                     // end of (digitalRead(sw2) == 0) Hour button
    if (OnHour < 10) {
      hr = String(OnHour);
      hr = "0" + hr;

    } else if (OnHour >= 10) {
      hr = String(OnHour);
    }

    if (digitalRead(sw3) == 1) {
      lcd.clear();
      OnMin = OnMin + 1;  // Min. incriment
      if (OnMin > 59) {
        OnMin = 0;  // if value exceeds 59 make it back to 0
      }

      lcd.setCursor(3, 1);
      lcd.print(OnMin);  // display min. on lcd
    }                    // end of (digitalRead(sw3) == 0) Min button

    if (OnMin < 10) {
      _min = String(OnMin);
      _min = "0" + _min;

    } else if (OnMin >= 10) {
      _min = String(OnMin);
    }
    delay(300);
    lcd.clear();
    if (digitalRead(sw1) == 0) {
      counter = true;
      screen = false;
    }  // end of (digitalRead(sw1) == 0) Select button
  }
}

/*function for running the servo */

void runServo() {
  lcd.clear();
  Serial.println("Time Matched");
  Serial.println(timeClient.getFormattedTime());
  lcd.setCursor(1, 0);
  lcd.print("Dispensing---");
  lcd.setCursor(1, 1);
  lcd.print("Please Wait-");
  delay(1000);

  while (counter == true) {
    digitalWrite(LED, HIGH);
    Serial.println("LIGHT ON");
    s1.attach(D3);  // servo attach D3 pin of arduino
    s1.write(s1speed);
    delay(s1delay);
    s1.detach();
    count = count + 1;
    Serial.print("Active count motor1 ");
    Serial.println(count);

    delay(2000);

    if (count == 2) {
      Serial.println("Motor 2 Started");
      s2.attach(D4);  // servo attach D4 pin of arduino
      s2.write(s2speed);
      delay(s2delay);
      s2.detach();
      count = count + 1;
      Serial.print("Active count motor2 ");
      Serial.println(count);
    }

    if (count > 2) {
      counter = false;
    }
  }  // end of while (count > 2)
}
