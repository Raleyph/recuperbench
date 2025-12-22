#ifndef STEPPER_AXIS_H
#define STEPPER_AXIS_H

#include <Arduino.h>

#define FORWARD                 0
#define BACKWARD                1

#define NO_PIN                  255

#define STEP_INTERVAL_DEFAULT   200
#define STEP_PULSE_WIDTH        2
#define STEPS_PER_CM            4000
#define STEPS_LIMIT             0xFFFFFFFF

enum AxisState : uint8_t {
  IDLE,
  AT_HOME,
  AT_END,
  MOVING_HOME,
  MOVING_END
};

class StepperAxis {
  private:
    uint8_t  m_stepPin, m_dirPin, m_enPin;
    uint8_t  m_minLimPin, m_maxLimPin;
    uint8_t  m_homeDir, m_endDir;

    bool     m_isOn = true;
    bool     m_isCAxis = false;

    uint16_t m_stepInterval = STEP_INTERVAL_DEFAULT;
    int32_t  m_speed = 50;

    uint32_t m_lastStepTime = 0;
    bool     m_isStepHigh = false;

    uint8_t  m_currentDir = FORWARD;
    uint32_t m_targetSteps = 0;
    uint32_t m_currentSteps = 0;

    bool     m_isMoving = false;

  private:
    void setDirection(uint8_t direction);

    bool isMinHit() const { return m_minLimPin != NO_PIN && digitalRead(m_minLimPin) == HIGH; }
    bool isMaxHit() const { return m_maxLimPin != NO_PIN && digitalRead(m_maxLimPin) == HIGH; }

    bool isLimitHit(uint8_t direction);

    bool step();

  public:
    StepperAxis(uint8_t stepPin, uint8_t dirPin, uint8_t enPin,
                uint8_t minLimPin, uint8_t maxLimPin,
                uint8_t homeDir, uint8_t endDir,
                bool isCAxis = false);
    
    AxisState movingState() const;
    
    void begin();
    void update();

    void enable() { digitalWrite(m_enPin, LOW); m_isOn = true; }
    void disable() { digitalWrite(m_enPin, HIGH); m_isOn = false; }
    void toggle();

    void setSpeed(uint32_t interval);
    void move(uint8_t direction, uint32_t steps = STEPS_LIMIT);

    void goHome() { move(m_homeDir); }
    void goEnd() { move(m_endDir); }

    int32_t speed() const { return m_speed; }
    uint8_t currentDir() const { return m_currentDir; }
    bool isMoving() const { return m_isMoving; }
  
  public:
    static constexpr uint16_t STEP_INTERVAL_MIN = 200;
    static constexpr uint16_t STEP_INTERVAL_MAX = 2000;
};

#endif
