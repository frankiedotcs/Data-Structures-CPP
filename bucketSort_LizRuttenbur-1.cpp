//Copyright 2015, Bradley Peterson, Weber State University, all rights reserved.


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <thread> //C++11 support!   Visual studio 2012+ users can use this if they want.
#include <mutex> 


//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <stdio.h>
#endif

using namespace std;

//*** Prototypes ***
void recQuickSort(unsigned long *arr, int first, int last);
void pressAnyKeyToContinue();

class BucketsCollection {
public:
	BucketsCollection(int bucketCapacity, int numBuckets);
	~BucketsCollection();
	void addItem(int bucket, unsigned long item);
	int getNumBuckets();
	int getNumItemsInABucket(int bucket);
	unsigned long * getBucketArray(int bucket);
	void copyBucketToAnotherArray(int bucket, unsigned long * destinationArray, int destinationArrayOffsetIndex);
	void copyOneBucketsIntoAnotherBuckets(BucketsCollection& smallBucket);
	void printAllBuckets();

private:
	unsigned long **buckets;
	int *itemCounter;
	int bucketSize;
	int numBuckets;

};


//***GLOBAL VARIABLES***
unsigned long *list;
int listSize;
int numBuckets;
int numThreads;
BucketsCollection *globalBuckets;
unsigned long ULONGMAX = 4294967295;

//*** Provide methods for the bucket class ***
BucketsCollection::BucketsCollection(int bucketCapacity, int numBuckets) {

	//Each bucket should be as bg as roughly the size of the list divided by number of buckets.
	//But some buckets will be bigger than others, so give an extra room.
	this->numBuckets = numBuckets;
	//Worst case scenario is that every value falls into one bucket.  Assume the worst case could 
	//happen and make sure each bucket could handle that much data.

	buckets = new unsigned long*[numBuckets];
	for (int i = 0; i < numBuckets; i++) {
		//printf("Requsting %d items for this bucket\n", listSize);
		buckets[i] = new unsigned long[bucketCapacity];
	}

	itemCounter = new int[numBuckets];
	for (int i = 0; i < numBuckets; i++) {
		itemCounter[i] = 0;
	}


}

BucketsCollection::~BucketsCollection() {

	for (int i = 0; i < numBuckets; i++) {
		delete[] buckets[i];
	}
	delete[] buckets;
	delete[] itemCounter;

}

void BucketsCollection::addItem(int bucket, unsigned long item) {
	//Pass in a bucket #, and the data, and it assigns that data to the bucket.

	buckets[bucket][itemCounter[bucket]] = item;
	itemCounter[bucket]++;
}

int BucketsCollection::getNumBuckets() {
	return numBuckets;
}

int BucketsCollection::getNumItemsInABucket(int bucket) {
	//You pass in the bucket #, it returns how many items that bucket contains.

	return itemCounter[bucket];
}

void BucketsCollection::printAllBuckets() {
	//Displays the contents of all buckets to the screen.

	printf("******\n");
	for (int i = 0; i < numBuckets; i++) {
		printf("Bucket number %d\n", i);
		for (int j = 0; j < itemCounter[i]; j++) {
			//cout << buckets[i][j] << " ";
			printf("%08X ", buckets[i][j]);

		}
		printf("\n");
	}
	printf("\n");
}

unsigned long * BucketsCollection::getBucketArray(int bucket) {
	//You pass in the bucket #, it returns the array that contains the bucket's data.

	return buckets[bucket];
}

void BucketsCollection::copyBucketToAnotherArray(int bucket, unsigned long * destinationArray, int destinationArrayOffsetIndex) {
	//Copies a bucket to an array.  You pass in the bucket #, the array, and the starting index (offset) or the array you are copying to
	//This then copies that bucket # into that array starting at that index (offset).

	for (int j = 0; j < itemCounter[bucket]; j++) {
		destinationArray[destinationArrayOffsetIndex + j] = buckets[bucket][j];
	}

}

void BucketsCollection::copyOneBucketsIntoAnotherBuckets(BucketsCollection& smallBucket) {
	//Copies all items in all buckets from one BucketsCollection object into another BucketsCollection object.  

	for (int i = 0; i < numBuckets; i++) {
		unsigned long * smallBucketArray = smallBucket.getBucketArray(i);
		//printf("Copying %d items between bucket %d\n", getNumItemsInABucket(bucket), bucket);
		for (int j = 0; j < smallBucket.getNumItemsInABucket(i); j++) {
			//printf("Copying %8X at index %d from small into big between bucket %d\n", smallBucketArray[i], i, bucket);
			addItem(i, smallBucketArray[j]);
		}
	}

}


//***Functions that help our bucket sort work.***
void printList() {
	for (int i = 0; i < listSize; i++) {
		//cout << list[i] << " ";
		printf("%08X ", list[i]);
	}
}

void createList() {

	list = new unsigned long[listSize];

	//generate random numbers
	srand(time(NULL));

	unsigned long num1;
	unsigned long num2;
	unsigned long num3;

	//generates 32 bit numbers
	for (int i = 0; i < listSize; i++) {
		//Make 15 bit numbers.  On Windows they're already 15 bit.
		//But in case the rand() gives us bigger random numbers, lets just make
		//everything 15 bit by only preserving the last 15 bits.  Then work with
		//what we've got and combine it back into a 32 bit number.
		num1 = rand() & 0x00007FFF;
		num2 = rand() & 0x00007FFF;
		num3 = rand() & 0x00007FFF;

		//shift num1 over 15 bits
		num1 = num1 << 15;
		//make a 30 bit number
		num1 = num1 | num2;

		//get two more bits
		num3 = num3 << 30;
		num3 = num3 & 0xC0000000;
		//make it a 32 bit number
		list[i] = num3 | num1;

	}
}

void placeIntoBuckets() {

	//TODO: Put the values into the appropriate buckets.
	//long buckets = ULONGMAX / numBuckets + 1;
	
	for (int i = 0; i < listSize; i++) {
		int bucket = (list[i] / (ULONGMAX / numBuckets + 1));
		globalBuckets->addItem(bucket, list[i]);
	}

}


void sortEachBucket() {

	
	
	for (int i = 0; i < numBuckets; ++i) {
		unsigned long *items = globalBuckets->getBucketArray(i);
		int size = globalBuckets->getNumItemsInABucket(i);
		recQuickSort(items, 0, size);
		
	}


	

}


void combineBuckets() {
	int size = 0;

	for (int i = 0; i < numBuckets; i++) {
		
		globalBuckets->copyBucketToAnotherArray(i, list, size);
		size = size + globalBuckets->getNumItemsInABucket(i);
	
		
		
	}
	//TODO: Copy each bucket back out to the original list[] array


}


void bucketSort(bool displayOutput) {

	//For the upcoming homeowork assignment, I think it will help you the most to split your work into these three functions.  
	placeIntoBuckets();


	if (displayOutput) {
		printf("Displaying each bucket's contents before sorting: \n");
		globalBuckets->printAllBuckets();
	}

	sortEachBucket();

	combineBuckets();

	if (displayOutput) {
		printf("Displaying each bucket's contents after sorting: \n");
		globalBuckets->printAllBuckets();
		pressAnyKeyToContinue();
		printf("Displaying what is hopefully a sorted array: \n");
		printList(); //See if it's all sorted.
	}


}


void swap(unsigned long *arr, int first, int second) {

	unsigned long temp;

	temp = arr[first];
	arr[first] = arr[second];
	arr[second] = temp;
}


int partition(unsigned long *arr, int first, int last) {
	unsigned long pivot;
	int index, smallIndex;

	//Take the middle value as the pivot.
	//swap(first, (first + last) / 2);
	pivot = arr[first];
	smallIndex = first;
	for (index = first + 1; index < last; index++) {
		if (arr[index] < pivot) {
			smallIndex++;
			swap(arr, smallIndex, index);
		}
	}

	//Move pivot into the sorted location
	swap(arr, first, smallIndex);

	//Tell where the pivot is
	return smallIndex;

}


void recQuickSort(unsigned long *arr, int first, int last) {
	//first is the first index
	//last is the one past the last index (or the size of the array
	//if first is 0)

	if (first < last) {
		//Get this sublist into two other sublists, one smaller and one bigger
		int pivotLocation = partition(arr, first, last);
		recQuickSort(arr, first, pivotLocation);
		recQuickSort(arr, pivotLocation + 1, last);
	}
}

void verifySort(unsigned long *arr, unsigned int arraySize) {
	for (int i = 1; i < arraySize; i++) {
		if (arr[i] < arr[i - 1]) {
			printf("ERROR, this list was not sorted correctly.  At index %d is value %08X.  At index %d is value %08X\n", i - 1, arr[i - 1], i, arr[i]);
			return;
		}
	}
	printf("PASSED SORT TEST - The list was sorted correctly\n");
}
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

int main() {



	//Set the listSize, numBuckets, and numThreads global variables.  
	listSize = 100;

	numBuckets = 2;
	numThreads = 2;
	createList();
	globalBuckets = new BucketsCollection(listSize, numBuckets);
	printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	printf("Displaying the unsorted list array:\n");
	printList(); //useful for debugging small amounts of numbers.  
	pressAnyKeyToContinue();
	bucketSort(true);
	verifySort(list, listSize);
	pressAnyKeyToContinue();
	delete globalBuckets;
	delete[] list;

	numBuckets = 4;
	numThreads = 4;
	createList();
	printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	globalBuckets = new BucketsCollection(listSize, numBuckets);
	pressAnyKeyToContinue();
	bucketSort(true);
	verifySort(list, listSize);
	pressAnyKeyToContinue();
	delete globalBuckets;
	delete[] list;

	printf("\nFor timing purposes, please make sure all printf statements are commented out\n");
	pressAnyKeyToContinue();

	listSize = 4000000;
	numBuckets = 1;
	numThreads = 1;
	createList();
	globalBuckets = new BucketsCollection(listSize, numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, this is effectively a quick sort\n", listSize, numBuckets, numThreads);
	auto start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> diff = end - start;
	printf("Finished quick sort simulation in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;


	listSize = 4000000;
	numBuckets = 2;
	numThreads = 2;
	createList();
	globalBuckets = new BucketsCollection(listSize, numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	numBuckets = 4;
	numThreads = 4;
	createList();
	globalBuckets = new BucketsCollection(listSize, numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	numBuckets = 8;
	numThreads = 8;
	createList();
	globalBuckets = new BucketsCollection(listSize, numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	numBuckets = 16;
	numThreads = 16;
	createList();
	globalBuckets = new BucketsCollection(listSize, numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	pressAnyKeyToContinue();
	return 0;
}