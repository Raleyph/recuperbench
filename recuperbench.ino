/*
 * Recuperbench Firmware v.1.1
 * Copyright © 2025 MG Inc. [https://mg-inc.org]
 * 
 * Designed by Raleyph
 * Copyright © 2025 Raleyph [https://github.com/Raleyph]
 *
 * The original workbench uses:
 * - Board GT2560 Rev A+
 * - MKS Mini 12864KBA v.1.2
 * - Step motors 42SHD0034-20B (2 on Z-Axis and 1 on C-Axis)
 * - Lead screws T8
 */

// ================== MOTOR CONSTRAINS ==================
#define STEPS_PER_CM         4000
#define STEPS_PER_REVOLUTION 200
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
// ================== ENCODER SETTINGS ==================
#define ENC_A           40
#define ENC_B           42
#define ENC_SW          19
// ================== LIBRARIES =========================
#include <EncButton.h>

#include "StepperAxis.h"

enum BenchState : uint8_t {
  INIT,
  READY,
  MACHINING,
  PART_REMOVAL,
  HOMING,
  ERROR
};

EncButton enc(ENC_A, ENC_B, ENC_SW, INPUT_PULLUP, INPUT_PULLUP);

StepperAxis cAxis (C_STEP_PIN, C_DIR_PIN, C_EN_PIN,
                   NO_PIN, NO_PIN, C_HOME_DIR, C_END_DIR, true);

StepperAxis zAxis (Z_STEP_PIN, Z_DIR_PIN, Z_EN_PIN,
                   Z_MIN_PIN, Z_MAX_PIN, Z_HOME_DIR, Z_END_DIR);

BenchState state = INIT;
BenchState prevState = (BenchState)255;

void setup() {
  Serial.begin(9600);

  cAxis.begin();
  zAxis.begin();
}

void loop() {
  enc.tick();

  cAxis.update();
  zAxis.update();

  fsmUpdate();
  changeMotorsSpeed();
}

void fsmUpdate() {
  if (state != prevState) {
    onEnter();
    prevState = state;
  }

  // FSM Transitions
  switch(state) {
    case INIT:
      if (zAxis.movingState() == AxisState::AT_HOME)
        state = READY;
      break;
    case READY:
      if (enc.click() && (zAxis.movingState() == AxisState::AT_HOME))
        state = MACHINING;
      break;
    case MACHINING:
      if (zAxis.movingState() == AxisState::AT_END)
        state = PART_REMOVAL;
      break;
    case PART_REMOVAL:
      if (enc.click())
        state = HOMING;
      break;
    case HOMING:
      if (zAxis.movingState() == AxisState::AT_HOME)
        state = READY;
      break;
  }
}

void onEnter() {
  switch (state) {
    case INIT:
      initialize();
      break;
    case READY:
      Serial.println("READY");
      break;
    case MACHINING:
      machining();
      break;
    case PART_REMOVAL:
      part_removal();
      break;
    case HOMING:
      homing();
      break;
  }
}

void initialize() {
  Serial.println("INIT");

  if (zAxis.movingState() != AxisState::AT_HOME)
    zAxis.goHome();
  
  cAxis.disable();
}

void machining() {
  Serial.println("MANCHINING");

  zAxis.goEnd();

  cAxis.enable();
  cAxis.move(FORWARD);
}

void part_removal() {
  Serial.println("PART_REMOVAL");

  cAxis.stop();
  cAxis.disable();
}

void homing() {
  Serial.println("HOMING");

  zAxis.goHome();
}

void changeMotorsSpeed() {
  if (!enc.turn()) return;
  zAxis.setSpeed(enc.dir());
  cAxis.setSpeed(enc.dir());
}
