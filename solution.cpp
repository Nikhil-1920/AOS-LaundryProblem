#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore>
#include <chrono>
#include <atomic>
#include <algorithm>

using namespace std;

// ANSI color codes for a visually appealing console output
namespace Color {
    const char* WHITE   = "\033[37m";
    const char* GREEN   = "\033[32m";
    const char* YELLOW  = "\033[33m";
    const char* RED     = "\033[31m";
    const char* RESET   = "\033[0m";
}

struct Student {
    int id;
    int arrivalTime;
    int washTime;
    int patienceTime;
};

class WashingMachineSimulation {
public:
    WashingMachineSimulation(int n, int m) : numStudents(n),
        machinesSemaphore(m), 
        studentsLeftCount(0) {}

    void run() {
        readStudentData();
        sortStudentsByArrival(); 
        startStudentThreads();
        waitForStudentsToFinish();
        printResults();
    }

private:
    int numStudents;
    vector<Student> students;
    vector<thread> studentThreads;

    counting_semaphore<> machinesSemaphore;
    mutex printMutex;
    atomic<int> studentsLeftCount;

    void readStudentData() {
        students.reserve(numStudents);
        for (int i = 0; i < numStudents; ++i) {
            int t, w, p;
            cin >> t >> w >> p;
            students.push_back({i + 1, t, w, p});
        }
    }

    // Sorts students primarily by arrival time and secondarily by ID
    void sortStudentsByArrival() {
        sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
            if (a.arrivalTime != b.arrivalTime) {
                return a.arrivalTime < b.arrivalTime;
            }
            return a.id < b.id;
        });
    }

    // Launches threads for students, simulating their arrival delays
    void startStudentThreads() {
        int lastArrivalTime = 0;

        for (const auto& student : students) {
            int sleepDuration = student.arrivalTime - lastArrivalTime;
            if (sleepDuration > 0) {
                this_thread::sleep_for(chrono::seconds(sleepDuration));
            }
            lastArrivalTime = student.arrivalTime;

            studentThreads.emplace_back([this, student]() {
                this->studentTask(student);
            });
        }
    }

    void studentTask(const Student& s) {
        {
            lock_guard<mutex> lock(printMutex);
            cout << Color::WHITE << "Student " << s.id << " arrives\n" << Color::RESET;
        }

        bool gotMachine = machinesSemaphore.try_acquire_for(chrono::seconds(s.patienceTime));

        if (gotMachine) {
            {
                lock_guard<mutex> lock(printMutex);
                cout << Color::GREEN << "Student " << s.id << " starts washing\n" << Color::RESET;
            }

            this_thread::sleep_for(chrono::seconds(s.washTime));

            {
                lock_guard<mutex> lock(printMutex);
                cout << Color::YELLOW << "Student " << s.id << " leaves after washing\n" << Color::RESET;
            }

            // Release machine for others
            machinesSemaphore.release();
        } else {
            {
                lock_guard<mutex> lock(printMutex);
                cout << Color::RED << "Student " << s.id << " leaves without washing\n" << Color::RESET;
            }

            // Count the student who couldn't wait
            studentsLeftCount++;
        }
    }

    void waitForStudentsToFinish() {
        for (auto& t : studentThreads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void printResults() {
        cout << studentsLeftCount << '\n';

        // Recommend more machines if 25% or more left without washing
        bool moreMachinesNeeded = (numStudents > 0) && (static_cast<double>(studentsLeftCount) / numStudents >= 0.25);

        cout << Color::WHITE;
        if (moreMachinesNeeded) {
            cout << "Yes\n";
        } else {
            cout << "No\n";
        }
        cout << Color::RESET;
    }
};

int main(void) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n_students, m_machines;
    cin >> n_students >> m_machines;

    WashingMachineSimulation simulation(n_students, m_machines);
    simulation.run();

    return 0;
}
