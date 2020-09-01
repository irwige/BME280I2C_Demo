/*
 * Project BME280_Alt_Test
 * Description: SPECIFICALLY FOR USE IN PARTICLE DEVICES (Tested on Argon and Boron)
 * Author: irwige
 * Date:
 */
#include "BME280I2C.h"

// Variables:
float temp, hum, pres;

//////////////////////////////
// CONSTANTS
//////////////////////////////
const int timeStep = 5000; // time step 5sec.

//////////////////////////////
// VARIABLES
//////////////////////////////
int lastLoop = millis() - timeStep;

// BME280I2C::Settings settings(
//     BME280::OSR_X1,
//     BME280::OSR_X1,
//     BME280::OSR_X1,
//     BME280::Mode_Forced,
//     BME280::StandbyTime_1000ms,
//     BME280::Filter_16,
//     BME280::SpiEnable_False,
//     BME280I2C::I2CAddr_0x77);

//BME280I2C bme(settings);
BME280I2C bme;

void setup()
{
  Serial.begin(115200);
  // Wire.setClock(CLOCK_SPEED_400KHZ);
  // delay(500);

  for (int i = 0; i < 10; i++)
  {
    if (!bme.begin())
    {
      Particle.publish("BME280", "Could not find BME280 sensor!", PRIVATE);
      delay(1000);
    }
    else
    {
      Particle.publish("BME280", "BME280 sensor found!", PRIVATE);
      i = 10;
    }
  }

  // not convinced this matters if I know I'm using a BME280... Doesnt hurt though
  switch (bme.chipModel())
  {
  case BME280::ChipModel_BME280:
    Particle.publish("Chip Model", "Found BME280 sensor! Success.", PRIVATE);
    break;
  case BME280::ChipModel_BMP280:
    Particle.publish("Chip Model", "Found BMP280 sensor! No Humidity available.", PRIVATE);
    break;
  default:
    Particle.publish("Chip Model", "Found UNKNOWN sensor! Error!", PRIVATE);
  }
}

void loop()
{

  if (millis() > lastLoop + timeStep)
  {
    lastLoop = millis();

    //////////////////////////////
    // Temp, Humidity, Pressure
    //////////////////////////////
    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    bme.read(pres, temp, hum, tempUnit, presUnit);

    const char Payload_template[] =
        "\"%s\": %0.2f"   // value1 (String + 2 decimal float)
        ", \"%s\": %0.2f" // value2 (String + 2 decimal float)
        ", \"%s\": %f"    // value3 (String + full float value)
        ;

    char Payload_data[512];
    snprintf(Payload_data, sizeof(Payload_data),
             Payload_template, // refers to format above
             /////// VALUES ///////
             "Temp (C)", temp,       // value1
             "Humidity (%)", hum,    // value2
             "Pressure (hPa)", pres) // value3
        ;

    Particle.publish("publishName", Payload_data, PRIVATE);
  }
}
