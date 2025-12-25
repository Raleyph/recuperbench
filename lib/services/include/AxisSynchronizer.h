// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#ifndef AXIS_SYNCHRONIZER_H
#define AXIS_SYNCHRONIZER_H

#include <StepperAxis.h>

class AxisSynchronizer {
  private:
    StepperAxis& m_zAxis;
    StepperAxis& m_cAxis;

    float m_ratio;
    float m_cAccumulator;
  
  public:
    AxisSynchronizer(StepperAxis& zAxis, StepperAxis& cAxis);

    AxisSynchronizer(const AxisSynchronizer&) = delete;
    AxisSynchronizer& operator=(const AxisSynchronizer&) = delete;

    void update();
    void setRatio(float ratio);
};

#endif // AXIS_SYNCHRONIZER_H
