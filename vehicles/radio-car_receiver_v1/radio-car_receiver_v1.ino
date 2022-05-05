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

#define borneENA        7      // On associe la borne "ENA" du L298N à la pin D10 de l'arduino
#define borneIN1        6       // On associe la borne "IN1" du L298N à la pin D9 de l'arduino
#define borneIN2        5       // On associe la borne "IN2" du L298N à la pin D8 de l'arduino
#define borneIN3        4       // On associe la borne "IN3" du L298N à la pin D7 de l'arduino
#define borneIN4        3       // On associe la borne "IN4" du L298N à la pin D6 de l'arduino
#define borneENB        2       // On associe la borne "ENB" du L298N à la pin D5 de l'arduino

// Our radio's id.
const static uint8_t RADIO_ID = 1;             
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
const static uint8_t RADIO_CHANNEL = 100;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t pX = 0; // Potentiomètre X : Puissance
    uint32_t pY = 0; // Potentiomètre Y : 
    uint32_t dX = 0; // Direction X : Abscisse
    uint32_t dY = 0; // Direction Y : Ordonnée
};

NRFLite _radio;
RadioPacket _radioData;

int vitesse = 0; 
void setup()
{
    Serial.begin(115200);
    
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

    _radioData.FromRadioId = RADIO_ID;

    pinMode(borneENA, OUTPUT);
    pinMode(borneIN1, OUTPUT);
    pinMode(borneIN2, OUTPUT);
    pinMode(borneIN3, OUTPUT);
    pinMode(borneIN4, OUTPUT);
    pinMode(borneENB, OUTPUT);
}

void avant(int value){
  digitalWrite(borneIN1, HIGH);               
  digitalWrite(borneIN2, LOW);   
   
  digitalWrite(borneIN3, HIGH);                
  digitalWrite(borneIN4, LOW);   

  moov_speed(value);
}

void arriere(int value){
  digitalWrite(borneIN1, LOW);                 
  digitalWrite(borneIN2, HIGH);   
  
  digitalWrite(borneIN3, LOW);                 
  digitalWrite(borneIN4, HIGH);   

  moov_speed(value);
}

void droite(int value){
  digitalWrite(borneIN1, LOW);                 
  digitalWrite(borneIN2, HIGH);   
   
  digitalWrite(borneIN3, HIGH);                
  digitalWrite(borneIN4, LOW);   

  moov_speed(value);
}

void gauche(int value){
  digitalWrite(borneIN1, HIGH);                
  digitalWrite(borneIN2, LOW);   
   
  digitalWrite(borneIN3, LOW);                 
  digitalWrite(borneIN4, HIGH); 

  moov_speed(value);
}

void moov_speed(int vitesse){
  analogWrite(borneENA, vitesse);
  analogWrite(borneENB, vitesse);
}

void moov_stop(){
  analogWrite(borneENA, 0);
  analogWrite(borneENB, 0);
}

void loop()
{    
    if (_radio.hasData())
    {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

        if(_radioData.dY < 400){
          avant(_radioData.pX);
        }else if(_radioData.dY > 600){
          arriere(_radioData.pX);
        }else if(_radioData.dX > 600){
          droite(300);
        }else if(_radioData.dX < 400){
          gauche(300);
        }else{
          moov_stop();
        }
    
    }else{
        Serial.println("No Signal");
    }
}
