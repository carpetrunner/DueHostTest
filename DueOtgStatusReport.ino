#include <Arduino.h>
#include "DueOtgStatusDriver.h"

#if defined(ARDUINO_SAM_DUE)
#include <USBHost.h>

USBHost usb;

DueOtgStatusDriver otgStatus;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000UL) {
  }

  usb.begin();
  otgStatus.begin(Serial);
  Serial.println(F("Due OTG host monitor started on programming USB serial."));
}

void loop() {
  usb.Task();
  otgStatus.poll();
}

#else

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println(F("This sketch requires Arduino Due (ARDUINO_SAM_DUE)."));
}

void loop() {
  delay(1000);
}

#endif
