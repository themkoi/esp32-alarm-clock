#include "charts.h"

Grafici plot{display}; // Initialize the plot with the display

// Arrays to hold temperature, humidity, and light data
float temperatureArray[CHART_READINGS];
float humidityArray[CHART_READINGS];
float lightArray[CHART_READINGS];

#define X_MINIMUM_VALUE_OFFSET -0.5
#define X_MAX_VALUE_OFFSET -0.5
#define CELL_VERTICAL_COUNT 3
#define WALL_VALUE_OFFSET float(0.08)
#define SIDE_TEXT_OFFSET 20
#define SIDE_TEXT_REPAIR 12.6

void sortList(float *numbers, int size)
{
    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = 0; j < size - i - 1; ++j)
        {
            if (numbers[j] > numbers[j + 1])
            {
                float temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
}

void createScaledArray(const float *inputArray, int size, int cellVerticalCount, float *outputArray)
{
    if (cellVerticalCount < 1 || size < cellVerticalCount)
        return;

    // Create a local copy of the input array for sorting
    float sortedArray[size];
    memcpy(sortedArray, inputArray, size * sizeof(float));

    // Sort the array in descending order
    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = i + 1; j < size; ++j)
        {
            if (sortedArray[i] < sortedArray[j])
            {
                float temp = sortedArray[i];
                sortedArray[i] = sortedArray[j];
                sortedArray[j] = temp;
            }
        }
    }

    // Assign evenly spaced values from the sorted array to the output
    for (int i = 0; i < cellVerticalCount; ++i)
    {
        int index = (i * (size - 1)) / (cellVerticalCount - 1);
        outputArray[i] = sortedArray[index];
    }
}

const ColorMapArray<2> really_black{
    white,
    white};

// Interval in milliseconds (30 seconds)
const unsigned long interval = 30000;
unsigned long previousMillis = 0; // Will store last time the function was called

// Function to find the highest value in the data
float findHighest(float *data, uint dataCount)
{
    float highest = data[0];
    for (uint i = 1; i < dataCount; i++)
    {
        if (data[i] > highest)
        {
            highest = data[i];
        }
    }
    return highest;
}

// Function to find the lowest value in the data
float findLowest(float *data, uint dataCount)
{
    float lowest = data[0];
    for (uint i = 1; i < dataCount; i++)
    {
        if (data[i] < lowest)
        {
            lowest = data[i];
        }
    }
    return lowest;
}

void showSideText(float *bothList, int bothSize)
{
    display.setFont(&DejaVu_LGC_Sans_Bold_8);

    // Sort the input list
    sortList(bothList, bothSize);

    // Debugging sorted list
    Serial.println("Sorted List:");
    Serial.println(String(bothList[0]));            // Highest
    Serial.println(String(bothList[bothSize - 1])); // Lowest

    float stepper = float(bothSize) / float(CELL_VERTICAL_COUNT);
    if (int(round(stepper)) == 0)
    {
        stepper = 1; // Avoid division by zero
    }

    Serial.println("Stepper: " + String(stepper));
    Serial.println("bothSize: " + String(bothSize));

    int valuesCount = -1;
    float values[CELL_VERTICAL_COUNT] = {0};

    // Populate `values` from `bothList` based on `stepper`
    for (float i = 0; i < bothSize; i += stepper)
    {
        int ti = int(floor(i));
        if (String(bothList[ti]) != "0.00" && String(bothList[ti]) != "-0.00" &&
            String(bothList[ti]) != "0" && String(bothList[ti]) != "-0")
        {
            bool contains = false;
            for (int j = 0; j < valuesCount + 1; j++)
            {
                if (values[j] == bothList[ti])
                {
                    contains = true;
                }
            }
            if (!contains)
            {
                valuesCount++;
                Serial.println("Value to show is: " + String(bothList[ti]) + " at value: " + String(valuesCount));
                values[valuesCount] = bothList[ti];
            }
            else
            {
                Serial.println("Duplicate");
            }
        }
        else
        {
            Serial.println("Weird 0");
        }
    }
    valuesCount++;

    // Sort the selected values
    sortList(values, valuesCount);

    // Create a scaled array from the values
    float scaledValues[CELL_VERTICAL_COUNT] = {0};
    createScaledArray(values, valuesCount, CELL_VERTICAL_COUNT, scaledValues);

    // Debugging scaled array
    Serial.println("Scaled Values:");
    for (int i = 0; i < CELL_VERTICAL_COUNT; i++)
    {
        Serial.println(scaledValues[i]);
    }

    display.setFont(&DejaVu_Sans_Condensed_Bold_7);

    float currentHeightDown = display.height() - 7;
    float currentHeightUp = SIDE_TEXT_OFFSET;

    float offset = (display.height() - (SIDE_TEXT_OFFSET - SIDE_TEXT_REPAIR)) / CELL_VERTICAL_COUNT;
    bool down = false;

    // Display the scaled values
    for (float i = 0; i < float(valuesCount) / 2; i += 0.5)
    {
        int ji = int(floor(i));
        Serial.println("i: " + String(i));
        Serial.println("ji: " + String(ji));

        if (down == true)
        {
            display.setCursor(0, round(currentHeightDown));
        }
        else
        {
            display.setCursor(0, round(currentHeightUp));
        }

        String number;
        if (down == true)
        {
            number = String(values[ji]);
        }
        else
        {
            number = String(values[valuesCount - ji - 1]);
        }
        Serial.println("Number is:" + number);
        Serial.println("Down is: " + BOOL_STR(down));

        int indexOfDecimal = number.indexOf(".");
        if (number.length() - 1 > indexOfDecimal && number[indexOfDecimal + 1] == '0')
        {
            number.remove(indexOfDecimal, number.length() - indexOfDecimal);
        }

        if (number.length() > 3)
        {
            /*
            if (isDecimalZero(bothList[i]) == true && String(int(bothList[i])).length() <= 3)
            {
              display.print(String(int(bothList[i])));
            }
            else
            */
            String part1 = number.substring(0, 3);
            String part2 = number.substring(3);
            if (part2.length() > 3)
            {
                part2 = part2.substring(0, 3);
            }
            display.println(part1);
            display.setCursor(0, display.getCursorY() - 3);
            display.print(part2);
        }
        else
        {
            if (down == true)
            {
                display.setCursor(0, display.getCursorY() + 7);
            }
            display.print(number);
        }

        if (down == true)
        {
            currentHeightDown = currentHeightDown - offset;
        }
        else
        {
            currentHeightUp = currentHeightUp + offset;
        }
        down = !down;
    }

    if (values[0] == 0.0)
    {
        display.setCursor(0, round(currentHeightUp));
        display.print("0");
    }
    display.setFont(&DejaVu_LGC_Sans_Bold_10);

    display.setFont(&DejaVu_LGC_Sans_Bold_10);
}

// Show the chart with the given title and data
void showChart(float *data, uint dataCount, String chartName)
{
    display.clearDisplay(); // Clear the display
    float highNum = findHighest(data, dataCount);
    float lowNum = findLowest(data, dataCount);

    float offsetMin = WALL_VALUE_OFFSET;
    float offsetMax = WALL_VALUE_OFFSET;
    if (dataCount == 1)
    {
        offsetMax = 0.0;
        offsetMin = 0.5;
    }

    // Prepare data for plotting
    DataArray<float> dataArr{data, dataCount, {lowNum - offsetMin, highNum + offsetMax}};
    DataLinear x{dataCount, {0.0 + X_MINIMUM_VALUE_OFFSET, float(dataCount + X_MAX_VALUE_OFFSET)}};

    plot.set_color_map(black_and_white);
    PlotOptions opts = plot_options.thickness(0.0).bar_filled(true);

    // Define the plotting window
    Window size = Window({0.11, 0.999}, {0, 1.0 - 0.03});

    // Plot the bar chart
    plot.bar(x, dataArr, white, size, opts);
    centerText(chartName, 10);

    // Display the data values in a list (optional)
    int bothSize = dataCount;
    float bothList[bothSize];

    for (int i = 0; i < dataCount; i++)
    {
        bothList[i] = data[i];
    }

    showSideText(bothList, bothSize);
    manager.sendOledAction(OLED_DISPLAY);
}

void loopGraph(const char *title, float *dataArray, size_t dataSize, float yMin, float yMax, float (*readValue)())
{
    static unsigned long lastRunTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastRunTime >= 20000)
    {
        lastRunTime = currentTime;
        showChart(dataArray, dataSize, title); // The graphing action is triggered here
    }
}

// Initialize temperature graph
void initTempGraph()
{
    showChart(temperatureArray, CHART_READINGS, "Temperature");
}

// Loop for updating the temperature graph
void loopTempGraph()
{
    checkExit();
    loopGraph("Temperature", temperatureArray, CHART_READINGS, 0, 50, readTemperature);
}

// Initialize humidity graph
void initHumidityGraph()
{
    showChart(humidityArray, CHART_READINGS, "Humidity");
}

// Loop for updating the humidity graph
void loopHumidityGraph()
{
    checkExit();
    loopGraph("Humidity", humidityArray, CHART_READINGS, 0, 100, readHumidity);
}

// Initialize light graph
void initLightGraph()
{
    showChart(lightArray, CHART_READINGS, "Light");
}

// Loop for updating the light graph
void loopLightGraph()
{
    checkExit();
    loopGraph("Light", lightArray, CHART_READINGS, 0, 1000, getLightLevel);
}
