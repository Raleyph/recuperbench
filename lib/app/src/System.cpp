// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#include <EncButton.h>

#include <Pins.h>
#include <Kinematics.h>

#include <StepperAxis.h>
#include <AxisSynchronizer.h>

#include "System.h"

SystemState System::m_state;
SystemState System::m_prevState;

//////////////////////////////////////////////////////////////////////////
// Objects
static StepperAxis zAxis(Z_STEP_PIN, Z_DIR_PIN, Z_EN_PIN,
                         Z_MIN_PIN, Z_MAX_PIN, Z_HOME_DIR, Z_END_DIR);

static StepperAxis cAxis(C_STEP_PIN, C_DIR_PIN, C_EN_PIN,
                         NO_PIN, NO_PIN, C_HOME_DIR, C_END_DIR, true);

static AxisSynchronizer sync(zAxis, cAxis);

static EncButton encoder(ENC_A, ENC_B, ENC_SW, INPUT_PULLUP, INPUT_PULLUP);

//////////////////////////////////////////////////////////////////////////
// Common Public Methods
void System::init() {
  initHardware();
  initMotion();

  m_state = INIT;
  m_prevState = (SystemState)255;
}

void System::update() {
  encoder.tick();
  updateFsm();
  changeSpeed();

  if (m_state == MACHINING) sync.update();
  else zAxis.update();
}

//////////////////////////////////////////////////////////////////////////
// System Initialization
void System::initHardware() {
  zAxis.begin();
  cAxis.begin();
}

void System::initMotion() {
  sync.setRatio(C_PER_Z);
}

//////////////////////////////////////////////////////////////////////////
// FSM
void System::updateFsm() {
  if (m_state != m_prevState) {
    onEnter();
    m_prevState = m_state;
  }

  // FSM Transitions
  switch(m_state) {
    case INIT:
      if (zAxis.movingState() == AxisState::AT_HOME)
        m_state = READY;
      break;

    case READY:
      if (encoder.click() && (zAxis.movingState() == AxisState::AT_HOME))
        m_state = MACHINING;
      break;

    case MACHINING:
      if (zAxis.movingState() == AxisState::AT_END)
        m_state = PART_REMOVAL;
      break;

    case PART_REMOVAL:
      if (encoder.click())
        m_state = HOMING;
      break;

    case HOMING:
      if (zAxis.movingState() == AxisState::AT_HOME)
        m_state = READY;
      break;
  }
}

void System::onEnter() {
  switch (m_state) {
    case INIT:
      onInit();
      break;
    case READY:
      onReady();
      break;
    case MACHINING:
      onMachining();
      break;
    case PART_REMOVAL:
      onPartRemoval();
      break;
    case HOMING:
      onHoming();
      break;
  }
}

//////////////////////////////////////////////////////////////////////////
// FSM Methods
void System::onInit() {
  Serial.println("INIT");

  if (zAxis.movingState() != AxisState::AT_HOME)
    zAxis.goHome();
  
  cAxis.disable();
}

void System::onReady() {
  Serial.println("READY");
}

void System::onMachining() {
  Serial.println("MACHINING");

  cAxis.enable();
  cAxis.move(FORWARD);

  zAxis.goToEnd();
}

void System::onPartRemoval() {
  Serial.println("PART_REMOVAL");

  cAxis.stop();
  cAxis.disable();
}

void System::onHoming() {
  Serial.println("HOMING");

  zAxis.goHome();
}

//////////////////////////////////////////////////////////////////////////
// Motion
void System::changeSpeed() {
  if (!encoder.turn()) return;
  zAxis.setSpeed(encoder.dir());
}
