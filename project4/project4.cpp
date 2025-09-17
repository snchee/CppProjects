#include <iostream>
using namespace std;

template <class DT>
class BoxTree {
protected:
    DT* _BoxValues;
    BoxTree<DT>* _left;
    BoxTree<DT>* _right;
    int _numValuesinBox;
    int _maxValuesinBox;
    DT _nodeValue;

public:
    BoxTree(int maxBoxSize) {
        _BoxValues = nullptr;
        _left = _right = nullptr;
        _numValuesinBox = 0;
        _maxValuesinBox = maxBoxSize;
        _nodeValue = 0;
    }

    ~BoxTree() {
        delete[] _BoxValues;
        delete _left;
        delete _right;
    }

    void buildFromArray(DT* sortedArray, int size) {
        if (size == 0) return;
        else if (size <= _maxValuesinBox) {
            _BoxValues = new DT[_maxValuesinBox];
            for (int i = 0; i < size; i++)
                _BoxValues[i] = sortedArray[i];
            _numValuesinBox = size;
            _left = _right = nullptr;
        } else {
            int m = size / 2;
            _nodeValue = sortedArray[m];
            _BoxValues = nullptr;
            _numValuesinBox = 0;
            _left = new BoxTree(_maxValuesinBox);
            _left->buildFromArray(sortedArray, m);
            _right = new BoxTree(_maxValuesinBox);
            _right->buildFromArray(sortedArray + m, size - m);
        }
    }

    void display() {
        if (_left) _left->display();
        if (_BoxValues) {
            cout << "[";
            for (int i = 0; i < _numValuesinBox; i++) {
                cout << _BoxValues[i];
                if (i != _numValuesinBox - 1)
                    cout << ", ";
            }
            cout << "] ";
        }
        if (_right) _right->display();
    }


    bool isFull() {
        return _numValuesinBox == _maxValuesinBox;
    }

    int binarySearch(DT value) {
        int low = 0, high = _numValuesinBox - 1;
        while (low <= high) {
            int mid = (low + high) / 2;
            if (_BoxValues[mid] == value)
                return mid;
            else if (_BoxValues[mid] < value)
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }

    bool find(DT value) {
        if (!_BoxValues && !_left && !_right) return false;
        if (_BoxValues) {
            return binarySearch(value) != -1;
        } else {
            if (value < _nodeValue) return _left->find(value);
            else return _right->find(value);
        }
    }

    void insert(DT value) {
        if (!_BoxValues && !_left && !_right) {
            _BoxValues = new DT[_maxValuesinBox];
            _BoxValues[0] = value;
            _numValuesinBox = 1;
            return;
        }

        if (_BoxValues) {
            if (!isFull()) {
                int pos = _numValuesinBox;
                while (pos > 0 && _BoxValues[pos - 1] > value) {
                    _BoxValues[pos] = _BoxValues[pos - 1];
                    pos--;
                }
                _BoxValues[pos] = value;
                _numValuesinBox++;
            } else {
                int tempSize = _maxValuesinBox + 1;
                DT* temp = new DT[tempSize];
                int i = 0;
                while (i < _maxValuesinBox && _BoxValues[i] < value) {
                    temp[i] = _BoxValues[i];
                    i++;
                }
                temp[i] = value;
                for (int j = i; j < _maxValuesinBox; j++)
                    temp[j + 1] = _BoxValues[j];

                int medianIndex = tempSize / 2;
                DT median = temp[medianIndex];

                delete[] _BoxValues;
                _BoxValues = nullptr;
                _numValuesinBox = 0;
                _nodeValue = median;

                _left = new BoxTree(_maxValuesinBox);
                _right = new BoxTree(_maxValuesinBox);

                _left->_BoxValues = new DT[_maxValuesinBox];
                for (int i = 0; i < medianIndex; i++)
                    _left->_BoxValues[i] = temp[i];
                _left->_numValuesinBox = medianIndex;

                _right->_BoxValues = new DT[_maxValuesinBox];
                for (int i = medianIndex; i < tempSize; i++)
                    _right->_BoxValues[i - medianIndex] = temp[i];
                _right->_numValuesinBox = tempSize - medianIndex;

                delete[] temp;
            }
        } else {
            if (value < _nodeValue) _left->insert(value);
            else _right->insert(value);
        }
    }

    void remove(DT value) {
        if (!_BoxValues && !_left && !_right) return;
        if (_BoxValues) {
            int pos = binarySearch(value);
            if (pos == -1) return;
            for (int i = pos; i < _numValuesinBox - 1; i++)
                _BoxValues[i] = _BoxValues[i + 1];
            _numValuesinBox--;
            if (_numValuesinBox == 0) {
                delete[] _BoxValues;
                _BoxValues = nullptr;
            }
        } else {
            if (value < _nodeValue) _left->remove(value);
            else _right->remove(value);

            if (_left && _right &&
                !_left->_BoxValues && !_right->_BoxValues &&
                !_left->_left && !_left->_right &&
                !_right->_left && !_right->_right) {

                delete _left;
                delete _right;
                _left = _right = nullptr;
                _BoxValues = nullptr;
                _numValuesinBox = 0;
                _nodeValue = 0;
            }
        }
    }
};

// ------------------ MAIN FUNCTION ------------------

int main() {
    int n, k, c;
    cin >> n >> k >> c;

    // Read initial sorted values.
    int* initialValues = new int[n];
    for (int i = 0; i < n; i++) {
        cin >> initialValues[i];
    }

    // Build the initial BoxTree.
    BoxTree<int> tree(k);
    cout << "\nBuilding initial tree...\n";
    tree.buildFromArray(initialValues, n);
    delete[] initialValues;

    // Print initial tree state.
    cout << "\nInitial Tree (In-Order Traversal of leaf boxes):\n";
    tree.display();
    cout << "\n----------------------------------------\n";

    // Process the command list.
    for (int i = 0; i < c; i++) {
        char command;
        cin >> command;
        if (command != 'P') {
            int tempVal;
            cin >> tempVal;
            if (command == 'I') {
                cout << "Inserting ... " << tempVal << endl;
                cout << "Tree before insertion:\n";
                tree.display();
                cout << "\n";
                tree.insert(tempVal);
                cout << "Tree after insertion:\n";
                tree.display();
                cout << endl;
            } else if (command == 'D') {
                cout << "Deleting ... " << tempVal << endl;
                cout << "Tree before deletion:\n";
                tree.display();
                cout << "\n";
                tree.remove(tempVal);
                cout << "Tree after deletion:\n";
                tree.display();
                cout << endl;
            } else if (command == 'F') {
                cout << "Finding ... " << tempVal << endl;
                cout << "Tree state for find:\n";
                tree.display();
                cout << "\n";
                bool found = tree.find(tempVal);
                cout << (found ? "True" : "False") << endl;
            }
        } else {
            cout << "Printing Tree ... " << endl;
            cout << "Tree state:\n";
            tree.display();
            cout << endl;
        }
    }
    return 0;
}


// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.