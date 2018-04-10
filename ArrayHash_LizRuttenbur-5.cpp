//Copyright 2015, Bradley Peterson, Weber State University, all rights reserved.

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <functional>

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <stdio.h>
#endif


using namespace std;

//forward declarations  (this helps with top-down compilation issues)
class manageMemory;
class Error;
template <typename KEY, typename VALUE> class arrayHashIterator;
template <typename KEY, typename VALUE> class arrayHashTable;
class product;


//--------------------------------------------------------------------------
//A class I designed to help keep track of how much memory you allocate
//Do not modify, this is not part of your assignment, it just helps test it.
//For this to work, a class needs to inherit off of this one. 
//Then this does the rest of the work, since it 
//overloaded new, new[], delete, and delete[].
//--------------------------------------------------------------------------
class manageMemory {
public:

	static std::size_t getTotalSize() {
		std::size_t total = 0;
		std::map<void *, std::size_t>::iterator iter;
		for (iter = mapOfAllocations.begin(); iter != mapOfAllocations.end(); ++iter) {
			total += iter->second;
		}
		return total;
	}

	//I overloaded the new and delete keywords so I could manually track allocated memory.
	void* operator new(std::size_t x){
		void *ptr = ::operator new(x);
	mapOfAllocations[ptr] = x;
	return ptr;
	}
		void* operator new[](std::size_t x) {
		void *ptr = ::operator new[](x);
		mapOfAllocations[ptr] = x;
		return ptr;
	}
		void operator delete(void* x) {
		mapOfAllocations.erase(x);
		::operator delete(x);
	}
	void operator delete[](void* x) {
		mapOfAllocations.erase(x);
		::operator delete[](x);
	}
private:
	static std::map<void *, std::size_t> mapOfAllocations;
};
std::map<void *, std::size_t> manageMemory::mapOfAllocations;



//--------------------------------------------
//The arrayHashIterator class
//--------------------------------------------
template <typename KEY, typename VALUE>
class arrayHashIterator : public manageMemory {
	friend class arrayHashTable<KEY, VALUE>;
public:
	arrayHashIterator();
	VALUE& operator*() const;
	arrayHashIterator operator++();
	bool operator!=(const arrayHashIterator& right) const;
private:
	KEY *keyArray;
	VALUE *valueArray;
	int *statusArray;
	int index;
	int arraySize;
	bool offTheRightEdge;
};

//The arrayHashIterator's constructor
template <typename KEY, typename VALUE>
arrayHashIterator<KEY, VALUE>::arrayHashIterator() {
	keyArray = NULL;
	valueArray = NULL;
	statusArray = NULL;
	index = 0;
	offTheRightEdge = true;
}


template <typename KEY, typename VALUE>
VALUE& arrayHashIterator<KEY, VALUE>::operator*() const {
	//TODO:
	//The iterator either has an index, and at that index the value is data
	//or the iterator's offTheRightEdge is true.  
	//If offTheRightEdge is true, throw an Error object
	//Otherwise, return the value that this iterator's index is referring to.

	if (offTheRightEdge == true) {
		throw Error();
	}
	else {
		return valueArray[index];
	}

}

template <typename KEY, typename VALUE>
arrayHashIterator<KEY, VALUE> arrayHashIterator<KEY, VALUE>::operator++() {
	//TODO:
	//We want to find the next index whose statusArray[index] is 1
	//Then we return *this
	//So set up a loop that keeps looping index (until index hits arraySize).
	//If the loop doesn't find a value, set offTheRightEdge to true.
	//If the loop does find a value, set offTheRightEdge to false.	
	

	if (offTheRightEdge == true) {
		return *this;
	}
	else {
		do {
			index = index + 1;

			if (index == arraySize) {

				offTheRightEdge = true;
				return *this;
			}
		} while (statusArray[index] != 1);

		return *this;
	}
}

template <typename KEY, typename VALUE>
bool arrayHashIterator<KEY, VALUE>::operator!=(const arrayHashIterator<KEY, VALUE>& right) const {
	//TODO: see if the "this" iterator and the right iterator are not equal.
	//To do this, check both iterators' index values and offTheRightEdge values

	if (this->offTheRightEdge == right.offTheRightEdge || this->index == right.index) {
		return false;
	}
	else {
		return true;
	}

}


//--------------------------------------------
//For error handling
//--------------------------------------------
class Error {};

//***********************************
//The hash table class
//***********************************
template <typename KEY, typename VALUE>
class arrayHashTable : public manageMemory {
public:

	arrayHashTable() { keyArray = NULL; valueArray = NULL; statusArray = NULL; arraySize = 0; }
	arrayHashTable(function<int(KEY, int)> hashFunction, int arraySize);
	arrayHashTable(const arrayHashTable& obj);
	~arrayHashTable();
	arrayHashTable& operator=(arrayHashTable tmp);

	void add(const KEY& key, const VALUE& value);
	bool exists(const KEY& key) const;
	void remove(const KEY& key);
	VALUE item(const KEY& key);
	VALUE& operator[](const KEY& key);

	arrayHashIterator<KEY, VALUE> begin() const;
	arrayHashIterator<KEY, VALUE> end() const;

	//friended so the checkTest function can have access to private data members of this class.
	friend void testSimpleIntHash();
	friend void testHashOfObjects();

protected:

	KEY *keyArray; //the key array
	VALUE *valueArray; //the array of values (these can hold your data/objects)
	int *statusArray;  //the status array, holding 0 (open slot), 1 (slot used), -1 (slot deleted)

private:
	int getWorstClump() const;
	int getTotalCount() const;
	function<int(KEY, int)> hash; //The hash function.  You would use it as if it was 
								  //a private method declared like so:
								  //int hash(KEY key, int arraySize) { /*does math*/ return someInt; }
	int arraySize;
};


template <typename KEY, typename VALUE>
arrayHashTable<KEY, VALUE>::arrayHashTable(function<int(KEY, int)> hashFunction, int arraySize) {

	//This just makes sure the object remebers the hash function pointer 
	//that was passed into this constructor.
	hash = hashFunction;
	this->arraySize = arraySize;

	//TODO: Allocate the three arrays.  
	//Make sure every status array element is set to zero.
	//keyArray is datatype KEY.  
	//valueArray is datatype VALUE.
	//statusArray is datatype int. 

	keyArray = new KEY[arraySize];
	valueArray = new VALUE[arraySize];
	statusArray = new int[arraySize];

	for (int i = 0; i < arraySize; i++) {
		statusArray[i] = 0;
	}


}


template <typename KEY, typename VALUE>
arrayHashTable<KEY, VALUE>::~arrayHashTable() {

	delete[] keyArray;
	delete[] valueArray;
	delete[] statusArray;

	keyArray = NULL;
	valueArray = NULL;
	statusArray = NULL;

}

//copy constructor
template <typename KEY, typename VALUE>
arrayHashTable<KEY, VALUE>::arrayHashTable(const arrayHashTable& obj) {
	this->hash = obj.hash;
	this->arraySize = obj.arraySize;
	if (arraySize > 0) {
		keyArray = new KEY[arraySize];
		valueArray = new VALUE[arraySize];
		statusArray = new int[arraySize];


		for (int i = 0; i < arraySize; i++) {
			this->keyArray[i] = obj.keyArray[i];
			this->valueArray[i] = obj.valueArray[i];
			this->statusArray[i] = obj.statusArray[i];
		}
	}
}

//overloaded assignment operator.  
template <typename KEY, typename VALUE>
arrayHashTable<KEY, VALUE>& arrayHashTable<KEY, VALUE>::operator=(arrayHashTable<KEY, VALUE> tmp) {
	std::swap(keyArray, tmp.keyArray);
	std::swap(valueArray, tmp.valueArray);
	std::swap(statusArray, tmp.statusArray);
	this->hash = tmp.hash;
	this->arraySize = tmp.arraySize;
	return *this;
}


template <typename KEY, typename VALUE>
void arrayHashTable<KEY, VALUE>::add(const KEY& key, const VALUE& value) {
	//TODO: 
	//Hash the key, get an index.  The index it returns matches up
	//to the index of the three arrays.  
	//Find the first available array index that is open for us to insert data into.  
	//Remember that if you hit the end of the array, you need to wrap around and 
	//start back again until you have searched all possibilities.
	//Once you find an index to use, insert the key into the key array
	//The value into the value array, and change the status array spot to 1.'

	int possibleBucket = hash(key, arraySize);
	int counter = 0;

	if (statusArray[possibleBucket] == 0 || statusArray[possibleBucket] == -1) {

		valueArray[possibleBucket] = value;
		keyArray[possibleBucket] = key;
		statusArray[possibleBucket] = 1;

	}
	else {
		while (statusArray[possibleBucket] == 1) {
			possibleBucket = possibleBucket + 1;
			counter++;
			if (possibleBucket >= arraySize) {
				possibleBucket = 0;
			}
			

			if (counter == arraySize) {
				break;
			}
		}
	}
	valueArray[possibleBucket] = value;
	keyArray[possibleBucket] = key;
	statusArray[possibleBucket] = 1;
}




template <typename KEY, typename VALUE>
bool arrayHashTable<KEY, VALUE>::exists(const KEY& key) const {
	//TODO:
	//Hash the key, get an index.  The index it returns matches up
	//to the index of the three arrays.  
	//Keep looking down the array until we find a key that matches.  Return true if we do
	//If we run into a status of 0, we can't look further. 
	//Remember that if you hit the end of the array, you need to wrap around and 
	//start back again until you have searched all arraySize) {

	int bucket = hash(key, arraySize);
	int count = 0;

	while (statusArray[bucket] == -1) {
		if (bucket < arraySize) {
			bucket = bucket + 1;
			count++;
		}
		else {
			if (bucket >= arraySize) {
				bucket = 0;
			}
		}
		if (count == arraySize) {
			break;
		}
	}


	while (statusArray[bucket] == 1) {
		if (keyArray[bucket] == key) {
			return true;
		}
		else {
			if (bucket < arraySize) {
				bucket = bucket + 1;
			
				count++;

			}
			else {
				if (bucket >= arraySize) {
					bucket = 0;
				}
			}
		}

		if (count == arraySize) {
			break;
		}
	}

	if (statusArray[bucket] == 0) {
		return false;
	}
}


template <typename KEY, typename VALUE>
VALUE arrayHashTable<KEY, VALUE>::item(const KEY& key) {
	//TODO:
	//Hash the key, get an index.  The index it returns matches up
	//to the index of the three arrays.  
	//Keep looking down the array until we find a key that matches.  Return the value
	//from the value array if we do find a matching key.
	//If we run into a status of 0, we can't look further, so throw an Error object.
	//Remember that if you hit the end of the array, you need to wrap around and 
	//start back again until you have searched all possibilities.
	//If you don't anything, throw an Error object.

	//To make it compile:


	int bucket = hash(key, arraySize);
	int count = 0;

	while (statusArray[bucket] == -1) {
		if (bucket < arraySize) {
			bucket = bucket + 1;
			count++;
		}
		else {
			if (bucket >= arraySize) {
				bucket = 0;
			}
		}
		if (count == arraySize) {
			break;
		}
	}


	while (statusArray[bucket] == 1) {
		if (keyArray[bucket] == key) {
			return valueArray[bucket];
		}
		else {
			if (bucket < arraySize) {
				bucket = bucket + 1;
			
				count++;

			}
			else {
				if (bucket >= arraySize) {
					bucket = 0;
				}
			}
		}

		if (count == arraySize) {
			break;
		}
	}

	if (statusArray[bucket] == 0) {
		throw Error();
	}

}
template <typename KEY, typename VALUE>
VALUE& arrayHashTable<KEY, VALUE>::operator[](const KEY& key) {
	//TODO:
	//Hash the key, get an index.  The index it returns matches up
	//to the index of the three arrays.  
	//Keep looking down the array until we find a key that matches.  Return the value
	//from the value array if we do find a matching key.
	//If we run into a status of 0, we can't look further, so throw an Error object.
	//Remember that if you hit the end of the array, you need to wrap around and 
	//start back again until you have searched all possibilities.
	//If you don't anything, throw an Error object.

	//To make it compile:
	int bucket = hash(key, arraySize);
	int count = 0;

	while (statusArray[bucket] == -1) {
		if (bucket < arraySize) {
			bucket = bucket + 1;
			count++;
		}
		else {
			if (bucket >= arraySize) {
				bucket = 0;
			}
		}
		if (count == arraySize) {
			break;
		}
	}


	while (statusArray[bucket] == 1) {
		if (keyArray[bucket] == key) {
			return valueArray[bucket];
		}
		else {
			if (bucket < arraySize) {
				bucket = bucket + 1;
				count++;

			}
			else {
				if (bucket >= arraySize) {
					bucket = 0;
				}
			}
		}

		if (count == arraySize) {
			break;
		}
	}

	if (statusArray[bucket] == 0) {
		throw Error();
	}

}

template <typename KEY, typename VALUE>
void arrayHashTable<KEY, VALUE>::remove(const KEY& key) {
	//TODO:
	//Hash the key, get an index.  The index it returns matches up
	//to the index of the three arrays.  
	//Keep looking down the array until we find a key that matches.  If we find
	//a matching key, just set the statusArray at that index to -1.
	//Remember that if you hit the end of the array, you need to wrap around and 
	//start back again until you have searched all possibilities.
	int bucket = hash(key, arraySize);
	int count = 0;

	while (statusArray[bucket] == -1) {
		if (bucket < arraySize) {
			bucket = bucket + 1;
			
			count++;
		}
		else {
			if (bucket >= arraySize) {
				bucket = 0;
			}
		}
		if (count == arraySize) {
			break;
		}
	}


	while (statusArray[bucket] == 1) {
		if (keyArray[bucket] == key) {
			statusArray[bucket] = -1;
		}
		else {
			if (bucket < arraySize) {
				bucket = bucket + 1; 

				count++;
				
			}
			else {
				if (bucket >= arraySize) {
					bucket = 0;
				}
			}
		}

		if (count == arraySize) {
			break;
		}
	}

}


template <typename KEY, typename VALUE>
arrayHashIterator<KEY, VALUE> arrayHashTable<KEY, VALUE>::begin() const {

	//I've given this to you.
	//It creates a new iterator object, sets it at the beginning
	//Then it advances the iterator forward until it finds the first 
	//array slot that has a statusArray of 1. (the ++temp does that work)

	arrayHashIterator<KEY, VALUE> temp;
	temp.keyArray = this->keyArray;
	temp.valueArray = this->valueArray;
	temp.statusArray = this->statusArray;
	temp.arraySize = this->arraySize;
	temp.index = 0;
	temp.offTheRightEdge = false;

	if (temp.statusArray[0] != 1) {
		//Go search for the first index that contains useful data
		++temp;
	}


	return temp;
}

template <typename KEY, typename VALUE>
arrayHashIterator<KEY, VALUE> arrayHashTable<KEY, VALUE>::end() const {
	//I've given this to you.  This points at the end of the arrayHashTable arrays
	//In other words, it points to the last index of the array and
	//the offTheRightEdge is set to true.

	arrayHashIterator<KEY, VALUE> temp;
	temp.keyArray = this->keyArray;
	temp.valueArray = this->valueArray;
	temp.statusArray = this->statusArray;
	temp.arraySize = this->arraySize;
	temp.index = this->arraySize;
	temp.offTheRightEdge = true;

	return temp;
}


template <typename KEY, typename VALUE>
int arrayHashTable<KEY, VALUE>::getWorstClump() const {
	int retVal = 0;
	if (statusArray != NULL) {
		int clumpCount = 0;
		bool foundPrevious = false;
		for (int i = 0; i < arraySize; i++) {
			if (statusArray[i] == 1) {
				clumpCount++;
			}
			else {
				if (retVal < clumpCount) {
					retVal = clumpCount;
				}
				clumpCount = 0;
			}
		}
	}
	return retVal;
}

template <typename KEY, typename VALUE>
int arrayHashTable<KEY, VALUE>::getTotalCount() const {
	int sum = 0;
	if (statusArray != NULL) {
		for (int i = 0; i < arraySize; i++) {
			if (statusArray[i] == 1) {
				sum++;
			}
		}
	}
	return sum;
}


//--------------------------------------------
//A quick and dirty class that simulates a product object.
//--------------------------------------------
class product {
public:
	void setCost(int cost);
	void setName(const string&);
	string getName();
	int getCost();
	string getAllInfo();
private:
	string name;
	int cost;
};
void product::setCost(int cost) {
	this->cost = cost;
}
void product::setName(const string& name) {
	this->name = name;
}
string product::getName() {
	return name;
}
int product::getCost() {
	return cost;
}
string product::getAllInfo() {
	stringstream ss;
	ss << "Name: " << name << ", Cost: " << cost << endl;
	return ss.str();
}

//This helps with testing, do not modify.
bool checkTest(string testName, string whatItShouldBe, string whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		if (whatItShouldBe == "") {
			cout << "***Failed test " << testName << " *** " << endl << "   Output was " << whatItIs << endl << "   Output should have been blank. " << endl;
		}
		else {
			cout << "***Failed test " << testName << " *** " << endl << "   Output was " << whatItIs << endl << "   Output should have been " << whatItShouldBe << endl;
		}
		return false;
	}
}

//This helps with testing, do not modify.
bool checkEmpty(string testName, string whatItIs) {

	if (whatItIs != "") {
		cout << "Passed " << testName << ", the data was " << whatItIs << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "   No data was found! " << endl;
		return false;
	}
}
//This helps with testing, do not modify.
bool checkTest(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "   Output was " << whatItIs << endl << "   Output should have been " << whatItShouldBe << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTestMemory(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "  You lost track of " << whatItIs << " bytes in memory!" << endl;
		return false;
	}
}

//This helps with testing, do not modify.
void testSimpleIntHash() {
	//Create a arrayHashTable. We want this to be a hash table with int keys, string object values,
	//And we also supply the hash function we want to use for integers..

	function <int(int, int)> intHashFunction = [](int key, int size) { return key % size; };

	arrayHashTable<int, string> myHash(intHashFunction, 1000);

	//Test #1, add "Jazz" into our hash with a key of 6.  Try to retrive it.  
	myHash.add(24006, "Jazz");
	checkTest("testSimpleIntHash #1", "Jazz", myHash.item(24006));

	//Test #2, attempt to get the Jazz using the operator[] code.
	checkTest("testSimpleIntHash #2", "Jazz", myHash[24006]);

	//Test #3, attempt to change the value at this location.  It should overwrite.
	myHash[24006] = "Nuggets";
	checkTest("testSimpleIntHash #3", "Nuggets", myHash[24006]);

	//Test #4, 
	//See if we can find it
	if (myHash.exists(24006)) {
		checkTest("testSimpleIntHash #4", "", "");
	}
	else {
		checkTest("testSimpleIntHash #4", "This test should have found an item with key 24006", "This test did NOT find an item with key 24006");
	}

	//Test #5, see if we can find something that doesn't exist yet.
	if (myHash.exists(1234)) {
		checkTest("testSimpleIntHash #5", "This test should NOT have found an item with key 1234.", "This test found an item with key 1234");
	}
	else {
		checkTest("testSimpleIntHash #5", "", "");
	}

	//Test #6, removing it from the hash
	myHash.remove(24006);
	if (myHash.exists(24006)) {
		checkTest("testSimpleIntHash #24006", "This test should NOT have found an item with key 24006.", "This test found an item with key 24006");
	}
	else {
		checkTest("testSimpleIntHash #24006", "", "");
	}

	//Add more into the hash
	myHash.add(16313, "Lakers");
	myHash.add(72325, "Bulls");
	myHash.add(121101, "Pelicans");
	myHash.add(83277, "Bucks");
	myHash.add(34212, "Thunder");
	myHash.add(63342, "Nets");
	myHash.add(29897, "Timberwolves");
	myHash.add(43123, "Hornets");
	myHash.add(21500, "Mavericks");

	//Attempt to retrieve some
	checkTest("testSimpleIntHash #7", "Thunder", myHash.item(34212));
	checkTest("testSimpleIntHash #8", "Bucks", myHash.item(83277));
	checkTest("testSimpleIntHash #9", "Hornets", myHash.item(43123));

	//Now count up how many are in there
	checkTest("testSimpleIntHash #10", 9, myHash.getTotalCount());

	//Now just verify that they aren't clumping up badly.
	int worst = 0;
	worst = myHash.getWorstClump();
	if (worst > 4) {
		cout << "Failed testSimpleIntHash #11!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 4." << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #11" << endl;
	}

	//Testing that we can iterate through all of them.
	int countOfItems = 0;

	for (auto iter : myHash) {
		//count up the length of all the strings
		countOfItems += iter.length();
		cout << iter << " ";
	}

	cout << endl;
	if (countOfItems != 63) {
		cout << "Failed testSimpleIntHash #12!  Tried iterating and counting all the characters of all values.  Got " << countOfItems << " instead of 63" << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #12" << endl;
	}


	//Now add in 500 more items.
	int key = 0;
	for (int i = 0; i < 500; i++) {

		//this next part just helps create some variation in generated W#s...
		if (i % 2 == 0) {
			key += 17;
		}
		else if (i % 3 == 0) {
			key += 23;
		}
		else if (i % 5 == 0) {
			key += 51;
		}
		else if (i % 7 == 0) {
			key += 13;
		}
		else {
			key += 71;
		}
		myHash.add(key, "a"); //Just add a bunch of letter a's
	}

	//Make sure they all go in there.  We put in 9 manually, then we added in 1000 more.  
	checkTest("testSimpleIntHash #13", 509, myHash.getTotalCount());

	worst = myHash.getWorstClump();
	if (worst > 25) {
		cout << "Failed testSimpleIntHash #14!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 25." << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #14" << endl;
	}

	//Now fill it up. Add in 491 more items.
	for (int i = 500; i < 991; i++) {

		//this next part just helps create some variation in generated W#s...
		if (i % 2 == 0) {
			key += 17;
		}
		else if (i % 3 == 0) {
			key += 23;
		}
		else if (i % 5 == 0) {
			key += 51;
		}
		else if (i % 7 == 0) {
			key += 13;
		}
		else {
			key += 71;
		}
		myHash.add(key, "b"); //Just add a bunch of letter b's
	}
	int count = myHash.getTotalCount();
	if (count != 1000) {
		cout << "Failed testSimpleIntHash #15!  There should have been 1000 items, but counted " << count << " items" << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #15" << endl;
	}

	//Now attepmt to use methods on a full hash.
		if (myHash.exists(1234)) {
		checkTest("testSimpleIntHash #16", "This test should NOT have found an item with key 1234.", "This test found an item with key 1234");
	}
	else {
		checkTest("testSimpleIntHash #16", "", "");
	}

	//Now attepmt to use methods on a full hash.  The add method should just return when it finds every element has been used.
	myHash.add(1234, "This can't go in");
	if (myHash.exists(1234)) {
		checkTest("testSimpleIntHash #17", "This test should NOT have found an item with key 1234.", "This test found an item with key 1234");
	}
	else {
		checkTest("testSimpleIntHash #17", "", "");
	}

	//Now remove those 991 items
	key = 0;
	for (int i = 0; i < 991; i++) {

		//this next part just helps create some variation in generated W#s...
		if (i % 2 == 0) {
			key += 17;
		}
		else if (i % 3 == 0) {
			key += 23;
		}
		else if (i % 5 == 0) {
			key += 51;
		}
		else if (i % 7 == 0) {
			key += 13;
		}
		else {
			key += 71;
		}
		myHash.remove(key);
	}

	count = myHash.getTotalCount();
	if (count != 9) {
		cout << "Failed testSimpleIntHash #18!  There should have been 9 items, but this counted " << count << " items" << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #18" << endl;
	}

	//Attempt to retrieve some
	checkTest("testSimpleIntHash #19", "Thunder", myHash.item(34212));
	checkTest("testSimpleIntHash #20", "Bucks", myHash.item(83277));
	checkTest("testSimpleIntHash #21", "Hornets", myHash.item(43123));


}

void testHashOfObjects() {

	//Create a arrayHashTable. We want this to be a hash table with int keys, string object values,
	//And we also supply the hash function we want to use for integers..
	function <int(string, int)> stringHashFunction = [](string key, int arraySize)
	{
		//use the djb2 hash algorithm. 
		unsigned long hash = 5381;
		int c;

		for (int i = 0; i < key.size(); i++)
			hash = hash * 33 + key.at(i);

		return hash % arraySize;

	};

	arrayHashTable<string, product> myHash(stringHashFunction, 2000);

	//Test #1, add in a studentObject.  Try to retrive it.  
	product tempProduct;
	tempProduct.setCost(5);
	tempProduct.setName("Silly string");
	myHash.add("12341-51231", tempProduct);
	checkTest("testHashOfObjects #1", "Silly string", myHash.item("12341-51231").getName());

	//Test #2, attempt to get the product using its ID code
	checkTest("testHashOfObjects #2", "Silly string", myHash["12341-51231"].getName());

	//Test #3, see what happens if two products have the same ID code.  This should overwrite the former.
	tempProduct.setCost(18);
	tempProduct.setName("Novelty foam hat");
	myHash["12341-51231"] = tempProduct;
	checkTest("testHashOfObjects #3", "Novelty foam hat", myHash["12341-51231"].getName());

	//Test #4, 
	//See if we can find it
	if (myHash.exists("12341-51231")) {
		checkTest("testHashOfObjects #4", "", "");
	}
	else {
		checkTest("testHashOfObjects #4", "This test should have found an item with key 12341-51231", "This test did NOT find an item with key 12341-51231");
	}

	//Test #5, see if we can find something that doesn't exist yet.
	if (myHash.exists("56756-75675")) {
		checkTest("testHashOfObjects #5", "This test should NOT have found an item with key 56756-75675.", "This test found an item with key56756-75675");
	}
	else {
		checkTest("testHashOfObjects #5", "", "");
	}

	//Test #6, removing it from the hash
	myHash.remove("12341-51231");
	if (myHash.exists("12341-51231")) {
		checkTest("testHashOfObjects #6", "This test should NOT have found an item with key 12341-51231.", "This test found an item with key 12341-51231");
	}
	else {
		checkTest("testHashOfObjects #6", "", "");
	}



	//Now throw in 1000 more items.
	int value = 0;
	string key;
	stringstream out;
	for (int i = 0; i < 1000; i++) {
		//this next part just helps create some variation for our produce ID codes.
		if (i % 2 == 0) {
			value += 107;
		}
		else if (i % 3 == 0) {
			value += 83;
		}
		else if (i % 5 == 0) {
			value += 47;
		}
		else if (i % 7 == 0) {
			value += 131;
		}
		else {
			value += 53;
		}
		key = "12345-";
		out.str("");
		if (value < 10000)
			key = key + "0";
		if (value < 1000)
			key = key + "0";
		if (value < 100)
			key = key + "0";
		if (value < 10)
			key = key + "0";
		out << value;
		string temp = out.str();
		if (temp.length() > 5) {
			temp = temp.substr(0, 5);
		}
		key = key + temp;
		//Whew, that took a while, but the W# is in key, and is ready to go

		//Create the student record, and load in values.
		tempProduct.setName("Acme product");
		tempProduct.setCost(rand() % 41);

		//Testing the hash table "add" method
		myHash.add(key, tempProduct);
	}

	//Make sure one went in correctly.  Retrieve it.
	checkEmpty("testHashOfObjects #7", myHash["12345-02112"].getAllInfo());

	//Make sure they all go in there.
	checkTest("testHashOfObjects #8", 1000, myHash.getTotalCount());

	//Now test how good your int hash function is.  
	int worst = myHash.getWorstClump();
	if (worst > 25) {
		cout << "Failed testHashOfObjects #9!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 25." << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #9." << endl;
	}
}

//This helps with testing, do not modify.
void testHashofHashes() {

	function <int(int, int)> intHashFunction = [](int key, int size) { return key % size; };

	function <int(string, int)> stringHashFunction = [](string key, int arraySize)
	{
		//use the djb2 hash algorithm. 
		unsigned long hash = 5381;
		int c;

		for (int i = 0; i < key.size(); i++)
			hash = hash * 33 + key.at(i);

		return hash % arraySize;

	};

	arrayHashTable< string, arrayHashTable<int, int> > studentAssignments(stringHashFunction, 2000);
	arrayHashTable<int, int> tempHash1(intHashFunction, 1000);
	studentAssignments.add("Alice", tempHash1);

	arrayHashTable<int, int> tempHash2(intHashFunction, 1000);
	studentAssignments.add("Bob", tempHash2);

	arrayHashTable<int, int> tempHash3(intHashFunction, 1000);
	studentAssignments.add("Karl", tempHash3);

	//Give Alice some assignment scores
	studentAssignments["Alice"].add(1, 73);
	studentAssignments["Alice"].add(2, 65);
	studentAssignments["Alice"].add(4, 91);
	//Ensure it went in
	checkTest("testHashofHashes #1", 65, studentAssignments["Alice"][2]);

	//And Bob
	studentAssignments["Bob"].add(1, 90);
	studentAssignments["Bob"].add(3, 84);
	studentAssignments["Bob"].add(4, 99);

	//And Karl
	studentAssignments["Karl"].add(1, 92);
	studentAssignments["Karl"].add(2, 92);
	studentAssignments["Karl"].add(3, 87);
	studentAssignments["Karl"].add(4, 10);

	//Now find the average of assignment 4 scores
	int average = (studentAssignments["Alice"][4] + studentAssignments["Bob"][4] + studentAssignments["Karl"][4]) / 3;
	checkTest("testHashofHashes #2", 66, average);

	int scores = 0;
	for (auto student : studentAssignments) {
		for (auto score : student) {
			scores += score;
			cout << score << " ";
		}
	}
	cout << endl;
	if (scores != 783) {
		cout << "Failed testHashofHashes #3!  Tried iterating and counting all the scores of all values.  Got " << scores << " instead of 783" << endl;
	}
	else {
		cout << "Passed testHashofHashes #3." << endl;
	}




	//clean up 
}

void pressAnyKeyToContinue() {
	cout << "Press any key to continue";

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

	//Each of these checks how many bytes you have used.
	checkTestMemory("Memory Leak/Allocation Test #1", 0, manageMemory::getTotalSize());

	testSimpleIntHash();

	//Checking if you have any memory leaks...
	checkTestMemory("Memory Leak/Allocation Test #2", 0, manageMemory::getTotalSize());

	testHashOfObjects();

	//Checking if you have any memory leaks...
	checkTestMemory("Memory Leak/Allocation Test #3", 0, manageMemory::getTotalSize());

	testHashofHashes();

	//Checking if you have any memory leaks...
	checkTestMemory("Memory Leak/Allocation Test #4", 0, manageMemory::getTotalSize());


	pressAnyKeyToContinue();

	return 0;
}