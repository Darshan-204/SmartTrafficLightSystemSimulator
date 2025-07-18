#include "Intersection.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_map>

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
            
            // Emergency detection logged (visual display handles this)
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
            
            // Emergency cleared (visual display handles this)
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

    // Transition priority lane: RED -> YELLOW -> GREEN
    if (priorityLight->getState() != LightState::GREEN) {
        priorityLight->setState(LightState::YELLOW);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        priorityLight->setState(LightState::GREEN);
    }
    // Ensure minimum green duration of 4 seconds
    auto emergencyDuration = std::chrono::seconds(90);
    if (emergencyDuration < std::chrono::seconds(4)) emergencyDuration = std::chrono::seconds(4);
    priorityLight->setDuration(emergencyDuration); // Extended time for emergency
    
    std::string vehicleTypeStr;
    switch (priorityLane->getEmergencyVehicleType()) {
        case EmergencyVehicleType::FIRE_TRUCK: vehicleTypeStr = "FIRE TRUCK"; break;
        case EmergencyVehicleType::AMBULANCE: vehicleTypeStr = "AMBULANCE"; break;
        case EmergencyVehicleType::POLICE: vehicleTypeStr = "POLICE"; break;
        default: vehicleTypeStr = "EMERGENCY VEHICLE"; break;
    }
    
    // Priority handled (visual display shows this)
}

void Intersection::optimizeTrafficFlow() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if all lanes have occupancy above 80%
    bool allAbove80 = true;
    for (const auto& [lane, light] : lanes) {
        if (lane->getOccupancyRatio() < 0.8) {
            allAbove80 = false;
            break;
        }
    }

    auto now = std::chrono::steady_clock::now();
    // Initialize missing lanes in lastGreenTime /  tracks when each lane last got a green light.
    for (const auto& [lane, light] : lanes) {
        if (lastGreenTime.find(lane.get()) == lastGreenTime.end()) {
            lastGreenTime[lane.get()] = now;
        }
    }

    // Enforce minimum green duration of 4 seconds for all lanes
    for (const auto& [lane, light] : lanes) {
        if (light->getState() == LightState::GREEN) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastGreenTime[lane.get()]);
            if (elapsed.count() < 5) {
                // Skip changing this lane's light if green duration < 4 sec
                return;
            }
        }
    }

    if (allAbove80) {
        // Find the lane not given green for the longest time
        auto oldestLaneIt = std::min_element(lanes.begin(), lanes.end(),
            [&](const auto& a, const auto& b) {
                return lastGreenTime[a.first.get()] < lastGreenTime[b.first.get()];
            });
        auto laneToGreen = oldestLaneIt->first;
        auto lightToGreen = oldestLaneIt->second;
        // Set all other lights to red
        for (auto& [_, otherLight] : lanes) {
            if (otherLight != lightToGreen && !otherLight->isInEmergencyMode()) {
                otherLight->setState(LightState::RED);
            }
        }
        // Set selected lane to green, with YELLOW transition
        if (!lightToGreen->isInEmergencyMode()) {
            if (lightToGreen->getState() != LightState::GREEN) {
                lightToGreen->setState(LightState::YELLOW);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                lightToGreen->setState(LightState::GREEN);
            }
            lastGreenTime[laneToGreen.get()] = now;
            // Adjust duration based on occupancy
            auto occupancyRatio = laneToGreen->getOccupancyRatio();
            auto duration = std::chrono::seconds(
                static_cast<int>(30 + (occupancyRatio * 30))); // 30-60 seconds
            if (duration < std::chrono::seconds(4)) duration = std::chrono::seconds(4);
            lightToGreen->setDuration(duration);
        }
    } else {
        // Find the lane with highest occupancy
        auto maxOccupancyLane = std::max_element(lanes.begin(), lanes.end(),
            [](const auto& a, const auto& b) {
                return a.first->getOccupancyRatio() < b.first->getOccupancyRatio();
            });
        // Update traffic light states based on occupancy
        for (auto& [lane, light] : lanes) {
            if (lane == maxOccupancyLane->first) {
                if (light->getState() != LightState::GREEN) {
                    for (auto& [_, otherLight] : lanes) {
                        if (otherLight != light && !otherLight->isInEmergencyMode()) {
                            otherLight->setState(LightState::RED);
                        }
                    }
                    if (!light->isInEmergencyMode()) {
                        light->setState(LightState::YELLOW);
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                        light->setState(LightState::GREEN);
                        lastGreenTime[lane.get()] = now;
                        auto occupancyRatio = lane->getOccupancyRatio();
                        auto duration = std::chrono::seconds(
                            static_cast<int>(30 + (occupancyRatio * 30))); // 30-60 seconds
                        if (duration < std::chrono::seconds(4)) duration = std::chrono::seconds(4);
                        light->setDuration(duration);
                    }
                }
            }
        }
    }
}
