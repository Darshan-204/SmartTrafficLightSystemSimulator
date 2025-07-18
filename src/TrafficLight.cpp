#include "TrafficLight.hpp"

TrafficLight::TrafficLight(const std::string& id)
    : id(id)
    , currentState(LightState::OFF)
    , stateDuration(std::chrono::seconds(30))
    , emergencyMode(false)
    , emergencyVehicleType(EmergencyVehicleType::NONE)
{}

void TrafficLight::setState(LightState newState) {
    currentState.store(newState);
}

LightState TrafficLight::getState() const {
    return currentState.load();
}

std::string TrafficLight::getId() const {
    return id;
}

void TrafficLight::setDuration(std::chrono::seconds duration) {
    std::lock_guard<std::mutex> lock(mutex);
    stateDuration = duration;
}

std::chrono::seconds TrafficLight::getDuration() const {
    std::lock_guard<std::mutex> lock(mutex);
    return stateDuration;
}

void TrafficLight::activateEmergencyMode(EmergencyVehicleType type) {
    emergencyMode.store(true);
    emergencyVehicleType.store(type);
}

void TrafficLight::deactivateEmergencyMode() {
    emergencyMode.store(false);
    emergencyVehicleType.store(EmergencyVehicleType::NONE);
}

bool TrafficLight::isInEmergencyMode() const {
    return emergencyMode.load();
}

EmergencyVehicleType TrafficLight::getEmergencyVehicleType() const {
    return emergencyVehicleType.load();
}
