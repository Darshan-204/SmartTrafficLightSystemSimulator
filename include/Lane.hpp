#pragma once

#include <vector>
#include <memory>
#include <thread>
#include "TrafficLight.hpp"

class Lane {
public:
    Lane(const std::string& id, int capacity);
    ~Lane() = default;

    void addVehicle();
    void removeVehicle();
    int getVehicleCount() const;
    std::string getId() const;
    int getCapacity() const;
    double getOccupancyRatio() const;
    
    // Emergency vehicle methods
    void setEmergencyVehicle(EmergencyVehicleType type);
    void clearEmergencyVehicle();
    EmergencyVehicleType getEmergencyVehicleType() const;
    bool hasEmergencyVehicle() const;

private:
    std::string id;
    std::atomic<int> vehicleCount;
    int capacity;
    std::atomic<EmergencyVehicleType> emergencyVehicle;
    mutable std::mutex mutex;
};
