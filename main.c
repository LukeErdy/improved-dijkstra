#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WIDTH 1024

typedef struct node {
	int row, column;
	double f_cost;
	struct node *previous;
} Node;

typedef struct {
	Node *nodes;
} Queue;

int is_within_bounds(int height, int width, int row, int column) {
	if (row >= 0 && row < height && column >= 0 && column < width) return 1;
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

	Node **neighbors = malloc(8 * sizeof(Node*));
	int neighbor_count = 0;
	for (int i = current->row - 1; i < current->row + 2; i++) {
		for (int j = current->column - 2; j < current->column + 3; j += 2) {
			if (!(i == current->row && j == current->column) && is_within_bounds(height, strlen(graph[0]), i, j)) {
				neighbors[neighbor_count] = malloc(sizeof(Node));
				neighbors[neighbor_count]->row = i;
				neighbors[neighbor_count]->column = j;

				double h_cost = (((destination->column - j) / 2) * ((destination->column - j) / 2))
							+ (destination->row - i) * (destination->row - i);
				//printf("%c, h_cost: %f\n", graph[i][j], h_cost);
				double g_cost;
				switch (graph[i][j]) {
					case '.':
						g_cost = 1;
						break;
					case ',':
						g_cost = 2;
						break;
					case 'o':
						g_cost = 3;
						break;
					case '=':
						g_cost = 50;
						break;
					default:
						g_cost = -1; // if g_cost == -1 that node is unwalkable
				}
				if ((current->row - 1 == i && current->column + 2 == j) || (current->row + 1 == i && current->column + 2 == j)
					|| (current->row + 1 == i && current->column - 2 == j) || (current->row - 1 == i && current->column - 2 == j)) {
					g_cost *= 1.5;
				}
				neighbors[neighbor_count]->f_cost = h_cost + g_cost;

				neighbors[neighbor_count]->previous = current;
				neighbor_count++;
			}
		}
	}
	for (int i = 0; i < neighbor_count; i++) {
		printf("neighbor: %c\nf_cost: %f\n\n", graph[neighbors[i]->row][neighbors[i]->column], neighbors[i]->f_cost);
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