// Copyright © 2025 MG Inc.
// Copyright © 2025 Raleyph

#include <UI.h>
#include <Kinematics.h>

#include "LCD.h"

//////////////////////////////////////////////////////////////////////////
// Constrains
static const uint32_t REDRAW_INTERVAL = 300;
static const uint32_t SPLASH_DURATION = 2000;

//////////////////////////////////////////////////////////////////////////
// Constructor & Common Methods
LCD::LCD(uint8_t clk, uint8_t data, uint8_t cs)
    : m_u8g2(U8G2_R0, clk, data, cs, U8X8_PIN_NONE),
      m_splashStartTime(0), m_isSplashActive(false)
{}

void LCD::begin()
{
    m_u8g2.begin();
    m_u8g2.enableUTF8Print();
}

void LCD::update(SystemState state, uint8_t speed, uint8_t savedSpeed)
{
    if (m_isSplashActive) {
        if (millis() - m_splashStartTime >= SPLASH_DURATION) {
            m_isSplashActive = false;
            clear();
        }
        return;
    }

    if (millis() - m_lastRedraw >= REDRAW_INTERVAL) {
        drawMain(state, speed, savedSpeed);
        m_lastRedraw = millis();
    }
}

void LCD::startSplash()
{
    m_isSplashActive = true;
    m_splashStartTime = millis();
    drawSplash();
}

uint8_t LCD::ticksToPercents(uint8_t speed)
{
    return (STEP_INTERVAL_MAX - speed + 1) * 10;
}

//////////////////////////////////////////////////////////////////////////
// Draw Methods
void LCD::drawSplash()
{
    m_u8g2.clearBuffer();

    m_u8g2.setFont(u8g2_font_9x18B_tf);

    const char* title = FIRMWARE_TITLE;
    uint8_t titleWidth = m_u8g2.getStrWidth(title);
    uint8_t titleX = (128 - titleWidth) / 2;

    m_u8g2.setCursor(titleX, 24);
    m_u8g2.print(title);

    m_u8g2.setFont(u8g2_font_4x6_tf);
    m_u8g2.setCursor(titleX, 34);
    m_u8g2.print(FIRMWARE_VERSION);

    m_u8g2.setFont(u8g2_font_6x10_tf);
    m_u8g2.setCursor(titleX, 50);
    m_u8g2.print(FIRMWARE_AUTHOR);

    m_u8g2.sendBuffer();
}

void LCD::drawMain(SystemState state,  uint8_t speed, uint8_t savedSpeed)
{
    m_u8g2.clearBuffer();

    m_u8g2.setFont(u8g2_font_6x12_tf);
    m_u8g2.setCursor(4, 12);
    m_u8g2.print("STATUS:");
    m_u8g2.setCursor(48, 12);
    m_u8g2.print(getStateName(state));

    m_u8g2.setCursor(4, 24);
    m_u8g2.print("SPEED:");
    m_u8g2.setCursor(42, 24);
    m_u8g2.print(ticksToPercents(speed));
    m_u8g2.print("%");

    m_u8g2.setCursor(4, 36);
    m_u8g2.print("ST.SPEED:");
    m_u8g2.setCursor(60, 36);
    m_u8g2.print(ticksToPercents(savedSpeed));
    m_u8g2.print("%");

    m_u8g2.sendBuffer();
}

void LCD::clear()
{
    m_u8g2.clearBuffer();
    m_u8g2.sendBuffer();
}
