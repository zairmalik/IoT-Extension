#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ACS712.h>

ACS712 sensorI(ACS712_05B, A0);
BlynkTimer timer;

void readCurrent();
double domestic(double);

char auth[] = "9cjMeMxUSTQ5CIk2XRL0HAjgHen9pI9-"; //"4gHu_v5zeyMdsxUnhcHG0FOZ6KpB2HZq"; <-- old one
char ssid[] = "dianamansion 2.4";
char pass[] = "diana@5066";

unsigned long previousTime = 0, currentTime = 0, timeDifference = 0;
double kWh = 0, cost = 0;

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  sensorI.calibrate();
  timer.setInterval(1000L, readCurrent);
}

void loop()
{
  Blynk.run();
  timer.run();
}

void readCurrent()
{
  double voltageAC = 230;
  double currentAC = sensorI.getCurrentAC();
  double power = currentAC*voltageAC;

  Blynk.virtualWrite(V0, currentAC);
  Blynk.virtualWrite(V1, power);

  previousTime = currentTime;
  currentTime = millis();
  timeDifference = currentTime - previousTime; //in milliseconds
  timeDifference /= 1000; //in seconds
  kWh += power*(timeDifference)/3600000;

  Blynk.virtualWrite(V2, kWh);

  cost = domestic(kWh);

  Blynk.virtualWrite(V3, cost);
}

double domestic(double energy)
{
  double cost = 0;

  if (energy <= 200)
  {
    cost = energy*21.8;
  }
  else if (energy <= 300)
  {
    cost = (200*21.8) + ((energy - 200)*33.4);
  }
  else if (energy <= 600)
  {
    cost = (200*21.8) + (100*33.4) + ((energy - 300)*51.6);
  }
  else if (energy <= 900)
  {
    cost = (200*21.8) + (100*33.4) + (300*51.6) + ((energy - 300)*54.6);
  }
  else if (energy > 900)
  {
    cost = (200*21.8) + (100*33.4) + (300*51.6) + (300*54.6) + ((energy - 900)*57.1);
  }

  return 3 + (cost*0.01);
}
