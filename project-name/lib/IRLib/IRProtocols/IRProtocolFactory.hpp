#pragma once

#include <memory>
#include "IRProtocolSettings.hpp"
#include "IRProtocolEnum.hpp"

class IRProtocolFactory {
public:
    static std::unique_ptr<IRProtocolSettings> createProtocolSettings(IRProtocol protocol);
};
