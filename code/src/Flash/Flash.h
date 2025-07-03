#ifndef Flash_H
#define Flash_H

#include "../defines.h"

void readAlarms();

void saveAlarms();

bool readOtaValue();
void saveOtaValue(bool Value);

#endif
