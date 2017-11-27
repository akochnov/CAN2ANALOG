// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13


#include <SPI.h>
#include "mcp_can.h"

#define BENCHMODE         true        //Run without canbus
#define TACHO_PIN         6
#define CHECK_ENGINE_PIN  12



// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

unsigned int engineSpeed = 0;
unsigned int engineTemp = 0;
bool checkEngine = false;

unsigned int rpm = 800;
unsigned long lastRpmChangedMillis = 0;
int rpmIncrement = 20;

void setup()
{
    Serial.begin(115200);
    
    pinMode(CHECK_ENGINE_PIN, OUTPUT);
    
    if (BENCHMODE) {
      Serial.println("BENCHMODE ON"); 
      return;
    }
    
    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}


void loop()
{
    unsigned char len = 0;
    unsigned char buf[8];

    if (true == BENCHMODE) simulateCANData();

    if(!BENCHMODE && CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned int canId = CAN.getCanId();
        
        if (canId == 790) engineSpeed = (int)(buf[3]*256 + buf[2])/6;       
        if (canId == 809) engineTemp = (int) buf[1]-48;
    }

    showEngineSpeed();

    

    
}

void simulateCANData()
{
  if ((millis() - lastRpmChangedMillis) > 15)
  {
    rpm = rpm + rpmIncrement;
    if (rpm > 7000 || rpm < 800) 
      {
        rpmIncrement = -rpmIncrement;
        digitalWrite(CHECK_ENGINE_PIN, HIGH);
        delay(500);
        digitalWrite(CHECK_ENGINE_PIN, LOW);
      }
    lastRpmChangedMillis = millis();
    engineSpeed = rpm;
  }
}

void showEngineSpeed()
{
  int hz = 0;
  hz = (int) engineSpeed / 30;
  if (hz < 31) hz = 31;
  tone(TACHO_PIN, hz);
  Serial.println(engineSpeed);
}

/*********************************************************************************************************
  END FILE
**** *****************************************************************************************************/
