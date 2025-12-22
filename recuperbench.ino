/*
 * Recuperbench Firmware v.1
 * Copyright © 2025 MG Inc. [https://mg-inc.org]
 * 
 * Designed by Raleyph
 */

// ================== C AXIS SETTINGS ===================
#define C_STEP_PIN      25
#define C_DIR_PIN       23
#define C_EN_PIN        27
#define C_HOME_DIR      FORWARD
#define C_END_DIR       BACKWARD
// ================== Z AXIS SETTINGS ===================
#define Z_STEP_PIN      37
#define Z_DIR_PIN       39
#define Z_EN_PIN        35
#define Z_MIN_PIN       30
#define Z_MAX_PIN       32
#define Z_HOME_DIR      BACKWARD
#define Z_END_DIR       FORWARD
// ================== LCD SETTINGS ======================
#define LCD_RS          20
#define LCD_RW          17
#define LCD_D4          16
#define LCD_D5          21
#define LCD_D6          5
#define LCD_D7          6
#define LCD_BEEPER_PIN  18
// ================== ENCODER SETTINGS ==================
#define ENC_A           40
#define ENC_B           42
#define ENC_SW          19
// ================== LIBRARIES =========================
#include <EncButton.h>
#include "StepperAxis.h"

EncButton enc(ENC_A, ENC_B, ENC_SW, INPUT_PULLUP, INPUT_PULLUP);

StepperAxis cAxis (C_STEP_PIN, C_DIR_PIN, C_EN_PIN, NO_PIN, NO_PIN, C_HOME_DIR, C_END_DIR, true);
StepperAxis zAxis (Z_STEP_PIN, Z_DIR_PIN, Z_EN_PIN, Z_MIN_PIN, Z_MAX_PIN, Z_HOME_DIR, Z_END_DIR);

bool isZMoved = false;

void setup() {
  pinMode(LCD_BEEPER_PIN, OUTPUT);

  cAxis.begin();
  zAxis.begin();

  cAxis.disable();
  cAxis.move(FORWARD);

  if (zAxis.movingState() != AxisState::AT_HOME) zAxis.goHome();
}

void loop() {
  enc.tick();

  cAxis.update();
  zAxis.update();

  processZAxis();
  processCAxis();
}

void processZAxis() {
  if (enc.click() && (zAxis.movingState() == AxisState::AT_HOME && !isZMoved)) {
    zAxis.move(FORWARD, 5 * STEPS_PER_CM);
    isZMoved = true;
  }

  if (enc.click() && zAxis.movingState() == AxisState::IDLE) {
    zAxis.goHome();
    isZMoved = false;
  }
}

void processCAxis() {
  if (enc.hold()) cAxis.toggle();
}
