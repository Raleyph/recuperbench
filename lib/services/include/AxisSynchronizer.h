// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#ifndef AXIS_SYNCHRONIZER_H
#define AXIS_SYNCHRONIZER_H

#include <StepperAxis.h>

class AxisSynchronizer {
  private:
    StepperAxis* m_master = nullptr;
    StepperAxis* m_slave = nullptr;

    volatile int32_t m_accumulator = 0;
    int32_t m_slaveSteps = 0;
    int32_t m_masterSteps = 1;
  
  public:
    AxisSynchronizer(StepperAxis* zAxis, StepperAxis* cAxis);

    AxisSynchronizer(const AxisSynchronizer&) = delete;
    AxisSynchronizer& operator=(const AxisSynchronizer&) = delete;

    void isrUpdate();
    void setRatio(uint16_t slaveStep, uint16_t masterSteps);
};

#endif // AXIS_SYNCHRONIZER_H
