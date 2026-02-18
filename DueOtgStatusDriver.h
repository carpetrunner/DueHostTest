#pragma once

#include <Arduino.h>

#if defined(ARDUINO_SAM_DUE)

enum class OtgState : uint8_t {
  WaitingForDevice,
  DeviceAttached,
  Enumerating,
  Addressed,
  Configured,
  Suspended,
  Disconnected
};

class DueOtgStatusDriver {
 public:
  void begin(HardwareSerial& serialPort);
  void poll();

 private:
  HardwareSerial* serial = nullptr;
  OtgState state = OtgState::WaitingForDevice;
  OtgState lastState = static_cast<OtgState>(255);
  unsigned long lastReportMs = 0;

  void reportLine(const char* message);
  bool isAnyPipeConfigured() const;
  const __FlashStringHelper* stateText(OtgState value) const;
  void reportState(bool force);
};

#else

class DueOtgStatusDriver {
 public:
  void begin(HardwareSerial&) {}
  void poll() {}
};

#endif
