#include <gtest/gtest.h>
#include "Lane.hpp"
#include "TrafficLight.hpp"
#include "Intersection.hpp"

TEST(LaneTest, TestVehicleCountOperations) {
    Lane lane("Test Lane", 10);
    EXPECT_EQ(lane.getVehicleCount(), 0);
    
    lane.addVehicle();
    EXPECT_EQ(lane.getVehicleCount(), 1);
    
    lane.removeVehicle();
    EXPECT_EQ(lane.getVehicleCount(), 0);
}

TEST(LaneTest, TestCapacityLimit) {
    Lane lane("Test Lane", 2);
    
    lane.addVehicle();
    lane.addVehicle();
    lane.addVehicle(); // Should not exceed capacity
    
    EXPECT_EQ(lane.getVehicleCount(), 2);
}

TEST(LaneTest, TestEmergencyVehicleDetection) {
    Lane lane("Test Lane", 10);
    
    EXPECT_FALSE(lane.hasEmergencyVehicle());
    EXPECT_EQ(lane.getEmergencyVehicleType(), EmergencyVehicleType::NONE);
    
    lane.setEmergencyVehicle(EmergencyVehicleType::AMBULANCE);
    EXPECT_TRUE(lane.hasEmergencyVehicle());
    EXPECT_EQ(lane.getEmergencyVehicleType(), EmergencyVehicleType::AMBULANCE);
    
    lane.clearEmergencyVehicle();
    EXPECT_FALSE(lane.hasEmergencyVehicle());
    EXPECT_EQ(lane.getEmergencyVehicleType(), EmergencyVehicleType::NONE);
}

TEST(TrafficLightTest, TestStateChanges) {
    TrafficLight light("Test Light");
    EXPECT_EQ(light.getState(), LightState::RED);
    
    light.setState(LightState::GREEN);
    EXPECT_EQ(light.getState(), LightState::GREEN);
    
    light.setState(LightState::YELLOW);
    EXPECT_EQ(light.getState(), LightState::YELLOW);
}

TEST(TrafficLightTest, TestEmergencyMode) {
    TrafficLight light("Test Light");
    
    EXPECT_FALSE(light.isInEmergencyMode());
    EXPECT_EQ(light.getEmergencyVehicleType(), EmergencyVehicleType::NONE);
    
    light.activateEmergencyMode(EmergencyVehicleType::FIRE_TRUCK);
    EXPECT_TRUE(light.isInEmergencyMode());
    EXPECT_EQ(light.getEmergencyVehicleType(), EmergencyVehicleType::FIRE_TRUCK);
    
    light.deactivateEmergencyMode();
    EXPECT_FALSE(light.isInEmergencyMode());
    EXPECT_EQ(light.getEmergencyVehicleType(), EmergencyVehicleType::NONE);
}

TEST(IntersectionTest, TestEmergencyVehicleReporting) {
    Intersection intersection("Test Intersection");
    
    auto lane = std::make_shared<Lane>("Test Lane", 10);
    auto light = std::make_shared<TrafficLight>("Test Light");
    
    intersection.addLane(lane, light);
    
    // Test emergency vehicle reporting
    intersection.reportEmergencyVehicle("Test Lane", EmergencyVehicleType::POLICE);
    EXPECT_TRUE(lane->hasEmergencyVehicle());
    EXPECT_EQ(lane->getEmergencyVehicleType(), EmergencyVehicleType::POLICE);
    EXPECT_TRUE(light->isInEmergencyMode());
    
    // Test clearing emergency vehicle
    intersection.clearEmergencyVehicle("Test Lane");
    EXPECT_FALSE(lane->hasEmergencyVehicle());
    EXPECT_FALSE(light->isInEmergencyMode());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
