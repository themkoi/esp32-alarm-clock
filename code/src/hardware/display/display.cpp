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
    0x08, 0x0A, 0x0C, 0x0E,
    0x11, 0x14, 0x17, 0x1B,
    0x20, 0x25, 0x2B, 0x31,
    0x37, 0x3B, 0x3E, 0x3F};

SemaphoreHandle_t oledMutex = nullptr;

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
            SSD1327_DISPLAYOFF, // 0xAE
            SSD1327_SETCONTRAST,
            0xFF,             // 0x81 -> Maximum brightness
            SSD1327_SEGREMAP, // 0xA0
            0x51,
            SSD1327_SETSTARTLINE,
            0x00, // 0xA1
            SSD1327_SETDISPLAYOFFSET,
            0x00, // 0xA2
            SSD1327_DISPLAYALLOFF,
            SSD1327_SETMULTIPLEX,
            0x7F,
            SSD1327_PHASELEN,
            0x51, // 0xB1
            SSD1327_GRAYTABLE,
            0x03, 0x05, 0x07, 0x09,
            0x0C, 0x0F, 0x12, 0x16,
            0x1A, 0x1F, 0x25, 0x2B,
            0x32, 0x38, 0x3C, 0x3F,
            SSD1327_DCLK,
            0x50, // 0xB3
            SSD1327_REGULATOR,
            0x01, // 0xAB -> Enable internal regulator
            SSD1327_PRECHARGE2,
            0x04, // 0xB6
            SSD1327_SETVCOM,
            0x0F, // 0xBE
            SSD1327_PRECHARGE,
            SSD1327_PRECHARGE_DEFAULT, // 0xBC 0x22
            SSD1327_FUNCSELB,
            0x62, // 0xD5
            SSD1327_CMDLOCK,
            0x12, // 0xFD
            SSD1327_NORMALDISPLAY,
            SSD1327_DISPLAYON};

        oled.oled_commandList(init_128x128, sizeof(init_128x128));

        oled.clearDisplay();
        oled.display();

        xSemaphoreGive(oledMutex);
    }

    startOrbitTask();
}

void OLED_MANAGER::display()
{
    if (xSemaphoreTake(oledMutex, portMAX_DELAY))
    {
        oled.display();
        xSemaphoreGive(oledMutex);
    }
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