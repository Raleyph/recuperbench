// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

enum SystemState : uint8_t {
  INIT,
  READY,
  MACHINING,
  PART_REMOVAL,
  HOMING
};

class System {
  private:
    static SystemState m_state;
    static SystemState m_prevState;
  
  public:
    static void init();
    static void update();

  private:
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
};

#endif
