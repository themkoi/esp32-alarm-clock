#pragma once

#include "../../defines.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <atomic>

// Size of one 128x128 frame in RAM: 4 bits per pixel, two pixels per byte
#define OLED_FRAME_BYTES (SCREEN_WIDTH * SCREEN_HEIGHT / 2)

class OrbitSSD1327 : public Adafruit_SSD1327 {
public:
    int16_t offsetX = 0;
    int16_t offsetY = 0;

    using Adafruit_SSD1327::Adafruit_SSD1327;

    void drawPixel(int16_t x, int16_t y, uint16_t color) override;
    void invalidateFullFrame();
};

struct TransitionMsg {
    uint8_t prevFrame[OLED_FRAME_BYTES];
    uint8_t targetFrame[OLED_FRAME_BYTES];
    uint32_t seq;
};

class OLED_MANAGER {
public:
    bool ScreenEnabled = true;
    bool dimmed = false;

    void initDisplay();
    void display();
    void disable();
    void enable();
    void fadeIn();
    void fadeOut();

    void setOffset(int16_t x, int16_t y);
    void stepOrbit();
    void startOrbitTask();

    bool snapshotFrame(uint8_t *dest);
    void suppressDisplay(bool suppress);
    
    // Non-blocking transition invocation accepting old and new buffers
    void startTransition(const uint8_t *prevBuffer, const uint8_t *newBuffer);
    void startTransitionTask();

    // Transition state inspection
    bool isTransitionPending() const { return pendingDisplay.load(); }
    bool isTransitionActive() const { return transitionRunning.load(); }

private:
    uint8_t orbitPhase = 0;
    TaskHandle_t orbitTaskHandle = nullptr;
    bool displaySuppressed = false;

    TaskHandle_t transitionTaskHandle = nullptr;
    QueueHandle_t transitionQueue = nullptr;

    std::atomic<bool> pendingDisplay{false};
    std::atomic<bool> transitionRunning{false};
    std::atomic<uint32_t> transitionSeq{0};

    static void OrbitTask(void *pvParameters);
    static void TransitionTask(void *pvParameters);
    void runTransitionLoop();
};

extern OrbitSSD1327 oled;
extern OLED_MANAGER oledMana;