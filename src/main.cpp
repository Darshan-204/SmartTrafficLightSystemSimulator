#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <vector>
#include "Intersection.hpp"

void simulateTraffic(std::shared_ptr<Lane> lane) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    while (true) {
        // Randomly add or remove vehicles
        if (dis(gen) < 0.7) { // 70% chance to add a vehicle
            lane->addVehicle();
        } else {
            lane->removeVehicle();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void simulateEmergencyVehicles(std::shared_ptr<Intersection> intersection,
                              const std::vector<std::shared_ptr<Lane>>& lanes) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> laneSelector(0, lanes.size() - 1);
    std::uniform_int_distribution<> vehicleTypeSelector(1, 3); // 1=Police, 2=Ambulance, 3=Fire
    std::uniform_int_distribution<> timingSelector(10, 30); // 10-30 seconds between emergencies
    
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
        
        // Emergency vehicle stays for 5-10 seconds
        std::uniform_int_distribution<> durationSelector(5, 10);
        std::this_thread::sleep_for(std::chrono::seconds(durationSelector(gen)));
        
        // Clear emergency vehicle
        intersection->clearEmergencyVehicle(selectedLane->getId());
    }
}

int main() {
    std::cout << "🚦 Smart Traffic Light System with Emergency Priority 🚦" << std::endl;
    std::cout << "==========================================================" << std::endl;
    
    // Create an intersection
    auto intersection = std::make_shared<Intersection>("Main Street & First Ave");

    // Create four lanes with their traffic lights
    auto northLane = std::make_shared<Lane>("North", 20);
    auto southLane = std::make_shared<Lane>("South", 20);
    auto eastLane = std::make_shared<Lane>("East", 20);
    auto westLane = std::make_shared<Lane>("West", 20);

    auto northLight = std::make_shared<TrafficLight>("North Light");
    auto southLight = std::make_shared<TrafficLight>("South Light");
    auto eastLight = std::make_shared<TrafficLight>("East Light");
    auto westLight = std::make_shared<TrafficLight>("West Light");

    // Add lanes to intersection
    intersection->addLane(northLane, northLight);
    intersection->addLane(southLane, southLight);
    intersection->addLane(eastLane, eastLight);
    intersection->addLane(westLane, westLight);

    // Create vector of lanes for emergency simulation
    std::vector<std::shared_ptr<Lane>> allLanes = {northLane, southLane, eastLane, westLane};

    // Start traffic simulation threads
    std::vector<std::thread> simulationThreads;
    simulationThreads.emplace_back(simulateTraffic, northLane);
    simulationThreads.emplace_back(simulateTraffic, southLane);
    simulationThreads.emplace_back(simulateTraffic, eastLane);
    simulationThreads.emplace_back(simulateTraffic, westLane);
    
    // Start emergency vehicle simulation thread
    simulationThreads.emplace_back(simulateEmergencyVehicles, intersection, allLanes);

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

    std::cout << "Simulation stopped." << std::endl;
    return 0;
}
