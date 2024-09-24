#include "Player.hpp"
#include <functional>

Player::Player()
    : id_(""), personalCode_(0) {
}

Player::~Player() {
}

void Player::setId(const std::string& id) {
    id_ = id;
    calculatePersonalCode();
}

std::string Player::getId() const {
    return id_;
}

uint32_t Player::getPersonalCode() const {
    return personalCode_;
}

void Player::calculatePersonalCode() {
    // Simple hash function to convert the ID string to a 32-bit code
    // You can replace this with a more robust hashing algorithm if needed
    std::hash<std::string> hasher;
    personalCode_ = static_cast<uint32_t>(hasher(id_));
}
