// Implementation of the Wall ADT

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Wall.h"

typedef struct rock *Rock;

struct wall {
	int height;
	int width;
	int numRocks;
	Rock **rocks;
};

static int compareRocks(const void *ptr1, const void *ptr2);

/**
 * Creates a new blank wall with the given dimensions
 */
Wall WallNew(int height, int width) {
	Wall w = malloc(sizeof(struct wall));
	assert(w != NULL);

	w->height = height;
	w->width = width;
	w->numRocks = 0;

	w->rocks = malloc(sizeof(Rock *) * height);
	for (int i = 0; i < height; i++) {
		w->rocks[i] = calloc(width, sizeof(struct rock));
	}

	return w;
}

/**
 * Frees all memory allocated to the wall 
 */
void WallFree(Wall w) {
	for (int i = 0; i < w->height; i++) {
		for (int j = 0; j < w->width; j++) {
			free(w->rocks[i][j]);
		}
		free(w->rocks[i]);
	}
	free(w->rocks);

	free(w);
}

/**
 * Returns the height of the wall
 */
int WallHeight(Wall w) {
	// TODO
	return w->height;
}

/**
 * Returns the width of the wall
 */
int WallWidth(Wall w) {
	// TODO
	return w->width;
}

/**
 * Adds a rock to the wall
 * If there is already a rock at the given coordinates, replaces it
 */
void WallAddRock(Wall w, struct rock rock) {
	int alreadyRock = 0;


	//If there is already a rock at the position, free it
	if (w->rocks[rock.row][rock.col] != NULL) {
		alreadyRock = 1;
		free(w->rocks[rock.row][rock.col]);
	}

	//allocate memory and place rock at row and col
	w->rocks[rock.row][rock.col] = malloc(sizeof(struct rock));
	assert(w->rocks[rock.row][rock.col] != NULL);
	w->rocks[rock.row][rock.col]->row = rock.row;
	w->rocks[rock.row][rock.col]->col = rock.col;
	w->rocks[rock.row][rock.col]->colour = rock.colour;

	if (alreadyRock == 0) {
		w->numRocks++;
	}
}

/**
 * Returns the number of rocks on the wall
 */
int WallNumRocks(Wall w) {
	// TODO
	return w->numRocks;
}

/**
 * Stores all rocks on the wall in the given `rocks` array and returns
 * the number of rocks stored. Assumes that the array is at least as
 * large as the number of rocks on the wall.
 */
int WallGetAllRocks(Wall w, struct rock rocks[]) {
	int rockCounter = 0;

	//loop through the entire wall and find all rocks
	for (int i = 0; i < w->height; i++) {
		for (int z = 0; z < w->width; z++) {
			if (w->rocks[i][z] != NULL) {
				rocks[rockCounter] = *w->rocks[i][z];
				rockCounter++;
			}
		}
	}

	return rockCounter;
}

/**
 * Stores all rocks that are within a distance of `dist` from the given
 * coordinates in the given `rocks` array and returns the number of rocks
 * stored. Assumes that the array is at least as large as the number of
 * rocks on the wall.
 */

int WallGetRocksInRange(Wall w, int row, int col, int dist,
                        struct rock rocks[]) {
	int maxRow, minRow, maxCol, minCol;
	int rockCounter = 0;

	//Ensure that the max and min row/cols do not go out of range
	maxRow = (row + dist >= w->height) ? w->height - 1 : row + dist;
	minRow = (row - dist <= 0) ? 1 : row - dist;
	maxCol = (col + dist >= w->width) ? w->width - 1 : col + dist;
	minCol = (col - dist <= 0) ? 1 : col - dist;


	//Loop and get all rocks in a square range of reach
	for (int i = minRow; i <= maxRow; i++) {
		for (int j = minCol; j <= maxCol; j++) {
			if (i >= 0 && i < w->height && j >= 0 && j < w->width &&
			    w->rocks[i][j] != NULL && w->rocks[i][j]->row == i &&
			    w->rocks[i][j]->col == j) {

				//check that the rock is within the range
				double sqrtDist = (pow(row - i, 2) + pow(col - j, 2));
				if (sqrtDist <= pow(dist, 2)) {
					rocks[rockCounter++] = *w->rocks[i][j];
				}
			}
		}
	}
	return rockCounter;
}

/**
 * Stores all rocks with the colour `colour` that are within a distance
 * of `dist` from the given coordinates in the given `rocks` array and
 * returns the number of rocks stored. Assumes that the array is at
 * least as large as the number of rocks on the wall.
 */
int WallGetColouredRocksInRange(Wall w, int row, int col, int dist,
                                Colour colour, struct rock rocks[]) {
	
	int maxRow, minRow, maxCol, minCol;
	int rockCounter = 0;

	//Ensure that the max and min row/cols do not go out of range
	maxRow = (row + dist >= w->height) ? w->height - 1 : row + dist;
	minRow = (row - dist <= 0) ? 1 : row - dist;
	maxCol = (col + dist >= w->width) ? w->width - 1 : col + dist;
	minCol = (col - dist <= 0) ? 1 : col - dist;

	for (int i = minRow; i <= maxRow; i++) {
		for (int j = minCol; j <= maxCol; j++) {
			if (i >= 0 && i < w->height && j >= 0 && j < w->width &&
			    w->rocks[i][j] != NULL && w->rocks[i][j]->row == i &&
			    w->rocks[i][j]->col == j) {

				//check that the rock is within the range and same colour
				double sqrtDist = pow(row - i, 2) + pow(col - j, 2);
				if (sqrtDist <= pow(dist, 2) &&
				    (w->rocks[i][j]->colour == colour)) {
					rocks[rockCounter++] = *w->rocks[i][j];
				}
			}
		}
	}
	return rockCounter;
}

////////////////////////////////////////////////////////////////////////

/**
 * Prints the wall out in a nice format
 * NOTE: DO NOT MODIFY THIS FUNCTION! This function will work once
 *       WallGetAllRocks and all the functions above it work.
 */
void WallPrint(Wall w) {
	int height = WallHeight(w);
	int width = WallWidth(w);
	int numRocks = WallNumRocks(w);
	struct rock *rocks = malloc(numRocks * sizeof(struct rock));
	WallGetAllRocks(w, rocks);
	qsort(rocks, numRocks, sizeof(struct rock), compareRocks);

	int i = 0;
	for (int y = height; y >= 0; y--) {
		for (int x = 0; x <= width; x++) {
			if ((y == 0 || y == height) && (x == 0 || x % 5 == 0)) {
				printf("+ ");
			} else if ((x == 0 || x == width) && (y == 0 || y % 5 == 0)) {
				printf("+ ");
			} else if (y == 0 || y == height) {
				printf("- ");
			} else if (x == 0 || x == width) {
				printf("| ");
			} else if (i < numRocks && y == rocks[i].row && x == rocks[i].col) {
				char *color;
				switch (rocks[i].colour) {
					case GREEN: color = "\x1B[32m"; break;
					case TEAL: color = "\x1B[96m"; break;
					case PINK: color = "\x1B[95m"; break;
					case RED: color = "\x1B[91m"; break;
					default: color = "\x1B[0m"; break;
				}
				printf("%so %s", color, RESET);
				i++;
			} else {
				printf("\u00B7 ");
			}
		}
		printf("\n");
	}

	free(rocks);
}

static int compareRocks(const void *ptr1, const void *ptr2) {
	struct rock *r1 = (struct rock *)ptr1;
	struct rock *r2 = (struct rock *)ptr2;
	if (r1->row != r2->row) {
		return r2->row - r1->row;
	} else {
		return r1->col - r2->col;
	}
}
