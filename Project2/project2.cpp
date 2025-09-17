#include <iostream>

using namespace std;

template <class DT>
class GLRow; // Forward declaration

template <class DT>
ostream &operator<<(ostream &s, GLRow<DT> &oneGLRow);

template <class DT>
class GLRow
{
    friend ostream &operator<< <DT>(ostream &s, GLRow<DT> &oneGLRow);

protected:
    DT *_Info;
    int _Next;
    int _Down;

public:
    GLRow()
    {
        _Info = nullptr;
        _Next = -1;
        _Down = -1;
    }

    GLRow(DT &newInfo)
    {
        _Info = new DT(newInfo);
        _Next = -1;
        _Down = -1;
    }

    GLRow(GLRow<DT> &anotherOne)
    {
        _Info = new DT(*(anotherOne._Info));
        _Next = anotherOne._Next;
        _Down = anotherOne._Down;
    }

    GLRow<DT> &operator=(GLRow<DT> &anotherOne)
    {
        if (this != &anotherOne)
        {
            if (_Info)
                delete _Info;
            _Info = new DT(*(anotherOne._Info));
            _Next = anotherOne._Next;
            _Down = anotherOne._Down;
        }
        return *this;
    }

    int getNext() { return _Next; }
    int getDown() { return _Down; }
    DT &getInfo() { return *_Info; }

    int setNext(int n)
    {
        _Next = n;
        return _Next;
    }

    int setDown(int d)
    {
        _Down = d;
        return _Down;
    }

    int setInfo(DT &x)
    {
        if (_Info)
            delete _Info;
        _Info = new DT(x);
        return *_Info;
    }

    ~GLRow()
    {
        delete _Info;
    }
};

template <class DT>
ostream &operator<<(ostream &s, GLRow<DT> &oneGLRow)
{
    if (oneGLRow._Info)
        s << *(oneGLRow._Info);
    else
        s << "NULL";
    s << " " << oneGLRow._Next << " " << oneGLRow._Down;
    return s;
}

template <class DT>
class ArrayGLL;

template <class DT>
ostream &operator<<(ostream &s, ArrayGLL<DT> &oneGLL);

template <class DT>
class ArrayGLL
{
    friend ostream &operator<< <DT>(ostream &s, ArrayGLL<DT> &oneGLL);

protected:
    GLRow<DT> *myGLL;
    int maxSize;
    int firstElement;
    int firstFree;

public:
    ArrayGLL()
    {
        myGLL = nullptr;
        maxSize = 0;
        firstElement = -1;
        firstFree = -1;
    }

    ArrayGLL(int size)
    {
        maxSize = size;
        myGLL = new GLRow<DT>[maxSize];
        firstElement = -1;
        firstFree = 0;

        // Link all free spots together
        for (int i = 0; i < maxSize - 1; i++)
            myGLL[i].setNext(i + 1);

        myGLL[maxSize - 1].setNext(-1); // Last free element points to -1
    }

    ArrayGLL(ArrayGLL<DT> &anotherOne)
    {
        maxSize = anotherOne.maxSize;
        firstElement = anotherOne.firstElement;
        firstFree = anotherOne.firstFree;
        myGLL = new GLRow<DT>[maxSize];
        for (int i = 0; i < maxSize; i++)
            myGLL[i] = anotherOne.myGLL[i];
    }

    ArrayGLL<DT> &operator=(ArrayGLL<DT> &anotherOne)
    {
        if (this != &anotherOne)
        {
            delete[] myGLL;
            maxSize = anotherOne.maxSize;
            firstElement = anotherOne.firstElement;
            firstFree = anotherOne.firstFree;
            myGLL = new GLRow<DT>[maxSize];
            for (int i = 0; i < maxSize; i++)
                myGLL[i] = anotherOne.myGLL[i];
        }
        return *this;
    }

    void displayHelper(int index)
    {
        if (index == -1)
            return;

        cout << myGLL[index].getInfo();

        if (myGLL[index].getDown() != -1)
        {
            cout << "(";
            displayHelper(myGLL[index].getDown());
            cout << ")";
        }

        if (myGLL[index].getNext() != -1)
        {
            cout << " ";
            displayHelper(myGLL[index].getNext());
        }
    }

    void display()
    {
        if (firstElement == -1)
        {
            cout << "(Empty GLL)" << endl;
            return;
        }
        cout << "(";
        displayHelper(firstElement);
        cout << ")" << endl;
    }

    int findHelper(int index, DT &key)
    {
        if (index == -1)
            return -1; // Not found

        if (myGLL[index].getInfo() == key)
            return index;

        // Recursively search the subtree and siblings
        int downSearch = findHelper(myGLL[index].getDown(), key);
        if (downSearch != -1)
            return downSearch; // Found in children

        return findHelper(myGLL[index].getNext(), key); // Continue searching siblings
    }

    int find(DT &key)
    {
        return findHelper(firstElement, key);
    }

    void findDisplayPathHelper(int curr, int key, bool &found)
    {
        if (curr == -1 || found)
            return; // Stop if we reach an invalid index or already found the key

        cout << myGLL[curr].getInfo() << " "; // Print the current node

        if (curr == key)
        {
            found = true; // If we found the key, stop further traversal
            return;
        }

        // First, try going _Down
        findDisplayPathHelper(myGLL[curr].getDown(), key, found);

        // If we haven't found the key yet, try _Next
        if (!found)
            findDisplayPathHelper(myGLL[curr].getNext(), key, found);
    }

    void findDisplayPath(DT &key)
    {
        int pos = find(key);
        if (pos == -1)
        {
            cout << "Key not found.\n";
            return;
        }

        cout << "(";

        bool found = false;
        findDisplayPathHelper(firstElement, pos, found);

        cout << ")" << endl;
    }

    int noFree()
    {
        return maxSize - size();
    }

    int sizeHelper(int index)
    {
        if (index == -1)
            return 0;

        // Count this node
        int count = 1;

        // Recursively count all nodes in the subtree (children and siblings)
        count += sizeHelper(myGLL[index].getDown());
        count += sizeHelper(myGLL[index].getNext());

        return count;
    }

    int size()
    {
        return sizeHelper(firstElement);
    }

    int parentPos(DT &key)
    {
        for (int i = 0; i < maxSize; i++)
        {
            if (myGLL[i].getDown() != -1 && myGLL[myGLL[i].getDown()].getInfo() == key)
                return i; // Found parent via _Down

            // Also check _Next for the key
            int nextNode = myGLL[i].getNext();
            while (nextNode != -1)
            {
                if (myGLL[nextNode].getInfo() == key)
                    return i; // Found parent via _Next
                nextNode = myGLL[nextNode].getNext();
            }
        }
        return -1; // Not found
    }

    GLRow<DT> &operator[](int pos)
    {
        return myGLL[pos];
    }

    int getFirstFree() { return firstFree; }
    int getFirstElement() { return firstElement; }

    void setFirstFree(int pos) { firstFree = pos; }
    void setFirstElement(int pos) { firstElement = pos; }

    ~ArrayGLL()
    {
        delete[] myGLL;
    }
};


int main()
{
int noElements, firstFree, firstElement;
int value, next, down, parentPos;
int pos = -1;
int keyValue;
// Read the number of elements
cout << "Enter number of elements: ";
cin >> noElements >> firstElement >> firstFree;
// Setting up the structure
ArrayGLL<int> firstGLL(noElements);
firstGLL.setFirstElement(firstElement);
firstGLL.setFirstFree(firstFree);
// Input validation and initialization of GLRows
for (int i = 0; i < noElements; i++) {
cin >> value >> next >> down;
// Dynamically create a new GLRow<int> for each entry
GLRow<int> newRow(value);
newRow.setNext(next);
newRow.setDown(down);
firstGLL[i] = newRow;
}
cout << "\n=== First GLL Structure ===\n";
firstGLL.display();
// Testing copy constructor
ArrayGLL<int>* secondGLL = new ArrayGLL<int>(firstGLL);
if (!secondGLL)
{
cerr << "Error: Memory allocation failed for secondGLL." << endl;
return 1;
}
int use = 600;
(*secondGLL)[1].setInfo(use);
use =700;
(*secondGLL)[2].setInfo(use);
cout << "\n=== Second GLL (After Modifications) ===\n";
(*secondGLL).display();
// Testing find function
keyValue = 700;
pos = (*secondGLL).find(keyValue);
if (pos != -1)
{
cout << "Element " << keyValue << " found at position: " << pos << endl;
cout << "Element details: " << (*secondGLL)[pos] << endl;
(*secondGLL).findDisplayPath(keyValue);
cout << endl;
}
else
{
cout << "Element " << keyValue << " not found." << endl;
}
// Testing parentPos function
parentPos = (*secondGLL).parentPos(keyValue);
if (parentPos != -1)
{
cout << "Parent of " << keyValue << " is at position: " << parentPos <<
endl;
cout << "Parent details: " << (*secondGLL)[parentPos] << endl;
}
else
{
cout << "Parent of " << keyValue << " not found." << endl;
}
// Testing size and free locations
cout << "\nSize of secondGLL: " << (*secondGLL).size() << endl;
cout << "Number of free locations: " << (*secondGLL).noFree() << endl;
// Test getFirstElement and getFirstFree
cout << "\nFirst element position: " << firstGLL.getFirstElement() << endl;
cout << "First free position: " << firstGLL.getFirstFree() << endl;
// Test setFirstElement and setFirstFree
firstGLL.setFirstElement(2);
firstGLL.setFirstFree(5);
cout << "Updated first element position: " << firstGLL.getFirstElement() <<
endl;
cout << "Updated first free position: " << firstGLL.getFirstFree() << endl;
// Test GLRow class methods
cout << "\n=== Testing GLRow Methods ===\n";
use = 50;
GLRow<int> testRow(use); // Creating a GLRow object
cout << "Initial Row: " << testRow << endl;
// Set and Get methods
testRow.setNext(8);
testRow.setDown(4);
use = 75;
testRow.setInfo(use);
cout << "Updated Row: " << testRow << endl;
cout << "Next Pointer: " << testRow.getNext() << endl;
cout << "Down Pointer: " << testRow.getDown() << endl;
cout << "Info: " << testRow.getInfo() << endl;
// Test copy constructor
GLRow<int> copiedRow(testRow);
cout << "Copied Row: " << copiedRow << endl;
// Test assignment operator
use = 0;
GLRow<int> assignedRow(use);
assignedRow = copiedRow;
cout << "Assigned Row: " << assignedRow << endl;
// Ensure different memory locations
if (&testRow != &copiedRow && &copiedRow != &assignedRow)
{
cout << "Deep copy successful: Objects have separate memory allocations."
<< endl;
}
else
{
cout << "Error: Deep copy failed!" << endl;
}
// Proper memory cleanup
delete secondGLL;
secondGLL = nullptr;
cout << "\n=== All Tests Completed Successfully! ===";
return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.