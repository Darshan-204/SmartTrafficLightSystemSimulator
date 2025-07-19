#include <atomic>
#include <mutex>
std::atomic<bool> pedestrianCrossingActive{false};
std::mutex pedestrianMutex;

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
        std::cout << "\033[2J\033[H";
    }

    static void displayIntersection(const std::vector<std::shared_ptr<Lane>>& lanes,
                                    const std::vector<std::shared_ptr<TrafficLight>>& lights) {
        clearScreen();

        std::cout << COLOR_BOLD COLOR_CYAN "🚦 Smart Traffic Light System - Live View 🚦" COLOR_RESET << std::endl;
        std::cout << "=============================================" << std::endl << std::endl;

        bool crossing = pedestrianCrossingActive.load();

        for (size_t i = 0; i < lanes.size(); ++i) {
            auto lane = lanes[i];
            auto light = lights[i];
            LightState lightState = light->getState();
            bool hasEmergency = lane->hasEmergencyVehicle();
            std::string redLight   = (crossing || lightState == LightState::RED) ? (COLOR_RED "🔴" COLOR_RESET) : (COLOR_WHITE "⚪" COLOR_RESET);
            std::string yellowLight= (crossing) ? (COLOR_WHITE "⚪" COLOR_RESET) : ((lightState == LightState::YELLOW) ? (COLOR_YELLOW"🟡" COLOR_RESET) : (COLOR_WHITE "⚪" COLOR_RESET));
            std::string greenLight = (crossing) ? (COLOR_WHITE "⚪" COLOR_RESET) : ((lightState == LightState::GREEN) ? (COLOR_GREEN "🟢" COLOR_RESET) : (COLOR_WHITE "⚪" COLOR_RESET));

            std::cout << redLight << " " << yellowLight << " " << greenLight;
            if (crossing) {
                std::cout << " " << COLOR_YELLOW << COLOR_BOLD << "🚶" << COLOR_RESET;
            } else if (hasEmergency) {
                std::cout << " " << COLOR_RED << COLOR_BOLD << "🚨" << COLOR_RESET;
            }
            std::cout << std::endl;

            std::string laneId = lane->getId();
            std::string arrow;
            if (laneId == "North") arrow = "↑ ";
            else if (laneId == "South") arrow = "↓ ";
            else if (laneId == "East") arrow = "→ ";
            else if (laneId == "West") arrow = "← ";

            std::cout << COLOR_BOLD << arrow << laneId << " Lane " << COLOR_RESET;

            int vehicleCount = lane->getVehicleCount();
            int capacity = lane->getCapacity();
            std::cout << "[";
            for (int j = 0; j < capacity; ++j) {
                if (j < vehicleCount) {
                    if (crossing && j == vehicleCount - 1) {
                        std::cout << COLOR_YELLOW "🚶" COLOR_RESET;
                    } else if (hasEmergency && j == vehicleCount - 1) {
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
                        }
                    } else {
                        std::cout << "🚗";
                    }
                } else {
                    std::cout << "  ";
                }
            }
            std::cout << "] ";

            double occupancy = lane->getOccupancyRatio() * 100;
            if (occupancy >= 80) {
                std::cout << COLOR_RED;
            } else if (occupancy >= 50) {
                std::cout << COLOR_YELLOW;
            } else {
                std::cout << COLOR_GREEN;
            }
            std::cout << std::fixed << std::setprecision(0) << occupancy << "%" COLOR_RESET;

            if (crossing) {
                std::cout << " " COLOR_YELLOW COLOR_BOLD "(PEDESTRIAN)" COLOR_RESET;
            } else if (hasEmergency) {
                std::string emergencyType;
                switch (lane->getEmergencyVehicleType()) {
                    case EmergencyVehicleType::POLICE: emergencyType = "POLICE"; break;
                    case EmergencyVehicleType::AMBULANCE: emergencyType = "AMBULANCE"; break;
                    case EmergencyVehicleType::FIRE_TRUCK: emergencyType = "FIRE TRUCK"; break;
                    default: emergencyType = "EMERGENCY"; break;
                }
                std::cout << " " COLOR_RED COLOR_BOLD "(" << emergencyType << ")" COLOR_RESET;
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Legend: 🚗=Vehicle 🚨=Emergency " << COLOR_YELLOW "🚶" COLOR_RESET
                  << "=Pedestrian " << COLOR_RED "🔴" COLOR_RESET << "=Red "
                  << COLOR_GREEN "🟢" COLOR_RESET << "=Green "
                  << COLOR_YELLOW"🟡" COLOR_RESET << "=Yellow" << std::endl;
        std::cout << "Press Enter to stop..." << std::endl;
    }
};

void simulateTraffic(std::shared_ptr<Lane> lane, std::shared_ptr<TrafficLight> light) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    while (true) {
        if (dis(gen) < 0.7) {
            lane->addVehicle();
        } else {
            if (light && light->getState() != LightState::RED && !pedestrianCrossingActive.load()) {
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
    std::uniform_int_distribution<> vehicleTypeSelector(1, 3);
    std::uniform_int_distribution<> timingSelector(15, 45);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(timingSelector(gen)));

        auto selectedLane = lanes[laneSelector(gen)];
        EmergencyVehicleType vehicleType;

        switch (vehicleTypeSelector(gen)) {
            case 1: vehicleType = EmergencyVehicleType::POLICE; break;
            case 2: vehicleType = EmergencyVehicleType::AMBULANCE; break;
            case 3: vehicleType = EmergencyVehicleType::FIRE_TRUCK; break;
            default: vehicleType = EmergencyVehicleType::AMBULANCE; break;
        }

        intersection->reportEmergencyVehicle(selectedLane->getId(), vehicleType);

        std::uniform_int_distribution<> durationSelector(8, 15);
        std::this_thread::sleep_for(std::chrono::seconds(durationSelector(gen)));

        intersection->clearEmergencyVehicle(selectedLane->getId());
    }
}

void displayLoop(const std::vector<std::shared_ptr<Lane>>& lanes,
                 const std::vector<std::shared_ptr<TrafficLight>>& lights) {
    while (true) {
        TrafficDisplay::displayIntersection(lanes, lights);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main() {
    std::cout << "🚦 Smart Traffic Light System with Emergency Priority 🚦" << std::endl;
    std::cout << "==========================================================" << std::endl;

    auto intersection = std::make_shared<Intersection>("Main Street & First Ave");

    auto northLane = std::make_shared<Lane>("North", 15);
    auto southLane = std::make_shared<Lane>("South", 15);
    auto eastLane = std::make_shared<Lane>("East", 15);
    auto westLane = std::make_shared<Lane>("West", 15);

    auto northLight = std::make_shared<TrafficLight>("North Light");
    auto southLight = std::make_shared<TrafficLight>("South Light");
    auto eastLight = std::make_shared<TrafficLight>("East Light");
    auto westLight = std::make_shared<TrafficLight>("West Light");

    intersection->addLane(northLane, northLight);
    intersection->addLane(southLane, southLight);
    intersection->addLane(eastLane, eastLight);
    intersection->addLane(westLane, westLight);

    std::vector<std::shared_ptr<Lane>> allLanes = {northLane, southLane, eastLane, westLane};
    std::vector<std::shared_ptr<TrafficLight>> allLights = {northLight, southLight, eastLight, westLight};

    std::vector<std::thread> simulationThreads;
    simulationThreads.emplace_back(simulateTraffic, northLane, northLight);
    simulationThreads.emplace_back(simulateTraffic, southLane, southLight);
    simulationThreads.emplace_back(simulateTraffic, eastLane, eastLight);
    simulationThreads.emplace_back(simulateTraffic, westLane, westLight);

    simulationThreads.emplace_back(simulateEmergencyVehicles, intersection, allLanes);

    simulationThreads.emplace_back([]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> waitTime(20, 40);
        std::uniform_int_distribution<> crossDuration(5, 10);
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(waitTime(gen)));
            {
                std::lock_guard<std::mutex> lock(pedestrianMutex);
                pedestrianCrossingActive = true;
            }
            std::this_thread::sleep_for(std::chrono::seconds(crossDuration(gen)));
            {
                std::lock_guard<std::mutex> lock(pedestrianMutex);
                pedestrianCrossingActive = false;
            }
        }
    });

    simulationThreads.emplace_back(displayLoop, allLanes, allLights);

    intersection->start();

    std::cout << "Traffic Light Simulation Started with Emergency Vehicle Priority!" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "- Adaptive traffic flow based on lane occupancy" << std::endl;
    std::cout << "- Emergency vehicle priority (Fire Truck > Ambulance > Police)" << std::endl;
    std::cout << "- Real-time emergency detection and response" << std::endl;
    std::cout << "\nPress Enter to stop..." << std::endl;
    std::cin.get();

    intersection->stop();
    for (auto& thread : simulationThreads) {
        thread.detach();
    }

    TrafficDisplay::clearScreen();
    std::cout << "Simulation stopped." << std::endl;
    return 0;
}
