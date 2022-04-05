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
#include <Servo.h>
#include <NRFLite.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Our radio's id.
const static uint8_t RADIO_ID = 1;             
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
const static uint8_t RADIO_CHANNEL = 100;

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

Servo SrvPowDr;
Servo SrvPowGa;
Servo SrvDir;

void setup()
{
    Serial.begin(115200);
    
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.display();
    delay(500);
    display.clearDisplay();
    
    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, RADIO_CHANNEL))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
    display.setTextSize(1);
    display.setTextColor(WHITE);

    _radioData.FromRadioId = RADIO_ID;
    SrvPowDr.attach(7);
    SrvPowGa.attach(5);
    SrvDir.attach(3);
}

void moov(int val){
  SrvPowDr.write(val);
  SrvPowGa.write(val);
}

void loop()
{
    display.setCursor(0, 0);
    
    if (_radio.hasData())
    {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

        display.print("TFT - [");
        display.print(_radioData.FromRadioId);
        display.println("]");
        
        display.print("P : ");
        display.print(map(_radioData.p1, 0, 1023, 0, 360));
        moov(map(_radioData.p1, 0, 1023, 0, 360));
        
        display.print(" | ");
        display.print("R : ");
        display.println(_radioData.p2);
        
        display.print("x : ");
        display.print(map(_radioData.dX, 0, 1023, 0, 180));
        SrvDir.write(map(_radioData.dX, 0, 1023, 0, 180));
        display.print(" | ");
        display.print("y : ");
        display.println(_radioData.dY);
        display.println("Sucess");
    
    }else{
        display.println("No Data");
    }
    
    display.display();
    delay(10);
    display.clearDisplay();
}
