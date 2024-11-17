#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WIDTH 1024

typedef struct node {
	int row, column;
	double g_cost;
	double f_cost;
	struct node *previous;
} Node;

int is_within_bounds(int height, int width, int row, int column) {
	if (row >= 0 && row < height && column >= 0 && column < width) return 1;
	return 0;
}

// compare function for qsort()
int compare(const void* a, const void* b) {
	const Node* nodeA = *(const Node**)a;
	const Node* nodeB = *(const Node**)b;
	if (nodeA->f_cost > nodeB->f_cost) return 1;
	if (nodeA->f_cost < nodeB->f_cost) return -1;
	return 0;
}

void pathfind(char** graph, int height) {
	// we only care about even columns (so long as it's not a \n)
	// true horizontal distance = (t - i) / 2
	Node *source = malloc(sizeof(Node));
	Node *destination = malloc(sizeof(Node));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < strlen(graph[i]); j += 2) {
			switch (graph[i][j]) {
				case '0':
					source->row = i;
					source->column = j;
					break;
				case '2':
					destination->row = i;
					destination->column = j;
					break;
			}
		}
	}
	Node *current = source;
	Node **open = malloc(height * strlen(graph[0]) * sizeof(Node*)); // go by coords, not pointer value
	Node **closed = malloc(height * strlen(graph[0]) * sizeof(Node*));
	int open_size = 0, closed_size = 0;
	open[open_size] = current; // remove after processing neighbors
	open_size++;
	Node* best_candidate = current;
	double best_cost = 1000000;

	while (1) {
		for (int i = current->row - 1; i < current->row + 2; i++) {
			for (int j = current->column - 2; j < current->column + 3; j += 2) {
				if (!(i == current->row && j == current->column) && is_within_bounds(height, strlen(graph[0]), i, j)) {
					Node *new = malloc(sizeof(Node));
					new->row = i;
					new->column = j;

					double h_cost = (((destination->column - j) / 2) * ((destination->column - j) / 2))
								+ ((destination->row - i) * (destination->row - i));
					//printf("%c, h_cost: %f\n", graph[i][j], h_cost);
					switch (graph[i][j]) {
						case '.':
							new->g_cost = 1;
							break;
						case ',':
							new->g_cost = 2;
							break;
						case 'o':
							new->g_cost = 3;
							break;
						case '=':
							new->g_cost = 50;
							break;
						case '2':
							// new->g_cost = -1;
							// double cost = 0;
							// Node* point = current;
							// while (1) {
							// 	cost += point->g_cost;
							// 	point = point->previous;
							// 	if (point == source) break;
							// }
							// if (cost < best_cost) {
							// 	best_cost = cost;
							// 	best_candidate = current;
							// }

							break;
						default:
							new->g_cost = -1; // if g_cost is - that node is unwalkable. Ignore it entirely
					}
					if ((current->row - 1 == i && current->column + 2 == j) || (current->row + 1 == i && current->column + 2 == j)
						|| (current->row + 1 == i && current->column - 2 == j) || (current->row - 1 == i && current->column - 2 == j)) {
						new->g_cost *= 1.5;
					}
					new->f_cost = h_cost + new->g_cost;
					new->previous = current;
					if (new->g_cost > 0) {
						// we add this neighbor to open if: 1) it's not in open and 2) it's not in closed. we check coords, not pointers
						int A = 0, B = 0;
						for (int k = 0; k < closed_size; k++) {
							if (closed[k]->row == new->row && closed[k]->column == new->column) {
								A = 1;
								break;
							}
						}
						for (int l = 0; l < open_size; l++) {
							if (open[l]->row == new->row && open[l]->column == new->column) {
								if (new->f_cost < open[l]->f_cost) {
									open[l]->g_cost = new->g_cost;
									open[l]->f_cost = new->f_cost;
									open[l]->previous = new->previous;
								}
								B = 1;
								break;
							}
						}
						if (A + B == 0) {
							open[open_size] = new;
							open_size++;
						}
					}
				}
			}
		}
		// remove current from open and add it to closed
		for (int i = 0; i < open_size; i++) {
			if (open[i] == current) {
				for (int k = i; k < open_size - 1; k++) open[k] = open[k + 1];
				break;
			}
		}
		open_size -= 1;
		//printf("size of open set: %d\n", open_size);
		if (!open_size) {
			// Node* point = best_candidate;
			// while (1) {
			// 	graph[point->row][point->column] = '*';
			// 	point = point->previous;
			// 	if (point == source) break;
			// }
			// for (int a = 0; a < height; a++) printf("%s", graph[a]);
			// printf("\nTotal cost: %f\n", best_cost);
			return;
		}

		closed[closed_size] = current;
		closed_size++;
		// sort open and assign new current
		qsort(open, open_size, sizeof(Node*), compare);
		current = open[0];
	}
}

int main() {
	char** graph = NULL;
	int height = 0;
	char* line = malloc(MAX_WIDTH * sizeof(char));
	size_t width = 0;
	while (getline(&line, &width, stdin) != -1) { // ignore newlines when pathfinding
		height++;
		graph = realloc(graph, height * sizeof(char*));
		graph[height - 1] = malloc(width * sizeof(char));
		strcpy(graph[height - 1], line);
	}
	pathfind(graph, height);
}