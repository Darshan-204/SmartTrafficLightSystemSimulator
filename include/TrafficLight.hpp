#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>

enum class LightState {
    OFF,
    RED,
    YELLOW,
    GREEN
};

enum class EmergencyVehicleType {
    NONE,
    AMBULANCE,
    POLICE,
    FIRE_TRUCK
};

class TrafficLight {
public:
    TrafficLight(const std::string& id);
    ~TrafficLight() = default;

    void setState(LightState newState);
    LightState getState() const;
    std::string getId() const;
    void setDuration(std::chrono::seconds duration);
    std::chrono::seconds getDuration() const;
    
    // Emergency priority methods
    void activateEmergencyMode(EmergencyVehicleType type);
    void deactivateEmergencyMode();
    bool isInEmergencyMode() const;
    EmergencyVehicleType getEmergencyVehicleType() const;

private:
    std::string id;
    std::atomic<LightState> currentState;
    std::chrono::seconds stateDuration;
    std::atomic<bool> emergencyMode;
    std::atomic<EmergencyVehicleType> emergencyVehicleType;
    mutable std::mutex mutex;
};
