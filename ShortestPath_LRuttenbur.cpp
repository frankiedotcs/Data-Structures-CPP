#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#endif

using namespace std;
class graphEdge;

//global arrays
int *graphWeights;
int *columns;
int *rows;
int *smallestWeight;
int getWeights(int vertex, int j);

//The collection of edge objects
vector<graphEdge> edges;
int numEdges;
int numNodes;

const int LARGE_NUMBER = 99999999;

//The class for an edge object.  
class graphEdge {
public:
	int sourceNode;
	int destNode;
	int weight;
	graphEdge() {};
	graphEdge(int sourceNode, int destNode, int weight) {
		this->sourceNode = sourceNode;
		this->destNode = destNode;
		this->weight = weight;

	}
};

void pressAnyKeyToContinue() {
	printf("Press any key to continue\n");

	//Linux and Mac users with g++ don't need this
	//But everyone else will see this message.
#ifndef __GNUC__
	_getch();
#else
	int c;
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
#endif
}

void shortestPath(int vertex) {

	//TODO: Fix the book's code so that it works without data

	//The argument is the vertex to search from
	for (int j = 0; j < numNodes; j++) {
		//intialization step
		smallestWeight[j] = getWeights(vertex, j);
	}
	//continue initializing
	bool *weightFound = new bool[numEdges];
	for (int j = 0; j < numNodes; j++) {
		weightFound[j] = false;
	}

	//The node we're at we assume we can get to with a cost of zero.
	weightFound[vertex] = true;
	smallestWeight[vertex] = 0;

	//For every node...
	for (int i = 0; i < numNodes - 1; i++) {
		if (i % 100 == 0) {
			cout << "Looking up shortest path for " << i << " of " << numNodes - 1 << " amount of nodes" << endl;

		}
		else if (i == numNodes - 2) {
			cout << "Looking up the shortest path for the last node" << endl;
		}
		int minWeight = LARGE_NUMBER;
		int v = -1;

		//Of the nodes who haven't been marked as completed,
		//or in other words, nodes which we aren't sure if we've found the
		//smallest path weight
		//Of those ndoes...find the node with the smallest current weight.
		for (int j = 0; j < numNodes; j++) {
			if (!weightFound[j]) {
				if (smallestWeight[j] < minWeight) {
					v = j;
					minWeight = smallestWeight[v];
				}
			}
		}

		//Ignore non connected nodes.
		if (v != -1) {

			//When I drew the red line on my notes.  
			weightFound[v] = true;
			//Now that we've found a new shortest possible weight (at node v)
			//look at all of v's neighborly costs, and see if we can get to v's neighbors
			//using v, at a better weight than what we already know.
			for (int j = 0; j < numNodes; j++) {
				if (!weightFound[j]) {
					if (minWeight + getWeights(v, j) < smallestWeight[j]) {
						smallestWeight[j] = minWeight + getWeights(v, j);
					}
				}
			}
		}
	}
}


//Some functions I wrote to help the stable sort know what to sort against.  
//You don't need to worry about these or modify them.  
bool compareFirstColumn(const graphEdge& x, const graphEdge& y) {
	if (x.sourceNode < y.sourceNode) {
		return true;
	}
	else {
		return false;
	}
}
bool compareSecondColumn(const graphEdge& x, const graphEdge& y) {
	if (x.destNode < y.destNode) {
		return true;
	}
	else {
		return false;
	}
}

void testRun() {
	//This loads the same values found in the book:
	graphEdge *edge;
	edge = new graphEdge(0, 1, 16);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(0, 3, 2);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(0, 4, 3);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(1, 2, 5);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(2, 1, 3);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(3, 1, 12);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(3, 4, 7);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(4, 1, 10);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(4, 2, 4);
	edges.push_back(*edge);
	delete edge;
	edge = new graphEdge(4, 3, 5);
	edges.push_back(*edge);
	delete edge;

	numNodes = 5;
	numEdges = 10;


}


void readFile() {
	ifstream inFile("rome99.gr");
	int counter = 0;
	int largestNode = 0;
	char throwaway;
	if (!inFile.good())
	{
		cerr << "The file wasn't found.  For Visual Studio users, make sure it's in the same directory as your solution's .vcxproj file." << endl;
		pressAnyKeyToContinue();
		exit(-1);
	}
	else {
		string line;
		graphEdge edge;
		while (getline(inFile, line)) {


			if (line.at(0) == 'a' && line.at(1) == ' ') {
				if (counter % 10000 == 0) {
					cout << "Reading edge # " << counter << endl;
				}
				//this line is one we keep, read in the data
				istringstream iss(line);
				iss >> throwaway >> edge.sourceNode >> edge.destNode >> edge.weight;
				if (edge.sourceNode > largestNode) {
					largestNode = edge.sourceNode;
				}

				if (edge.destNode > largestNode) {
					largestNode = edge.destNode;
				}
				edges.push_back(edge);
				counter++;
			}
		}
		numNodes = largestNode;
		numEdges = counter;


		//Create a zero node with an edge that points to itself with a weight of 0.
		//The file node data starts at node #1, so we want to make 
		//everything clean by letting edge 1 take index 1 in our graphWeights array,
		//and this zero node can take index 0.  
		graphEdge zeroEdge(0, 0, 0);
		edges.push_back(zeroEdge);
		numNodes++;
		numEdges++;
		//Because we started a zero node, increase the numNodes by 1., 
		cout << "Finished reading " << numNodes << " nodes and " << numEdges << " edges." << endl;
	}
}

void createCsrArrays() {

	//TODO:
	//Take the collection of graphEdge objects in our edges vector, and convert them into CSR format
	//For example, this graph form figure 12-8 would look like this as a 2-D array:
	// 0         16        99999999  2         3
	// 99999999  0         5         99999999  99999999
	// 99999999  3         0         99999999  99999999
	// 99999999  12        99999999  0         7
	// 99999999  10        4         5         0
	//But we don't have a 2D array in this assignment.  We have a vector/collection object.
	//In this vector object, edges are simply listed one edge at a time, so the 0th
	//object should contain (0, 1, 16) for 0 goes to 1 with a cost of 16.

	//So use the vector and turn the data into CSR format, we will have it look like this:

	//Note: This only takes about 15 lines of code.  But you need to carefully think this out.  Do not
	//be discouraged when your first few attempts don't work.
	//graphWeights: 16 2  3  5  3  12 7 10 4  5
	//columns:      1  3  4  2  1  1  4 1  2  3
	//row:          0  3  4  5  7  10

	//You should use three global arrays, with names of graphWeights, columns, and row.
	//Here would also be a good place to size the smallestWeight array.  It has the same size as 
	//graphWeights and columns.  Also make sure you declare it globally, just as you did the 
	//prior three arrays.
	graphWeights = new int[numEdges];
	columns = new int [numEdges];
	rows = new int [numNodes + 1];
	smallestWeight = new int [numNodes];
	int count = 0;
	rows[0] = 0;

	vector <graphEdge>::iterator front;
	front = edges.begin();


	for (int i = 0; i < numEdges; i++, front++) {
		graphWeights[i] = front->weight;
		columns[i] = front->destNode;
		
	}

	for (int i = 0; i < numNodes; i++) {
		for (front = edges.begin(); front != edges.end(); front++) {
			if (front->sourceNode == i) {
				count++;
			}
		}
		rows[i + 1] = count;
	}
}

void deleteArrays() {
	//TODO:

	//Delete your arrays you created in createCsrArrays()
	delete [] graphWeights;
	delete [] rows;
	delete [] columns;
	delete [] smallestWeight;


	graphWeights = NULL;
	rows = NULL;
	columns = NULL;
	smallestWeight = NULL;
}

int getWeights(int v, int j) {

	for (int k = rows[v]; k < rows[v+1]; k++) {
		if (columns[k] == j) {
			return graphWeights[k];
		}
		
	}
	return LARGE_NUMBER;
}

int main() {

	cout << "Would you like to do: " << endl << "1. Test run" << endl << "2. Full run" << endl << "Enter your selection: ";
	int selection;
	cin >> selection;
	if (selection == 1) {
		testRun();
	}
	else if (selection == 2) {
		readFile();

	}

	//The collection of edge objects is just an unsorted collection.  
	//So use a stable sort to sort by first column and second column so 
	//they are in a clean order ready to go into CSR format.
	stable_sort(edges.begin(), edges.end(), compareSecondColumn);
	cout << "Halfway done sorting..." << endl;
	stable_sort(edges.begin(), edges.end(), compareFirstColumn);
	cout << "Finished sorting..." << endl;

	createCsrArrays();

	if (selection == 1) {
		cout << "Test run debugging.  Your CSR arrrays are: " << endl;
		cout << "weights: ";
		for (int i = 0; i < numEdges; i++) {
			cout << graphWeights[i] << " ";
		}
		cout << endl;
		cout << "columns: ";
		for (int i = 0; i < numEdges; i++) {
			cout << columns[i] << " ";
		}
		cout << endl;
		cout << "row: ";
		for (int i = 0; i < numNodes + 1; i++) {
			cout << rows[i] << " ";
		}
		cout << endl;
	}


	int userNode = 0;
	cout << "Which node would you like to search from: ";
	cin >> userNode;

	//call our shortest path algorithm
	shortestPath(userNode);

	do {
		cout << "Which node do you want to see weights for (-1 to exit): ";
		cin >> userNode;

		if (userNode == -1) {
			break;
		}
		if (userNode >= 0 && userNode < numNodes) {
			cout << "For node " << userNode << " the cost is " << smallestWeight[userNode] << endl;
		}
		else {
			cerr << "Error: That's no node with that ID!" << endl;
		}
	} while (true);


	deleteArrays();

	pressAnyKeyToContinue();
}