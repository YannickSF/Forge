
#include <SPI.h>
#include <NRFLite.h>
#include <Wire.h>

int hSol = A7;
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
uint8_t RADIO_ID = 0;             // Our radio's id.
uint8_t DESTINATION_RADIO_ID = 1; // Id of the radio we will transmit to.
uint8_t RADIO_CHANNEL = 95;
NRFLite _radio;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t val1 = 0; // humidité Sol
    uint32_t val2  = 0; // Température
    uint32_t val3 = 0; // Pression
    uint32_t val4 = 0; // Altitud
};

RadioPacket _radioData;

int readSoil()
{
    return analogRead(hSol);
}

void setup()
{
    Serial.begin(115200);
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, RADIO_CHANNEL))
      {
          Serial.println("Cannot communicate with radio");
          while (1); // Wait here forever.
     }
     _radioData.FromRadioId = RADIO_ID;
     
    pinMode(hSol, INPUT);
}


void loop()
{  
   _radioData.val1 = readSoil();
   Serial.println(readSoil());
   
  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))){
        Serial.println(" >...Tx");
  }else{
        Serial.println(" >...");
  }
}
