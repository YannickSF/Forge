
#include <SPI.h>
#include <NRFLite.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const static int Button1 = 3;
const static int Button2 = 4;
const static int Button3 = 5;

const static int VRx = A0;
const static int VRy = A1;
const static int PotX = A2;
const static int PotY = A3;
const static int interX = A6;
const static int interY = A7;
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

uint8_t RADIO_ID = 0;             // Our radio's id.
uint8_t DESTINATION_RADIO_ID = 1; // Id of the radio we will transmit to.
uint8_t RADIO_CHANNEL = 100;
NRFLite _radio;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t pX = 0; // Potentiomètre X : Puissance
    uint32_t pY = 0; // Potentiomètre Y : 
    uint32_t dX = 0; // Direction X : Abscisse
    uint32_t dY = 0; // Direction Y : Ordonnée
};

RadioPacket _radioData;
int FlyMode = 0;
boolean stab_mode = false; 

void setup()
{
    Serial.begin(115200);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }else{
      display.display();
      delay(500);
      display.clearDisplay(); 
    }
    
    init_radio();

    pinMode(Button1, INPUT);
    pinMode(Button2, INPUT);
    pinMode(Button3, INPUT);
    
    pinMode(interX, INPUT);
    pinMode(interY, INPUT);
    
    pinMode(VRx, INPUT);
    pinMode(VRy, INPUT);
    pinMode(PotX, INPUT);
    pinMode(PotY, INPUT);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);

    _radioData.FromRadioId = RADIO_ID;
    if( analogRead(interY) > 500 ){
      stab_mode = true;
    }else{
      stab_mode = false;
    }
  
    if(analogRead(interX) > 500 ){
      init_1();
    }else {
      init_2();
    }
}

void init_radio(){
  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, RADIO_CHANNEL))
      {
          Serial.println("Cannot communicate with radio");
          display.print("Cannot connect to Radio Hardware.");
          while (1); // Wait here forever.
     }
}

void init_1(){
  FlyMode = 1;
  _radioData.pX = 0;
  _radioData.pY = 0;
  _radioData.dX = 0;
  _radioData.dY = 0;
}

void init_2(){
  FlyMode = 2;
  _radioData.pX = 0;
  _radioData.pY = 0;
  _radioData.dX = 0;
  _radioData.dY = 0;
}

void change_radio(){
    if (digitalRead(Button1) == HIGH) {
      if(DESTINATION_RADIO_ID < 6){
        DESTINATION_RADIO_ID += 1;
      }else{
        DESTINATION_RADIO_ID = 0;
      }
      init_radio();
    }

    if (digitalRead(Button2) == HIGH) {
      if(RADIO_CHANNEL < 125){
        RADIO_CHANNEL -= 1;
      }
      init_radio();
    }

    if (digitalRead(Button3) == HIGH) {
      if(RADIO_CHANNEL > 0){
        RADIO_CHANNEL += 1;
      }
      init_radio();
    }
}

void check_mode(int value){
  
  if(FlyMode != value){
    if(value == 1){
      init_1();
    }
    if(value == 2){
      init_2();
    }
  }
}

// Car / AirPlane 
void mode_1(){
    check_mode(1);
    _radioData.dX = analogRead(VRx);
    _radioData.dY = analogRead(VRy);

    if( stab_mode == true ) {
      if(analogRead(PotY) == 1023){
        if(_radioData.pY < 1022){
          _radioData.pY += 1;
        }
      }else if(analogRead(PotY) == 0){
        if(_radioData.pY > 0){
         _radioData.pY -= 1; 
        }
      } 
    }else{
      _radioData.pY = analogRead(PotY);
    }

    _radioData.pX = 0;
    
    display.print("{P*} : ");
    display.println(map(_radioData.pY, 0, 1023, -561, 600)); 
    display.print("{<>} : ");
    display.print(_radioData.dX);
    display.print("|");
    display.println(_radioData.dY);
}

// Drone
void mode_2(){
    check_mode(2);
    _radioData.dX = analogRead(VRx);
    _radioData.dY = analogRead(VRy);

    if( stab_mode == true ) {
      if(analogRead(PotY) == 1023){
        if(_radioData.pY < 1022){
          _radioData.pY += 1;
        }
      }else if(analogRead(PotY) == 0){
        if(_radioData.pY > 0){
         _radioData.pY -= 1; 
        }
      } 
    }else{
      _radioData.pY = analogRead(PotY);
    }

    if( stab_mode == true ) {
     if(analogRead(PotX) == 1023){
        if(_radioData.pX < 1022){
          _radioData.pX += 1;
        }
      }else if(analogRead(PotX) == 0){
        if(_radioData.pX > 0){
          _radioData.pX -= 1;
        }
      } 
    }else{
      _radioData.pX = analogRead(PotX);
    }    
    display.print("{<>} : ");
    display.print(_radioData.pY);
    display.print("|");
    display.println(_radioData.pX);    
    display.print("{<>} : ");
    display.print(_radioData.dX);
    display.print("|");
    display.println(_radioData.dY);
}

void loop()
{   
    display.setCursor(0, 0);

    change_radio();
    if( analogRead(interY) > 500 ){
      stab_mode = false;
    }else{
      stab_mode = true;
    }

    if(analogRead(interX) > 500 ){
      mode_2();
    }else {
      mode_1();
    }

    display.print("Ex:");
    display.print(RADIO_ID);
    display.print(" Ch:");
    display.print(RADIO_CHANNEL);
    display.print(" FlM:");
    display.print(FlyMode);
    display.print("|");
    display.println(stab_mode);
    display.print("Rx:");
    display.print(DESTINATION_RADIO_ID);
    
    if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))){
        display.println(" >...Tx");
    }else{
        display.println(" >...");
    }
    
    
    display.display();
    display.clearDisplay();
}
