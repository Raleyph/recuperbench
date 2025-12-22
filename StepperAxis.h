#ifndef STEPPER_AXIS_H
#define STEPPER_AXIS_H

#define FORWARD      0
#define BACKWARD     1

#define NO_PIN       255

#define STEPS_PER_CM 4000

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

    bool     m_isOn;
    bool     m_isCAxis;

    uint16_t m_stepInterval;

    uint32_t m_lastStepTime;
    bool     m_isStepHigh;

    uint8_t  m_currentDir;
    uint32_t m_targetSteps;
    uint32_t m_currentSteps;

    bool     m_isMoving;
  
    static constexpr uint32_t STEPS_LIMIT = UINT32_MAX;
    static constexpr uint16_t STEP_INTERVAL_DEFAULT = 200;

  private:
    void setDirection(uint8_t direction);

    bool isMinHit() const;
    bool isMaxHit() const;
    bool isLimitHit(uint8_t direction);

    bool step();

  public:
    StepperAxis(uint8_t stepPin, uint8_t dirPin, uint8_t enPin,
                uint8_t minLimPin, uint8_t maxLimPin,
                uint8_t homeDir, uint8_t endDir,
                bool isCAxis = false, uint16_t startStepInterval = STEP_INTERVAL_DEFAULT);
    
    AxisState movingState() const;
    
    void begin();
    void update();

    void enable();
    void disable();
    void toggle();

    void setSpeed(int8_t speedDir);
    void move(uint8_t direction, uint32_t steps = STEPS_LIMIT);

    void goHome() { move(m_homeDir); }
    void goEnd() { move(m_endDir); }

    bool isOn() const { return m_isOn; }
    uint8_t currentDir() const { return m_currentDir; }
    bool isMoving() const { return m_isMoving; }

    static constexpr uint16_t STEP_INTERVAL_MIN = 200;
    static constexpr uint16_t STEP_INTERVAL_MAX = 2000;
};

#endif
