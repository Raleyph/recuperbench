// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#include "AxisSynchronizer.h"

AxisSynchronizer::AxisSynchronizer(StepperAxis* master, StepperAxis* slave)
  : m_master(master), m_slave(slave) {}

void AxisSynchronizer::isrUpdate() {
  if (!m_master || !m_slave || !m_master->didStepISR()) return;

  m_accumulator += m_slaveSteps;

  if (m_accumulator >= m_masterSteps) {
    m_slave->forceStepISR();
    m_accumulator -= m_masterSteps;
  }
}

void AxisSynchronizer::setRatio(uint16_t slaveStep, uint16_t masterSteps)
{
  if (masterSteps == 0) return;

  noInterrupts();

  m_slaveSteps = slaveStep;
  m_masterSteps = masterSteps;
  m_accumulator = 0;

  interrupts();
}
