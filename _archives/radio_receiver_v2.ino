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

struct RadioPacketEngine // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t pX = 0; // Potentiomètre X : Puissance
    uint32_t pY = 0; // Potentiomètre Y : 
    uint32_t dX = 0; // Direction X : Abscisse
    uint32_t dY = 0; // Direction Y : Ordonnée
};

struct RadioPacketSensor // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t temp = 0; // Température
    uint32_t hSol = 0; // humidité Sol
    uint32_t pression = 0; // pression
    uint32_t altitud = 0; // altitude
};


NRFLite _radio;
RadioPacketEngine _radioDataEng;
RadioPacketSensor _radioDataSen;


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
    
    if (_radio.hasData())
    {
      // Norme d'usage utiliser les channel < 100 pour les capteurs. 
      if ( RADIO_CHANNEL > 99){
        _radio.readData(&_radioDataEng); // Note how '&' must be placed in front of the variable name.
          display.print("ExId:");
          display.print(_radioDataEng.FromRadioId);   
          display.println("  >...Tx");
          
          Serial.print(_radioDataEng.FromRadioId);
          Serial.print("-");
          Serial.print(_radioDataEng.pX);
          Serial.print("-");
          Serial.print(_radioDataEng.pY);
          Serial.print("-");
          Serial.print(_radioDataEng.dX);
          Serial.print("-");
          Serial.println(_radioDataEng.dY);
          
          display.print("{1}:");
          display.print(_radioDataEng.pX);
          display.print("#");
          display.println(_radioDataEng.pY);
          display.print("{2}:");
          display.print(_radioDataEng.dX);
          display.print("#");
          display.println(_radioDataEng.dY);
      }else{
          _radio.readData(&_radioDataSen); // Note how '&' must be placed in front of the variable name.
          display.print("ExId:");
          display.print(_radioDataSen.FromRadioId);
          display.println("  >...Tx");
          
          Serial.print(_radioDataSen.FromRadioId);
          Serial.print("-");
          Serial.print(_radioDataSen.temp);
          Serial.print("-");
          Serial.print(_radioDataSen.hSol);
          Serial.print("-");
          Serial.print(_radioDataSen.pression);
          Serial.print("-");
          Serial.println(_radioDataSen.altitud);
          
          display.print("#1:");
          display.print(_radioDataSen.temp);
          display.print("#S2:");
          display.println(_radioDataSen.hSol);
          display.print("#3:");
          display.print(_radioDataSen.pression);
          display.print("#4:");
          display.println(_radioDataSen.altitud);
      }        
    
    }else{
        Serial.println("None");
        
        display.print("ExId:");
        display.print("@");
        display.print("  >...");
        
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
      Serial.print(RADIO_ID);
      Serial.print("-");
      Serial.print(RADIO_CHANNEL);
      Serial.print("|");
  
      display.print("RxID:");
      display.print(RADIO_ID);
      display.print(" CH:");
      display.println(RADIO_CHANNEL);
    
      listen_radio();
     
    }

    display.display();
    display.clearDisplay();
}
