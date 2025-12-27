// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#ifndef STEPPER_AXIS_H
#define STEPPER_AXIS_H

#include <Arduino.h>

constexpr uint8_t FORWARD  = 0;
constexpr uint8_t BACKWARD = 1;
constexpr uint8_t NO_PIN   = 255;

enum AxisState : uint8_t {
  IDLE,
  AT_HOME,
  AT_END,
  MOVING_HOME,
  MOVING_END
};

class StepperAxis {
  private:
    //////////////////////////////////////////////////////////////////////////
    // Common Fields
    uint8_t m_stepPin, m_dirPin, m_enPin;
    uint8_t m_minLimPin, m_maxLimPin;
    uint8_t m_homeDir, m_endDir;

    bool m_isOn;
    bool m_isCAxis;

    //////////////////////////////////////////////////////////////////////////
    // ISR
    volatile bool m_isMoving;
    volatile bool m_isStepHigh;
    volatile bool m_isStepEvent;    

    volatile uint8_t  m_currentDir;
    volatile uint32_t m_targetSteps;
    volatile uint32_t m_currentSteps;

    volatile uint16_t m_tickCounter;
    volatile uint8_t  m_ticksPerStep;

    //////////////////////////////////////////////////////////////////////////
    // Constrains
    static constexpr uint32_t STEPS_LIMIT = UINT32_MAX;

  private:
    void setDirection(uint8_t direction);

    bool isMinHit() const;
    bool isMaxHit() const;
    bool isLimitHit(uint8_t direction);

  public:
    StepperAxis(uint8_t stepPin, uint8_t dirPin, uint8_t enPin,
                uint8_t minLimPin, uint8_t maxLimPin,
                uint8_t homeDir, uint8_t endDir,
                bool isCAxis = false);
    
    void begin();

    //////////////////////////////////////////////////////////////////////////
    // On / Off
    void enable();
    void disable();
    void toggle();

    //////////////////////////////////////////////////////////////////////////
    // Movement
    void move(uint8_t direction, uint32_t steps = STEPS_LIMIT);
    void stop();
    void setSpeed(int8_t dir);

    void isrUpdate();
    void forceStepISR();

    void goHome() { move(m_homeDir); }
    void goToEnd() { move(m_endDir); }

    //////////////////////////////////////////////////////////////////////////
    // Properties
    AxisState movingState() const;

    bool isOn() const { return m_isOn; }
    bool isCAxis() const { return m_isCAxis; }

    bool isMoving() const { return m_isMoving; }
    inline bool didStepISR() const { return m_isStepEvent; }

    uint8_t currentDir() const { return m_currentDir; }
    uint16_t ticksPerStep() const { return m_ticksPerStep; }
};

#endif // STEPPER_AXIS_H
