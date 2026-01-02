/*
 * Button handler implementation for Telescope Altimeter
 */

#include "button.h"
#include "config.h"

ButtonHandler::ButtonHandler(uint8_t pin)
  : buttonPin(pin),
    lastButtonState(HIGH),
    buttonPressTime(0),
    longPressDetected(false),
    shortPressReady(false),
    longPressReady(false) {
}

void ButtonHandler::begin() {
  pinMode(buttonPin, INPUT_PULLUP);
}

void ButtonHandler::update() {
  bool buttonState = digitalRead(buttonPin);

  // Clear flags at the start of each update
  shortPressReady = false;
  longPressReady = false;

  // Detect button press (HIGH to LOW transition)
  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressTime = millis();
    longPressDetected = false;
  }

  // Detect long press while button is held
  if (buttonState == LOW && !longPressDetected) {
    if (millis() - buttonPressTime > LONG_PRESS_TIME) {
      longPressDetected = true;
      longPressReady = true;
    }
  }

  // Detect button release
  if (buttonState == HIGH && lastButtonState == LOW) {
    unsigned long pressDuration = millis() - buttonPressTime;

    if (pressDuration < LONG_PRESS_TIME && !longPressDetected) {
      shortPressReady = true;
    }
  }

  lastButtonState = buttonState;
}

bool ButtonHandler::wasShortPressed() {
  return shortPressReady;
}

bool ButtonHandler::wasLongPressed() {
  return longPressReady;
}
