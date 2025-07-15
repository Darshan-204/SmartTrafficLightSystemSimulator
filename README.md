# Smart Traffic Light System Simulator with Emergency Vehicle Priority

A C++-based intelligent traffic light controller simulation that adapts to real-time vehicle density and provides priority signaling for emergency vehicles.

## Features

### Core Traffic Management
- **Adaptive Traffic Flow**: Light durations automatically adjust based on lane occupancy (30-60 seconds)
- **Real-time Optimization**: System continuously monitors all lanes and prioritizes busier traffic
- **Multi-threaded Simulation**: Separate threads for traffic generation and intersection control

### Emergency Vehicle Priority System 🚨
- **Automatic Detection**: System detects emergency vehicles in any lane
- **Priority Hierarchy**: Fire Truck > Ambulance > Police
- **Immediate Response**: Emergency vehicles get instant green light with extended duration (90 seconds)
- **Smart Coordination**: Multiple emergency vehicles are handled based on priority order
- **Clear Notifications**: Real-time console output with emergency status updates

### Technical Implementation
- **Thread Safety**: Uses `std::mutex` and `std::atomic` for safe multi-threading
- **Object-Oriented Design**: Modular classes for `Lane`, `TrafficLight`, and `Intersection`
- **State Management**: Comprehensive state tracking for both normal and emergency operations
- **Unit Testing**: Complete test suite using Google Test framework

## Building and Running

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (GCC 7+ or Clang 5+)
- Make build system

### Build Instructions
```bash
mkdir -p build
cd build
cmake ..
make
```

### Running the Simulation
```bash
./SmartTrafficLight
```

### Running Tests
```bash
./tests/traffic_tests
```

## System Architecture

### Classes

#### `TrafficLight`
- Manages light states (RED, YELLOW, GREEN)
- Handles emergency mode activation/deactivation
- Thread-safe state management with atomic operations

#### `Lane`
- Tracks vehicle count and capacity
- Detects and manages emergency vehicle presence
- Calculates occupancy ratios for optimization

#### `Intersection`
- Central controller coordinating all lanes and lights
- Implements traffic flow optimization algorithms
- Handles emergency vehicle priority protocols

#### Emergency Vehicle Types
```cpp
enum class EmergencyVehicleType {
    NONE,
    AMBULANCE,
    POLICE,
    FIRE_TRUCK
};
```

## Emergency Vehicle Protocol

1. **Detection**: Emergency vehicle detected in lane
2. **Priority Assignment**: Vehicle type determines priority level
3. **Signal Override**: All other lights turn red immediately
4. **Green Light**: Emergency lane gets green light for 90 seconds
5. **Monitoring**: System tracks emergency vehicle progress
6. **Clearance**: Normal traffic flow resumes after emergency vehicle passes

## Sample Output

```
🚦 Smart Traffic Light System with Emergency Priority 🚦
==========================================================
Traffic Light Simulation Started with Emergency Vehicle Priority!
Features:
- Adaptive traffic flow based on lane occupancy
- Emergency vehicle priority (Fire Truck > Ambulance > Police)
- Real-time emergency detection and response

Lane North got green light for 45 seconds (occupancy: 75%)
🚨 EMERGENCY: AMBULANCE detected in lane South - Activating priority signal!
🚨 PRIORITY: AMBULANCE in lane South has green light (90 seconds)
✅ Emergency vehicle cleared from lane South
Lane East got green light for 35 seconds (occupancy: 50%)
```

## Future Enhancements

- **Pedestrian Crossing Integration**: Add pedestrian signal support
- **Traffic Analytics**: Historical data collection and analysis
- **Network Communication**: Multi-intersection coordination
- **Machine Learning**: Predictive traffic pattern optimization
- **Mobile App Integration**: Real-time traffic updates for users

## Testing

The system includes comprehensive unit tests covering:
- Basic traffic light operations
- Lane vehicle management
- Emergency vehicle detection and clearance
- Intersection coordination logic
- Thread safety verification

Run tests with: `./tests/traffic_tests`

## License

This project is for educational purposes, demonstrating C++ concepts including:
- Multi-threading with `std::thread`
- Thread synchronization with `std::mutex`
- Atomic operations for thread-safe data access
- Object-oriented design patterns
- State machine implementation
- Real-time system simulation
## Team
Darshan M P
Santosh
Sujith
Sarvesh
Ganesh Patil

