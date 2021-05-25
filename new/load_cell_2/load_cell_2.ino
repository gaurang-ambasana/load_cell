#include <HX711.h>

#define DOUT  A2
#define CLK  A3

HX711 scale(DOUT, CLK);

float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup

void setup() {
  Serial.begin(9600);

  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {
recalibration:
  boolean go out = false;
  Serial.println("Put the weight on");
  delay(5000);
  for (int i = 0; i < 25000; i = i + 10) {
    scale.set_scale(i); //Adjust to this calibration factor
    delay(10);
    float compareValueFloat = scale.get_units(20) * 100;
    int compareValue = round(compareValueFloat);
    Serial.println(compareValue);
    if (compareValue < 0) {
      Serial.println("ERROR");
      go out = true;
      break;
    }
    if ((compareValue - 500) < 30 && (compareValue - 500) >= 5) {
      Serial.println("+100");
      i += 100;
    }
    if ((compareValue - 500) >= 30 && (compareValue - 500) < 100) {
      Serial.println("+400");
      i += 400;
    }
    if ((compareValue - 500) >= 100 && (compareValue - 500) < 500) {
      Serial.println("+1000");
      i += 1000;
    }
    if ((compareValue - 500) >= 500) {
      Serial.println("+2000");
      i += 2000;
    }

    if (compareValue == 500) {
      calibration_factor = i;
      Serial.print(" calibration_factor: ");
      Serial.print(calibration_factor);
      break;
    }
  }
  if (go out) {
    goto recalibration;
  }
  Serial.println("Take the weight off");
  delay(5000);
  Serial.println("Tara");
  scale.tare();
  while (true) {
    Serial.print(scale.get_units());
  }
}
