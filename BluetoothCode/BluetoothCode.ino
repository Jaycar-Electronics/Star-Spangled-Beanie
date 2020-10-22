#include "BluetoothSerial.h"

#define ARRAY_LEN(x) (sizeof x / sizeof x[0])
#define IS_UPPERCASE(x) (x >= 65 && x <= 90)
#define LED_BUILTIN 13
typedef struct
{
  char letter;     // a - z to designate this Led strip
  uint8_t pin;     // what pin it is connected to
  long delay_time; //set to negative for always on
  long lasttime;   //used internally
} PinMap;

PinMap bindings[] = {
    {'a', LED_BUILTIN, 200L, 0}, // 'a' is LED_BUILTIN, flashing once a second.
    {'b', 2, 100L, 0},           // 'b' is LED row on pin 2, flashing in 100ms
};

// uppercase means that it is 'activated'

BluetoothSerial SerialBT;

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("Beanie"); //gotta be small name

  for (int i = 0; i < ARRAY_LEN(bindings); i++)
  {
    pinMode(bindings[i].pin, OUTPUT);
  }
}

void loop()
{
  if (SerialBT.available())
  {
    char letter = SerialBT.read();
    bool activate = false;

    if (IS_UPPERCASE(letter))
    {
      activate = true;
      Serial.print("Uppercase:");
      Serial.print(letter);
      Serial.print(" -> ");
      letter += 32;
      Serial.print(letter);
    }

    setPinMap(letter, activate);
  }

  for (int i = 0; i < ARRAY_LEN(bindings); i++)
  {
    if (IS_UPPERCASE(bindings[i].letter))
    {
      if (bindings[i].delay_time > 0)
      {
        digitalWrite(bindings[i].pin, HIGH);
        continue;
      }
      if ((millis() - bindings[i].lasttime) > bindings[i].delay_time)
      {
        Serial.print("Toggling ");
        Serial.print(bindings[i].pin);
        Serial.print(" ");
        Serial.println(bindings[i].letter);
        Serial.println(digitalRead(bindings[i].pin));
        digitalWrite(bindings[i].pin, !digitalRead(bindings[i].pin));

        bindings[i].lasttime = millis();
      }
    }
    else
    {
      //lowercase letter
      digitalWrite(bindings[i].pin, LOW);
    }
  }

  delay(20);
}

void setPinMap(char letter, bool activation)
{
  char t = 0;
  for (int i = 0; i < ARRAY_LEN(bindings); i++)
  {
    t = bindings[i].letter;
    if (t == letter || t + 32 == letter)
    {
      //right letter but uppercase
      if (activation)
      {
        if (!IS_UPPERCASE(t))
        {
          Serial.println("Setting upcase for");
          Serial.println(bindings[i].letter);
          bindings[i].letter -= 32;
        }
      }
      else
      {
        if (IS_UPPERCASE(t))
        {
          Serial.println("Setting lowercase for");
          Serial.println(bindings[i].letter);
          bindings[i].letter += 32;
        }
      }
      return;
    }
  }
}