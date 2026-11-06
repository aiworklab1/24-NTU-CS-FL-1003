#include <Arduino.h>

#define DAC_PIN 25
#define PWM_PIN 16

void setup()
{
    Serial.begin(115200);

    pinMode(PWM_PIN, OUTPUT);

    Serial.println("ESP32 Simultaneous DAC/PWM Test");
}

void loop()
{
    int values[] = {0, 64, 128, 192, 255};

    for (int i = 0; i < 5; i++)
    {
        // DAC Output
        dacWrite(DAC_PIN, values[i]);

        // PWM Output
        analogWrite(PWM_PIN, values[i]);

        Serial.print("DAC Value: ");
        Serial.print(values[i]);

        Serial.print(" | PWM Value: ");
        Serial.println(values[i]);

        delay(2000);
    }
}