#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <sstream>
#include "Intersection.hpp"

// ANSI color codes for better visualization
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

class TrafficDisplay {
public:
    static void clearScreen() {
        std::cout << "\033[2J\033[H"; // Clear screen and move cursor to top
    }
    
    static void displayIntersection(const std::vector<std::shared_ptr<Lane>>& lanes,
                                  const std::vector<std::shared_ptr<TrafficLight>>& lights) {
        clearScreen();
        
        std::cout << COLOR_BOLD COLOR_CYAN "🚦 Smart Traffic Light System - Live View 🚦" COLOR_RESET << std::endl;
        std::cout << "=============================================" << std::endl << std::endl;
        
        // Display each lane
        for (size_t i = 0; i < lanes.size(); ++i) {
            displayLane(lanes[i], lights[i]);
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Legend: 🚗=Vehicle 🚨=Emergency 🔴=Red 🟢=Green 🟡=Yellow" << std::endl;
        std::cout << "Press Enter to stop..." << std::endl;
    }
    
private:
    static void displayLane(std::shared_ptr<Lane> lane, std::shared_ptr<TrafficLight> light) {
        std::string laneId = lane->getId();
        int vehicleCount = lane->getVehicleCount();
        int capacity = lane->getCapacity();
        bool hasEmergency = lane->hasEmergencyVehicle();
        LightState lightState = light->getState();
        
        // Lane header with traffic light
        std::cout << COLOR_BOLD;
        if (laneId == "North") std::cout << "   ↑ ";
        else if (laneId == "South") std::cout << "   ↓ ";
        else if (laneId == "East") std::cout << "   → ";
        else if (laneId == "West") std::cout << "   ← ";
        
        std::cout << laneId << " Lane " COLOR_RESET;
        
        // Traffic light display
        switch (lightState) {
            case LightState::RED:
                std::cout << COLOR_RED "🔴" COLOR_RESET;
                break;
            case LightState::YELLOW:
                std::cout << COLOR_YELLOW "🟡" COLOR_RESET;
                break;
            case LightState::GREEN:
                std::cout << COLOR_GREEN "🟢" COLOR_RESET;
                break;
        }
        
        if (light->isInEmergencyMode()) {
            std::cout << " " COLOR_RED COLOR_BOLD "🚨 EMERGENCY" COLOR_RESET;
        }
        
        std::cout << std::endl;
        
        // Lane visualization
        std::cout << "   [";
        
        // Show vehicles in the lane
        for (int i = 0; i < capacity; ++i) {
            if (i < vehicleCount) {
                if (hasEmergency && i == vehicleCount - 1) {
                    // Show emergency vehicle
                    switch (lane->getEmergencyVehicleType()) {
                        case EmergencyVehicleType::POLICE:
                            std::cout << COLOR_BLUE "🚓" COLOR_RESET;
                            break;
                        case EmergencyVehicleType::AMBULANCE:
                            std::cout << COLOR_WHITE "🚑" COLOR_RESET;
                            break;
                        case EmergencyVehicleType::FIRE_TRUCK:
                            std::cout << COLOR_RED "🚒" COLOR_RESET;
                            break;
                        default:
                            std::cout << "🚗";
                            break;
                    }
                } else {
                    std::cout << "🚗";
                }
            } else {
                std::cout << "  ";
            }
        }
        
        std::cout << "] ";
        
        // Display occupancy percentage
        double occupancy = lane->getOccupancyRatio() * 100;
        if (occupancy >= 80) {
            std::cout << COLOR_RED;
        } else if (occupancy >= 50) {
            std::cout << COLOR_YELLOW;
        } else {
            std::cout << COLOR_GREEN;
        }
        
        std::cout << std::fixed << std::setprecision(0) << occupancy << "%" COLOR_RESET;
        
        if (hasEmergency) {
            std::string emergencyType;
            switch (lane->getEmergencyVehicleType()) {
                case EmergencyVehicleType::POLICE: emergencyType = "POLICE"; break;
                case EmergencyVehicleType::AMBULANCE: emergencyType = "AMBULANCE"; break;
                case EmergencyVehicleType::FIRE_TRUCK: emergencyType = "FIRE TRUCK"; break;
                default: emergencyType = "EMERGENCY"; break;
            }
            std::cout << " " COLOR_RED COLOR_BOLD "(" << emergencyType << ")" COLOR_RESET;
        }
    }
};

void simulateTraffic(std::shared_ptr<Lane> lane, std::shared_ptr<TrafficLight> light) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    while (true) {
        // Randomly add or remove vehicles
        if (dis(gen) < 0.7) { // 70% chance to add a vehicle
            lane->addVehicle();
        } else {
            // Only remove vehicle if the light is not red
            if (light && light->getState() != LightState::RED) {
                lane->removeVehicle();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void simulateEmergencyVehicles(std::shared_ptr<Intersection> intersection,
                              const std::vector<std::shared_ptr<Lane>>& lanes) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> laneSelector(0, lanes.size() - 1);
    std::uniform_int_distribution<> vehicleTypeSelector(1, 3); // 1=Police, 2=Ambulance, 3=Fire
    std::uniform_int_distribution<> timingSelector(15, 45); // 15-45 seconds between emergencies
    
    while (true) {
        // Wait random time between emergency vehicles
        std::this_thread::sleep_for(std::chrono::seconds(timingSelector(gen)));
        
        // Select random lane and emergency vehicle type
        auto selectedLane = lanes[laneSelector(gen)];
        EmergencyVehicleType vehicleType;
        
        switch (vehicleTypeSelector(gen)) {
            case 1: vehicleType = EmergencyVehicleType::POLICE; break;
            case 2: vehicleType = EmergencyVehicleType::AMBULANCE; break;
            case 3: vehicleType = EmergencyVehicleType::FIRE_TRUCK; break;
            default: vehicleType = EmergencyVehicleType::AMBULANCE; break;
        }
        
        // Report emergency vehicle
        intersection->reportEmergencyVehicle(selectedLane->getId(), vehicleType);
        
        // Emergency vehicle stays for 8-15 seconds
        std::uniform_int_distribution<> durationSelector(8, 15);
        std::this_thread::sleep_for(std::chrono::seconds(durationSelector(gen)));
        
        // Clear emergency vehicle
        intersection->clearEmergencyVehicle(selectedLane->getId());
    }
}

void displayLoop(const std::vector<std::shared_ptr<Lane>>& lanes,
                const std::vector<std::shared_ptr<TrafficLight>>& lights) {
    while (true) {
        TrafficDisplay::displayIntersection(lanes, lights);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Update every second
    }
}

int main() {
    std::cout << "🚦 Smart Traffic Light System with Emergency Priority 🚦" << std::endl;
    std::cout << "==========================================================" << std::endl;
    
    // Create an intersection
    auto intersection = std::make_shared<Intersection>("Main Street & First Ave");

    // Create four lanes with their traffic lights
    auto northLane = std::make_shared<Lane>("North", 15);
    auto southLane = std::make_shared<Lane>("South", 15);
    auto eastLane = std::make_shared<Lane>("East", 15);
    auto westLane = std::make_shared<Lane>("West", 15);

    auto northLight = std::make_shared<TrafficLight>("North Light");
    auto southLight = std::make_shared<TrafficLight>("South Light");
    auto eastLight = std::make_shared<TrafficLight>("East Light");
    auto westLight = std::make_shared<TrafficLight>("West Light");

    // Add lanes to intersection
    intersection->addLane(northLane, northLight);
    intersection->addLane(southLane, southLight);
    intersection->addLane(eastLane, eastLight);
    intersection->addLane(westLane, westLight);

    // Create vectors for easy access
    std::vector<std::shared_ptr<Lane>> allLanes = {northLane, southLane, eastLane, westLane};
    std::vector<std::shared_ptr<TrafficLight>> allLights = {northLight, southLight, eastLight, westLight};

    // Start traffic simulation threads
    std::vector<std::thread> simulationThreads;
    simulationThreads.emplace_back(simulateTraffic, northLane, northLight);
    simulationThreads.emplace_back(simulateTraffic, southLane, southLight);
    simulationThreads.emplace_back(simulateTraffic, eastLane, eastLight);
    simulationThreads.emplace_back(simulateTraffic, westLane, westLight);
    
    // Start emergency vehicle simulation thread
    simulationThreads.emplace_back(simulateEmergencyVehicles, intersection, allLanes);
    
    // Start display thread
    simulationThreads.emplace_back(displayLoop, allLanes, allLights);

    // Start the intersection controller
    intersection->start();

    std::cout << "Traffic Light Simulation Started with Emergency Vehicle Priority!" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "- Adaptive traffic flow based on lane occupancy" << std::endl;
    std::cout << "- Emergency vehicle priority (Fire Truck > Ambulance > Police)" << std::endl;
    std::cout << "- Real-time emergency detection and response" << std::endl;
    std::cout << "\nPress Enter to stop..." << std::endl;
    std::cin.get();

    // Cleanup
    intersection->stop();
    for (auto& thread : simulationThreads) {
        thread.detach(); // In a real system, we'd want to properly join these
    }

    TrafficDisplay::clearScreen();
    std::cout << "Simulation stopped." << std::endl;
    return 0;
}
