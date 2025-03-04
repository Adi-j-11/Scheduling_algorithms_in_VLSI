#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NODES 100
#define MAX_RESOURCES 10

#define ADDER 1
#define MULTIPLIER 2

typedef struct {
    int operation_type;  // 1 for addition, 2 for multiplication
    int predecessors[MAX_NODES];  // List of predecessors
    int num_predecessors;  // Number of predecessors
    int num_successors; // Number of successors
    int successors[MAX_NODES]; // list of successors   
    int scheduled_time;  // Scheduled start time
    int end_time;  // Time the operation completes
    int mobility; // possible movement 
    bool in_progress; 
} Node;

typedef struct {
    int resource_type; // 1 for addition, 2 for multiplication
    int id;
    int latency; // Execution time for the resource
    bool available;
    int next_time; // Next available time for the resource
} Resource;

Resource resources[MAX_RESOURCES];

Node graph[MAX_NODES];
Node ASAP_graph[MAX_NODES];
Node ALAP_graph[MAX_NODES];
bool check_predecessors(int num_predecessors, int scheduled[], int predecessors[]) {
    for (int i = 0; i < num_predecessors; i++) {
        if (scheduled[predecessors[i]] == 0) 
            return false;
    }
    return true;
}
bool check_successors(int num_successor, int scheduled[], int successor[]) {
    for (int i = 0; i < num_successor; i++) {
        if (scheduled[successor[i]] == 0) 
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
int get_last_time(int num_nodes){
    int end = 0 ;
    for (int i = 0; i < num_nodes; i++){
        if (graph[i].end_time > end)
            end = graph[i].end_time;
    }
    return end;
}

void clear(int num_nodes){
    for(int i = 0; i <num_nodes; i++){
        graph[i].end_time = -1;
        graph[i].scheduled_time =-1;
        graph[i].in_progress = false;
    }
}
void alap_scheduling(int num_nodes, int total_resources, int scheduled[]){
    memset(scheduled, 0, sizeof(int)*num_nodes);
    int time = get_last_time(num_nodes);
    clear(num_nodes);
    bool complete = false;

    while (!complete && time >= 0) {
        // Schedule operations
        for (int i = 0; i < total_resources; i++) {
            if (resources[i].next_time == time)
            resources[i].available = true;
        }
        for (int i = 0; i < num_nodes; i++) {
            if (graph[i].scheduled_time == time)
                scheduled[i] = 1;
        }
        for (int i = 0; i < num_nodes; i++) {
            
            if (scheduled[i] == 0 && check_successors(graph[i].num_successors, scheduled, graph[i].successors) && graph[i].in_progress == false) {   

                int resource_id = find_available_resource(graph[i].operation_type, total_resources);
                
                if (resource_id != -1) { // A resource is available
                    graph[i].scheduled_time = time - resources[resource_id].latency;
                    graph[i].end_time = time ; // Compute end time
                    resources[resource_id].available = false;
                    resources[resource_id].next_time = graph[i].scheduled_time; // Update resource availability
                    graph[i].in_progress = true;
                }
            }
        }

        // Move to the next available time slot if progress was made
        time--;
        
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

void heuristic_scheduling(int num_nodes, int total_resources, int scheduled[]) {
    memset(scheduled, 0, sizeof(int) * num_nodes);
    clear(num_nodes);
    int time = 0;
    bool complete = false;

    FILE *file = fopen("intermediate.txt", "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    while (!complete) {
        fprintf(file, "\nTime Slot %d:\n", time);

        // Mark resources as available if their next available time is now
        for (int i = 0; i < total_resources; i++) {
            if (resources[i].next_time == time)
                resources[i].available = true;
        }

        // Mark nodes as completed if they finish execution at this time
        for (int i = 0; i < num_nodes; i++) {
            if (graph[i].end_time == time)
                scheduled[i] = 1;
        }

        // Identify vertices that can be scheduled (separating Adders and Multipliers)
        bool found_adder = false, found_multiplier = false;

        fprintf(file, "  Can be scheduled (Adder): ");
        for (int i = 0; i < num_nodes; i++) {
            if (scheduled[i] == 0 && check_predecessors(graph[i].num_predecessors, scheduled, graph[i].predecessors) 
                && !graph[i].in_progress && find_available_resource(ADDER, total_resources) != -1) {
                if (graph[i].operation_type == ADDER) {
                    fprintf(file, "Node %d ", i);
                    found_adder = true;
                }
            }
        }
        if (!found_adder) fprintf(file, "None");
        fprintf(file, "\n");

        fprintf(file, "  Can be scheduled (Multiplier): ");
        for (int i = 0; i < num_nodes; i++) {
            if (scheduled[i] == 0 && check_predecessors(graph[i].num_predecessors, scheduled, graph[i].predecessors) 
                && !graph[i].in_progress && find_available_resource(MULTIPLIER, total_resources) != -1) {
                if (graph[i].operation_type == MULTIPLIER) {
                    fprintf(file, "Node %d ", i);
                    found_multiplier = true;
                }
            }
        }
        if (!found_multiplier) fprintf(file, "None");
        fprintf(file, "\n");

        // Schedule operations
        fprintf(file, "  Scheduled: ");
        bool found_scheduled = false;
        for (int i = 0; i < num_nodes; i++) {
            if (scheduled[i] == 0 && check_predecessors(graph[i].num_predecessors, scheduled, graph[i].predecessors) && !graph[i].in_progress) {
                int resource_id = find_available_resource(graph[i].operation_type, total_resources);

                if (resource_id != -1) { // A resource is available
                    graph[i].scheduled_time = time;
                    graph[i].end_time = time + resources[resource_id].latency; // Compute end time
                    resources[resource_id].available = false;
                    resources[resource_id].next_time = graph[i].end_time; // Update resource availability
                    graph[i].in_progress = true;
                    fprintf(file, "Node %d ", i);
                    found_scheduled = true;
                }
            }
        }
        if (!found_scheduled) fprintf(file, "None");
        fprintf(file, "\n");

        // Identify vertices in progress
        fprintf(file, "  In progress: ");
        bool found_in_progress = false;
        for (int i = 0; i < num_nodes; i++) {
            if (graph[i].in_progress && scheduled[i] == 0) {
                fprintf(file, "Node %d ", i);
                found_in_progress = true;
            }
        }
        if (!found_in_progress) fprintf(file, "None");
        fprintf(file, "\n");

        // Move to the next available time slot
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

    fclose(file);
}



#include <stdlib.h>


int main() {
    int num_nodes, total_resources;
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

    // Read number of operations (nodes)
    fscanf(file, "%d", &num_nodes);

    // Read operation types
    for (int i = 0; i < num_nodes; i++) {
        fscanf(file, "%d", &graph[i].operation_type);
        graph[i].num_predecessors = 0;
        graph[i].num_successors = 0;
        graph[i].scheduled_time = -1;
        graph[i].end_time = -1;
        graph[i].mobility = 0;
    }

    // Read dependencies (edges)
    int edges;
    fscanf(file, "%d", &edges);

    for (int i = 0; i < edges; i++) {
        int from, to;
        fscanf(file, "%d %d", &from, &to);
        graph[to].predecessors[graph[to].num_predecessors++] = from;
        graph[from].successors[graph[from].num_successors++] = to;
    }

    // Read number of resources
    fscanf(file, "%d", &total_resources);

    // Read resource details
    for (int i = 0; i < total_resources; i++) {
        fscanf(file, "%d %d", &resources[i].resource_type, &resources[i].latency);
        resources[i].id = i;
        resources[i].next_time = 0;
        resources[i].available = true;
    }

    fclose(file);

    // Run scheduling algorithms
    asap_scheduling(num_nodes, total_resources, scheduled);
    for(int i = 0; i<num_nodes; i++) {
        ASAP_graph[i] = graph[i];
    }

    alap_scheduling(num_nodes, total_resources, scheduled);
    for(int i = 0; i<num_nodes; i++) {
        ALAP_graph[i] = graph[i];
    }

    // set mobility
    for(int i = 0; i<num_nodes; i++) {
        graph[i].mobility = ALAP_graph[i].scheduled_time - ASAP_graph[i].scheduled_time;
    }
    heuristic_scheduling(num_nodes, total_resources, scheduled);

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
