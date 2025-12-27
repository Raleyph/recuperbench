// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#include <stdint.h>

#define C_HOME_DIR  FORWARD
#define C_END_DIR   BACKWARD

#define Z_HOME_DIR  BACKWARD
#define Z_END_DIR   FORWARD

static constexpr uint8_t STEP_INTERVAL_MIN = 2;
static constexpr uint8_t STEP_INTERVAL_MAX = 11;

static constexpr uint16_t Z_STEPS = 25;     // Steps per 50 mm
static constexpr uint16_t C_STEPS = 4;      // Steps per 360 deg.
