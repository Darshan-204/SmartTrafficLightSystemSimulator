#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

struct LaneInfo {
    std::string name;
    int numVehicles;
    std::string emergencyType;
    std::string trafficLight;
    std::string priorityStatus;
};

std::string getEmergencyIcon(const std::string& type) {
    if (type == "Police") return "🚓 Police";
    if (type == "Ambulance") return "🚑 Ambulance";
    if (type == "Fire Truck") return "🚒 Fire Truck";
    return "None";
}

std::string getTrafficLightIcon(const std::string& color) {
    if (color == "GREEN") return "\x1b[32mGREEN\x1b[0m";
    return "\x1b[31mRED\x1b[0m";
}

std::string getPriorityIcon(const std::string& status) {
    if (status == "Active") return "\x1b[32mActive\x1b[0m";
    return "\x1b[31mWaiting\x1b[0m";
}

int main() {
    std::vector<LaneInfo> lanes = {
        {"North", 0, "None", "RED", "Waiting"},
        {"South", 0, "None", "RED", "Waiting"},
        {"East", 0, "None", "RED", "Waiting"},
        {"West", 0, "None", "RED", "Waiting"}
    };

    std::cout << "Enter number of vehicles and emergency type for each lane.\n";
    std::cout << "Available emergency types: None, Police, Ambulance, Fire Truck\n";
    for (auto& lane : lanes) {
        std::cout << lane.name << " lane - Vehicles: ";
        std::cin >> lane.numVehicles;
        std::cout << lane.name << " lane - Emergency type (choose from: None, Police, Ambulance, Fire Truck): ";
        std::cin.ignore();
        std::getline(std::cin, lane.emergencyType);
        if (lane.emergencyType != "None") {
            lane.priorityStatus = "Active";
        }
    }

    // Set traffic light: only one lane can be green (priority: emergency > max vehicles)
    int greenIdx = -1;
    for (int i = 0; i < 4; ++i) {
        if (lanes[i].emergencyType != "None") {
            greenIdx = i;
            break;
        }
    }
    if (greenIdx == -1) {
        int maxVehicles = -1;
        for (int i = 0; i < 4; ++i) {
            if (lanes[i].numVehicles > maxVehicles) {
                maxVehicles = lanes[i].numVehicles;
                greenIdx = i;
            }
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (i == greenIdx) {
            lanes[i].trafficLight = "GREEN";
            lanes[i].priorityStatus = "Active";
        } else {
            lanes[i].trafficLight = "RED";
            if (lanes[i].priorityStatus != "Active")
                lanes[i].priorityStatus = "Waiting";
        }
    }

    // Print table function
    auto printTable = [&]() {
        std::cout << "\n+--------+---------------+-------------------+--------------+-----------------+\n";
        std::cout << "| Lane   | No. of Vehicles | Emergency Type   | Traffic Light | Priority Status |\n";
        std::cout << "+--------+---------------+-------------------+--------------+-----------------+\n";
        for (const auto& lane : lanes) {
            std::cout << "| " << std::setw(6) << lane.name
                      << " | " << std::setw(13) << lane.numVehicles
                      << " | " << std::setw(17) << getEmergencyIcon(lane.emergencyType)
                      << " | " << std::setw(12) << getTrafficLightIcon(lane.trafficLight)
                      << " | " << std::setw(15) << getPriorityIcon(lane.priorityStatus)
                      << " |\n";
        }
        std::cout << "+--------+---------------+-------------------+--------------+-----------------+\n";
    };
    printTable();

    // Pedestrian crossing
    bool pedestrianWaiting = false;

    while (true) {
        std::cout << "\nOptions:\n1. Add vehicle\n2. Set emergency\n3. Add pedestrian waiting\n4. Remove pedestrian waiting\n5. Show table\n6. Decrease vehicles in green lane\n7. Exit\nChoose option: ";
        int option;
        std::cin >> option;
        if (option == 1) {
            std::string laneName;
            int addCount = 1;
            std::cout << "Enter lane name (North/South/East/West): ";
            std::cin >> laneName;
            std::cout << "How many vehicles to add? ";
            std::cin >> addCount;
            for (auto& lane : lanes) {
                if (lane.name == laneName) {
                    lane.numVehicles += addCount;
                    break;
                }
            }
        } else if (option == 2) {
            std::string laneName;
            std::cout << "Enter lane name (North/South/East/West): ";
            std::cin >> laneName;
            std::cout << "1. Add emergency\n2. Remove emergency\nChoose: ";
            int emOpt;
            std::cin >> emOpt;
            std::cin.ignore();
            if (emOpt == 1) {
                std::string emergencyType;
                std::cout << "Enter emergency type (Police/Ambulance/Fire Truck): ";
                std::getline(std::cin, emergencyType);
                for (auto& lane : lanes) {
                    if (lane.name == laneName) {
                        lane.emergencyType = emergencyType;
                        lane.priorityStatus = "Active";
                        break;
                    }
                }
            } else if (emOpt == 2) {
                for (auto& lane : lanes) {
                    if (lane.name == laneName) {
                        lane.emergencyType = "None";
                        lane.priorityStatus = "Waiting";
                        break;
                    }
                }
            } else {
                std::cout << "Invalid option.\n";
            }
        } else if (option == 3) {
            pedestrianWaiting = true;
            std::cout << "Pedestrian is now waiting to cross the road!\n";
        } else if (option == 4) {
            pedestrianWaiting = false;
            std::cout << "Pedestrian is no longer waiting to cross the road.\n";
        } else if (option == 5) {
            // Emergency has higher priority than pedestrian
            bool anyEmergency = false;
            for (int i = 0; i < 4; ++i) {
                if (lanes[i].emergencyType != "None") {
                    anyEmergency = true;
                    break;
                }
            }
            if (anyEmergency) {
                int greenIdx = -1;
                for (int i = 0; i < 4; ++i) {
                    if (lanes[i].emergencyType != "None") {
                        greenIdx = i;
                        break;
                    }
                }
                if (greenIdx == -1) {
                    int maxVehicles = -1;
                    for (int i = 0; i < 4; ++i) {
                        if (lanes[i].numVehicles > maxVehicles) {
                            maxVehicles = lanes[i].numVehicles;
                            greenIdx = i;
                        }
                    }
                }
                for (int i = 0; i < 4; ++i) {
                    if (i == greenIdx) {
                        lanes[i].trafficLight = "GREEN";
                        lanes[i].priorityStatus = "Active";
                    } else {
                        lanes[i].trafficLight = "RED";
                        if (lanes[i].priorityStatus != "Active")
                            lanes[i].priorityStatus = "Waiting";
                    }
                }
            } else if (pedestrianWaiting) {
                for (int i = 0; i < 4; ++i) {
                    lanes[i].trafficLight = "RED";
                    if (lanes[i].priorityStatus != "Active")
                        lanes[i].priorityStatus = "Waiting";
                }
            } else {
                int greenIdx = -1;
                for (int i = 0; i < 4; ++i) {
                    if (lanes[i].numVehicles > 0) {
                        if (greenIdx == -1 || lanes[i].numVehicles > lanes[greenIdx].numVehicles) {
                            greenIdx = i;
                        }
                    }
                }
                for (int i = 0; i < 4; ++i) {
                    if (i == greenIdx && greenIdx != -1) {
                        lanes[i].trafficLight = "GREEN";
                        lanes[i].priorityStatus = "Active";
                    } else {
                        lanes[i].trafficLight = "RED";
                        if (lanes[i].priorityStatus != "Active")
                            lanes[i].priorityStatus = "Waiting";
                    }
                }
            }
            printTable();
            if (pedestrianWaiting && !anyEmergency) {
                std::cout << "\n[Pedestrian is waiting to cross the road!]\n";
            }
        } else if (option == 6) {
            // Decrease vehicles in green lane by 5
            int greenIdx = -1;
            if (!pedestrianWaiting) {
                for (int i = 0; i < 4; ++i) {
                    if (lanes[i].trafficLight == "GREEN") {
                        greenIdx = i;
                        break;
                    }
                }
                if (greenIdx != -1) {
                    if (lanes[greenIdx].numVehicles >= 5)
                        lanes[greenIdx].numVehicles -= 5;
                    else
                        lanes[greenIdx].numVehicles = 0;
                    std::cout << "Decreased vehicles in " << lanes[greenIdx].name << " lane by 5.\n";
                    // If vehicles become zero and emergency is set, remove emergency and reassign green
                    if (lanes[greenIdx].numVehicles == 0 && lanes[greenIdx].emergencyType != "None") {
                        lanes[greenIdx].emergencyType = "None";
                        lanes[greenIdx].priorityStatus = "Waiting";
                        // Find next eligible lane (with emergency and vehicles > 0)
                        int nextGreen = -1;
                        for (int i = 0; i < 4; ++i) {
                            if (i != greenIdx && lanes[i].emergencyType != "None" && lanes[i].numVehicles > 0) {
                                nextGreen = i;
                                break;
                            }
                        }
                        // If none, pick lane with max vehicles > 0
                        if (nextGreen == -1) {
                            int maxVehicles = -1;
                            for (int i = 0; i < 4; ++i) {
                                if (i != greenIdx && lanes[i].numVehicles > maxVehicles) {
                                    maxVehicles = lanes[i].numVehicles;
                                    nextGreen = i;
                                }
                            }
                            if (maxVehicles <= 0) nextGreen = -1;
                        }
                        // Set green or all red
                        for (int i = 0; i < 4; ++i) {
                            if (i == nextGreen && nextGreen != -1) {
                                lanes[i].trafficLight = "GREEN";
                                lanes[i].priorityStatus = "Active";
                            } else {
                                lanes[i].trafficLight = "RED";
                                if (lanes[i].priorityStatus != "Active")
                                    lanes[i].priorityStatus = "Waiting";
                            }
                        }
                        if (nextGreen == -1)
                            std::cout << "Emergency removed from " << lanes[greenIdx].name << ". No eligible lane for green light. All lights are red.\n";
                        else
                            std::cout << "Emergency removed from " << lanes[greenIdx].name << ". Green light reassigned.\n";
                    }
                } else {
                    std::cout << "No green light lane to decrease vehicles.\n";
                }
            } else {
                std::cout << "Cannot decrease vehicles while pedestrian is waiting.\n";
            }
        } else if (option == 7) {
            break;
        } else {
            std::cout << "Invalid option. Try again.\n";
        }
    }
    return 0;
}
