#pragma once

#include <vector>
#include <memory>
#include <thread>
#include "Lane.hpp"
#include "TrafficLight.hpp"
#include <unordered_map>

class Intersection {
public:
    Intersection(const std::string& id);
    ~Intersection();

    void addLane(std::shared_ptr<Lane> lane, std::shared_ptr<TrafficLight> light);
    void start();
    void stop();
    bool isRunning() const;
    
    // Emergency vehicle methods
    void reportEmergencyVehicle(const std::string& laneId, EmergencyVehicleType type);
    void clearEmergencyVehicle(const std::string& laneId);
    
private:
    void controlLoop();
    void optimizeTrafficFlow();
    void handleEmergencyVehicles();

    std::string id;
    std::vector<std::pair<std::shared_ptr<Lane>, std::shared_ptr<TrafficLight>>> lanes;
    std::atomic<bool> running;
    std::unique_ptr<std::thread> controlThread;
    std::atomic<bool> emergencyActive;
    mutable std::mutex mutex;

    // Track last green time for each lane
    std::unordered_map<Lane*, std::chrono::steady_clock::time_point> lastGreenTime;
};
