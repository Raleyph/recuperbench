#include "Arduino.h"
#include <math.h>

#include "StepperAxis.h"

StepperAxis::StepperAxis(uint8_t stepPin, uint8_t dirPin, uint8_t enPin,
                         uint8_t minLimPin, uint8_t maxLimPin,
                         uint8_t homeDir, uint8_t endDir,
                         bool isCAxis)
  : m_stepPin(stepPin), m_dirPin(dirPin), m_enPin(enPin),
    m_minLimPin(minLimPin), m_maxLimPin(maxLimPin),
    m_homeDir(homeDir), m_endDir(endDir),
    m_isOn(true), m_isCAxis(isCAxis),
    m_lastStepTime(0), m_isStepHigh(false),
    m_currentDir(FORWARD), m_targetSteps(0), m_currentSteps(0),
    m_isMoving(false)
{}

AxisState StepperAxis::movingState() const {
  if (m_isMoving) return (m_currentDir == FORWARD) ? MOVING_END : MOVING_HOME;
  if (isMinHit()) return AT_HOME;
  if (isMaxHit()) return AT_END;
  return IDLE;
}

void StepperAxis::setDirection(uint8_t direction) {
  m_currentDir = direction;
  digitalWrite(m_dirPin, direction ? HIGH : LOW);
}

bool StepperAxis::isLimitHit(uint8_t direction) {
  if (direction == FORWARD && isMaxHit()) return true;
  if (direction == BACKWARD && isMinHit()) return true;
  return false;
}

bool StepperAxis::step() {
  uint32_t now = micros();

  if (!m_isStepHigh) {
    if (now - m_lastStepTime >= m_stepInterval) {
      digitalWrite(m_stepPin, HIGH);
      m_lastStepTime = now;
      m_isStepHigh = true;
    }
  } else {
    if (now - m_lastStepTime >= STEP_PULSE_WIDTH) {
      digitalWrite(m_stepPin, LOW);
      m_isStepHigh = false;
      return true;
    }
  }

  return false;
}

void StepperAxis::begin() {
  pinMode(m_stepPin, OUTPUT);
  pinMode(m_dirPin,  OUTPUT);
  pinMode(m_enPin,   OUTPUT);

  if (m_minLimPin != NO_PIN) pinMode(m_minLimPin, INPUT_PULLUP);
  if (m_maxLimPin != NO_PIN) pinMode(m_maxLimPin, INPUT_PULLUP);

  enable();
}

void StepperAxis::update() {
  if (!m_isMoving || isLimitHit(m_currentDir)) {
    m_isMoving = false;
    return;
  }

  if (step()) {
    m_currentSteps++;

    if (m_currentSteps >= m_targetSteps) {
      m_isMoving = false;
    }
  }
}

void StepperAxis::toggle() {
  if (m_isOn) disable();
  else enable();
}

void StepperAxis::move(uint8_t direction, uint32_t steps) {
  if (steps == 0) return;
  if (isLimitHit(direction)) return;

  if (!m_isCAxis && (steps == STEPS_LIMIT)) {
    if ((m_minLimPin == NO_PIN) && (direction == m_homeDir)) return;
    if ((m_maxLimPin == NO_PIN) && (direction == m_endDir)) return;
  }

  setDirection(direction);

  m_targetSteps = steps;
  m_currentSteps = 0;
  m_isMoving = true;
}
