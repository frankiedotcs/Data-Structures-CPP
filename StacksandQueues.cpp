//Copyright 2016, Bradley Peterson, Weber State University, All rights reserved.

#include <iostream>
#include <string>
#include <sstream>
//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#endif

using namespace std;


//**********************************
//Write your code below here
//**********************************

template <typename T>
class StackCustom {
public:
	
	StackCustom(unsigned int capacity); //constructor
	~StackCustom(); //deconstructor
	void push(const T& index);
	T pop();
	int size();
	
	

private:
	T*arr;
	unsigned int capacity;
	unsigned int currentSize;
};

//constructor
template <typename T>
StackCustom<T>::StackCustom(unsigned int capacity) {
	arr = new T[capacity];
	this->capacity = capacity;
	this->currentSize = 0;
}
//deconstructor
template <typename T>
StackCustom<T>::~StackCustom() {
	delete[] arr;
	arr = NULL;
}

//push method

template <typename T>
void StackCustom<T>::push(const T& data) {

	if (currentSize >= capacity) {
		throw 1;
		
	}
	else {
		arr[currentSize] = data;
		currentSize++;
	}
}


//pop method

template <typename T>
T StackCustom<T>::pop() {

	if (currentSize == 0) {
		throw 1;
	}
	else {
		
		currentSize--;
	}
	return arr[currentSize];
}

//size method
template <typename T>

int StackCustom<T>::size(){
	
	return currentSize;
}


template <typename T>
class QueueCustom {
public:
	QueueCustom(unsigned int capacity); //constructor
	~QueueCustom(); //deconstructor
	void push_back(const T& data);
	T pop_front();
	int size();
	int data;
	int tempFront;
	
private:
	T*arr;
	unsigned int front;
	unsigned int back;
	unsigned int capacity;
	unsigned int currentSize;
	
};

template <typename T>
//constructor
QueueCustom<T>::QueueCustom(unsigned int capacity) {
	int currentSize = 0;
	arr = new T[capacity];
	this->capacity = capacity;
	this->currentSize = 0;
	this->front = 0;
	this->back = 0;
	
	
}

template <typename T>
//destructor
QueueCustom<T>::~QueueCustom() {
	delete[] arr;
	arr = NULL;
}

template <typename T>
void QueueCustom<T>::push_back(const T& data) {
	
	if (currentSize >= capacity){
		throw 1;
	}
	else{
		if (back >= capacity){
			back = 0;
		}
		
			arr[back] = data;
			currentSize++;
			back++;
			
		}
	

}

template <typename T>
T QueueCustom<T>::pop_front() {
	if (currentSize == 0){
		throw 1;
	}
	else{
		if (front >= capacity){
			front = 0;
		}

			tempFront = front;
			currentSize--;
			front++;
		}
	
	
	return arr[tempFront];
		
}

template <typename T>
int QueueCustom<T>::size() {

	return currentSize;
}


//**********************************
//Write your code above here
//**********************************

//This helps with testing, do not modify.
bool checkTest(string testName, int whatItShouldBe, int whatItIs) {

    if (whatItShouldBe == whatItIs) {
        cout << "Passed " << testName << endl;
        return true;
    }
    else { 
        cout << "***Failed test " << testName << " *** " << endl << "   Output was "<< whatItIs << endl << "   Output should have been " << whatItShouldBe << endl;
        return false;
    }
}


//This helps with testing, do not modify.
bool checkTest(string testName, string whatItShouldBe, string whatItIs) {

    if (whatItShouldBe == whatItIs) {
        cout << "Passed " << testName << endl;
        return true;
    }
    else { 
        if (whatItShouldBe == "") {
            cout << "***Failed test " << testName << " *** " << endl << "   Output was "<< whatItIs << endl << "   Output should have been blank. " << endl;
        } else {
            cout << "***Failed test " << testName << " *** " << endl << "   Output was "<< whatItIs << endl << "   Output should have been " << whatItShouldBe << endl;
        }
        return false;
    }
}

//This helps with testing, do not modify.
bool checkTestMemory(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	} else {
		cout << "***Failed test " << testName << " *** " << endl << ".  ";
        cout << "You are manually managing "<< whatItIs << " bytes in memory, but it should be " << whatItShouldBe << " bytes." << endl;
		return false;
	}
}



//This helps with testing, do not modify.
void testStackCustom() {
	
    string result;
    string caughtError;
    StackCustom<int> *stack = new StackCustom<int>(5);

    stack->push(1);
    int data = stack->pop();
    checkTest("testStackCustom #1", 1, data);
    
    stack->push(1);
    stack->push(2);
    stack->push(3);
    stack->push(4);
    stack->push(5);
    checkTest("testStackCustom #2", 5, stack->pop());
    checkTest("testStackCustom #3", 4, stack->pop());
    checkTest("testStackCustom #4", 3, stack->pop());
    checkTest("testStackCustom #5", 2, stack->pop());
    checkTest("testStackCustom #6", 1, stack->pop());

    //now cover error handling
    try {
        result = stack->pop();
    } catch (int e) {
        caughtError = "caught";
    }
    checkTest("testStackCustom #7", "caught", caughtError);
    
    //check currentSize
	checkTest("testStackCustom #8", 0, stack->size());
	stack->push(12);
    stack->push(32);
    checkTest("testStackCustom #9", 2, stack->size());

    //now test filling it up
    stack->push(14);
    stack->push(53);
    stack->push(47);
	checkTest("testStackCustom #10", 5, stack->size());
    
    //now cover error handling
	caughtError = "";
    try {
       stack->push(8);
    } catch (int e) {
        caughtError = "caught";
    }
    checkTest("testStackCustom #11", "caught", caughtError);
    
    delete stack;
    
    //test some strings
    StackCustom<string> *sstack = new StackCustom<string>(10);

    sstack->push("pencil");
    sstack->push("pen");
    sstack->push("marker");
    
    checkTest("testStackCustom #12", 3, sstack->size());

    //remove pen from the stack.
    string temp = sstack->pop();
    sstack->pop();
    sstack->push(temp);

    //see if it worked 
    checkTest("testStackCustom #13", "marker", sstack->pop());
    checkTest("testStackCustom #14", "pencil", sstack->pop());
    
    delete sstack;
    
}



//This helps with testing, do not modify.
void testQueueCustom() {
    
    string result;
    string caughtError;
    QueueCustom<string> *pQueue = new QueueCustom<string>(5);

    //Tests push_back
    pQueue->push_back("penny");
    pQueue->push_back("nickel");
    pQueue->push_back("dime");
    pQueue->push_back("quarter");

    checkTest("testQueueCustom #1", 4, pQueue->size());
    
    checkTest("testQueueCustom #2", "penny", pQueue->pop_front());
    checkTest("testQueueCustom #3", 3, pQueue->size());
    
    checkTest("testQueueCustom #4", "nickel", pQueue->pop_front());
    checkTest("testQueueCustom #5", "dime", pQueue->pop_front());
    checkTest("testQueueCustom #6", "quarter", pQueue->pop_front());
    checkTest("testQueueCustom #7", 0, pQueue->size());
    
    caughtError = "not caught";
    try {
        result = pQueue->pop_front();
    } catch (int e) {
        caughtError = "caught";
    }
    checkTest("testQueueCustom #8", "caught", caughtError);
    checkTest("testQueueCustom #9", 0, pQueue->size());
    
    //Try it again.  This should make it wrap around, and fill up.
    pQueue->push_back("penny");
    pQueue->push_back("nickel");
    pQueue->push_back("dime");
    pQueue->push_back("quarter");

    checkTest("testQueueCustom #10", "penny", pQueue->pop_front());
    pQueue->push_back("half dollar");
    pQueue->push_back("silver dollar");
    
  //It should be full, no more room to add more.
    caughtError = "not caught";
    try {
        pQueue->push_back("million dollar bill");  
    } catch (int e) {
        caughtError = "caught";
    }
    checkTest("testQueueCustom #11", "caught", caughtError);
    
    checkTest("testQueueCustom #12", "nickel", pQueue->pop_front());
    checkTest("testQueueCustom #13", "dime", pQueue->pop_front());
    checkTest("testQueueCustom #14", "quarter", pQueue->pop_front());
    checkTest("testQueueCustom #15", "half dollar", pQueue->pop_front());
    checkTest("testQueueCustom #16", "silver dollar", pQueue->pop_front());
    caughtError = "not caught";
    try {
        result = pQueue->pop_front();
    } catch (int e) {
        caughtError = "caught";
    }
    checkTest("testQueueCustom #17", "caught", caughtError);
    
    //Test adding and removing back and forth
    pQueue->push_back("penny");
    checkTest("testQueueCustom #18", "penny", pQueue->pop_front());
    pQueue->push_back("nickel");
    checkTest("testQueueCustom #19", "nickel", pQueue->pop_front());
    pQueue->push_back("dime");
    checkTest("testQueueCustom #20", "dime", pQueue->pop_front());
    pQueue->push_back("quarter");
    checkTest("testQueueCustom #21", "quarter", pQueue->pop_front());
    pQueue->push_back("half dollar");
    checkTest("testQueueCustom #22", "half dollar", pQueue->pop_front());
    pQueue->push_back("silver dollar");
    checkTest("testQueueCustom #23", 1, pQueue->size());
    
    checkTest("testQueueCustom #24", "silver dollar", pQueue->pop_front());
    caughtError = "not caught";
    try {
        result = pQueue->pop_front();
    } catch (int e) {
        caughtError = "caught";
    }
    checkTest("testQueueCustom #25", "caught", caughtError);
    
    delete pQueue;
    
}

void pressAnyKeyToContinue() {
    cout << "Press any key to continue...";

    //Linux and Mac users with g++ don't need this
    //But everyone else will see this message.
#ifndef __GNUC__
    _getch();
    
#else
    int c;
    fflush( stdout );
    do c = getchar(); while ((c != '\n') && (c != EOF));
#endif
    cout << endl;
}


int main () {

	{
		testStackCustom();
		pressAnyKeyToContinue();
		testQueueCustom();
		pressAnyKeyToContinue();
	}
	cout << "Shutting down the program" << endl;
    return 0;
}