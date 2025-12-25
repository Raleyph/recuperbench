/*
 * Recuperbench Firmware v.1.3
 * Copyright © 2025 MG Inc. [https://mg-inc.org]
 * 
 * Designed by Raleyph
 * Copyright © 2025 Raleyph [https://github.com/Raleyph]
 *
 * The original workbench uses:
 * - Board GT2560 Rev A+
 * - MKS Mini 12864KBA v.1.2
 * - Step motors 42SHD0034-20B (2 on Z-Axis and 1 on C-Axis)
 * - Lead screws T8
 */

#include <Arduino.h>

#include <System.h>

void setup() {
  Serial.begin(9600);
  System::init();
}

void loop() {
  System::update();
}
