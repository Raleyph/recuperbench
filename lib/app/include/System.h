// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#ifndef SYSTEM_H
#define SYSTEM_H

#include <SystemState.h>

class System {
  private:
    static SystemState m_state;
    static SystemState m_prevState;
  
  public:
    static void init();
    static void update();

  private:
    static void initTimer();

    static void initHardware();
    static void initMotion();

    static void updateFsm();
    static void onEnter();

    static void onInit();
    static void onReady();
    static void onMachining();
    static void onPartRemoval();
    static void onHoming();

    static void changeSpeed();
    static void saveSpeed(uint16_t speed);
    static uint16_t loadSpeed();
};

#endif // SYSTEM_H
