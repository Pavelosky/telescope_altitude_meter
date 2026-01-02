/*
 * Button handler for Telescope Altimeter
 * Handles button debouncing and press detection
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class ButtonHandler {
public:
  ButtonHandler(uint8_t pin);

  // Initialization
  void begin();

  // Update button state (call in main loop)
  void update();

  // Check for button events
  bool wasShortPressed();
  bool wasLongPressed();

private:
  uint8_t buttonPin;
  bool lastButtonState;
  unsigned long buttonPressTime;
  bool longPressDetected;
  bool shortPressReady;
  bool longPressReady;
};

#endif // BUTTON_H
