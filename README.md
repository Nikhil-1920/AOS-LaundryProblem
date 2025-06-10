# Advanced Operating Systems: Assignment 5

This project is a multithreaded C++ simulation of students using a limited number of washing machines in a dormitory. It solves a classic resource allocation and concurrency problem by using modern C++ features to manage student threads, machine access, and event logging according to a specific set of rules.

## Features

- **Multithreaded Simulation**: Each student is simulated in a dedicated thread to handle their actions concurrently.
- **Resource Management**: Machine availability is managed efficiently using `std::counting_semaphore`.
- **Synchronization**: Thread-safe printing and statistics are ensured by `std::mutex` and `std::atomic` to prevent race conditions.
- **Patience Handling**: Implements a non-busy waiting mechanism where students will leave if a machine does not become available within their patience time.
- **FCFS Logic**: Guarantees that students who arrive first get access to machines first. A student's original index is used as a tie-breaker for simultaneous arrivals.
- **Dynamic Event Logging**: The simulation prints color-coded, real-time status updates for each student's actions (arriving, washing, leaving).

## Algorithm and Implementation

### Initialization

The program starts by reading the number of students (N) and washing machines (M). It then reads the details for each of the N students: arrival time `T_i`, washing time `W_i`, and patience `P_i`.

### FCFS Sorting

To adhere to the "First-Come, First-Served" rule, the list of students is sorted primarily by their arrival time, and secondarily by their original index (ID).

### Concurrency Model

- The main thread orchestrates the simulation. It iterates through the sorted list of students and sleeps for the required duration to simulate each student's arrival at the correct time `T_i`.
- A new thread is launched for each student as they "arrive".

### Resource Acquisition & Patience

- Each student thread attempts to acquire a washing machine by calling `try_acquire_for` on a `std::counting_semaphore`.
- The timeout for this operation is set to the student's patience time, `P_i`. This elegantly handles the patience constraint without busy waiting.
- If a machine is acquired, the student "washes" for `W_i` seconds and then releases the semaphore.
- If the call times out, the student "leaves without washing," and a counter for unsuccessful students is atomically incremented.

### Conclusion

After all threads complete, the program prints the total number of students who left. It then determines if more washing machines are needed. More machines are needed if at least 25% of the students were unable to wash their clothes. The program outputs **"Yes"** or **"No"** accordingly.

## How to Compile

This project requires a C++ compiler with support for the C++20 standard (for `std::counting_semaphore`).

Save the code as `solution.cpp` and compile using the following command:

```bash
g++ solution.cpp -o solution -std=c++20 -pthread
```

## How to Run

Execute the compiled program from your terminal:

```bash
./solution
```

The program will then wait for input. Paste the input data and press Enter.

## Sample Input

```
5 2
6 3 5
3 4 3
6 5 2
2 9 6
8 5 2
```

## Sample Execution

> Note: The exact order of the event lines may vary between different runs due to the non-deterministic nature of thread scheduling, but the final numerical result should be the same.

```
Student 4 arrives
Student 4 starts washing
Student 2 arrives
Student 2 starts washing
Student 1 arrives
Student 3 arrives
Student 2 leaves after washing
Student 1 starts washing
Student 5 arrives
Student 3 leaves without washing
Student 1 leaves after washing
Student 5 starts washing
Student 4 leaves after washing
Student 5 leaves after washing
1
No
```

## Output Format

The program produces a series of real-time events, color-coded for readability:

- **White**: A student arrives to get their clothes washed.
- **Green**: A student gets an empty washing machine.
- **Yellow**: A student leaves after their washing is complete.
- **Red**: A student leaves without getting their clothes washed.

### Final Output

After the event log, the program prints two final lines:

1. An integer representing the total number of students who left without washing.
2. A final conclusion of `"Yes"` or `"No"` on whether more machines are needed.
