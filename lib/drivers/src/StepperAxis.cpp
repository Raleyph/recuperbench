// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#include <Kinematics.h>

#include "StepperAxis.h"

//////////////////////////////////////////////////////////////////////////
// Constructor & Common Methods & FSM
StepperAxis::StepperAxis(uint8_t stepPin, uint8_t dirPin, uint8_t enPin,
                         uint8_t minLimPin, uint8_t maxLimPin,
                         uint8_t homeDir, uint8_t endDir,
                         bool isCAxis)
  : m_stepPin(stepPin), m_dirPin(dirPin), m_enPin(enPin),
    m_minLimPin(minLimPin), m_maxLimPin(maxLimPin),
    m_homeDir(homeDir), m_endDir(endDir),
    m_isOn(true), m_isCAxis(isCAxis),
    m_isMoving(false), m_isStepHigh(false), m_isStepEvent(false),
    m_currentDir(FORWARD), m_targetSteps(0), m_currentSteps(0),
    m_tickCounter(0), m_ticksPerStep(STEP_INTERVAL_MIN)
{}

void StepperAxis::begin() {
  pinMode(m_stepPin, OUTPUT);
  pinMode(m_dirPin,  OUTPUT);
  pinMode(m_enPin,   OUTPUT);

  if (m_minLimPin != NO_PIN) pinMode(m_minLimPin, INPUT_PULLUP);
  if (m_maxLimPin != NO_PIN) pinMode(m_maxLimPin, INPUT_PULLUP);

  enable();
}

//////////////////////////////////////////////////////////////////////////
// Direction & Limits
void StepperAxis::setDirection(uint8_t direction) {
  m_currentDir = direction;
  digitalWrite(m_dirPin, direction ? HIGH : LOW);
}

bool StepperAxis::isMinHit() const {
  return m_minLimPin != NO_PIN && digitalRead(m_minLimPin) == HIGH;
}

bool StepperAxis::isMaxHit() const {
  return m_maxLimPin != NO_PIN && digitalRead(m_maxLimPin) == HIGH;
}

bool StepperAxis::isLimitHit(uint8_t direction) {
  if (direction == FORWARD && isMaxHit()) return true;
  if (direction == BACKWARD && isMinHit()) return true;
  return false;
}

//////////////////////////////////////////////////////////////////////////
// On / Off
void StepperAxis::enable() {
  digitalWrite(m_enPin, LOW);
  m_isOn = true;
}

void StepperAxis::disable() {
  digitalWrite(m_enPin, HIGH);
  m_isOn = false;
}

void StepperAxis::toggle() {
  if (m_isOn) disable();
  else enable();
}

//////////////////////////////////////////////////////////////////////////
// Movement
void StepperAxis::move(uint8_t direction, uint32_t steps) {
  if (steps == 0) return;
  if (isLimitHit(direction)) return;

  if (!m_isCAxis && (steps == STEPS_LIMIT)) {
    if ((m_minLimPin == NO_PIN) && (direction == m_homeDir)) return;
    if ((m_maxLimPin == NO_PIN) && (direction == m_endDir)) return;
  }

  setDirection(direction);

  noInterrupts();

  m_targetSteps  = steps;
  m_currentSteps = 0;
  m_tickCounter  = 0;
  m_isMoving     = true;

  interrupts();
}

void StepperAxis::stop() {
  noInterrupts();
  m_isMoving = false;
  interrupts();
}

void StepperAxis::setSpeedTicks(uint8_t ticks)
{
  if (m_isCAxis) return;
  ticks = constrain(ticks, STEP_INTERVAL_MIN, STEP_INTERVAL_MAX);
  m_ticksPerStep = ticks;
}

void StepperAxis::changeSpeed(int8_t dir)
{
    if (m_isCAxis || dir == 0) return;

    noInterrupts();

    int8_t v = m_ticksPerStep;

    if (dir > 0)
        v -= 1;
    if (dir < 0)
        v += 1;

    v = constrain((uint8_t)v, STEP_INTERVAL_MIN, STEP_INTERVAL_MAX);
    m_ticksPerStep = v;

    interrupts();
}

void StepperAxis::isrUpdate()
{
  m_isStepEvent = false;

  if (!m_isMoving) return;
  if (isLimitHit(m_currentDir)) {
    m_isMoving = false;
    return;
  }

  m_tickCounter++;

  if (m_tickCounter < m_ticksPerStep) return;
  m_tickCounter = 0;

  if (m_isStepHigh) {
    digitalWrite(m_stepPin, LOW);
    m_isStepHigh = false;

    m_currentSteps++;
    m_isStepEvent = true;

    if (m_currentSteps >= m_targetSteps)
      m_isMoving = false;
  } else {
    digitalWrite(m_stepPin, HIGH);
    m_isStepHigh = true;
  }
}

void StepperAxis::forceStepISR()
{
  digitalWrite(m_stepPin, HIGH);
  digitalWrite(m_stepPin, LOW);
  m_currentSteps++;
}

//////////////////////////////////////////////////////////////////////////
// Properties
AxisState StepperAxis::movingState() const {
  if (m_isMoving)
    return (m_currentDir == FORWARD) ? MOVING_END : MOVING_HOME;
  
  if (isMinHit()) return AT_HOME;
  if (isMaxHit()) return AT_END;

  return IDLE;
}
