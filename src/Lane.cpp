#include "Lane.hpp"

Lane::Lane(const std::string& id, int capacity)
    : id(id)
    , vehicleCount(0)
    , capacity(capacity)
    , emergencyVehicle(EmergencyVehicleType::NONE)
{}

void Lane::addVehicle() {
    std::lock_guard<std::mutex> lock(mutex);
    if (vehicleCount < capacity) {
        vehicleCount++;
    }
}

void Lane::removeVehicle() {
    std::lock_guard<std::mutex> lock(mutex);
    if (vehicleCount > 0) {
        vehicleCount--;
    }
}

int Lane::getVehicleCount() const {
    return vehicleCount.load();
}

std::string Lane::getId() const {
    return id;
}

int Lane::getCapacity() const {
    return capacity;
}

double Lane::getOccupancyRatio() const {
    return static_cast<double>(getVehicleCount()) / capacity;
}

void Lane::setEmergencyVehicle(EmergencyVehicleType type) {
    emergencyVehicle.store(type);
}

void Lane::clearEmergencyVehicle() {
    emergencyVehicle.store(EmergencyVehicleType::NONE);
}

EmergencyVehicleType Lane::getEmergencyVehicleType() const {
    return emergencyVehicle.load();
}

bool Lane::hasEmergencyVehicle() const {
    return emergencyVehicle.load() != EmergencyVehicleType::NONE;
}
