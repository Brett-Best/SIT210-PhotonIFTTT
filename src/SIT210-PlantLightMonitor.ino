/*
 * Project SIT210-CloudFunction
 * Author: Brett T Best
 * Date: April 2019
 */

#include <Particle.h>
#include <vector>
#include <numeric>

using namespace std;

#define DEBUG 0
#define AVG_LENGTH 30                // The amount of samples to calcualte an average light reading.
#define SAMPLE_RATE 1                // The sample rate in seconds.
#define MINIMUM_SUN_LIGHT_LEVEL 1500 // The minimum value to be considered sunny.

uint16_t sPhotosensorPin = A0;
uint16_t sLEDPin = D7;

const char *sEmojiSun = "☀️";
const char *sEmojiMoon = "🌑";
const char *sParticleEventName = "PlantLightStatusChanged";

vector<int32_t> lightReadings;
bool isSunny = false;

int32_t averageOfLightReadings();

#if DEBUG
void printLightReadingsDebug()
{
  Serial.printf("Average of light readings: %i, light readings: [", averageOfLightReadings());

  for (auto const &value : lightReadings)
  {
    Serial.printf("%i,", value);
  }

  Serial.printf("]\n");
}
#endif

void setup()
{
  Serial.begin(9600);

  pinMode(sLEDPin, OUTPUT);

#if DEBUG
  while (!Serial.available())
  {
    // Wait for the serial port to connect.
  }
#endif
}

void loop()
{
  int32_t lightReading = analogRead(sPhotosensorPin);
  lightReadings.insert(lightReadings.begin(), lightReading);

  if (AVG_LENGTH < lightReadings.size())
  {
    lightReadings.resize(AVG_LENGTH);
  }

  if (AVG_LENGTH > lightReadings.size())
  {
    Serial.printlnf("Light reading: %i, waiting for more samples...", lightReading);
  }
  else
  {
    int32_t lightReadingsAverage = averageOfLightReadings();

    Serial.printlnf("Light reading: %i, average: %i", lightReading, lightReadingsAverage);

    bool previousIsSunny = isSunny;
    isSunny = MINIMUM_SUN_LIGHT_LEVEL < lightReadingsAverage;

    if (previousIsSunny && !isSunny) // Light -> Dark
    {
      Serial.printlnf("Event Trigged: %s, Status: %s -> %s", sParticleEventName, sEmojiSun, sEmojiMoon);

      Particle.publish(sParticleEventName, sEmojiMoon);
      digitalWrite(sLEDPin, LOW);
    }
    else if (!previousIsSunny && isSunny) // Dark -> Light
    {
      Serial.printlnf("Event Trigged: %s, Status: %s -> %s", sParticleEventName, sEmojiMoon, sEmojiSun);

      Particle.publish(sParticleEventName, sEmojiSun);
      digitalWrite(sLEDPin, HIGH);
    }
  }

#if DEBUG
  printLightReadingsDebug();
#endif

  delay(1000 * SAMPLE_RATE);
}

int32_t averageOfLightReadings()
{
  return accumulate(lightReadings.begin(), lightReadings.end(), 0) / lightReadings.size();
}
