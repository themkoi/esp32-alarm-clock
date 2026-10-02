#include "display.h"

SPIClass spi(FSPI);
OrbitSSD1327 oled(128, 128, &spi, OLED_DC, OLED_RESET, OLED_CS, 5000000UL);

OLED_MANAGER oledMana;

#define SSD1327_PRECHARGE_DEFAULT 0x22

const uint8_t graytable_highCotr[] = {
    SSD1327_GRAYTABLE,
    0x03, 0x05, 0x07, 0x09,
    0x0C, 0x0F, 0x12, 0x16,
    0x1A, 0x1F, 0x25, 0x2B,
    0x32, 0x38, 0x3C, 0x3F};

const uint8_t graytable_lowCotr[] = {
    SSD1327_GRAYTABLE,
    0x05, 0x07, 0x09, 0x0B,
    0x0E, 0x11, 0x14, 0x18,
    0x1D, 0x22, 0x28, 0x2E,
    0x34, 0x39, 0x3D, 0x3F};

SemaphoreHandle_t oledMutex = nullptr;

// Transition configuration & static buffers
#define TRANS_STEPS 5         // number of animation frames
#define TRANS_STEP_DELAY_MS 0 // pause between animation frames
#define TRANS_MAX_PIXELS 1536 // max number of moving dots
#define TRANS_GRID_N 8        // 8x8 grid of 16x16 px cells used for matching

struct TransPixel
{
    uint16_t pos; // (y << 7) | x
    uint8_t v;    // gray level 0..15
};

struct TransDot
{
    uint16_t fromPos;
    uint16_t toPos;
    uint8_t vFrom;
    uint8_t vTo;
    uint8_t startStep; // small random delay so the dots do not move in lockstep
};

// Heap/Static buffer allocation to protect the task stack space
static uint8_t transPrevFrame[OLED_FRAME_BYTES];
static uint8_t transTargetFrame[OLED_FRAME_BYTES];
static uint8_t transMsgPrevBuf[OLED_FRAME_BYTES];
static uint8_t transMsgTargetBuf[OLED_FRAME_BYTES];

static TransPixel transTargets[TRANS_MAX_PIXELS];
static TransDot transDots[TRANS_MAX_PIXELS];
static uint16_t transCellStart[TRANS_GRID_N * TRANS_GRID_N];
static uint16_t transCellCount[TRANS_GRID_N * TRANS_GRID_N];
static uint32_t transRng = 0x9E3779B9u;

// Static transition message container to prevent large stack allocation
static TransitionMsg staticTransMsg;

static inline uint8_t framePixelGet(const uint8_t *buf, int16_t x, int16_t y)
{
    uint8_t b = buf[x / 2 + y * (SCREEN_WIDTH / 2)];
    return (x & 1) ? (b & 0x0F) : (b >> 4);
}

static inline void framePixelSet(uint8_t *buf, int16_t x, int16_t y, uint8_t v)
{
    uint8_t *p = &buf[x / 2 + y * (SCREEN_WIDTH / 2)];
    if (x & 1)
    {
        *p = (uint8_t)((*p & 0xF0) | (v & 0x0F));
    }
    else
    {
        *p = (uint8_t)((*p & 0x0F) | ((v & 0x0F) << 4));
    }
}

static inline uint32_t transRand()
{
    transRng = transRng * 1664525u + 1013904223u;
    return transRng >> 8;
}

void OrbitSSD1327::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    int16_t px = x + offsetX;
    int16_t py = y + offsetY;

    if (px < 0)
        px = 0;
    else if (px >= SCREEN_WIDTH)
        px = SCREEN_WIDTH - 1;

    if (py < 0)
        py = 0;
    else if (py >= SCREEN_HEIGHT)
        py = SCREEN_HEIGHT - 1;

    Adafruit_SSD1327::drawPixel(px, py, color);
}

void OrbitSSD1327::invalidateFullFrame()
{
    window_x1 = 0;
    window_y1 = 0;
    window_x2 = WIDTH - 1;
    window_y2 = HEIGHT - 1;
}

void OLED_MANAGER::initDisplay()
{
    if (oledMutex == nullptr)
    {
        oledMutex = xSemaphoreCreateMutex();
    }

    Serial.println("SSD1327 OLED init");
    spi.begin(OLED_CLK, -1, OLED_MOSI, OLED_CS);

    if (!oled.begin(0x3D, true))
    {
        Serial.println("Unable to initialize OLED");
        while (1)
            vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        const uint8_t init_128x128[] = {
            SSD1327_DISPLAYOFF,
            SSD1327_SETCONTRAST,
            0xFF,
            SSD1327_SEGREMAP,
            0x51,
            SSD1327_SETSTARTLINE,
            0x00,
            SSD1327_SETDISPLAYOFFSET,
            0x00,
            SSD1327_DISPLAYALLOFF,
            SSD1327_SETMULTIPLEX,
            0x7F,
            SSD1327_PHASELEN,
            0x51,
            SSD1327_GRAYTABLE,
            0x03, 0x05, 0x07, 0x09,
            0x0C, 0x0F, 0x12, 0x16,
            0x1A, 0x1F, 0x25, 0x2B,
            0x32, 0x38, 0x3C, 0x3F,
            SSD1327_DCLK,
            0x50,
            SSD1327_REGULATOR,
            0x01,
            SSD1327_PRECHARGE2,
            0x04,
            SSD1327_SETVCOM,
            0x0F,
            SSD1327_PRECHARGE,
            SSD1327_PRECHARGE_DEFAULT,
            SSD1327_FUNCSELB,
            0x62,
            SSD1327_CMDLOCK,
            0x12,
            SSD1327_NORMALDISPLAY,
            SSD1327_DISPLAYON};

        oled.oled_commandList(init_128x128, sizeof(init_128x128));

        oled.clearDisplay();
        oled.display();

        xSemaphoreGive(oledMutex);
    }

    startOrbitTask();
    startTransitionTask();
}

void OLED_MANAGER::display()
{
    if (displaySuppressed)
        return;

    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        oled.display();
        xSemaphoreGive(oledMutex);
    }
}

bool OLED_MANAGER::snapshotFrame(uint8_t *dest)
{
    if (dest == nullptr)
        return false;

    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        memcpy(dest, oled.getBuffer(), OLED_FRAME_BYTES);
        xSemaphoreGive(oledMutex);
        return true;
    }
    return false;
}

void OLED_MANAGER::suppressDisplay(bool suppress)
{
    displaySuppressed = suppress;
}

void OLED_MANAGER::disable()
{
    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        ScreenEnabled = false;

        oled.oled_command(SSD1327_DISPLAYOFF);

        oled.oled_command(SSD1327_REGULATOR);
        oled.oled_command(0x00);

        oled.oled_command(SSD1327_PRECHARGE);
        oled.oled_command(0x00);

        xSemaphoreGive(oledMutex);
    }
}

void OLED_MANAGER::enable()
{
    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        ScreenEnabled = true;

        oled.oled_command(SSD1327_REGULATOR);
        oled.oled_command(0x01);

        oled.oled_command(SSD1327_PRECHARGE);
        oled.oled_command(SSD1327_PRECHARGE_DEFAULT);

        oled.oled_command(SSD1327_DISPLAYON);

        xSemaphoreGive(oledMutex);
    }
}

void OLED_MANAGER::fadeIn()
{
    if (!dimmed)
        return;
    bool isNight = checkForNight();
    int maxContrast = MAX_CONTRAST;
    int minContrast = MIN_CONTRAST;

    if (isNight)
    {
        maxContrast = MAX_CONTRAST_NIGHT;
        minContrast = MIN_CONTRAST_NIGHT;
    }
    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        for (int dim = minContrast; dim <= maxContrast; dim += 10)
        {
            oled.oled_command(SSD1327_SETCONTRAST);
            oled.oled_command(dim);
            vTaskDelay(pdMS_TO_TICKS(20));
        }

        oled.oled_commandList(graytable_highCotr, sizeof(graytable_highCotr));
        dimmed = false;

        xSemaphoreGive(oledMutex);
    }
}

void OLED_MANAGER::fadeOut()
{
    if (dimmed)
        return;

    bool isNight = checkForNight();
    int maxContrast = MAX_CONTRAST;
    int minContrast = MIN_CONTRAST;
    if (isNight)
    {
        maxContrast = MAX_CONTRAST_NIGHT;
        minContrast = MIN_CONTRAST_NIGHT;
    }

    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        for (int dim = maxContrast; dim >= minContrast; dim -= 10)
        {
            oled.oled_command(SSD1327_SETCONTRAST);
            oled.oled_command(dim);
            vTaskDelay(pdMS_TO_TICKS(20));
        }

        oled.oled_commandList(graytable_lowCotr, sizeof(graytable_lowCotr));
        dimmed = true;

        xSemaphoreGive(oledMutex);
    }
}

void OLED_MANAGER::setOffset(int16_t x, int16_t y)
{
    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        oled.offsetX = x;
        oled.offsetY = y;
        xSemaphoreGive(oledMutex);
    }
}

void OLED_MANAGER::stepOrbit()
{
    orbitPhase = (orbitPhase + 1) % 4;

    int16_t x = (orbitPhase == 1 || orbitPhase == 2) ? 1 : 0;
    int16_t y = (orbitPhase == 2 || orbitPhase == 3) ? 1 : 0;

    setOffset(x, y);
}

void OLED_MANAGER::OrbitTask(void *pvParameters)
{
    OLED_MANAGER *mgr = (OLED_MANAGER *)pvParameters;

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(SCREEN_ORBIT_INTERVAL));
        mgr->stepOrbit();
    }
}

void OLED_MANAGER::startOrbitTask()
{
    if (orbitTaskHandle == nullptr)
    {
        xTaskCreate(
            OrbitTask,
            "OLED_Orbit_Task",
            2048,
            this,
            1,
            &orbitTaskHandle);
    }
}

// ---------------------------------------------------------------------------
// Async Transition Implementation
// ---------------------------------------------------------------------------

void OLED_MANAGER::startTransitionTask()
{
    if (transitionQueue == nullptr)
    {
        transitionQueue = xQueueCreate(1, sizeof(TransitionMsg *));
    }

    if (transitionTaskHandle == nullptr)
    {
        xTaskCreate(
            TransitionTask,
            "OLED_Trans_Task",
            8192,
            this,
            2,
            &transitionTaskHandle);
    }
}

void OLED_MANAGER::startTransition(const uint8_t *prevBuffer, const uint8_t *newBuffer)
{
    if (prevBuffer == nullptr || newBuffer == nullptr)
        return;

    // Increment sequence counter first so active transition loops abort immediately
    uint32_t currentSeq = ++transitionSeq;

    // Direct memory copy into static storage array members
    memcpy(transMsgPrevBuf, prevBuffer, OLED_FRAME_BYTES);
    memcpy(transMsgTargetBuf, newBuffer, OLED_FRAME_BYTES);

    memcpy((void *)staticTransMsg.prevFrame, transMsgPrevBuf, OLED_FRAME_BYTES);
    memcpy((void *)staticTransMsg.targetFrame, transMsgTargetBuf, OLED_FRAME_BYTES);
    staticTransMsg.seq = currentSeq;

    pendingDisplay.store(true);

    TransitionMsg *pMsg = &staticTransMsg;
    xQueueOverwrite(transitionQueue, &pMsg);
}

void OLED_MANAGER::TransitionTask(void *pvParameters)
{
    OLED_MANAGER *mgr = static_cast<OLED_MANAGER *>(pvParameters);
    mgr->runTransitionLoop();
}

void OLED_MANAGER::runTransitionLoop()
{
    TransitionMsg *pMsg = nullptr;

    while (true)
    {
        if (xQueueReceive(transitionQueue, &pMsg, portMAX_DELAY) == pdTRUE && pMsg != nullptr)
        {
            uint32_t activeSeq = pMsg->seq;

            // Check for immediate abort before doing math
            if (transitionSeq.load() != activeSeq)
            {
                continue;
            }

            if (xSemaphoreTake(oledMutex, portMAX_DELAY))
            {
                // If a transition is active, capture live buffer so new transition starts smoothly
                if (transitionRunning.load())
                {
                    memcpy(transPrevFrame, oled.getBuffer(), OLED_FRAME_BYTES);
                }
                else
                {
                    memcpy(transPrevFrame, pMsg->prevFrame, OLED_FRAME_BYTES);
                }
                memcpy(transTargetFrame, pMsg->targetFrame, OLED_FRAME_BYTES);
                xSemaphoreGive(oledMutex);
            }

            transitionRunning.store(true);

            // Skip identical frames immediately
            if (memcmp(transPrevFrame, transTargetFrame, OLED_FRAME_BYTES) == 0)
            {
                pendingDisplay.store(false);
                transitionRunning.store(false);
                continue;
            }

            // Grid counting & target mapping
            memset(transCellCount, 0, sizeof(transCellCount));
            uint16_t targetCount = 0;
            for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
            {
                for (int16_t x = 0; x < SCREEN_WIDTH; x++)
                {
                    uint8_t o = framePixelGet(transPrevFrame, x, y);
                    uint8_t n = framePixelGet(transTargetFrame, x, y);
                    if (n > o && targetCount < TRANS_MAX_PIXELS)
                    {
                        transCellCount[(y / 16) * TRANS_GRID_N + (x / 16)]++;
                        targetCount++;
                    }
                }
            }

            uint16_t offset = 0;
            for (int i = 0; i < TRANS_GRID_N * TRANS_GRID_N; i++)
            {
                transCellStart[i] = offset;
                offset += transCellCount[i];
            }

            memset(transCellCount, 0, sizeof(transCellCount));
            uint16_t filled = 0;
            for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
            {
                for (int16_t x = 0; x < SCREEN_WIDTH; x++)
                {
                    uint8_t o = framePixelGet(transPrevFrame, x, y);
                    uint8_t n = framePixelGet(transTargetFrame, x, y);
                    if (n > o && filled < TRANS_MAX_PIXELS)
                    {
                        int cell = (y / 16) * TRANS_GRID_N + (x / 16);
                        uint16_t slot = transCellStart[cell] + transCellCount[cell];
                        transTargets[slot].pos = (uint16_t)((y << 7) | x);
                        transTargets[slot].v = n;
                        transCellCount[cell]++;
                        filled++;
                    }
                }
            }

            // Early sequence abort check before dot calculation
            if (transitionSeq.load() != activeSeq)
            {
                continue;
            }

            // Dot calculation
            uint16_t dotCount = 0;
            for (int16_t y = 0; y < SCREEN_HEIGHT && dotCount < TRANS_MAX_PIXELS; y++)
            {
                for (int16_t x = 0; x < SCREEN_WIDTH && dotCount < TRANS_MAX_PIXELS; x++)
                {
                    uint8_t o = framePixelGet(transPrevFrame, x, y);
                    uint8_t n = framePixelGet(transTargetFrame, x, y);
                    if (o <= n)
                        continue;

                    int16_t cx = x / 16;
                    int16_t cy = y / 16;
                    int foundCell = -1;
                    for (int r = 0; r < TRANS_GRID_N && foundCell < 0; r++)
                    {
                        for (int dy = -r; dy <= r && foundCell < 0; dy++)
                        {
                            for (int dx = -r; dx <= r; dx++)
                            {
                                int adx = dx < 0 ? -dx : dx;
                                int ady = dy < 0 ? -dy : dy;
                                if (adx < r && ady < r)
                                    continue;

                                int16_t nx = cx + dx;
                                int16_t ny = cy + dy;
                                if (nx < 0 || nx >= TRANS_GRID_N || ny < 0 || ny >= TRANS_GRID_N)
                                    continue;

                                int cell = ny * TRANS_GRID_N + nx;
                                if (transCellCount[cell] > 0)
                                    foundCell = cell;
                            }
                        }
                    }
                    if (foundCell < 0)
                        continue;

                    uint16_t k = transCellStart[foundCell] + (uint16_t)(transRand() % transCellCount[foundCell]);
                    uint16_t last = transCellStart[foundCell] + transCellCount[foundCell] - 1;
                    TransPixel tmp = transTargets[k];
                    transTargets[k] = transTargets[last];
                    transTargets[last] = tmp;
                    transCellCount[foundCell]--;

                    transDots[dotCount].fromPos = (uint16_t)((y << 7) | x);
                    transDots[dotCount].toPos = transTargets[k].pos;
                    transDots[dotCount].vFrom = (uint8_t)(o - n);
                    transDots[dotCount].vTo = transTargets[k].v;
                    transDots[dotCount].startStep = (uint8_t)(transRand() % 4);
                    dotCount++;
                }
            }

            // Animation Loop with instant interruption detection
            bool interrupted = false;
            for (int step = 1; step <= TRANS_STEPS; step++)
            {
                if (transitionSeq.load() != activeSeq)
                {
                    interrupted = true;
                    break;
                }

                if (xSemaphoreTake(oledMutex, portMAX_DELAY))
                {
                    uint8_t *live = oled.getBuffer();
                    int alpha = (step * 16) / TRANS_STEPS;

                    for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
                    {
                        for (int16_t x = 0; x < SCREEN_WIDTH; x++)
                        {
                            uint8_t o = framePixelGet(transPrevFrame, x, y);
                            uint8_t n = framePixelGet(transTargetFrame, x, y);
                            uint8_t v = (o >= n) ? (uint8_t)(n + ((o - n) * (16 - alpha)) / 16)
                                                 : (uint8_t)((n * alpha) / 16);
                            framePixelSet(live, x, y, v);
                        }
                    }

                    for (uint16_t i = 0; i < dotCount; i++)
                    {
                        int16_t fx = transDots[i].fromPos & 0x7F;
                        int16_t fy = transDots[i].fromPos >> 7;
                        int16_t tx = transDots[i].toPos & 0x7F;
                        int16_t ty = transDots[i].toPos >> 7;

                        int a = (step - (int)transDots[i].startStep) * 16 / (TRANS_STEPS - 3);
                        if (a < 0)
                            a = 0;
                        if (a > 16)
                            a = 16;

                        uint8_t nOrigin = framePixelGet(transTargetFrame, fx, fy);
                        if (a == 0)
                        {
                            framePixelSet(live, fx, fy, (uint8_t)(transDots[i].vFrom + nOrigin));
                        }
                        else
                        {
                            framePixelSet(live, fx, fy, nOrigin);
                            int16_t px = fx + ((tx - fx) * a) / 16;
                            int16_t py = fy + ((ty - fy) * a) / 16;
                            uint8_t v = (uint8_t)(transDots[i].vFrom + ((int16_t)transDots[i].vTo - transDots[i].vFrom) * a / 16);
                            framePixelSet(live, px, py, v);
                        }
                    }

                    oled.invalidateFullFrame();
                    oled.display();
                    xSemaphoreGive(oledMutex);
                }

                // Yield briefly to let the main app task post new transition messages immediately
                vTaskDelay(pdMS_TO_TICKS(TRANS_STEP_DELAY_MS > 0 ? TRANS_STEP_DELAY_MS : 1));
            }

            if (!interrupted)
            {
                if (xSemaphoreTake(oledMutex, portMAX_DELAY))
                {
                    uint8_t *live = oled.getBuffer();
                    memcpy(live, transTargetFrame, OLED_FRAME_BYTES);
                    oled.invalidateFullFrame();
                    oled.display();
                    xSemaphoreGive(oledMutex);
                }

                pendingDisplay.store(false);
                transitionRunning.store(false);
            }
        }
    }
}