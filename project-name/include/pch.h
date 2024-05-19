#ifndef PCH_H
#define PCH_H

#ifdef EMBEDDED_BUILD
    #include <Arduino.h>
    #include "driver/rmt.h"
#else
    #include "mocks/mockRmt.h"
#endif // EMBEDDED_BUILD

#include "configuration.h"
#include "configurationAdvanced.h"
#include "configurationBackend.h"

#endif // PCH_H
