// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <U8g2lib.h>

#include <SystemState.h>

class LCD {
    private:
        U8G2_ST7920_128X64_F_SW_SPI m_u8g2;

        uint32_t m_lastRedraw;
        
        uint32_t m_splashStartTime;
        bool m_isSplashActive;

    private:
        uint8_t ticksToPercents(uint8_t speed);

        void drawSplash();
        void drawMain(SystemState state,  uint8_t speed, uint8_t savedSpeed);
        void clear();

    public:
        LCD(uint8_t clk, uint8_t data, uint8_t cs);

        void begin();
        void update(SystemState state,  uint8_t speed, uint8_t savedSpeed);
        void startSplash();
};

#endif // LCD_H
