#include <stdio.h>
#include <stdbool.h>

#define MAX_NODES 100

#define ADDER_LATENCY 1
#define MULTIPLIER_LATENCY 2

#define RESOURCES 10
int total_resources = RESOURCES;
typedef struct {
    int operation_type;  // 1 for addition, 2 for multiplication
    int predecessors[MAX_NODES];  // List of predecessors
    int num_predecessors;  // Number of predecessors
    int num_successors; // Number of successors
    int successors[MAX_NODES]; // list of successors  
    int scheduled_time;  // Scheduled start time
    int end_time;  // Time the operation completes
    bool in_progress; 
} Node;

typedef struct {
    int resource_type; // 1 for addition, 2 for multiplication
    int id;
    int latency; // Execution time for the resource
    bool available;
    int next_time; // Next available time for the resource
} Resource;

Resource resources[RESOURCES] = {
    {1, 0, ADDER_LATENCY, true, 0}, {1, 1, ADDER_LATENCY, true, 0}, {1, 2, ADDER_LATENCY, true, 0},
    {1, 3, ADDER_LATENCY, true, 0}, {1, 4, ADDER_LATENCY, true, 0},
    {2, 5, MULTIPLIER_LATENCY, true, 0}, {2, 6, MULTIPLIER_LATENCY, true, 0}, {2, 7, MULTIPLIER_LATENCY, true, 0},
    {2, 8, MULTIPLIER_LATENCY, true, 0}, {2, 9, MULTIPLIER_LATENCY, true, 0}
};


Node graph[MAX_NODES];

bool check_predecessors(int num_predecessors, int scheduled[], int predecessors[]) {
    for (int i = 0; i < num_predecessors; i++) {
        if (scheduled[predecessors[i]] == 0) 
            return false;
    }
    return true;
}


// Find an available resource for an operation type at a given time
int find_available_resource(int operation_type, int total_resources) {
    for (int i = 0; i < total_resources; i++) {
        if (resources[i].resource_type == operation_type && resources[i].available ) {
            return i; // Found available resource
        }
    }
    return -1; // No available resource found
}

void asap_scheduling(int num_nodes, int total_resources, int scheduled[]) {
    int time = 0;
    bool complete = false;

    while (!complete) {
        // Schedule operations
        for (int i = 0; i < total_resources; i++) {
            if (resources[i].next_time == time)
            resources[i].available = true;
        }
        for (int i = 0; i < num_nodes; i++) {
            if (graph[i].end_time == time)
                scheduled[i] = 1;
        }
        for (int i = 0; i < num_nodes; i++) {
            if (scheduled[i] == 0 && check_predecessors(graph[i].num_predecessors, scheduled, graph[i].predecessors) && graph[i].in_progress == false) {
                

                int resource_id = find_available_resource(graph[i].operation_type, total_resources);
                
                if (resource_id != -1) { // A resource is available
                    graph[i].scheduled_time = time;
                    graph[i].end_time = time + resources[resource_id].latency; // Compute end time
                    resources[resource_id].available = false;
                    resources[resource_id].next_time = graph[i].end_time; // Update resource availability
                    graph[i].in_progress = true;
                }
            }
        }

        // Move to the next available time slot if progress was made
        time++;
        
        // Check if all nodes are scheduled
        complete = true;
        for (int i = 0; i < num_nodes; i++) {
            if (scheduled[i] == 0) {
                complete = false;
                break;
            }
        }
    }
}

void printSchedule(int num_nodes) {
    printf("\nScheduled Operations:\n");
    for (int i = 0; i < num_nodes; i++) {
        printf("Node %d (Operation Type: %d) -> Scheduled at Time %d, Ends at Time %d\n",
               i, graph[i].operation_type, graph[i].scheduled_time, graph[i].end_time);
    }
}

int main() {
    int num_nodes;
    int scheduled[MAX_NODES] = {0};
    FILE *file = fopen("input.txt", "r");
    FILE *output_file = fopen("output.txt", "w");

    if (!file) {
        printf("Error: Could not open input file.\n");
        return 1;
    }
    if (!output_file) {
        printf("Error: Could not open output file.\n");
        fclose(file);
        return 1;
    }

    fscanf(file, "%d", &num_nodes);

    for (int i = 0; i < num_nodes; i++) {
        fscanf(file, "%d", &graph[i].operation_type);
        graph[i].num_predecessors = 0;
        graph[i].num_successors = 0;
        graph[i].scheduled_time = -1;
        graph[i].end_time = -1;
        graph[i].in_progress = false;
    }

    int edges;
    fscanf(file, "%d", &edges);

    for (int i = 0; i < edges; i++) {
        int from, to;
        fscanf(file, "%d %d", &from, &to);
        graph[to].predecessors[graph[to].num_predecessors++] = from;
        graph[from].successors[graph[from].num_successors++] = to;
    }
   

    fclose(file);

    // Run scheduling algorithms
    asap_scheduling(num_nodes, total_resources, scheduled);

    // Write the results to output.txt
    fprintf(output_file, "Scheduled Operations:\n");
    for (int i = 0; i < num_nodes; i++) {
        fprintf(output_file, "Node %d (Operation Type: %d) -> Scheduled at Time %d, Ends at Time %d\n",
                i, graph[i].operation_type, graph[i].scheduled_time, graph[i].end_time);
    }

    fclose(output_file);
    printf("Scheduling results written to output.txt\n");

    return 0;
}
