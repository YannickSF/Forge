#include <Adafruit_BMP085.h>


#include <SPI.h>
#include <NRFLite.h>
#include <Wire.h>

#include <SFE_BMP180.h> 

SFE_BMP180 pressure;
#define ALTITUDE 1655.0


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

   if (pressure.begin())  // initialisation du capteur
      Serial.println("Initialisation du BMP180 reussie");
    else
    {
      Serial.println("Echec de l'initialisation du BMP180:  verifiez les connexions.\n\n");
      while(1); // Pause forever.
    }
    
    pinMode(hSol, INPUT);

    _radioData.FromRadioId = RADIO_ID;
}


void loop()
{   
   char status;
   double T,P,SL;

   status = pressure.startTemperature();
   if (status != 0){
      delay(status);

      status = pressure.getTemperature(T);
      if (status != 0){
         Serial.print("temperature: ");
         Serial.print(T,2);
         Serial.println(" deg C");

         status = pressure.startPressure(3);
         if (status != 0){
            delay(status);

            status = pressure.getPressure(P,T);
            if (status != 0){
               Serial.print("absolute pressure: ");
               Serial.print(P,2);
               Serial.print(" mb, ");
               Serial.print(P*0.0295333727,2);
               Serial.println(" inHg");

              SL = pressure.sealevel(P,ALTITUDE);
              Serial.print("relative (sea-level) pressure: ");
              Serial.print(SL,2);
              Serial.print(" mb, ");
              Serial.print(SL*0.0295333727,2);
              Serial.println(" inHg");
           }
         }
      }
   }
   
   _radioData.val1 = readSoil();
   _radioData.val2 = T;
   _radioData.val3 = P;
   _radioData.val4 = SL;
   Serial.println(readSoil());

  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))){
        Serial.println(" >...Tx");
  }else{
        Serial.println(" >...");
  }
}
