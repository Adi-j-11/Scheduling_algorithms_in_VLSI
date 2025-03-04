#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 100
#define MAX_EDGES 200

typedef struct {
    int from, to;
} Edge;

int main() {
    FILE *dotFile = fopen("dag.dot", "r");
    FILE *outputFile = fopen("input.txt", "w");
    
    if (!dotFile || !outputFile) {
        printf("Error: Could not open file.\n");
        return 1;
    }
    
    char line[256];
    int num_nodes = 0, num_edges = 0;
    int operation_types[MAX_NODES];
    Edge edges[MAX_EDGES];
    
    // Read .dot file
    while (fgets(line, sizeof(line), dotFile)) {
        int node, op_type, from, to;
        if (sscanf(line, " %d [ label = \"%d\"]", &node, &op_type) == 2) {
            operation_types[node] = op_type;
            if (node + 1 > num_nodes) num_nodes = node + 1;
        } else if (sscanf(line, " %d -> %d", &from, &to) == 2) {
            edges[num_edges].from = from;
            edges[num_edges].to = to;
            num_edges++;
        }
    }
    fclose(dotFile);
    
    // Write to input.txt
    fprintf(outputFile, "%d\n", num_nodes);
    for (int i = 0; i < num_nodes; i++) {
        fprintf(outputFile, "%d\n", operation_types[i]);
    }
    
    fprintf(outputFile, "%d\n", num_edges);
    for (int i = 0; i < num_edges; i++) {
        fprintf(outputFile, "%d %d\n", edges[i].from, edges[i].to);
    }
    
    // Placeholder for resources (modify as needed)
    int total_resources = 3;
    fprintf(outputFile, "%d\n", total_resources);
    fprintf(outputFile, "1 1\n2 2\n2 2\n");
    
    fclose(outputFile);
    printf("Converted graph.dot to input.txt successfully.\n");
    return 0;
}
