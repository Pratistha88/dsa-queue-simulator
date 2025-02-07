# Traffic Junction Queue Simulator

This project is an implementation of a **queue-based traffic junction simulator**. It is designed to manage traffic at a junction with four major roads (A, B, C, D) and their respective lanes using a queue data structure. The simulation manages both normal and priority traffic conditions, ensuring that vehicles are served efficiently from each lane.

Two programs are provided:

###simulator.c:
 Simulates the junction by reading incoming vehicle data from different lanes and controlling the traffic lights.

###traffic_generator.c:
 Randomly generates vehicles on each lane and communicates this data to the simulator via a shared file.

This project is part of the COMP202 course on Data Structures and Algorithms, using the SDL2 library for graphical simulation and queues to handle the traffic.

##Features:
###Queue Implementation
**Vehicle Queue:** Each road (A, B, C, D) has a corresponding vehicle queue. Vehicles are enqueued as they are generated and dequeued when they pass through the junction.
**Lane/Light Queue:** A priority queue manages the lanes. If a specific lane (like AL2) exceeds 10 waiting vehicles, it is given the highest priority.

###Algorithms Used
**Queue Operations:** Standard enqueue and dequeue operations for handling vehicles.
**Priority Lane:** When the vehicle count in AL2 exceeds 10, it is served first until the count drops below 5, at which point normal 
conditions resume.

###Time Complexity
**Enqueue operation:** O(1)
**Dequeue operation:** O(1)
**Priority check:** O(n), where n is the number of lanes (4 in this case).

## Contributors:
**Pratistha Thapa**