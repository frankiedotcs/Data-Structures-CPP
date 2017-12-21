#include <iostream>
#include <string>
#include <stack>
#include <sstream>
#include <math.h>

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <stdio.h>
#endif

using namespace std;

//******************
//The binary tree class
//******************

class Node {
public:
	Node * llink;
	Node * rlink;
	string data;
};

//******************
//The Process Math Class
//*************
class processMath {

private:
	stack<string> mathStack;
	double castStrToDouble(string const &s);
	string castDoubleToStr(const double d);
	void initialize();
	bool isDigit(char c);
	bool isOperator(char c);
	void expressionProcessor(Node *p);
	void computeExpression(Node *p);
	Node * root;
	void erase(Node * p);
	void inOrderTraversal(Node * p);
	void postOrderTraversal(Node * p);
	

protected:
	string expression;
	int position;

public:
	processMath();
	void displayParseTree();
	void expressionProcessor(string &expression);
	string computeExpression();
	void insertItem(string data);
	void erase();
	void inOrderTraversal();
	void postOrderTraversal();


};




void processMath::insertItem(string item) {
	Node * temp = new Node();
	temp->data = item;
	temp->llink = NULL;
	temp->rlink = NULL;
	if (!root) {
		// empty tree
		root = temp;
	}
	else {
		Node * cur = root;
		bool breakout = false;
		while (!breakout) {
			if (item < cur->data) {
				//can we go left
				if (cur->llink) {
					cur = cur->llink;
				}
				else {
					//place it into the tree
					cur->llink = temp;
					breakout = true;
				}
			}
			else { // item is > data
				if (cur->rlink) {
					//we can go right
					cur = cur->rlink;
				}
				else {
					cur->rlink = temp;
					breakout = true;
				}

			}

		}
	}
}



void processMath::inOrderTraversal() {
	inOrderTraversal(root);
}


void processMath::inOrderTraversal(Node * p) {
	if (p) { // not null, ie not a leaf
		inOrderTraversal(p->llink);
		cout << p->data << " ";
		inOrderTraversal(p->rlink);
	}
}

void processMath::postOrderTraversal() {
	postOrderTraversal(root);
}

void processMath::postOrderTraversal(Node* p) {
	if (p) { // not null, ie not a leaf
		postOrderTraversal(p->llink);
		postOrderTraversal(p->rlink);
		cout << p->data << " ";
	}
}

void processMath::erase() {
	erase(root);
}

void processMath::erase(Node * p) {
	if (p) { // not null, ie not a leaf
		erase(p->llink);
		erase(p->rlink);
		delete p;
	}
}

void processMath::initialize() {
	expression = "";
	position = 0;
	while (!mathStack.empty()) {
		mathStack.pop();
	}

}

double processMath::castStrToDouble(const string &s) {
	istringstream iss(s);
	double x;
	iss >> x;
	return x;
}

string processMath::castDoubleToStr(const double d) {
	ostringstream oss;
	oss << fixed << d;
	return oss.str();

}

processMath::processMath() {
	initialize();
}

bool processMath::isDigit(char c) {

	if (c >= '0' && c <= '9') {
		return true;
	}
	else {
		return false;
	}

}

bool processMath::isOperator(char c) {

	switch (c) {

	case '+':
		return true;
		break;
	case '-':
		return true;
		break;
	case '*':
		return true;

		break;
	case '/':
		return true;

		break;
	case '^':
		return true;
		break;

	default:
		return false;
	}


}
void processMath::expressionProcessor(Node *p) {

	

	while (position < expression.size()) {

		if (expression[position] == '(') {
			//make new node
			Node * cur = new Node();
			//set node->link=NULL
			cur->llink = NULL;
			cur->rlink = NULL;
			//stick it on the left of your current node that p is pointing to
			p->llink = cur;
			//then increment position
			position++;
			//recursively go left.
			expressionProcessor(p->llink);

		}
		else if (isDigit(expression[position]) == true) {
			string temp;
			
			while (isDigit(expression[position]) == true) {
				//concatenate the expression onto the temp string
				temp += expression[position];
				//increment position
				position++;
			}
				//put the data into that node.
			p->data = temp;
				//call return
			return;
		
		}
		else if (isOperator(expression[position]) == true) {

			//store
			p->data = expression[position];
			
			//create new node making sure the children are NULL
			Node * cur = new Node();
			cur->llink = NULL;
			cur->rlink = NULL;
			//stick it on the right
			p->rlink = cur;
			//increment position
			position++;
			//recursively go right
			expressionProcessor(p->rlink);

		}
		else if (expression[position] == ')') {
			//increment position
			position++;
			//call return
			return;

		}
		else if (expression[position] == '\n' || expression[position] == ' ') {
			//increment position
			position++;
		}
	}
}

void processMath::expressionProcessor(string& expression) {

	if (expression.size() > 0) {
		this->expression = expression;
		position = 0;
		Node * cur = new Node();
		root = cur;
		expressionProcessor(root);
	}

}

string processMath::computeExpression() {
	string answer;

	computeExpression(root);
	mathStack.top();
	answer = mathStack.top();
	mathStack.pop();

	return answer;
}

void processMath::computeExpression(Node *p) {

	string temp;
	string temp2;
	string finalAnswer;
	double result1;
	double result2;
	double answer;



	if (p != NULL) {

		computeExpression(p->llink);
		computeExpression(p->rlink);

		if (p->data == "+" || p->data == "-" || p->data == "/" || p->data == "^" || p->data == "*") {

			temp = mathStack.top();
			mathStack.pop();
			temp2 = mathStack.top();
			mathStack.pop();

			if (p->data == "+") {

				result1 = castStrToDouble(temp);
				result2 = castStrToDouble(temp2);
				answer = (result1 + result2);
				finalAnswer = castDoubleToStr(answer);
				mathStack.push(finalAnswer);
			}
			else if (p->data == "*") {
				result1 = castStrToDouble(temp);
				result2 = castStrToDouble(temp2);
				answer = (result2*result1);
				finalAnswer = castDoubleToStr(answer);
				mathStack.push(finalAnswer);
			}
			else if (p->data == "/") {
				result1 = castStrToDouble(temp);
				result2 = castStrToDouble(temp2);
				answer = (result2 / result1);
				finalAnswer = castDoubleToStr(answer);
				mathStack.push(finalAnswer);

			}
			else if (p->data == "^") {
				result1 = castStrToDouble(temp);
				result2 = castStrToDouble(temp2);
				answer = pow(result2,result1);
				finalAnswer = castDoubleToStr(answer);
				mathStack.push(finalAnswer);
			}
			else if (p->data == "-") {
				result1 = castStrToDouble(temp);
				result2 = castStrToDouble(temp2);
				answer = (result2 -  result1);
				finalAnswer = castDoubleToStr(answer);
				mathStack.push(finalAnswer);
			}
		}
		else {
			mathStack.push(p->data);
			position++;

		}
	}
}

void processMath::displayParseTree() {
	cout << "The expression seen using in-order traversal: ";
	inOrderTraversal();
	cout << endl;
	cout << "The expression seen using post-order traversal: ";
	postOrderTraversal();
	cout << endl;

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
	processMath *mp = new processMath;

	string expression = "(4+7)";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 11

	expression = "(543+321)";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 864

	expression = "(7-4)";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 3

	expression = "((2-5)-5)";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display -8

	expression = "(4^3)";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 64

	expression = "(5*(6/2))";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 15

	expression = "((1 + 2) * (3 + 4))";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 21

	expression = "(((((3+12)-7)*120)/(2+3))^3)";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 7077888

	expression = "(((((9+(2*(110-(30/2))))*8)+1000)/2)+(((3^4)+1)/2))";
	mp->expressionProcessor(expression);
	mp->displayParseTree();
	cout << "The result is: " << mp->computeExpression() << endl; //Should display 1337

	pressAnyKeyToContinue();
	return 0;
}