/*

Module:  simple_feather.ino

Function:
        Example app matching the documentation in the project
	README.md, showing how to configure a board explicitly

Copyright notice and License:
        See LICENSE file accompanying this project.

Author:
        Terry Moore, MCCI Corporation	November 2018
        Alex Coy, ECE 4950 TA for Spring 2021
	Jack Long, MAE 4220 Student Spring 2025

Notes:
	This app is not complete -- it only presents skeleton
	code for the methods you must provide in order to
	use this library. However, it compiles!

*/

#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#define FILLMEIN (#Don't edit this stuff. Fill in the appropriate FILLMEIN values.)
#warning "You must fill in your keys with the right values from the TTN control panel"
#endif

#include <Arduino_LoRaWAN_ttn.h>
#include <lmic.h>
#include <hal/hal.h>
#include "keys.h"
#include "Catena_Fram32k.h"
#include "TTN_mapper_packager.h"
#include <TinyGPS.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "Wire.h"
#include "filemgmt.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SparkFun_I2C_Expander_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_I2C_Expander_Arduino_Library


#include <SD.h>
File g_file;

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// D-Pad setup
SFE_PCA95XX io; // Defaults to the PCA9554 at I2C address 0x20


#define GPS_TIME 5000

uint64_t lastTime = 0;
uint32_t bufferLength = 8;
static uint8_t messageBuffer[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
static eerl::ttnmapbuf_t buf;
bool can_use_gps = true;

TinyGPS gps;



SFE_UBLOX_GNSS myGNSS;
eerl::TtnPackager packer;
// constexpr uint64_t TX_INTERVAL = 15000;
static bool sending_packet = false;
bool loraConnected;
int menu_idx = 0; //OLED
bool press_up;
bool press_down;
bool press_left;
bool press_right;
bool press_sel;
uint64_t tx_interval = 15000;


// state vars
bool state_menu;
bool state_walkscan;
bool state_drivescan;
bool screen_on;




// D-pad vars
const int buttonUp = 0;
const int buttonDown = 1;
const int buttonRight = 2;
const int buttonLeft = 3;
const int buttonCenter = 4;
const int ledBlue = 5;
const int ledGreen = 6;
const int ledRed = 7;
 int menuIndex = 0;
const int kDebounceDelay = 200; //ms

static const unsigned char signal_icon[] PROGMEM = {
  0b00000000,  // Row 0 (top)
  0b00000000,  // Row 1
  0b00000001,  // Row 2 – tallest bar only
  0b00000001,  // Row 3 – tallest bar
  0b00000101,  // Row 4 – bars 3 and 4
  0b00000101,  // Row 5 – bars 3 and 4
  0b00010101,  // Row 6 – bars 2, 3, and 4
  0b01010101   // Row 7 (bottom) – all 4 bars
};

static const unsigned char no_signal_icon[] PROGMEM = {
  0b00000000,  // Row 0 (top)
  0b00000000,  // Row 1
  0b00000000,  // Row 2
  0b00000000,  // Row 3
  0b00000000,  // Row 4
  0b00000101,  // Row 5 – bars 3 and 4
  0b00010101,  // Row 6 – bars 2, 3, and 4
  0b00010101   // Row 7 (bottom) – all 4 bars
};

static const unsigned char gps_icon[] PROGMEM = {
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};


#ifdef __cplusplus
extern "C" {
#endif

  void myStatusCallback(void *data, bool success) {
    Serial.println("Enter status callback");
    if (success) {
      Serial.println("Succeeded!");
      loraConnected = true;  //added for OLED
    } else {
      Serial.println("Failed!");
      loraConnected = false;  //added for OLED
    }
    can_use_gps = true;
    digitalWrite(LED_BUILTIN, 0);
    sending_packet = false;
  }

  void dateTime(uint16_t *date, uint16_t *time) {
    if (myGNSS.getDateValid()) {
      // return date using FAT_DATE macro to format fields
      *date = FAT_DATE(myGNSS.getYear(), myGNSS.getMonth(), myGNSS.getDay());
    }
    if (myGNSS.getTimeValid()) {
      *time = FAT_TIME(myGNSS.getHour(), myGNSS.getMinute(), myGNSS.getSecond());
    }
  }

#ifdef __cplusplus
}
#endif

class cMyLoRaWAN : public Arduino_LoRaWAN_ttn {
  bool M_FRAM_GOOD = false;
public:
  McciCatena::cFram32k theFram;
  cMyLoRaWAN(){};

  bool begin(const lmic_pinmap &map) {
    M_FRAM_GOOD = true;

    if (!theFram.begin()) {
      Serial.println("Fram begin fail");
      //return false;
    }

    if (!theFram.begin() || !theFram.initialize()) {
      Serial.println("Fram init fail");
      M_FRAM_GOOD = false;
    }


    if (!Arduino_LoRaWAN_ttn::begin(map))
      return false;

    return true;
  }

protected:
  // you'll need to provide implementations for each of the following.
  virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo *) override;
  virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
  virtual void NetSaveSessionState(const SessionState &State);
  virtual bool NetGetSessionState(SessionState &State);
  virtual bool GetAbpProvisioningInfo(
    AbpProvisioningInfo *pProvisioningInfo);
};

// set up the data structures.
cMyLoRaWAN myLoRaWAN{};

// The pinmap. This form is convenient if the LMIC library
// doesn't support your board and you don't want to add the
// configuration to the library (perhaps you're just testing).
// This pinmap matches the FeatherM0 LoRa. See the arduino-lmic
// docs for more info on how to set this up.
const cMyLoRaWAN::lmic_pinmap myPinMap = {
  .nss = 8,
  .rxtx = cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = { 3, 6, cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN },
  .rxtx_rx_active = 0,
  .rssi_cal = 0,
  .spi_freq = 8000000,
};

void send_packet() {
  // Prepare upstream data transmission at the next possible time.
  //LMIC_setTxData2(1, (u1_t *)&buf, sizeof(eerl::ttnmapbuf_t)-1, 0);
  myLoRaWAN.SendBuffer((u1_t *)&buf, sizeof(eerl::ttnmapbuf_t), myStatusCallback, NULL, false, 1);
  Serial.println(F("Packet queued"));
  can_use_gps = false;
  sending_packet = true;
  lastTime = millis();
  digitalWrite(LED_BUILTIN, 1);
}

// void show() {
//   display.display();
//   delay(2000);
//   display.fillScreen(SH110X_WHITE);
// }

void setup() {
  // simply pass the pinmap to the begin() method.
  Serial.begin(115200);
  Serial1.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);
  {
    uint64_t lt = millis();
    while (!Serial && millis() - lt < 5000)
      ;
  }
  Serial.println("Serial begin");

  pinMode(A1, OUTPUT);
  digitalWrite(A1, 1);

  pinMode(8, OUTPUT);
  digitalWrite(8, 1);

  // OLED init
  if (!display.begin(0x3C, true)) {
    Serial.println(F("OLED not found"));
    while (1)
      ;
  }
  display.setTextSize(1);                // normal 1× text
  display.setTextColor(SH110X_WHITE);

  state_menu    = true;   // start in the menu
  screen_on     = true;   // allow the screen to turn on
  state_walkscan  = false;
  state_drivescan = false;


  display.clearDisplay();
  display.display();
  Serial.println("Startup Screen");

  // STARTUP SCREEN
  display.setTextSize(2);     
  display.setCursor(16, 10); // for a centered X value: X = (124 - (characters * 8))/2 
  display.println(" LoRaWan");
  display.setCursor(52, 20);
  //display.println(*CHANGE THIS*);
  display.setCursor(28, 35);
  display.println("Tracker");

  display.display();
  display.setTextSize(1);     
  

  // Init lorawan
  if (!myLoRaWAN.begin(myPinMap)) {
    Serial.println("LoRaWAN begin fail");
    while (1)
      ;
  }
  // Init GPS
  delay(3000);
  if (myGNSS.begin() == false) {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }
  can_use_gps = true;

   // Initialize the PCA9554, default address = 0x20
  if (io.begin() == false) //Device Address, Number of GPIO
  {
    Serial.println("PCA9554 not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  io.pinMode(buttonUp, INPUT);
  io.pinMode(buttonDown, INPUT);
  io.pinMode(buttonLeft, INPUT);
  io.pinMode(buttonRight, INPUT);
  io.pinMode(buttonCenter, INPUT);
  io.pinMode(ledRed, OUTPUT);
  io.pinMode(ledGreen, OUTPUT);
  io.pinMode(ledBlue, OUTPUT);






  startSD();
  setGlobalFile();
  if (g_file) {
    Serial.println("Successful SD Reset");
    g_file.println("{\"Successful Reset\":true}");
    //g_file.println("{\"DEVEUI\":");
    g_file.close();
  }

  lastTime = millis();

  if (myLoRaWAN.IsProvisioned())
    Serial.println("Provisioned for something");
  else
    Serial.println("Not provisioned.");
  //LMIC.datarate = 1;
  //myLoRaWAN.SendBuffer(messageBuffer, bufferLength, myStatusCallback, NULL, false, 1);
  //pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  myGNSS.setI2COutput(COM_TYPE_UBX);                  //Set the I2C port to output both NMEA and UBX messages
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);  //Save (only) the communications port settings to flash and BBR
  //myGNSS.setNMEAOutputPort(Serial);
  send_packet();
}

void check_button_presses() {
  // clear the pressed flags each time
  press_up = press_down = press_left = press_right = press_sel = 0;

  // keep track of last time any button was accepted
  static unsigned long lastDebounceTime = 0;

  // figure out which (if any) button is currently down
  int curr = -1;
  if (io.digitalRead(buttonUp)    == LOW) curr = buttonUp;
  else if (io.digitalRead(buttonDown)  == LOW) curr = buttonDown;
  else if (io.digitalRead(buttonLeft)  == LOW) curr = buttonLeft;
  else if (io.digitalRead(buttonRight) == LOW) curr = buttonRight;
  else if (io.digitalRead(buttonCenter)== LOW) curr = buttonCenter;

  unsigned long now = millis();

  // only register a new press if it’s been > kDebounceDelay since the last one
  if (curr != -1 && (now - lastDebounceTime > kDebounceDelay)) {
    lastDebounceTime = now;  // reset lock‑out timer

    switch (curr) {
      case buttonUp:
        press_up = 1;
        Serial.println("Up");
        break;
      case buttonDown:
        press_down = 1;
        Serial.println("Down");
        break;
      case buttonLeft:
        press_left = 1;
        Serial.println("Left");
        break;
      case buttonRight:
        press_right = 1;
        Serial.println("Right");
        break;
      case buttonCenter:
        press_sel = 1;
        Serial.println("Center");
        break;
    }
  }
}


void drawMenu(){
  display.clearDisplay();

  display.setCursor(40, 0);
  display.println("Menu");
  display.drawLine(0, 10, 128, 10, SH110X_WHITE);

  // Menu options
  if (menuIndex == 0) {
    display.setCursor(0, 20);
    display.print("> ");
    display.print("Drive Scan");
    
    display.setCursor(10, 30);
    display.print("Walk Scan");

  } else if (menuIndex == 1) {
    display.setCursor(10, 20);
    display.print("Drive Scan");

    display.setCursor(0, 30);
    display.print("> ");
    display.print("Walk Scan");
  }

  display.display();
}
float readBatteryVoltage() {
  int raw = analogRead(A7);              // 1023
  float vOut = (raw * 3.3f) / 1023.0f;            // divided voltage at pin
  return vOut * 2.0f;                            // compensate for 2:1 divider
}
// map voltage→0–100%, clamp at extremes
uint8_t voltageToPercent(float v) {
  float pct = (v - 3.0) / (4.18 - 3.0) * 100.0f;
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;
  return static_cast<uint8_t>(pct + 0.5f);
}

void loop() {
  myLoRaWAN.loop();

  static unsigned long lt_gps = millis();

  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // Check for button presses 

  check_button_presses();

  if (state_menu){
    //MENU SCREEN - need to connect the D-pad
    drawMenu();                         // ← redraw every pass
  // Serial.println(" drew menu");
  if (press_up)   menuIndex = (menuIndex - 1 + 2) % 2;
  if (press_down) menuIndex = (menuIndex + 1) % 2;
  if (press_sel)  {  // state transition from menu to scan
    if (menu_idx == 0) {
        state_menu = 0;
        state_drivescan = 1;
        display.clearDisplay();
        display.display();


      } else if (menu_idx == 1) {
        state_menu = 0; 
        state_walkscan = 1;
        display.clearDisplay();
        display.display();
      }
   }
    
  }

  if (state_walkscan || state_drivescan){
    if (state_walkscan){
      tx_interval = 60000;
    }else{
      tx_interval = 15000;
    }

  // turn screen off if down button pressed, or go back to menu

    if (press_down){
      screen_on = 0;
    }
    if (press_up){
      screen_on = 1;
    }

    if (press_left){
      state_walkscan = 0;
      state_drivescan = 0;
      state_menu = 1;
    }
     


    if (millis() - lt_gps > GPS_TIME) {
      lt_gps = millis();
      Serial.println("Look for new GPS");

      if (myGNSS.getGnssFixOk()) {
        digitalWrite(LED_BUILTIN, 1);
        delay(100);
        digitalWrite(LED_BUILTIN, 0);
        float flat, flon, falt;
        unsigned long age;

        flat = (float)myGNSS.getLatitude() / 10000000;
        Serial.println("Lat: ");
        Serial.println(flat);


        flon = (float)myGNSS.getLongitude() / 10000000;
        Serial.println("Long: ");
        Serial.println(flon);
        falt = (float)myGNSS.getAltitude() / 1000;  // in meters
        Serial.println("Alt: ");
        Serial.println(falt);


        packer.setLat(flat);
        packer.setLon(flon);
        packer.setAlt(falt);
        packer.setHdop(myGNSS.getPDOP());
        packer.setStamp(myGNSS.getUnixEpoch());

        packer.packBuffer(&buf);
        uint16_t vBat = analogRead(A7);
        buf.i_bat = vBat;

        setGlobalFile();
        if (g_file) {
          g_file.print("{");
          g_file.print("\"latitude\":");
          g_file.print(buf.f_lat, 14);
          g_file.print(",");
          g_file.print("\"longitude\":");
          g_file.print(buf.f_lon, 14);
          g_file.print(",");
          g_file.print("\"altitude\":");
          g_file.print(buf.alt, 14);
          g_file.print(",");
          g_file.print("\"pdop\":");
          g_file.print(buf.hdop);

          if (myGNSS.getDateValid()) {
            g_file.print(",");
            g_file.print("\"year\":");
            g_file.print(myGNSS.getYear());
            g_file.print(",");
            g_file.print("\"month\":");
            g_file.print(myGNSS.getMonth());
            g_file.print(",");
            g_file.print("\"day\":");
            g_file.print(myGNSS.getDay());
          }
          if (myGNSS.getTimeValid()) {
            g_file.print(",");
            g_file.print("\"hour\":");
            g_file.print(myGNSS.getHour());
            g_file.print(",");
            g_file.print("\"minute\":");
            g_file.print(myGNSS.getMinute());
            g_file.print(",");
            g_file.print("\"second\":");
            g_file.print(myGNSS.getSecond());
          }
        }



        if (millis() - lastTime > tx_interval) {

          if (g_file) {
            g_file.print(",");
            g_file.print("\"LoRaWANSend\":");
            g_file.print(loraConnected);
            g_file.print(",");
            g_file.print("\"txpow\":");
            g_file.print(LMIC.radio_txpow);
            g_file.print("}\n");
            g_file.close();
            Serial.println("File Closed");
          }
          buf.txpow = LMIC.radio_txpow;
          send_packet();
          Serial.println("Return after send_packet");
        }

        else {
          if (g_file) {
            g_file.print("}\n");
            g_file.close();
            Serial.println("File Closed");
          }
        }
        if (screen_on){
          // //OLED display update
          


          display.clearDisplay();

          //SCAN SCREEN: Time and date section
          
          // display.setCursor(25, 10);
          // display.print(myGNSS.getMonth());
          // display.print(" / ");
          // display.print(myGNSS.getDay());
          // display.println(" / 2025");

          display.setCursor(10, 10);
          display.print((myGNSS.getHour() - 4)%24);
          display.print(":");
          display.print(myGNSS.getMinute());
          display.print(":");
          display.println(myGNSS.getSecond());

          display.drawLine(0, 30, 128, 30, SH110X_WHITE);

          // SCAN SCREEN: Battery status (top-right corner)
          display.drawRect(110, 2, 16, 8, SH110X_WHITE);  // Battery outline
          display.drawRect(126, 4, 2, 4, SH110X_WHITE);   // Battery terminal



          int batteryFillWidth = voltageToPercent(readBatteryVoltage());

          display.fillRect(112, 4, batteryFillWidth, 4, SH110X_WHITE);
          display.setCursor(100, 10);
          display.print(batteryFillWidth);
          //inputs (x, y, width, height, color)

          display.drawLine(0, 30, 128, 30, SH110X_WHITE);

          //SCAN SCREEN: connection status section
          if (loraConnected) {
            display.drawBitmap(25, 37, signal_icon, 8, 8, SH110X_WHITE);
            display.setCursor(5, 50);
            display.print("Connected");
          } else {
            display.drawBitmap(25, 37, no_signal_icon, 8, 8, SH110X_WHITE);
            display.setCursor(5, 50);
            display.println("No Signal");
          }

          display.drawLine(64, 30, 64, 64, SH110X_WHITE);

          //SCAN SCREEN: Latitude and longitude section
          display.setCursor(80, 40);
          display.print(flat, 4);
          display.println(", ");

          display.setCursor(80, 50);
          display.println(flon, 4);

          display.display();
        }else{
          display.clearDisplay();
          display.display();

        }

      }

      else {
          digitalWrite(LED_BUILTIN, 1);
          delay(100);
          digitalWrite(LED_BUILTIN, 0);
          delay(100);
          digitalWrite(LED_BUILTIN, 1);
          delay(100);
          digitalWrite(LED_BUILTIN, 0);
          delay(100);
          digitalWrite(LED_BUILTIN, 1);
          delay(100);
          digitalWrite(LED_BUILTIN, 0);

        if (screen_on){
          
          // //OLED default display
          // display.setCursor(25, 10);
          // display.print("0");
          // display.print("/");
          // display.println("00/0000");
          display.clearDisplay();

          display.setCursor(20, 13);
          display.print("0");
          display.print(":0");
          display.println(":00");

          display.drawLine(0, 30, 128, 30, SH110X_WHITE);

          // battery
          // SCAN SCREEN: Battery status (top-right corner)
          display.drawRect(110, 2, 16, 8, SH110X_WHITE);  // Battery outline
          display.drawRect(126, 4, 2, 4, SH110X_WHITE);   // Battery terminal

          Serial.print("Voltage: ");
          Serial.println(readBatteryVoltage());
          int batteryFillWidth = voltageToPercent(readBatteryVoltage()) * 15.0 / 100.0;
          display.fillRect(110, 4, batteryFillWidth, 4, SH110X_WHITE);
          display.setCursor(110, 15);
          batteryFillWidth = batteryFillWidth * 100/ 15;
          display.print(batteryFillWidth);


          if (loraConnected) {
            display.drawBitmap(25, 37, signal_icon, 8, 8, SH110X_WHITE);
            display.setCursor(5, 50);
            display.print("Connected");
          } else {
            display.drawBitmap(25, 37, no_signal_icon, 8, 8, SH110X_WHITE);
            display.setCursor(5, 50);
            display.println("No Signal");
          }

          display.drawLine(64, 30, 64, 64, SH110X_WHITE);

          display.display();
        }else{
          display.clearDisplay();
          display.display();

        }
      }
    }
  }
}

// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool cMyLoRaWAN::GetOtaaProvisioningInfo(
  OtaaProvisioningInfo *pInfo) {
  if (pInfo) {
    memcpy_P(pInfo->AppEUI, APPEUI, 8);
    memcpy_P(pInfo->DevEUI, DEVEUI, 8);
    memcpy_P(pInfo->AppKey, APPKEY, 16);
  }
  return true;
}

void cMyLoRaWAN::NetSaveSessionInfo(
  const SessionInfo &Info,
  const uint8_t *pExtraInfo,
  size_t nExtraInfo) {
  if (!M_FRAM_GOOD) return;
  theFram.saveField(McciCatena::cFramStorage::kNwkSKey, Info.V2.NwkSKey);
  theFram.saveField(McciCatena::cFramStorage::kAppSKey, Info.V2.AppSKey);
  theFram.saveField(McciCatena::cFramStorage::kNetID, Info.V2.NetID);
  theFram.saveField(McciCatena::cFramStorage::kDevAddr, Info.V2.DevAddr);
}

void cMyLoRaWAN::NetSaveSessionState(const SessionState &State) {
  if (!M_FRAM_GOOD) return;
  theFram.saveField(McciCatena::cFramStorage::kLmicSessionState, State);
}

bool cMyLoRaWAN::NetGetSessionState(SessionState &State) {
  if (!M_FRAM_GOOD) return false;
  return theFram.getField(McciCatena::cFramStorage::kLmicSessionState, State);
}

bool cMyLoRaWAN::GetAbpProvisioningInfo(
  AbpProvisioningInfo *pProvisioningInfo) {
  if (!M_FRAM_GOOD) return false;
  bool nd = false;
  if (!pProvisioningInfo) {
    pProvisioningInfo = new AbpProvisioningInfo;
    nd = true;
  }
  SessionState ts;
  if (!theFram.getField(McciCatena::cFramStorage::kLmicSessionState, ts))
    return false;
  if (!theFram.getField(McciCatena::cFramStorage::kAppSKey, pProvisioningInfo->AppSKey) || !theFram.getField(McciCatena::cFramStorage::kNwkSKey, pProvisioningInfo->NwkSKey) || !theFram.getField(McciCatena::cFramStorage::kNetID, pProvisioningInfo->NetID) || !theFram.getField(McciCatena::cFramStorage::kDevAddr, pProvisioningInfo->DevAddr))
    return false;

  pProvisioningInfo->FCntDown = ts.V1.FCntDown;
  pProvisioningInfo->FCntUp = ts.V1.FCntUp;

  if (nd)
    delete pProvisioningInfo;

  Serial.println("Load saved state");
  return true;
}
