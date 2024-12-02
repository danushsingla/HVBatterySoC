//
// Created by joelkub on 12/2/24.
//

#include <iostream>
#include <vector>
using namespace std;

class RedBlackTree {
    struct Node {
        vector<float> voltages(96, 0);
        float current;
        int time;
        int color;
        Node *parent = nullptr;
        Node *left = nullptr;
        Node *right = nullptr;

        Node(vector<float> v, float i, int t, int c) {
            for (int i = 0; i < 96; i++) {
                voltages[i] = v[i];
            }
            current = c;
            time = t;
            color = c;
        };

        Node* search(int t) {
            if (t < time) {
                if (t == left->time) {
                    return left;
                }
                else {
                    left->seach(t);
                }
            }
            else if (t > time) {
                if (t == right->time) {
                    return right;
                }
                else {
                    right->seach(t);
                }
            }
            else {
              return nullptr;
            }
        };
    }

    Node* root;
public:
    // 0 - Black         1 - Red

    RedBlackTree() {
        root = nullptr;
    };

    RedblackTree(vector<float> v, float i, int t){
        root = new Node(v, i, t, 0);
    };

    void insert(vector<float> v, float i, int t) {
        if (root == nullptr) {
            root = new Node(value);
        }
    };

    Node* search(int t) {
      if (root == nullptr) {
        return nullptr;
      }
      return root->search(t);

    };

    vector<Node*> search_range(int start, int end) {
        if (root == nullptr) {
            return vector<Node*>();
        }
        vector<Node*> result;
        Node* start = root->search(start);
        Node* end = root->search(end);
        if (start == end) {
            result.push_back(start);
            return result;
        }
        else if (start == nullptr || end == nullptr) {
            return vector<Node*>();
        }
    };
};
