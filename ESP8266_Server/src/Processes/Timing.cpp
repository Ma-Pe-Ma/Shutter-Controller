#include "Timing.h"
#include "../LittleFSHelper.h"
#include "../TimeCalibration.h"

RawMessage Timing::generateMessage() {
    int intCurrent = (int) (targetValue * 100);
    return {"T", String(id + 1), String(intCurrent)};
}