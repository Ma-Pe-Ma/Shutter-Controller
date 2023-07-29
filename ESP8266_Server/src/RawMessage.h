#ifndef RAWMESSAGE_H
#define RAWMESSAGE_H

#include "Arduino.h"

struct RawMessage {
    String type;
    String result;
    String additional;
};

#endif