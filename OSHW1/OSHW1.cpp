#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <queue>


struct PCB { //Process Control Block
    int process_num; int arrival_time; int burst_time; int remaining_burst_time;
    int finish_time; int waiting_time; int turnaround_time; int start_time;
    bool started;

    PCB(int id, int arrival, int burst)
        : process_num(id), arrival_time(arrival), burst_time(burst),
        remaining_burst_time(burst), finish_time(0), waiting_time(0),
        turnaround_time(0), start_time(0), started(false) {
    }
};



void firstComeFirstServe(std::vector<PCB>& processes) {
    int current_time = 0;
    for (auto& process : processes) {
        if (current_time < process.arrival_time)
            current_time = process.arrival_time;
        process.start_time = current_time;
        process.finish_time = current_time + process.burst_time;
        process.turnaround_time = process.finish_time - process.arrival_time;
        process.waiting_time = process.start_time - process.arrival_time;
        current_time = process.finish_time;
    }
}

void shortestRemainingTime(std::vector<PCB>& processes) {
    auto comp = [](const PCB* a, const PCB* b) { return a->remaining_burst_time > b->remaining_burst_time; };
    std::priority_queue<PCB*, std::vector<PCB*>, decltype(comp)> pq(comp);
    int current_time = 0;
    size_t index = 0;

    while (!pq.empty() || index < processes.size()) {
        while (index < processes.size() && processes[index].arrival_time <= current_time) {
            pq.push(&processes[index]);
            index++;
        }
        if (pq.empty()) {
            current_time = processes[index].arrival_time;
            continue;
        }
        PCB* current = pq.top();
        pq.pop();

        int next_time = (index < processes.size()) ? processes[index].arrival_time : std::numeric_limits<int>::max();
        int exec_time = std::min(current->remaining_burst_time, next_time - current_time);

        current->remaining_burst_time -= exec_time;
        current_time += exec_time;

        if (current->remaining_burst_time > 0) {
            pq.push(current);
        }
        else {
            current->finish_time = current_time;
            current->turnaround_time = current->finish_time - current->arrival_time;
            current->waiting_time = current_time - current->arrival_time - current->burst_time; 
        }
    }
}

void round_robin(std::vector<PCB>& processes, int quantum) {
    std::queue<PCB*> q;
    int current_time = 0;
    size_t index = 0;

    while (!q.empty() || index < processes.size()) {
        while (index < processes.size() && processes[index].arrival_time <= current_time) {
            q.push(&processes[index++]);
        }
        if (q.empty()) {
            current_time = processes[index].arrival_time;
            continue;
        }
        PCB* current = q.front();
        q.pop();

        int exec_time = std::min(current->remaining_burst_time, quantum);
        current->remaining_burst_time -= exec_time;
        current_time += exec_time;

        if (current->remaining_burst_time > 0) {
            q.push(current);
        }
        else {
            current->finish_time = current_time;
            current->turnaround_time = current->finish_time - current->arrival_time;
            current->waiting_time = current->turnaround_time - current->burst_time;
        }
    }
}

std::vector<PCB> read_processes(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    std::vector<PCB> processes;
    int id = 1, arrival, burst;

    while (file >> arrival >> burst) {
        processes.emplace_back(id++, arrival, burst);
    }

    if (processes.empty()) {
        throw std::runtime_error("No processes found in file: " + filename);
    }

    file.close();
    return processes;
}


int main() {
    std::string filename = "C:/Users/ttc/source/repos/OSHW1/OSHW1/Processes.txt";

    std::vector<PCB> processes = read_processes(filename);
    if (processes.empty()) {
        std::cout << "No processes read from file.\n";
        return 1;
    }

    std::cout << "Select the scheduling algorithm:\n";
    std::cout << "1. First-Come First-Served (FCFS)\n";
    std::cout << "2. Shortest Remaining Time (SRT)\n";
    std::cout << "3. Round-Robin (RR)\n";
    std::cout << "4.when you finish press ok to exit \n";
    std::cout << "Enter your choice: ";
    int select;
    std::cin >> select;

    switch (select) {
    case 1:
        firstComeFirstServe(processes);
        break;
    case 2:
        shortestRemainingTime(processes);
        break;
    case 3:
        int quantum;
        std::cout << "Enter time quantum: ";
        std::cin >> quantum;
        round_robin(processes, quantum);
        break;
    default:
        std::cout << "Invalid selection.\n";
        return 1;
    }

    std::cout << "PID\tArrival\tBurst\tFinish\tWaiting\tTurnaround\n";
    int total_cpu_active_time = 0;
    int total_duration = 0;
    for (const auto& process : processes) {
        std::cout << process.process_num << "\t"
            << process.arrival_time << "\t"
            << process.burst_time << "\t"
            << process.finish_time << "\t"
            << process.waiting_time << "\t"
            << process.turnaround_time << "\n";
        total_cpu_active_time += process.burst_time;
        if (process.finish_time > total_duration) {
            total_duration = process.finish_time;
        }
    }




    float cpu_utilization = (float)total_cpu_active_time / total_duration * 100;
    std::cout << "CPU Utilization: " << cpu_utilization << "%\n";

    std::cout << "\n";

    std::cout << "\nGantt Chart:\n";
    for (const auto& process : processes) {
        std::cout << "P" << process.process_num - 1 << " [" << process.start_time << " - " << process.finish_time << "] ";
    }



    return 0;
}