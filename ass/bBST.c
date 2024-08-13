
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "bBST.h"

////////////////////////////////////////////////////////////////////////
/* IMPORTANT:
   Do NOT modify the code above this line.
*/
////////////////////////////////////////////////////////////////////////

// Balanced Binary Search Trees program written by z5418201 on 30/03/23

////////////////////////////////////////////////////////////////////////
// Auxiliary function prototypes

// Write Auxiliary function prototypes here, and declare them as static

////////////////////////////////////////////////////////////////////////
static void freeAVL(Node node);
static int treeHeight(Node node);
static Node insertAVL(Node node, int key);
static int findBalance(Node node);
static Node deleteAVL(Node node, int key);
static Node rotateLeft(Node node);
static Node rotateRight(Node node);
static Node newNode(int key);
static int max(int a, int b);
static void freeAVL(Node node);
static void addList(Node node, List l);
static Node traverseSmallest(Node node, int *count, int k);
static Node traverseLargest(Node node, int *count, int k);
static int traverseFloor(Node node, int key);
static int traverseCeiling(Node node, int key);
static void addSearchList(Node node, int lower, int upper, List l);
static Node findLCA(Node node, int a, int b);
////////////////////////////////////////////////////////////////////////

/**
 * Creates a new empty tree.
 * The time complexity of this function must be O(1).
 */
Tree TreeNew(void) {
	//Allocates memory for new tree
	Tree t = (Tree)malloc(sizeof(struct tree));
	//If memory allocation was unsucessful, throw error
	if (t == NULL) {
		fprintf(stderr, "there is not enough memory!\n");
		exit(EXIT_FAILURE);
	}
	//Set root node to NULL
	t->root = NULL;
	return t;
}

////////////////////////////////////////////////////////////////////////

/**
 * Frees all memory allocated for the given tree.
 * The time complexity of this function must be O(n).
 */
static void freeAVL(Node node) {
	//once all nodes are freed, return
	if (node == NULL) {
		return;
	}
	
	//recursively frees all nodes of tree
	freeAVL(node->left);
	freeAVL(node->right);
	free(node);
}

void TreeFree(Tree t) {
	//Call recursive free function
	freeAVL(t->root);
	free(t);
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches the tree for a given key and returns true if the key is in
 * the tree or false otherwise.
 * The time complexity of this function must be O(log n).
 */
bool TreeSearch(Tree t, int key) {

	// if root node is NULL, the tree is empty 
	if (t->root == NULL) {
		return false;
	}
	Node curr = t->root;

	//iterate and search for the key
	while (curr != NULL) {
		if (curr->key > key) {
			curr = curr->left;
		} else if (curr->key < key) {
			curr = curr->right;
		} else {

			//if key is found return true
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////

// This function finds and returns the height of a node in the tree
static int treeHeight(Node node) {

	//if node is empty return 0 as the height
	if (node == NULL) {
		return 0;
	}

	//return height int from the node struct
	return node->height;
}


// This function takes two integers a and b, and returns the maximum of the two.
// If a is greater than b, it returns a. Otherwise, it returns b.
static int max(int a, int b) {

	// If a is greater than b, it returns a. Otherwise, returns b.
	if (a > b) {
		return a;
	}
	return b;
}
	
	
// This function creates a new Node with the given key.
// It initializes the Node's fields and returns a pointer to the new Node.
// If there is not enough memory, it prints an error message and exits the program.
static Node newNode(int key) {
	Node node = (Node)malloc(sizeof(struct node));
	if (node == NULL) {
		fprintf(stderr, "there is not enough memory!\n");
		exit(EXIT_FAILURE);
	}

	node->key = key;
	node->left = NULL;
	node->right = NULL;
	node->height = 1;
	return node;
}

// This function performs a left rotation on the given Node.
// It updates the Node's height and returns a pointer to the new root of the rotated subtree.
static Node rotateLeft(Node node) {
	Node temp = node->right;
	node->right = temp->left;
	temp->left = node;
	node->height = 1 + max(treeHeight(node->left), treeHeight(node->right));
	temp->height = 1 + max(treeHeight(temp->left), treeHeight(temp->right));
	return temp;
}

// This function performs a right rotation on the given Node.
// It updates the Node's height and returns a pointer to the new root of the rotated subtree.
static Node rotateRight(Node node) {
	Node temp = node->left;
	node->left = temp->right;
	temp->right = node;
	node->height = 1 + max(treeHeight(node->left), treeHeight(node->right));
	temp->height = 1 + max(treeHeight(temp->left), treeHeight(temp->right));
	return temp;
}

// This function finds the balance factor of the given Node.
// It subtracts the height of the right subtree from the height of the left subtree.
// If the Node is NULL, it returns 0.

static int findBalance(Node node) {
	if (node == NULL) {
		return 0;
	} else {
		return treeHeight(node->left) - treeHeight(node->right);
	}
}


// This function inserts a Node with the given key into the AVL tree rooted at the given Node.
// It uses the proper AVL insertion algorithm to maintain the AVL tree properties.
// It returns a pointer to the new root of the AVL tree.
static Node insertAVL(Node node, int key) {

	// If the Node is NULL, create a new Node with the given key.
	if (node == NULL) {
		return (newNode(key));
	}

	// If the key is less than the Node's key, insert it into the left subtree.
	if (key < node->key) {
		node->left = insertAVL(node->left, key);
	} else if (key > node->key) {

		// If the key is greater than the Node's key, insert it into the right subtree.
		node->right = insertAVL(node->right, key);
	} else {

		// If the key is already present in the tree, return the Node.
		return node;
	}

	// Update the height of the Node.
	node->height = 1 + max(treeHeight(node->left), treeHeight(node->right));

	// Find the balance factor of the Node.
	int balance = findBalance(node);

	// Handles Left Left and Left Right balance cases
	if (balance > 1) {
		if (key > node->left->key) {
			node->left = rotateLeft(node->left);
			return rotateRight(node);
		}
		return rotateRight(node);

	//Handles Right Right and Right Left balance cases
	} else if (balance < -1) {
		if (key < node->right->key) {
			node->right = rotateRight(node->right);
			return rotateLeft(node);
		}
		return rotateLeft(node);
	}

	return node;
}

bool TreeInsert(Tree t, int key) {

	//If key is already in tree, or key is undefined, return false
	if (TreeSearch(t, key) == true) {
		return false;
	} else if (key == UNDEFINED) {
		return false;
	}

	t->root = insertAVL(t->root, key);

	return true;
}

////////////////////////////////////////////////////////////////////////

/**
 * Deletes the given key from the tree if it is present.
 * You must use the proper AVL deletion algorithm.
 * Returns true if the key was deleted successfully, or false if the key
 * was not present in the tree.
 * The time complexity of this function must be O(log n).
 */


// This function Deletes a Node with the given key into the AVL tree rooted at the given Node.
// It uses the proper AVL deletion algorithm to maintain the AVL tree properties.
// It returns a pointer to the new root of the AVL tree.
static Node deleteAVL(Node node, int key) {

	// If the node is NULL, return it (base case)
	if (node == NULL) {
		return node;
	}

	// If the key is less than the current node's key, traverse to the left subtree
	if (key < node->key) {
		node->left = deleteAVL(node->left, key);

	// If the key is greater than the current node's key, traverse to the right subtree
	} else if (key > node->key) {
		node->right = deleteAVL(node->right, key);

	// If the key matches the current node's key
	} else {

		// If the node has only one child or no child
		if (node->left == NULL || node->right == NULL) {

			// Set the temp node to the non-NULL child
			Node temp;
			if (node->left) {
				temp = node->left;
			} else {
				temp = node->right;
			}

			// If the temp node is NULL, set it to the current node and set the current node to NULL
			if (temp == NULL) {
				temp = node;
				node = NULL;
			} else {

				// Copy the contents of the temp node into the current node
				*node = *temp;
			}

			// Free the temp node
			free(temp);

		// If the node has two children
		} else {
			
			// Find the node with the smallest key in the right subtree
			Node curr = node->right;
			while (curr->left != NULL) {
				curr = curr->left;
			}
			// Copy the key of the smallest node in the right subtree into the current node	
			node->key = curr->key;

			// Delete the node with the smallest key in the right subtree
			node->right = deleteAVL(node->right, curr->key);
		}
		
		if (node == NULL) {
			return node;
		}
	}

	// Update the height of the Node.
	node->height = 1 + max(treeHeight(node->left), treeHeight(node->right));
	
	// Find the balance factor of the Node.
	int balance = findBalance(node);

	// Handles Left Left and Left Right balance cases
	if (balance > 1) {
		if (key > node->left->key) {
			node->left = rotateLeft(node->left);
			return rotateRight(node);
		}
		return rotateRight(node);

	//Handles Right Right and Right Left balance cases
	} else if (balance < -1) {
		if (key < node->right->key) {
			node->right = rotateRight(node->right);
			return rotateLeft(node);
		}
		return rotateLeft(node);
	}

	return node;
}

bool TreeDelete(Tree t, int key) {

	// If the key is not in the tree, return false
	if (TreeSearch(t, key) == false) {
		return false;
	}

	t->root = deleteAVL(t->root, key);
	return true;
}

////////////////////////////////////////////////////////////////////////

/**
 * Creates a list containing all the keys in the given tree in ascending
 * order.
 * The time complexity of this function must be O(n).
 */


// This function takes a Node and adds its keys to a List in order.
// It is called recursively for each node in the tree using its left and right subtrees.

static void addList(Node node, List l) {
	if (node == NULL) {
		return;
	}

	addList(node->left, l);
	ListAppend(l, node->key);
	addList(node->right, l);
}

List TreeToList(Tree t) {
	List l = ListNew();
	addList(t->root, l);
	return l;
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th smallest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the smallest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */


// This recursive function finds and returns the k-th smallest key in the Tree
static Node traverseSmallest(Node node, int *count, int k) {
	if (node == NULL) {
		return NULL;
	}

	// Recursively search the left subtree.
	Node left = traverseSmallest(node->left, count, k);

	if (left != NULL) {
		return left;
	}

	// Increment the count of nodes seen so far.
	(*count)++;

	if (*count == k) {
		return node;
	}

	// Recursively search the right subtree.
	return traverseSmallest(node->right, count, k);
}

int TreeKthSmallest(Tree t, int k) {
	// TODO: Complete this function
	int count = 0;
	int returnValue = traverseSmallest(t->root, &count, k)->key;
	return returnValue;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the k-th largest key in the tree.
 * Assumes that k is between 1 and the number of nodes in the tree.
 * k = 1 will return the largest value in the tree.
 * The time complexity of this function must be O(log n + k).
 */


// This recursive function finds and returns the k-th largest key in the Tree
static Node traverseLargest(Node node, int *count, int k) {
	if (node == NULL) {
		return NULL;
	}

	// Recursively search the right subtree.
	Node right = traverseLargest(node->right, count, k);

	if (right != NULL) {
		return right;
	}

	// Increment the count of nodes seen so far.
	(*count)++;
	if (*count == k) {
		return node;
	}

	// Recursively search the left subtree.
	return traverseLargest(node->left, count, k);
}

int TreeKthLargest(Tree t, int k) {
	int count = 0;
	int returnValue = traverseLargest(t->root, &count, k)->key;
	return returnValue;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the least common ancestor of two keys, a and b.
 * Returns UNDEFINED if either a or b are not present in the tree.
 * a may be larger than b, or equal to each other.
 * Either a or b may be the LCA in some cases.
 * The time complexity of this function must be O(log n).
 */

// This function finds the (LCA) of two nodes with keys 'a' and 'b' 
static Node findLCA(Node node, int a, int b) {

	// Loop until we reach a leaf node or we find the LCA node
	while (node != NULL) {

		// If both a and b are less than the current node's key, 
		//move to the left subtree
		if (node->key > a && node->key > b) {
			node = node->left;
		
		// If both a and b are greater than the current node's key, 
		//move to the right subtree
		} else if (node->key < a && node->key < b) {
			node = node->right;

		// If the current node's key is between a and b, 
		//we have found the LCA node
		} else {
			break;
		}
	}

	return node;
}


// This function finds the lowest common ancestor (LCA) of nodes a and b
// It returns the LCA node's key, 
// or UNDEFINED if either 'a' or 'b' is not found in the tree
int TreeLCA(Tree t, int a, int b) {
	if (TreeSearch(t, a) == false || TreeSearch(t, b) == false) {
		return UNDEFINED;
	}

	Node lca = findLCA(t->root, a, b);

	// If the LCA node is found, return its key. Otherwise, return UNDEFINED
	if (lca != NULL) {
		return lca->key;
	} else {
		return UNDEFINED;
	}
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the largest key less than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */

// This function traverses the tree to find the floor key that is 
// the largest key less than or equal to the given key.
// It returns UNDEFINED if there is no such key.
static int traverseFloor(Node node, int key) {

	// Have reached leaf node 
	if (node == NULL) {
		return UNDEFINED;
	}

	// If the current node's key is equal to 'key', we have found the floor key
	if (node->key == key) {
		return key;
	}

	// If the current node's key is greater than the given key,
	// we move left to find the floor key.
	if (node->key > key) {
		return traverseFloor(node->left, key);

	// If the current node's key is less than the given key,
	// we move right and check if the right subtree has a larger floor key.
	} else if (node->key < key) {
		int floor = traverseFloor(node->right, key);
		if (node->key >= floor) {
			return node->key;
		} else {
			return floor;
		}
	}

	// no floor key found
	return UNDEFINED;
}


// This function calls the traverseFloor function to find the floor key.
int TreeFloor(Tree t, int key) {
	int floor = traverseFloor(t->root, key);
	return floor;
}

////////////////////////////////////////////////////////////////////////

/**
 * Returns the smallest key greater than or equal to the given value.
 * Returns UNDEFINED if there is no such key.
 * The time complexity of this function must be O(log n).
 */

// This function traverses the tree to find the ceiling key that is 
// the smallest key greater than or equal to the given key.
// It returns UNDEFINED if there is no such key.
static int traverseCeiling(Node node, int key) {

	// If we have reached a leaf node, there is no ceiling key.
	if (node == NULL) {
		return UNDEFINED;
	}

	// If the current node's key is equal to the given key, it is the ceiling key.
	if (node->key == key) {
		return key;
	}

	// If the current node's key is less than the given key,
	// we move right to find the ceiling key.
	if (node->key < key) {
		return traverseCeiling(node->right, key);

	// If the current node's key is greater than the given key,
	// we move left and check if the left subtree has a smaller ceiling key.	
	} else if (node->key > key) {
		int ceiling = traverseCeiling(node->left, key);
		if (ceiling == UNDEFINED || node->key <= ceiling) {
			return node->key;
		} else {
			return ceiling;
		}
	}

	// the current node is the ceiling key
	return node->key;
}

// This function calls the traverseCeiling function to find the ceiling key.
int TreeCeiling(Tree t, int key) {
	int ceiling = traverseCeiling(t->root, key);
	return ceiling;
}

////////////////////////////////////////////////////////////////////////

/**
 * Searches for all keys between the two given keys (inclusive) and
 * returns the keys in a list in ascending order.
 * Assumes that lower is less than or equal to upper.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 */


// This function searches for all keys between the two given keys (inclusive)
// and returns the keys in a list in ascending order.
static void addSearchList(Node node, int lower, int upper, List l) {

	// If the current node is NULL, there are no keys to add to the list
	if (node == NULL) {
		return;
	}

	// Recursively search the left subtree of the current node
	addSearchList(node->left, lower, upper, l);

	// If the current node's key is within the range of [lower, upper], 
	// add it to the list
	if (node->key >= lower && node->key <= upper) {
		ListAppend(l, node->key);
	}

	// Recursively search the right subtree of the current node
	addSearchList(node->right, lower, upper, l);
}


// This function creates a new list, calls the helper function to add all keys within the specified range,
// and returns the list of keys in ascending order.
List TreeSearchBetween(Tree t, int lower, int upper) {

	List l = ListNew();
	addSearchList(t->root, lower, upper, l);

	return l;
}

////////////////////////////////////////////////////////////////////////
