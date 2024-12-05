//
// Created by joelkub on 12/2/24.
//

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct Node {
    vector<double> voltages = vector<double>(96,0);
    double current = 0.0;
    double time = 0.0;
    bool color;
    Node *parent = nullptr;
    Node *left = nullptr;
    Node *right = nullptr;

    Node(vector<double> v, double i, double t, bool c) {
        for (double j = 0; j < v.size(); j++) {
            voltages[j] = v[j];
        }
        current = i;
        time = t;
        color = c;
    }

    Node* search(double t) {
        if (t < time) {
            if (t == left->time) {
                return left;
            }
            else {
                left->search(t);
            }
        }
        else if (t > time) {
            if (t == right->time) {
                return right;
            }
            else {
                right->search(t);
            }
        }
        else {
            return nullptr;
        }
    }

    Node* search_range(double t) {
        if (t == time) {
            return this;
        }
        if (t < time) {
            if (left != nullptr) {
                Node* temp = left;
                while (temp->right != nullptr) {
                    temp = temp->right;
                }
                if (t > temp->time) {
                    return this;
                }
                return left->search_range(t);
            }
        }
        else {
            if (right == nullptr) {
                return this;
            }
            Node* temp = right;
            while (temp->left != nullptr) {
                temp = temp->left;
            }
            if (t < temp->time) {
                return temp;
            }
            return right->search_range(t);
        }
    }
};

class RedBlackIterator {
    // Find inorder successor
    Node* successor(Node* node) {
        if (node == nullptr) return nullptr;


        if (node->right != nullptr) {
            Node* ret = node->right;
            while (ret->left != nullptr) ret = ret->left;
            return ret;
        }

        Node* parent = node->parent;
        Node* temp = node;
        while (parent != nullptr && temp == parent->right) {
            temp = parent;
            parent = parent->parent;
        }
        return parent;
    }

    // Find inorder predecessor
    Node* predecessor(Node* node) {
        if (node == nullptr) return nullptr;

        if (node->left != nullptr) {
            Node* ret = node->left;
            while (ret->right != nullptr) ret = ret->right;
            return ret;
        }

        Node* parent = node->parent;
        while (parent && node == parent->left) {
            node = parent;
            parent = parent->parent;
        }
        return parent;
    }

    Node* current = nullptr;
public:
    using reference = double&;
    using pointer = double*;
    using iterator_category = std::bidirectional_iterator_tag;
    explicit RedBlackIterator(Node *node = nullptr) : current(node) {}

    // Reference and dereference operators
    reference operator*() const {
        static double default_value = -1;
        if (current == nullptr) {
            return default_value;
        }

        return current->time;
    }
    pointer operator->() const {return &(current->time);}

    // Pre-increment operator
    RedBlackIterator& operator++() {
        current = successor(current);
        return *this;
    }
    // Post-increment operator
    RedBlackIterator operator++(int) {
        RedBlackIterator temp = *this;
        ++(*this);
        return temp;
    }

    // Pre-decrement operator
    RedBlackIterator& operator--() {
        current = predecessor(current);
        return *this;
    }
    // Post-decrement operator
    RedBlackIterator operator--(int) {
        RedBlackIterator temp = *this;
        --(*this);
        return temp;
    }
    // Equality operators
    bool operator==(const RedBlackIterator& other) const { return current == other.current; }
    bool operator!=(const RedBlackIterator& other) const { return current != other.current; }
};

class RedBlackTree {
    Node* root = nullptr;
    int count = 0; // tracks how many nodes there are, mainly for tracking if the are 3 or less nodes in the tree
public:
    using iterator = RedBlackIterator;
    // 0 (false) - Black         1 (true) - Red

    RedBlackTree() {
        root = nullptr;
    };

    RedBlackTree(vector<double> v, double i, double t) {
        root = new Node(v, i, t, false);
        count++;
    };

    iterator begin() {
        Node* node = root;
        if (root == nullptr) {
            return iterator(nullptr);
        }
        while (node->left != nullptr) {
            node = node->left;
        }
        return iterator(node);
    }

    iterator end() {
        Node* node = root;
        if (root == nullptr) {
            return iterator(nullptr);
        }
        while (node->right != nullptr) {
            node = node->right;
        }
        return ++iterator(node);
    }

    void insert(vector<double> v, double i, double t) {
        if (root == nullptr) {
            root = new Node(v, i, t, false);
            count++;
        }
        else {
            // First insert a node and color it red
            Node* new_node = new Node(v, i, t, true);
            Node* p = root;
            while (p != nullptr) {
                if (new_node->time > p->time) {
                    if (p->right == nullptr) {break;}
                    p = p->right;
                }
                else if (new_node->time < p->time) {
                    if (p->left == nullptr) {break;}
                    p = p->left;
                }
            }
            if (new_node->time > p->time) {
                p->right = new_node;
                new_node->parent = p;
            }
            else {
                p->left = new_node;
                new_node->parent = p;
            }
            count++;

            // After inserting the new node, make corrections as needed
            check_color(new_node);
        }
    };

    void right_right_rotate(Node* parent) {
        Node* T3 = parent->left;
        Node* grand_parent = parent->parent;
        Node* great_grand_parent = grand_parent->parent;
        grand_parent->right = T3;
        if (T3 != nullptr) {
            T3->parent = grand_parent;
        }

        parent->left = grand_parent;
        grand_parent->parent = parent;

        parent->parent = great_grand_parent;
        if (great_grand_parent == nullptr) {
            parent->parent = nullptr;
        }
        else if (grand_parent->time > great_grand_parent->time) {
            great_grand_parent->right = parent;
        }
        else {
            great_grand_parent->left = parent;
        }

        bool temp = grand_parent->color;
        grand_parent->color = parent->color;
        parent->color = temp;

        if (root == grand_parent) {
            root = parent;
        }
    }

    void left_left_rotate(Node* parent) {
        Node* T3 = parent->right;
        Node* grand_parent = parent->parent;
        Node* great_grand_parent = grand_parent->parent;

        grand_parent->left = T3;
        T3->parent = grand_parent;

        parent->right = grand_parent;
        grand_parent->parent = parent;

        parent->parent = great_grand_parent;
        if (great_grand_parent == nullptr) {
            parent->parent = nullptr;
        }
        else if (grand_parent->time > great_grand_parent->time) {
            great_grand_parent->right = parent;
        }
        else {
            great_grand_parent->left = parent;
        }

        bool temp = grand_parent->color;
        grand_parent->color = parent->color;
        parent->color = temp;

        if (root == grand_parent) {
            root = parent;
        }
    }

    void right_rotate(Node* p) {
        Node* T4 = p->right;
        Node* parent = p->parent;
        Node* grand_parent = p->parent->parent;

        parent->left = T4;
        if (T4 != nullptr) {
            T4->parent = parent;
        }

        p->right = parent;
        parent->parent = p;

        p->parent = grand_parent;
        if (grand_parent != nullptr) {
            grand_parent->right = p;
        }
    }

    void left_rotate(Node* p) {
        Node* T2 = p->left;
        Node* parent = p->parent;
        Node* grand_parent = p->parent->parent;

        parent->right = T2;
        if (T2 != nullptr) {
            T2->parent = parent;
        }

        p->left = parent;
        parent->parent = p;

        p->parent = grand_parent;
        if (grand_parent != nullptr) {
            grand_parent->left = p;
        }
    }

    void check_color(Node* p) { // This function is made with aid from GeeksForGeeks
        if (p->parent == nullptr) { // Base case, root node
            p->color = false;
            return;
        }
        if (count >= 3) {
            Node* parent = p->parent;
            Node* grand_parent = p->parent->parent;
            Node* uncle = nullptr;
            if (grand_parent == nullptr) {
                uncle = nullptr;
            }
            else if (parent->time > grand_parent->time) {
                uncle = grand_parent->left;
            }
            else {
                uncle = grand_parent->right;
            }

            // Check for rotations
            if (parent->color == true && uncle != nullptr && uncle->color == true) { // No rotation case
                parent->color = false;
                uncle->color = false;
                grand_parent->color = true;
                check_color(grand_parent);
            }
            else if (parent->color == true && (uncle == nullptr || uncle->color == false)) {
                if (grand_parent == nullptr) {
                    check_color(parent);
                }
                else if (parent->time > grand_parent->time) { // RL and RR
                    if (p->time > parent->time) { // RR
                        right_right_rotate(parent);
                        check_color(parent);
                    }
                    else { // RL
                        right_rotate(p);
                        right_right_rotate(p);
                        check_color(parent);
                    }
                }
                else { // LL and LR
                    if (p->time < parent->time) { // LL
                        left_left_rotate(parent);
                        check_color(parent);
                    }
                    else {
                        left_rotate(p);
                        left_left_rotate(p);
                        check_color(parent);
                    }
                }
            }
        }
    }

    Node* search(double t) {
        if (root == nullptr) {
            return nullptr;
        }
        return root->search(t);
    }

    iterator search_range(double start) {
        if (root == nullptr) {
            return iterator(nullptr);
        }
        if (start <= *iterator(begin())) {
            return iterator(begin());
        }
        if (start > *iterator(end())) {
            return iterator(nullptr);
        }
        Node* ret = root->search_range(start);
        return iterator(ret);
    }

    void printNode(Node* root, std::ofstream& outFile) {
        if (!root) {
            // If the node is null, skip it (no output for null nodes)
            return;
        }
        cout << "Time: " << root->time << endl;
        if(root->left) cout << "Left time: " << root->left->time << endl;
        if(root->right) cout << "Right time: " << root->right->time << endl;

        // Start writing the data in the format: <time> <color Boolean> <left_node time> <right_node time> <current integer> <voltages vector list (96 integers)>
        auto left_time = (root->left) ? root->left->time : -1;  // If no left child, assign -1
        auto right_time = (root->right) ? root->right->time : -1;  // If no right child, assign -1

        cout << "Left variable time: " << left_time << endl;
        cout << "Right variable time: " << right_time << endl;

        outFile << root->time << " " 
                << root->color << " " 
                << left_time << " " 
                << right_time << " " 
                << root->current << " ";

        // Output the voltages vector
        for (int i = 0; i < root->voltages.size(); ++i) {
            outFile << root->voltages[i] << " ";
        }

        outFile << std::endl;  // Newline after each node's data
    }

    void print() {
        std::ofstream outFile("data_smaller.txt");  // Open the file for writing

        //Create queue
        queue<Node*> q;

        q.push(root);  // Push the root node to the queue
        while(!q.empty()) {
            Node* current = q.front();  // Get the front of the queue
            q.pop();  // Pop the front of the queue

            // Print the current node
            printNode(current, outFile);

            // If the left child exists, push it to the queue
            if (current->left) {
                q.push(current->left);
            }

            // If the right child exists, push it to the queue
            if (current->right) {
                q.push(current->right);
            }
        }

        outFile.close();  // Close the file after writing
    }
};

// int main()
// {
//   CSVReader reader("./full_project_data.xlsx");
//   reader.nextStringLine();
//   int i = 0;
//   std::vector<std::vector<double>> raw_lines;
//   while (reader.ready()) {
//     auto line = reader.nextDataLine();
//     if (!line.empty()) {
//       raw_lines.push_back(line);
//     }
//   }

//   RedBlackTree rb_tree;

//   for (auto line : raw_lines) {
//       rb_tree.insert(
//           std::vector<double>(line.begin() + 1, line.begin() + 1 + 96),
//           line[line.size() - 1], line[0]);
//     }

//     rb_tree.print();
//     return 0;
// }
