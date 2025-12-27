// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#include <EncButton.h>

#include <Pins.h>
#include <Kinematics.h>

#include <StepperAxis.h>
#include <AxisSynchronizer.h>

#include <LCD.h>

#include "System.h"

SystemState System::m_state;
SystemState System::m_prevState;

//////////////////////////////////////////////////////////////////////////
// Objects
static StepperAxis zAxis(Z_STEP_PIN, Z_DIR_PIN, Z_EN_PIN,
                         Z_MIN_PIN, Z_MAX_PIN, Z_HOME_DIR, Z_END_DIR);

static StepperAxis cAxis(C_STEP_PIN, C_DIR_PIN, C_EN_PIN,
                         NO_PIN, NO_PIN, C_HOME_DIR, C_END_DIR, true);

static AxisSynchronizer sync(&zAxis, &cAxis);

static EncButton encoder(ENC_A, ENC_B, ENC_SW, INPUT_PULLUP, INPUT_PULLUP);

static LCD lcd(LCD_CLK, LCD_DATA, LCD_CS);

//////////////////////////////////////////////////////////////////////////
// ISR
ISR(TIMER1_COMPA_vect) {
    zAxis.isrUpdate();
    sync.isrUpdate();
}

//////////////////////////////////////////////////////////////////////////
// Common Public Methods
void System::init() {
  Serial.begin(9600);

  initHardware();
  initMotion();
  initTimer();

  m_state = INIT;
  m_prevState = (SystemState)255;
}

void System::update() {
  encoder.tick();

  updateFsm();
  changeSpeed();

  lcd.update(m_state, zAxis.ticksPerStep());
}

//////////////////////////////////////////////////////////////////////////
// System Initialization
void System::initHardware() {
  zAxis.begin();
  cAxis.begin();
  lcd.begin();
  lcd.startSplash();
}

void System::initMotion() {
  sync.setRatio(C_STEPS, Z_STEPS);
}

void System::initTimer()
{
  noInterrupts();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 99;

  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11);
  TIMSK1 |= (1 << OCIE1A);

  interrupts();
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
  if (zAxis.movingState() != AxisState::AT_HOME)
    zAxis.goHome();
  
  cAxis.disable();
}

void System::onReady() {}

void System::onMachining() {
  cAxis.enable();
  cAxis.move(FORWARD);
  zAxis.goToEnd();
}

void System::onPartRemoval() {
  cAxis.stop();
  cAxis.disable();
}

void System::onHoming() {
  zAxis.goHome();
}

//////////////////////////////////////////////////////////////////////////
// Motion
void System::changeSpeed() {
  if (!encoder.turn()) return;

  static uint8_t counter = 0;
  counter++;
  
  if (counter == 2) {
    zAxis.setSpeed(encoder.dir());
    counter = 0;
  }
}
