#ifndef _HUFF_TREE_H
#define _HUFF_TREE_H

#include <iostream>
#include <queue>
#include <map>
#include <string>

namespace huffman {

struct Node {
    uchar c;
    int freq;
    Node *left;
    Node *right;
    Node(uchar _c, int f, Node *l = nullptr, Node *r = nullptr)
        : c(_c), freq(f), left(l), right(r) {}
    bool operator<(const Node &node)
        const { // 重载，优先队列的底层数据结构std::heap是最大堆
        return freq > node.freq;
    }
};

class huffTree {
  public:
    huffTree(const std::map<uchar, int> &afMap) {
        for (auto i : afMap) {
            Node n(i.first, i.second);
            q.push(n);
        }
        makehuffTree();
    }
    ~huffTree() {
        Node node = q.top();
        _deleteTree(node.left);
        _deleteTree(node.right);
    }
    void makehuffTree() {
        while (q.size() != 1) {
            Node *left = new Node(q.top());
            q.pop();
            Node *right = new Node(q.top());
            q.pop();
            Node node('R', left->freq + right->freq, left, right);
            q.push(node);
        }
    }
    void huffmanCode(std::map<uchar, std::string> &codeMap) {
        Node node = q.top();
        std::string prefix;
        _huffmanCode(&node, prefix, codeMap);
    }
    Node getHuffTree() { return q.top(); }
    void watch() {
        Node node = q.top();
        _treeWatch(&node);
    }

  private:
    struct nodeBacklog { // 回溯点
        Node *node;
        int next_sub_idx;
    };
    enum { LeftIndex, RightIndex };
    enum { MaxLevel = 64 };

    static void nbl_push(nodeBacklog *nbl, nodeBacklog **top,
                         nodeBacklog **bottom) {
        if (*top - *bottom < MaxLevel) {
            (*(*top)++) =
                *nbl; // 这里其实已经将nbl的值拷贝到*top指向的位置，不能直接*top
                      //= nbl，否则会随nbl变化
        }
    }
    static nodeBacklog *nbl_pop(nodeBacklog **top, nodeBacklog **bottom) {
        return *top > *bottom ? --*top : nullptr;
    }
    static bool _isLeaf(Node *n) {
        return n->left == nullptr && n->right == nullptr;
    }

  private:
    void _deleteTree(Node *n) {
        if (!n)
            return;
        _deleteTree(n->left);
        _deleteTree(n->right);
        delete n;
    }
    void _treeWatch(Node *root);
    static void _printNode(Node *n) {
        if (!n) {
            printf("%c:%d\n", n->c, n->freq);
        }
    }
    void _huffmanCode(Node *root, std::string &prefix,
                      std::map<uchar, std::string> &codeMap) {
        std::string tmp = prefix;
        if (root->left != nullptr) {
            prefix += '0';
            if (_isLeaf(root->left)) {
                codeMap[root->left->c] = prefix;
            } else {
                _huffmanCode(root->left, prefix, codeMap);
            }
        }
        if (root->right != nullptr) {
            prefix = tmp;
            prefix += '1';
            if (_isLeaf(root->right)) {
                codeMap[root->right->c] = prefix;
            } else {
                _huffmanCode(root->right, prefix, codeMap);
            }
        }
    }

  private:
    std::priority_queue<Node> q;
};

void huffTree::_treeWatch(Node *root) {
    int level = 0;
    Node *node = root;
    nodeBacklog nbl;
    nodeBacklog *p_nbl = nullptr;
    nodeBacklog *top, *bottom, nbl_stack[MaxLevel];
    top = bottom = nbl_stack;

    for (;;) {
        if (node != nullptr) {
            int sub_index = p_nbl != nullptr ? p_nbl->next_sub_idx : RightIndex;
            p_nbl = nullptr;
            if (_isLeaf(node) || sub_index == LeftIndex) {
                nbl.node = nullptr;
                nbl.next_sub_idx = RightIndex;
            } else {
                nbl.node = node;
                nbl.next_sub_idx = LeftIndex;
            }
            nbl_push(&nbl, &top, &bottom);
            level++;
            node = sub_index == LeftIndex ? node->left : node->right;
        } else {
            p_nbl = nbl_pop(&top, &bottom);
            if (p_nbl == nullptr)
                break;
            node = p_nbl->node;
            level--;
        }
    }
}

} // namespace huffman

#endif