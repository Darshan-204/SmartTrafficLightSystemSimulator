# lane_input

This is a simple C++ console program to simulate a smart traffic light system for an intersection with four lanes (North, South, East, West).

## Features
- Enter the number of vehicles and emergency type for each lane.
- Display a table showing lane status, emergency, traffic light, and priority.
- Add vehicles to any lane interactively.
- Set or remove emergency status for any lane.
- Add or remove a pedestrian waiting to cross (pedestrian gets priority only if no emergency is present).
- Manually decrease vehicles in the green lane.
- Automatically remove emergency and reassign green light if a lane with emergency becomes empty.

## How to Run
1. Compile the program:
   ```sh
   g++ lane_input.cpp -o lane_input
   ```
2. Run the program:
   ```sh
   ./lane_input
   ```

## Menu Options
- **Add vehicle:** Add any number of vehicles to a selected lane.
- **Set emergency:** Add or remove an emergency type (Police, Ambulance, Fire Truck) for a lane.
- **Add/Remove pedestrian waiting:** Simulate a pedestrian waiting to cross. Pedestrian gets priority only if no emergency is present.
- **Show table:** Display the current status of all lanes.
- **Decrease vehicles in green lane:** Decrease the number of vehicles in the green lane by 5 (simulates vehicles passing through).
- **Exit:** Quit the program.

## Notes
- Emergency vehicles always get priority over pedestrians.
- If a lane with an emergency becomes empty, its emergency is cleared and the green light is reassigned.
- If no lane has vehicles, all lights are red.

