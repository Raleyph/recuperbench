// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>

enum SystemState : uint8_t {
  INIT,
  READY,
  MACHINING,
  PART_REMOVAL,
  HOMING
};

inline const char* getStateName(SystemState state) {
    switch (state) {
        case INIT:          return "INIT";
        case READY:         return "READY";
        case MACHINING:     return "MACHINING";
        case PART_REMOVAL:  return "PART REMOVAL";
        case HOMING:        return "HOMING";
        default:            return "ERROR";
    }
}

#endif // SYSTEM_STATE_H
