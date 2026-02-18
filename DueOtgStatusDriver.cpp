#include "DueOtgStatusDriver.h"

#if defined(ARDUINO_SAM_DUE)

#ifndef UOTGHS_HSTISR_DCONNI
#define UOTGHS_HSTISR_DCONNI 0u
#endif
#ifndef UOTGHS_HSTISR_DDISCI
#define UOTGHS_HSTISR_DDISCI 0u
#endif
#ifndef UOTGHS_HSTISR_RSTI
#define UOTGHS_HSTISR_RSTI 0u
#endif
#ifndef UOTGHS_HSTISR_HWUPI
#define UOTGHS_HSTISR_HWUPI 0u
#endif
#ifndef UOTGHS_HSTISR_RXRSMI
#define UOTGHS_HSTISR_RXRSMI 0u
#endif

#ifndef UOTGHS_HSTICR_DCONNIC
#define UOTGHS_HSTICR_DCONNIC 0u
#endif
#ifndef UOTGHS_HSTICR_DDISCIC
#define UOTGHS_HSTICR_DDISCIC 0u
#endif
#ifndef UOTGHS_HSTICR_RSTIC
#define UOTGHS_HSTICR_RSTIC 0u
#endif
#ifndef UOTGHS_HSTICR_HWUPIC
#define UOTGHS_HSTICR_HWUPIC 0u
#endif
#ifndef UOTGHS_HSTICR_RXRSMIC
#define UOTGHS_HSTICR_RXRSMIC 0u
#endif

#ifndef UOTGHS_HSTPIPISR_CFGOK
#define UOTGHS_HSTPIPISR_CFGOK 0u
#endif

void DueOtgStatusDriver::begin(HardwareSerial& serialPort) {
  serial = &serialPort;
  state = OtgState::WaitingForDevice;
  lastState = static_cast<OtgState>(255);
  lastReportMs = 0;
  reportState(true);
}

void DueOtgStatusDriver::poll() {
  const uint32_t hstisr = UOTGHS->UOTGHS_HSTISR;

  if (UOTGHS_HSTISR_DCONNI && (hstisr & UOTGHS_HSTISR_DCONNI)) {
    state = OtgState::DeviceAttached;
    UOTGHS->UOTGHS_HSTICR = UOTGHS_HSTICR_DCONNIC;
    reportLine("Native USB: device attach detected");
  }

  if (UOTGHS_HSTISR_RSTI && (hstisr & UOTGHS_HSTISR_RSTI)) {
    state = OtgState::Enumerating;
    UOTGHS->UOTGHS_HSTICR = UOTGHS_HSTICR_RSTIC;
    reportLine("Native USB: bus reset complete, enumeration in progress");
  }

  if (UOTGHS_HSTISR_DDISCI && (hstisr & UOTGHS_HSTISR_DDISCI)) {
    state = OtgState::Disconnected;
    UOTGHS->UOTGHS_HSTICR = UOTGHS_HSTICR_DDISCIC;
    reportLine("Native USB: device disconnected");
  }

  if (UOTGHS_HSTISR_HWUPI && (hstisr & UOTGHS_HSTISR_HWUPI)) {
    UOTGHS->UOTGHS_HSTICR = UOTGHS_HSTICR_HWUPIC;
    reportLine("Native USB: host wakeup event");
  }

  if (UOTGHS_HSTISR_RXRSMI && (hstisr & UOTGHS_HSTISR_RXRSMI)) {
    state = OtgState::Suspended;
    UOTGHS->UOTGHS_HSTICR = UOTGHS_HSTICR_RXRSMIC;
    reportLine("Native USB: resume/suspend signaling detected");
  }

  if (state != OtgState::Disconnected && state != OtgState::WaitingForDevice) {
    const uint32_t address = UOTGHS->UOTGHS_HSTADDR1;
    if (address != 0u && state < OtgState::Addressed) {
      state = OtgState::Addressed;
      reportLine("Native USB: device address assigned");
    }

    if (isAnyPipeConfigured() && state < OtgState::Configured) {
      state = OtgState::Configured;
      reportLine("Native USB: endpoint(s) configured");
    }
  }

  if (state == OtgState::Disconnected) {
    state = OtgState::WaitingForDevice;
  }

  reportState(false);
}

void DueOtgStatusDriver::reportLine(const char* message) {
  if (serial != nullptr) {
    serial->println(message);
  }
}

bool DueOtgStatusDriver::isAnyPipeConfigured() const {
  for (uint8_t pipe = 1; pipe < 10; ++pipe) {
    if (UOTGHS_HSTPIPISR_CFGOK && (UOTGHS->UOTGHS_HSTPIPISR[pipe] & UOTGHS_HSTPIPISR_CFGOK)) {
      return true;
    }
  }
  return false;
}

const __FlashStringHelper* DueOtgStatusDriver::stateText(OtgState value) const {
  switch (value) {
    case OtgState::WaitingForDevice:
      return F("waiting for device");
    case OtgState::DeviceAttached:
      return F("attached");
    case OtgState::Enumerating:
      return F("enumerating");
    case OtgState::Addressed:
      return F("addressed");
    case OtgState::Configured:
      return F("configured");
    case OtgState::Suspended:
      return F("suspended");
    case OtgState::Disconnected:
      return F("disconnected");
    default:
      return F("unknown");
  }
}

void DueOtgStatusDriver::reportState(bool force) {
  const unsigned long now = millis();
  if (!force && state == lastState && (now - lastReportMs) < 1000UL) {
    return;
  }

  if (serial != nullptr) {
    serial->print(F("OTG status: "));
    serial->println(stateText(state));
  }

  lastState = state;
  lastReportMs = now;
}

#endif
