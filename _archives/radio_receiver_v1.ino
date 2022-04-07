/*
Demonstrates simple RX and TX operation.
Any of the Basic_TX examples can be used as a transmitter.
Please read through 'NRFLite.h' for a description of all the methods available in the library.
Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND
*/

#include <SPI.h>
#include <NRFLite.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const static int Button1 = 3;
const static int Button2 = 6;
const static int Button3 = 5;

// Our radio's id.
uint8_t RADIO_ID = 1;             
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
uint8_t RADIO_CHANNEL = 100;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t p1 = 0; // Potentiomètre 1 : Puissance
    uint32_t p2 = 0; // Potentiomètre 1 : Puissance
    uint32_t dX; // Direction 1 : Abscisse
    uint32_t dY; // Direction 2 : Ordonnée
};

NRFLite _radio;
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

    display.setTextSize(1);
    display.setTextColor(WHITE);
    init_radio();
}

void init_radio(){

    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, RADIO_CHANNEL))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
}

void listen_radio(){
    Serial.print(RADIO_ID);
    Serial.print("-");
    Serial.print(RADIO_CHANNEL);
    Serial.print("| ");
    
    _radioData.FromRadioId = RADIO_ID;
    if (_radio.hasData())
    {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.
        Serial.println("Sucess");
        
        display.print("RxID:");
        display.print(RADIO_ID);
        display.print(" CH:");
        display.println(RADIO_CHANNEL);
        display.println(">..............Tx");
    
    }else{
        Serial.println("No_Data");

        display.print("RxID:");
        display.print(RADIO_ID);
        display.print(" CH:");
        display.println(RADIO_CHANNEL);
        display.println(">...............");
    }
}

void change_radio(){
    if (digitalRead(Button1) == HIGH) {
      if(RADIO_ID < 6){
        RADIO_ID += 1;
        init_radio();
      }else{
        RADIO_ID = 0;
        init_radio();
      }
    }

    if (digitalRead(Button2) == HIGH) {
      if(RADIO_CHANNEL > 0){
        RADIO_CHANNEL -= 1;
        init_radio();
      }
    }

    if (digitalRead(Button3) == HIGH) {
      if(RADIO_CHANNEL < 125){
        RADIO_CHANNEL += 1;
        init_radio();
      }
    }
}

void change_radio_digital(char r_address, char r_channel){
  
 if( r_address == '1'){
    RADIO_ID += 1;
  }
  
  if( r_channel == '1'){
    RADIO_CHANNEL += 1;
  }
  
  init_radio();
}


void loop()
{
    display.setCursor(0, 0);
    change_radio();
    
    if( Serial.available() ){
      
      char address = Serial.read();
      char channel = Serial.read();
      
      Serial.print(address);
      Serial.print(channel);
      
      change_radio_digital(address, channel);
      Serial.println("|Search..");
      
    }else{
     listen_radio(); 
     
    }

    display.display();
    display.clearDisplay();
}
