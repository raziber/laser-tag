#pragma once

#ifdef EMBEDDED_BUILD
    #include <Arduino.h>
    #include "driver/rmt.h"
#else
    #include "mocks/mockRmt.h"
#endif // EMBEDDED_BUILD

#include "configuration.h"
#include "configurationAdvanced.h"
#include "configurationBackend.h"
#include "irProtocolsConfig.h"
#include "irProtocols.h"

#include <vector>
#include <array>
#include <memory>
