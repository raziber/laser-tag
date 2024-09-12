#include "IRProtocolFactory.hpp"
#include <stdexcept>

#include "NECProtocolSettings.hpp"
#include "LTTOProtocolSettings.hpp"
#include "SamsungProtocolSettings.hpp"
#include "CustomProtocolSettings.hpp"

std::unique_ptr<IRProtocolSettings> IRProtocolFactory::createProtocolSettings(IRProtocol protocol){
    switch (protocol) {
        case IRProtocol::NEC:
            return std::make_unique<NECProtocolSettings>();
        case IRProtocol::SAMSUNG:
            return std::make_unique<SamsungProtocolSettings>();
        case IRProtocol::LTTO:
            return std::make_unique<LTTOProtocolSettings>();
        case IRProtocol::CUSTOM:
            return std::make_unique<CustomProtocolSettings>();
        default:
            throw std::invalid_argument("Unsupported protocol");
    }

    return nullptr;
}
