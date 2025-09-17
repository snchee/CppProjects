#include <iostream>
#include <string>

using namespace std;

class Tree {
private:
    int numNodes;
    int parent[100];
    int children[100][100];
    int childCount[100];
    int root;

public:
    Tree(int n) {
        numNodes = n;
        for (int i = 0; i < n; i++) {
            parent[i] = -1;
            childCount[i] = 0;
        }
    }

    void addNode(int parentNode, int childNode) {
        parent[childNode] = parentNode;
        children[parentNode][childCount[parentNode]++] = childNode;
    }

    void setRoot(int rootNode) {
        root = rootNode;
    }

    int getParent(int x) {
        return parent[x];
    }

    int getLevel(int x) {
        int level = 1;
        while (parent[x] != -1) {
            x = parent[x];
            level++;
        }
        return level;
    }

    void getNodesAtLevel(int level) {
        cout << "The nodes at level " << level << " are:\n";
        for (int i = 0; i < numNodes; i++) {
            if (getLevel(i) == level) {
                cout << i << " ";
            }
        }
        cout << endl << endl;
    }

    void getChildren(int x) {
        cout << "The children of node " << x << " are:\n";
        if (childCount[x] == 0) {
            cout << "None" << endl;
        } else {
            // Sort children before printing
            for (int i = 0; i < childCount[x] - 1; i++) {
                for (int j = i + 1; j < childCount[x]; j++) {
                    if (children[x][i] > children[x][j]) {
                        swap(children[x][i], children[x][j]);
                    }
                }
            }
            for (int i = 0; i < childCount[x]; i++) {
                cout << children[x][i] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }


    bool isLeaf(int x) {
        return (childCount[x] == 0);
    }

    void getLeafNodes(int x) {
        cout << "The leaf nodes for node " << x << " are:\n";

        bool found = false;

        // Recursive function to print leaf nodes in preorder traversal
        printLeafNodesPreorder(x, found);

        if (!found) cout << "None";
        cout << endl;
    }

    // Preorder traversal to print leaf nodes
    void printLeafNodesPreorder(int x, bool &found) {
        if (isLeaf(x)) {
            if (found) cout << " ";
            cout << x;
            found = true;
        }

        // Visit children in ascending order (preorder)
        for (int i = 0; i < childCount[x] - 1; i++) {
            for (int j = i + 1; j < childCount[x]; j++) {
                if (children[x][i] > children[x][j]) {
                    swap(children[x][i], children[x][j]);
                }
            }
        }

        for (int i = 0; i < childCount[x]; i++) {
            printLeafNodesPreorder(children[x][i], found);
        }
    }


    int getLCA(int x, int y) {
        bool visited[100] = {false};
        while (x != -1) {
            visited[x] = true;
            x = parent[x];
        }
        while (y != -1) {
            if (visited[y]) return y;
            y = parent[y];
        }
        return -1;
    }

    void getSubtreeNodes(int x) {
        cout << "The nodes in subtree rooted by " << x << " are:\n" << endl;

        // Use a recursive helper function for preorder traversal
        printSubtreePreorder(x);

        cout << endl;  // Ensure proper newline formatting
    }

    // Recursive preorder traversal function
    void printSubtreePreorder(int x) {
        cout << x << " ";  // Print the node first (Preorder)

        // Ensure children are visited in ascending order
        for (int i = 0; i < childCount[x] - 1; i++) {
            for (int j = i + 1; j < childCount[x]; j++) {
                if (children[x][i] > children[x][j]) {
                    swap(children[x][i], children[x][j]);
                }
            }
        }

        // Recur for each child
        for (int i = 0; i < childCount[x]; i++) {
            printSubtreePreorder(children[x][i]);
        }
    }

    // Recursive function to print all descendants of x
    void printSubtree(int x) {
        cout << x << " ";


        for (int i = 0; i < childCount[x]; i++) {
            printSubtree(children[x][i]);
        }
    }

    int getHeight(int x) {
        int maxHeight = 1;
        for (int i = 0; i < numNodes; i++) {
            if (parent[i] == x) {
                maxHeight = max(maxHeight, getHeight(i) + 1);
            }
        }
        return maxHeight;
    }

    void findPath(int x, int y) {
        cout << "The path between " << x << " and " << y << " is:\n";

        int pathX[100], pathY[100];
        int lenX = 0, lenY = 0;

        // Store path from x to root
        while (x != -1) {
            pathX[lenX++] = x;
            x = parent[x];
        }

        // Store path from y to root
        while (y != -1) {
            pathY[lenY++] = y;
            y = parent[y];
        }

        // Find common ancestor
        int i = lenX - 1, j = lenY - 1;
        while (i >= 0 && j >= 0 && pathX[i] == pathY[j]) {
            i--; j--;
        }

        // Print path from x to LCA
        for (int k = 0; k <= i + 1; k++) cout << pathX[k] << " ";

        // Print path from LCA to y
        for (int k = j; k >= 0; k--) cout << pathY[k] << " ";

        cout << endl;
    }


    void displayParentArray() {
        cout << "Displaying the Parent Array:\n" << endl;
        for (int i = 0; i < numNodes; i++) {
            cout << "Child: " << i << " Parent: " << parent[i] << endl;
        }
    }
};

int main() {
    int totalNodes;
    cin >> totalNodes;

    Tree tree(totalNodes);

    while (true) {
        string firstWord;
        cin >> firstWord;

        if (!isdigit(firstWord[0])) {
            cin.putback(firstWord[0]);
            break;
        }

        int parentNode = stoi(firstWord);
        int numChildren;
        cin >> numChildren;

        for (int j = 0; j < numChildren; j++) {
            int childNode;
            cin >> childNode;
            tree.addNode(parentNode, childNode);
        }
    }

    // Process queries
    string query;
    while (cin >> query) {
        int x, y;
        if (query == "P") {
            cin >> x;
            cout << endl << "The parent of node " << x << " is: " << tree.getParent(x) << endl << endl;
        } else if (query == "C") {
            cin >> x;
            tree.getChildren(x);
        } else if (query == "L") {
            cin >> x;
            cout << endl << "The level of node " << x << " is: " << tree.getLevel(x) << endl << endl;
        } else if (query == "E") {
            cin >> x;
            tree.getNodesAtLevel(x);
        } else if (query == "I") {
            cin >> x;
            cout << "The node " << x << (tree.isLeaf(x) ? " is a leaf." : " is not a leaf.") << endl << endl;
        } else if (query == "G") {
            cin >> x;
            tree.getLeafNodes(x);
        } else if (query == "A") {
            cin >> x >> y;
            cout << "The LCA of " << x << " and " << y << " is: " << tree.getLCA(x, y) << endl << endl;
        } else if (query == "N") {
            cin >> x;
            tree.getSubtreeNodes(x);
        } else if (query == "F") {
            cin >> x >> y;
            tree.findPath(x, y);
        } else if (query == "H") {
            cin >> x;
            cout << "The Height of " << x << " is: " << tree.getHeight(x) << endl << endl;
        }
    }

    tree.displayParentArray();
    return 0;
}