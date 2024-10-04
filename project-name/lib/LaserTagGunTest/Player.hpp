#pragma once

#include <string>
#include <cstdint>

class Player {
public:
    Player();
    ~Player();

    void setId(const std::string& id);
    std::string getId() const;
    uint32_t getPersonalCode() const;

private:
    std::string id_;
    uint32_t personalCode_;

    void calculatePersonalCode();
};
