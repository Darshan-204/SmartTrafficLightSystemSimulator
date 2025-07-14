#include "Intersection.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>

Intersection::Intersection(const std::string& id)
    : id(id)
    , running(false)
    , emergencyActive(false)
{}

Intersection::~Intersection() {
    stop();
}

void Intersection::addLane(std::shared_ptr<Lane> lane, std::shared_ptr<TrafficLight> light) {
    std::lock_guard<std::mutex> lock(mutex);
    lanes.emplace_back(lane, light);
}

void Intersection::start() {
    if (!running.exchange(true)) {
        controlThread = std::make_unique<std::thread>(&Intersection::controlLoop, this);
    }
}

void Intersection::stop() {
    if (running.exchange(false)) {
        if (controlThread && controlThread->joinable()) {
            controlThread->join();
        }
    }
}

bool Intersection::isRunning() const {
    return running.load();
}

void Intersection::reportEmergencyVehicle(const std::string& laneId, EmergencyVehicleType type) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [lane, light] : lanes) {
        if (lane->getId() == laneId) {
            lane->setEmergencyVehicle(type);
            light->activateEmergencyMode(type);
            emergencyActive.store(true);
            
            std::string vehicleTypeStr;
            switch (type) {
                case EmergencyVehicleType::AMBULANCE: vehicleTypeStr = "AMBULANCE"; break;
                case EmergencyVehicleType::POLICE: vehicleTypeStr = "POLICE"; break;
                case EmergencyVehicleType::FIRE_TRUCK: vehicleTypeStr = "FIRE TRUCK"; break;
                default: vehicleTypeStr = "UNKNOWN"; break;
            }
            
            std::cout << "🚨 EMERGENCY: " << vehicleTypeStr << " detected in lane " 
                     << laneId << " - Activating priority signal!" << std::endl;
            break;
        }
    }
}

void Intersection::clearEmergencyVehicle(const std::string& laneId) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [lane, light] : lanes) {
        if (lane->getId() == laneId) {
            lane->clearEmergencyVehicle();
            light->deactivateEmergencyMode();
            
            // Check if any other lanes have emergency vehicles
            bool anyEmergencyActive = false;
            for (const auto& [otherLane, otherLight] : lanes) {
                if (otherLane->hasEmergencyVehicle()) {
                    anyEmergencyActive = true;
                    break;
                }
            }
            emergencyActive.store(anyEmergencyActive);
            
            std::cout << "✅ Emergency vehicle cleared from lane " << laneId << std::endl;
            break;
        }
    }
}

void Intersection::controlLoop() {
    using namespace std::chrono_literals;
    
    while (running) {
        if (emergencyActive.load()) {
            handleEmergencyVehicles();
        } else {
            optimizeTrafficFlow();
        }
        std::this_thread::sleep_for(500ms); // Faster response time for emergencies
    }
}

void Intersection::handleEmergencyVehicles() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Find all lanes with emergency vehicles
    std::vector<std::pair<std::shared_ptr<Lane>, std::shared_ptr<TrafficLight>>> emergencyLanes;
    
    for (auto& [lane, light] : lanes) {
        if (lane->hasEmergencyVehicle()) {
            emergencyLanes.emplace_back(lane, light);
        }
    }
    
    if (emergencyLanes.empty()) {
        emergencyActive.store(false);
        return;
    }
    
    // Priority order: Fire Truck > Ambulance > Police
    auto getEmergencyPriority = [](EmergencyVehicleType type) {
        switch (type) {
            case EmergencyVehicleType::FIRE_TRUCK: return 3;
            case EmergencyVehicleType::AMBULANCE: return 2;
            case EmergencyVehicleType::POLICE: return 1;
            default: return 0;
        }
    };
    
    // Sort by priority
    std::sort(emergencyLanes.begin(), emergencyLanes.end(),
        [&](const auto& a, const auto& b) {
            return getEmergencyPriority(a.first->getEmergencyVehicleType()) >
                   getEmergencyPriority(b.first->getEmergencyVehicleType());
        });
    
    // Give green light to highest priority emergency vehicle
    auto [priorityLane, priorityLight] = emergencyLanes[0];
    
    // Set all other lights to red
    for (auto& [lane, light] : lanes) {
        if (light != priorityLight) {
            light->setState(LightState::RED);
        }
    }
    
    // Set priority lane to green with extended duration
    priorityLight->setState(LightState::GREEN);
    priorityLight->setDuration(std::chrono::seconds(90)); // Extended time for emergency
    
    std::string vehicleTypeStr;
    switch (priorityLane->getEmergencyVehicleType()) {
        case EmergencyVehicleType::FIRE_TRUCK: vehicleTypeStr = "FIRE TRUCK"; break;
        case EmergencyVehicleType::AMBULANCE: vehicleTypeStr = "AMBULANCE"; break;
        case EmergencyVehicleType::POLICE: vehicleTypeStr = "POLICE"; break;
        default: vehicleTypeStr = "EMERGENCY VEHICLE"; break;
    }
    
    std::cout << "🚨 PRIORITY: " << vehicleTypeStr << " in lane " 
             << priorityLane->getId() << " has green light (90 seconds)" << std::endl;
}

void Intersection::optimizeTrafficFlow() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Find the lane with highest occupancy
    auto maxOccupancyLane = std::max_element(lanes.begin(), lanes.end(),
        [](const auto& a, const auto& b) {
            return a.first->getOccupancyRatio() < b.first->getOccupancyRatio();
        });
    
    // Update traffic light states based on occupancy
    for (auto& [lane, light] : lanes) {
        if (lane == maxOccupancyLane->first) {
            if (light->getState() != LightState::GREEN) {
                // First set other lights to red
                for (auto& [_, otherLight] : lanes) {
                    if (otherLight != light && !otherLight->isInEmergencyMode()) {
                        otherLight->setState(LightState::RED);
                    }
                }
                // Then set this light to green
                if (!light->isInEmergencyMode()) {
                    light->setState(LightState::GREEN);
                    
                    // Adjust duration based on occupancy
                    auto occupancyRatio = lane->getOccupancyRatio();
                    auto duration = std::chrono::seconds(
                        static_cast<int>(30 + (occupancyRatio * 30))); // 30-60 seconds
                    light->setDuration(duration);
                    
                    std::cout << "Lane " << lane->getId() << " got green light for "
                             << duration.count() << " seconds (occupancy: "
                             << occupancyRatio * 100 << "%)" << std::endl;
                }
            }
        }
    }
}
