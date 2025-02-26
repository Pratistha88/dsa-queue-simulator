# DSA QUEUE SIMULATOR 🚘🚥
# Author: [Pratistha Thapa]

## Summary:
This code is a simple implementation of a queue data structure using C and SDL3. 
It inclues a simulation of a 4 junction road with 3 lanes on each.


## Features:
### Queue Implementation
**Vehicle Queue:** Each road (A, B, C, D) has a corresponding vehicle queue. Vehicles are enqueued as they are generated and dequeued when they pass through the junction.
**Lane Queue:** A priority queue manages the lanes. If a specific lane (like AL2) exceeds 10 waiting vehicles, it is given the highest priority.

### Algorithms Used
**Queue Operations:** Standard enqueue and dequeue operations for handling vehicles.
**Priority Lane:** When the vehicle count in AL2 exceeds 10, it is served first until the count drops below 5, at which point normal 
conditions resume.

### Vechicles data
![vechicle data have been given as random](img/image.png)