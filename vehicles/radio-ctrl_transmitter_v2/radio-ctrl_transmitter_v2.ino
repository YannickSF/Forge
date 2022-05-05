
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

const static int Pot = A0;
const static int VRx = A1;
const static int VRy = A2;

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
    
    pinMode(interX, INPUT);
    pinMode(interY, INPUT);

    pinMode(Pot, INPUT);
    pinMode(VRx, INPUT);
    pinMode(VRy, INPUT);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);

    _radioData.FromRadioId = RADIO_ID;
    _radioData.pX = 0;
    _radioData.pY = 0;
    _radioData.dX = 0;
    _radioData.dY = 0;
    
}

void init_radio(){
  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, RADIO_CHANNEL))
      {
          Serial.println("Cannot communicate with radio");
          display.print("Cannot connect to Radio Hardware.");
          while (1); // Wait here forever.
     }
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
}

void drive(){
    _radioData.pX = analogRead(Pot);
    _radioData.pY = 0;
    _radioData.dX = analogRead(VRx);
    _radioData.dY = analogRead(VRy);
    
    display.print("{P*} : ");
    display.println(_radioData.pX); 
    display.print("{<>} : ");
    display.print(_radioData.dX);
    display.print("|");
    display.println(_radioData.dY);
}


void loop()
{   
    display.setCursor(0, 0);
    if(analogRead(interX) > 500 ){
    
    }
    if(analogRead(interY) > 500 ){
      
    }
    
    change_radio();
    drive();

    display.print("Ex:");
    display.print(RADIO_ID);
    display.print(" Ch:");
    display.println(RADIO_CHANNEL);
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
