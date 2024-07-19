#pragma once

#ifdef EMBEDDED_BUILD
    #include <Arduino.h>
    #include "driver/rmt.h"
#else
    #include "mocks/mockRmt.h"
#endif // EMBEDDED_BUILD

#include "irProtocols.h"
#include "configuration.h"
#include "configurationAdvanced.h"
#include "configurationBackend.h"
#include "irProtocolsConfig.h"

#include <vector>
#include <array>
#include <memory>
