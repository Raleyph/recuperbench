// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#include "AxisSynchronizer.h"

AxisSynchronizer::AxisSynchronizer(StepperAxis& zAxis, StepperAxis& cAxis)
  : m_zAxis(zAxis), m_cAxis(cAxis) {}

void AxisSynchronizer::setRatio(float ratio) {
  if (ratio <= 0.0f) return;
  m_ratio = ratio;
}

void AxisSynchronizer::update() {
  m_zAxis.update();

  if (!m_zAxis.didStep()) return;

  m_cAccumulator += m_ratio;

  if (m_cAccumulator >= 1.0f) {
    m_cAxis.update();
    m_cAccumulator -= 1.0f;
  }
}
