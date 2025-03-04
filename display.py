import matplotlib.pyplot as plt
import networkx as nx
import re

def read_schedule_output(scheduled_output_file): 
    graph = {}

    with open(scheduled_output_file, "r") as file:
        lines = file.readlines()

    for line in lines:
        match = re.search(r"Node (\d+) \(Operation Type: (\d+)\) -> Scheduled at Time (\d+), Ends at Time (\d+)", line)
        if match:
            node_id = int(match.group(1))
            operation_type = int(match.group(2))
            scheduled_time = int(match.group(3))
            end_time = int(match.group(4))

            graph[node_id] = {
                "operation_type": operation_type,
                "scheduled_time": scheduled_time,
                "end_time": end_time,
                "duration": end_time - scheduled_time,
                "successors": []
            }

    return graph

def read_dependencies(input_file, graph):
    """Reads dependencies from the input file and updates the graph."""
    with open(input_file, "r") as file:
        lines = file.readlines()
    
    num_nodes = int(lines[0].strip())
    edge_count = int(lines[num_nodes + 1].strip())

    for i in range(edge_count):
        from_node, to_node = map(int, lines[num_nodes + 2 + i].split())
        if from_node in graph:
            graph[from_node]["successors"].append(to_node)
        if to_node not in graph:
            graph[to_node] = {"operation_type": None, "scheduled_time": None, "end_time": None, "duration": None, "successors": []}

def draw_topological_graph(graph, ax):
    """Draws a top-down dependency graph with better alignment and spacing."""
    G = nx.DiGraph()

    all_nodes = set(graph.keys())
    all_successors = {succ for node in graph for succ in graph[node]["successors"]}
    
    source_nodes = all_nodes - all_successors  # Nodes with no predecessors
    sink_nodes = all_nodes - {node for node in graph if graph[node]["successors"]}  # Nodes with no outgoing edges

    for node, data in graph.items():
        G.add_node(node)
        for succ in data["successors"]:
            G.add_edge(node, succ)

    S1, S2 = "S1", "S2"
    G.add_node(S1)
    G.add_node(S2)

    for src in source_nodes:
        G.add_edge(S1, src)
    for sink in sink_nodes:
        G.add_edge(sink, S2) 

    node_levels = {S1: 0}  
    for node in nx.topological_sort(G):  
        predecessors = list(G.predecessors(node))
        node_levels[node] = max([node_levels[p] for p in predecessors], default=-1) + 1

    levels = {}
    for node, level in node_levels.items():
        if level not in levels:
            levels[level] = []
        levels[level].append(node)

    pos = {}
    for level, nodes in levels.items():
        spacing = 3  
        x_positions = range(-len(nodes) * spacing // 2, len(nodes) * spacing // 2, spacing)
        for x, node in zip(x_positions, nodes):
            pos[node] = (x, -level)

    color_map = {1: "royalblue", 2: "tomato"}
    node_colors = ["gray" if n in (S1, S2) else color_map.get(graph.get(n, {}).get("operation_type"), "gray") for n in G.nodes]

    ax.set_title("Topological Dependency Graph", fontsize=14, fontweight="bold")
    nx.draw(G, pos, with_labels=True, node_color=node_colors, node_size=2500, edge_color="black",
            font_size=12, font_weight="bold", arrows=True, ax=ax)

    ax.legend(["Addition (Type 1)", "Multiplication (Type 2)"], loc="upper left", fontsize=10, frameon=True)

def draw_gantt_chart(graph, ax):
    """Draws a Gantt chart to represent the schedule in a structured format."""
    colors = {1: "royalblue", 2: "tomato"}  
    y_labels = []
    y_positions = []
    
    sorted_nodes = sorted(graph.items(), key=lambda x: x[1]["scheduled_time"])

    for i, (node, data) in enumerate(sorted_nodes):
        if data["scheduled_time"] is not None:
            ax.barh(node, data["duration"], left=data["scheduled_time"], color=colors.get(data["operation_type"], "gray"), 
                    edgecolor="black", height=0.6)
            ax.text(data["scheduled_time"] + data["duration"] / 2, node, f"Node {node}", ha="center", va="center", 
                    color="white", fontsize=10, fontweight="bold")
            y_labels.append(f"Node {node}")
            y_positions.append(node)

    ax.set_yticks(y_positions)
    ax.set_yticklabels(y_labels, fontsize=11)
    ax.set_xlabel("Time", fontsize=12, fontweight="bold")
    ax.set_ylabel("Nodes", fontsize=12, fontweight="bold")
    ax.set_title("Gantt Chart for Scheduled Operations", fontsize=14, fontweight="bold")
    ax.grid(axis="x", linestyle="--", alpha=0.6)

    ax.legend(["Addition (Type 1)", "Multiplication (Type 2)"], loc="upper right", fontsize=10, frameon=True)

# Run the program
scheduled_output_file = "output.txt"
input_file = "input.txt"

graph = read_schedule_output(scheduled_output_file)
read_dependencies(input_file, graph)

# Create a figure with two subplots
fig, axes = plt.subplots(1, 2, figsize=(20, 8))

# Draw graphs in respective subplots
draw_topological_graph(graph, axes[0])
draw_gantt_chart(graph, axes[1])

# Show both plots together
plt.tight_layout()
plt.show()
