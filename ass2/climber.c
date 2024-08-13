// Implementation of boulder climbing algorithms

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PQ.h"
#include "Queue.h"
#include "Wall.h"
#include "climber.h"

struct wall {
	int height;
	int width;
	int numRocks;
	Rock **rocks;
};

struct graph {
	int nV;          // #vertices
	int nE;          // #edges
	double **edges;  // adjacency matrix storing positive weights
	                 // 0 if nodes not adjacent
};
typedef struct graph *Graph;

void reversePath(struct rock *path, int length) {
	int i, j;
	struct rock temp;

	for (i = 0, j = length - 1; i < j; i++, j--) {
		temp = path[i];
		path[i] = path[j];
		path[j] = temp;
	}
}

struct path findShortestPath(Wall w, int reach, Colour colour) {
	int height = WallHeight(w);
	int width = WallWidth(w);

	//Allocate memory for visited and predecessor 2D arrays
	bool **visited = malloc(sizeof(bool *) * height);
	for (int i = 0; i < height; i++) {
		visited[i] = calloc(width, sizeof(bool));
	}

	Rock **predecessor = malloc(sizeof(struct rock *) * height);
	for (int i = 0; i < height; i++) {
		predecessor[i] = calloc(width, sizeof(struct rock));
	}

	int maxItems = (reach * reach);

	int predCount = 0;
	struct rock *path = malloc(sizeof(struct rock) * maxItems);

	Queue q = QueueNew();

	//Enqueue rocks within starting reach with the same colour to the queue.
	// This avoids having to call the function multiple times for each starting rock

	struct rock *allRocks = malloc((height * width) * sizeof(struct rock));
	int totalNumRocks = WallGetAllRocks(w, allRocks);

	for (int i = 0; i < totalNumRocks; i++) {
		if (allRocks[i].colour == colour && allRocks[i].row <= reach) {
			QueueEnqueue(q, &allRocks[i]);
		}
	}

	//If no rocks of the given colour are found within the given reach, return an empty path.
	if (!QueueSize(q)) {
		struct path p = {0, NULL};
		return p;
	}

	Rock src = QueueFront(q);

	while (!QueueIsEmpty(q)) {
		Rock v = QueueDequeue(q);

		if (visited[v->row][v->col]) {
			continue;
		}

		visited[v->row][v->col] = true;

		//If the current rock is within reach of the top wall, we have found a path.
		if (v->row + reach >= height) {
			Rock current = v;

			//Backtrack through the predecessor array to construct the path.
			while (src != NULL && current != NULL &&
			       (current->row != src->row || current->col != src->col)) {
				path[predCount] = *current;
				current = predecessor[current->row][current->col];
				predCount++;
			}

			// If the source rock is not out of reach, add it to the path.
			if (src->row - reach >= 0) {
				path[predCount] = *src;
				predCount++;
			}
			break;
		}

		//Find all rocks within reach of rock v.
		struct rock *inReach = malloc(maxItems * sizeof(struct rock));
		int numInReach = WallGetColouredRocksInRange(w, v->row, v->col, reach,
		                                             colour, inReach);
		for (int i = 0; i < numInReach; i++) {
			//If they have been visited before, or are in the predecessor array skip the rock
			if (visited[inReach[i].row][inReach[i].col] ||
			    predecessor[inReach[i].row][inReach[i].col] != NULL) {
				continue;
			}

			for (int z = 0; z < totalNumRocks; z++) {
				if (allRocks[z].row == inReach[i].row &&
				    allRocks[z].col == inReach[i].col) {
					QueueEnqueue(q, &allRocks[z]);
				}
			}

			predecessor[inReach[i].row][inReach[i].col] = v;
		}
		free(inReach);
	}

	QueueFree(q);

	for (int i = 0; i < height; i++) {
		free(predecessor[i]);
	}
	free(predecessor);

	for (int i = 0; i < height; i++) {
		free(visited[i]);
	}
	free(visited);

	free(allRocks);

	reversePath(path, predCount);

	struct path p = {predCount, path};

	return p;
}

struct path findMinEnergyPath(Wall w, int reach, int energyCosts[NUM_COLOURS]) {
	PQ pq = PQNew();
	int height = WallHeight(w);
	int width = WallWidth(w);

	struct rock *allRocks = malloc((height * width) * sizeof(struct rock));
	int totalNumRocks = WallGetAllRocks(w, allRocks);

	//Enqueue possible starting rocks
	for (int i = 0; i < totalNumRocks; i++) {
		Rock startingRock = &allRocks[i];
		if (startingRock->row <= reach) {
			double startingEnergyCost = 0.0;
			Edge startingEdge = (Edge){
			    .v = startingRock, .w = NULL, .weight = startingEnergyCost};
			PQInsert(pq, startingEdge);
		}
	}

	// Create arrays for visited rocks and previous rocks
	bool **visited = malloc(sizeof(bool *) * height);
	for (int i = 0; i < height; i++) {
		visited[i] = calloc(width, sizeof(bool));
	}
	Rock **prev = malloc(sizeof(struct rock *) * height);
	for (int i = 0; i < height; i++) {
		prev[i] = calloc(width, sizeof(struct rock));
	}
	for (int i = 0; i < w->height; i++) {
		for (int j = 0; j < w->width; j++) {
			visited[i][j] = false;
			prev[i][j] = NULL;
		}
	}

	// While there are still edges with lowest weight in the priority queue
	while (!PQIsEmpty(pq)) {
		Edge e = PQExtract(pq);
		Rock v = e.v;

		// If we've already visited this rock, skip it
		if (visited[v->row][v->col]) {
			continue;
		}

		// Otherwise mark it as visited and update previous
		visited[v->row][v->col] = true;
		prev[v->row][v->col] = e.w;

		//Reached the top of the wall, we have found a path
		if (v->row + reach >= height) {
			int numRocks = 0;
			Rock r = v;

			while (r != NULL) {
				numRocks++;
				r = prev[r->row][r->col];
			}

			struct rock *rocks = malloc(numRocks * sizeof(struct rock));

			assert(rocks != NULL);

			// Traverse the prev array backwards to populate the rocks array
			r = v;
			for (int i = numRocks - 1; i >= 0; i--) {
				rocks[i] = *r;
				r = prev[r->row][r->col];
			}

			for (int i = 0; i < height; i++) {
				free(prev[i]);
			}
			free(prev);

			for (int i = 0; i < height; i++) {
				free(visited[i]);
			}
			free(visited);
			PQFree(pq);
			free(allRocks);

			struct path p = {numRocks, rocks};
			return p;
		}
		struct rock *inReach = malloc((reach * reach) * sizeof(struct rock));

		int numInReach = WallGetRocksInRange(w, v->row, v->col, reach, inReach);
		for (int i = 0; i < numInReach; i++) {
			Rock f = &inReach[i];
			Colour c = f->colour;

			// Calculate energy cost
			double energyCost = energyCosts[c];
			double totalEnergyCost = e.weight + energyCost;

			for (int z = 0; z < totalNumRocks; z++) {
				if (allRocks[z].row == f->row && allRocks[z].col == f->col) {
					// Add w to the priority queue with the total energy cost as weight
					Edge newEdge = (Edge){.v = w->rocks[f->row][f->col],
					                      .w = v,
					                      .weight = totalEnergyCost};
					PQInsert(pq, newEdge);
				}
			}
		}
		free(inReach);
	}
	for (int i = 0; i < height; i++) {
		free(prev[i]);
	}
	free(prev);

	for (int i = 0; i < height; i++) {
		free(visited[i]);
	}
	free(visited);

	free(allRocks);

	PQFree(pq);
	struct path p = {0, NULL};
	return p;
}

struct path findMinTurnsPath(Wall w, int reach, int energyCosts[NUM_COLOURS],
                             int maxEnergy) {
	int currentEnergy = maxEnergy;

	//Call findMinEnergyPath to find shortest energy path
	struct path p = findMinEnergyPath(w, reach, energyCosts);

	//Create a new path array with space for rests
	struct rock *path = malloc(sizeof(struct rock) * (p.numRocks * 2));
	int numPlacedRocks = 0;

	//Check if rest is needed. If so, add the rock twice to the new path array
	for (int i = 0; i < p.numRocks; i++) {
		currentEnergy -= energyCosts[p.rocks[i].colour];
		if (i + 1 < p.numRocks &&
		    (currentEnergy - energyCosts[p.rocks[i + 1].colour]) <= 0) {
			path[numPlacedRocks] = p.rocks[i];
			numPlacedRocks++;
			path[numPlacedRocks] = p.rocks[i];
			numPlacedRocks++;
			currentEnergy = maxEnergy;
		} else {
			currentEnergy -= energyCosts[p.rocks[i].colour];
			path[numPlacedRocks] = p.rocks[i];
			numPlacedRocks++;
		}
	}

	free(p.rocks);
	struct path returnPath = {numPlacedRocks, path};

	return returnPath;
}
