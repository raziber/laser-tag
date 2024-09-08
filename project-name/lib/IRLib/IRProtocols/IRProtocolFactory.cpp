#include "IRProtocolFactory.hpp"
#include <stdexcept>

#include "IRProtocolsSettings/NECProtocolSettings.hpp"
#include "IRProtocolsSettings/LTTOProtocolSettings.hpp"
#include "IRProtocolsSettings/SamsungProtocolSettings.hpp"
#include "IRProtocolsSettings/CustomProtocolSettings.hpp"

static std::unique_ptr<IRProtocolSettings> IRProtocolFactory::createProtocolSettings(IRProtocol protocol){
    switch (protocol) {
        case IRProtocol::NEC:
            return std::make_unique<NECProtocolSettings>();
        // Add more cases for other protocols
        case IRProtocol::SAMSUNG:
            return std::make_unique<SAMSUNGProtocolSettings>();
            break;
        case IRProtocol::LTTO:
            return std::make_unique<LTTOProtocolSettings>();
            break;
        case IRProtocol::CUSTOM:
            return std::make_unique<CUSTOMProtocolSettings>();
            break;
        default:
            throw std::invalid_argument("Unsupported protocol");
    }

    return nullptr;
}


