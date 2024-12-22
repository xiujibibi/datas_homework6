#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>
using namespace std;

// m-way 搜尋樹節點結構
struct MWayTreeNode {
    vector<int> keys;
    vector<MWayTreeNode*> children;
    int n; // 當前鍵數量
    bool isLeaf;

    MWayTreeNode(int m, bool leaf) : isLeaf(leaf), n(0) {
        keys.resize(m - 1);
        children.resize(m, nullptr);
    }
};

// m-way 搜尋樹類別
class MWayTree {
    MWayTreeNode* root;
    int m;

public:
    MWayTree(int _m) : root(nullptr), m(_m) {}

    // 插入操作
    void insert(int key) {
        if (!root) {
            root = new MWayTreeNode(m, true);
            root->keys[0] = key;
            root->n = 1;
        }
        else {
            MWayTreeNode* node = root;

            // 如果根節點已滿，進行分裂
            if (node->n == m - 1) {
                MWayTreeNode* newRoot = new MWayTreeNode(m, false);
                newRoot->children[0] = node;
                root = newRoot;
                splitChild(newRoot, 0);
            }

            // 插入新鍵值
            insertNonFull(root, key);
        }
    }

    // 插入非滿節點
    void insertNonFull(MWayTreeNode* node, int key) {
        int i = node->n - 1;

        if (node->isLeaf) {
            // 如果是葉子節點，直接插入
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->n++;
        }
        else {
            // 否則，尋找子節點
            while (i >= 0 && key < node->keys[i]) i--;
            i++;

            if (node->children[i]->n == m - 1) {
                // 如果子節點已滿，進行分裂
                splitChild(node, i);

                // 分裂後選擇正確的子節點
                if (key > node->keys[i]) i++;
            }

            insertNonFull(node->children[i], key);
        }
    }

    // 分裂子節點
    void splitChild(MWayTreeNode* parent, int i) {
        MWayTreeNode* fullChild = parent->children[i];
        MWayTreeNode* newChild = new MWayTreeNode(m, fullChild->isLeaf);

        // 創建新節點的關鍵字和子節點
        newChild->n = m / 2;
        for (int j = 0; j < m / 2; ++j) {
            newChild->keys[j] = fullChild->keys[j + m / 2];
        }

        // 更新父節點的鍵值
        for (int j = parent->n; j > i; --j) {
            parent->children[j + 1] = parent->children[j];
        }
        parent->children[i + 1] = newChild;
        parent->keys[i] = fullChild->keys[m / 2];

        // 更新節點的鍵值數量
        fullChild->n = m / 2;

        // 更新父節點的鍵值數量
        parent->n++;
    }

    // 顯示操作
    void display(MWayTreeNode* node, int depth = 0) {
        if (!node) return;
        for (int i = 0; i < depth; ++i) cout << "  ";
        for (int i = 0; i < node->n; ++i) {
            if (i > 0) cout << "-";
            cout << node->keys[i];
        }
        cout << endl;

        for (int i = 0; i <= node->n; ++i) {
            display(node->children[i], depth + 1);
        }
    }

    void display() { display(root); }

    // 刪除操作
    void deleteKey(int key) {
        if (!root) return;
        deleteKeyFromNode(root, key);
    }

    // 刪除節點中的鍵
    void deleteKeyFromNode(MWayTreeNode* node, int key) {
        int i = 0;
        while (i < node->n && node->keys[i] < key) i++;

        // 如果找到鍵
        if (i < node->n && node->keys[i] == key) {
            if (node->isLeaf) {
                // 如果是葉子節點，直接刪除
                for (int j = i; j < node->n - 1; ++j) {
                    node->keys[j] = node->keys[j + 1];
                }
                node->n--;
            }
            else {
                // 內部節點刪除
                if (node->children[i]->n >= m / 2) {
                    // 從前一子樹中借
                    MWayTreeNode* pred = node->children[i];
                    while (!pred->isLeaf) {
                        pred = pred->children[pred->n];
                    }
                    node->keys[i] = pred->keys[pred->n - 1];
                    deleteKeyFromNode(pred, node->keys[i]);
                }
                else if (node->children[i + 1]->n >= m / 2) {
                    // 從後一子樹中借
                    MWayTreeNode* succ = node->children[i + 1];
                    while (!succ->isLeaf) {
                        succ = succ->children[0];
                    }
                    node->keys[i] = succ->keys[0];
                    deleteKeyFromNode(succ, node->keys[i]);
                }
                else {
                    // 合併
                    mergeChildren(node, i);
                    deleteKeyFromNode(node->children[i], key);
                }
            }
        }
        else if (!node->isLeaf) {
            // 如果鍵不在此節點，則向下遞歸
            deleteKeyFromNode(node->children[i], key);
        }
    }

    // 合併子節點
    void mergeChildren(MWayTreeNode* parent, int i) {
        MWayTreeNode* leftChild = parent->children[i];
        MWayTreeNode* rightChild = parent->children[i + 1];
        leftChild->keys[leftChild->n] = parent->keys[i];
        for (int j = 0; j < rightChild->n; ++j) {
            leftChild->keys[leftChild->n + 1 + j] = rightChild->keys[j];
        }
        for (int j = 0; j <= rightChild->n; ++j) {
            leftChild->children[leftChild->n + 1 + j] = rightChild->children[j];
        }
        leftChild->n += rightChild->n + 1;

        // 更新父節點的鍵
        for (int j = i; j < parent->n - 1; ++j) {
            parent->keys[j] = parent->keys[j + 1];
        }
        for (int j = i + 1; j < parent->n; ++j) {
            parent->children[j] = parent->children[j + 1];
        }
        parent->n--;
        delete rightChild;
    }
};

// B-tree 節點結構
struct BTreeNode {
    vector<int> keys;
    vector<BTreeNode*> children;
    int t;
    int n;
    bool isLeaf;

    BTreeNode(int _t, bool leaf) : t(_t), isLeaf(leaf), n(0) {
        keys.resize(2 * t - 1);
        children.resize(2 * t, nullptr);
    }
};

// B-tree 類別
class BTree {
    BTreeNode* root;
    int t;

public:
    BTree(int _t) : root(nullptr), t(_t) {}

    // 插入操作
    void insert(int key) {
        if (!root) {
            root = new BTreeNode(t, true);
            root->keys[0] = key;
            root->n = 1;
        }
        else {
            // 如果根節點已滿，進行分裂
            if (root->n == 2 * t - 1) {
                BTreeNode* newRoot = new BTreeNode(t, false);
                newRoot->children[0] = root;
                root = newRoot;
                splitChild(root, 0);
            }

            insertNonFull(root, key);
        }
    }

    // 插入非滿節點
    void insertNonFull(BTreeNode* node, int key) {
        int i = node->n - 1;

        if (node->isLeaf) {
            // 如果是葉子節點，直接插入
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->n++;
        }
        else {
            // 否則，尋找子節點
            while (i >= 0 && key < node->keys[i]) i--;
            i++;

            if (node->children[i]->n == 2 * t - 1) {
                // 如果子節點已滿，進行分裂
                splitChild(node, i);

                // 分裂後選擇正確的子節點
                if (key > node->keys[i]) i++;
            }

            insertNonFull(node->children[i], key);
        }
    }

    // 分裂子節點
    void splitChild(BTreeNode* parent, int i) {
        BTreeNode* fullChild = parent->children[i];
        BTreeNode* newChild = new BTreeNode(t, fullChild->isLeaf);

        // 創建新節點的關鍵字和子節點
        newChild->n = t - 1;
        for (int j = 0; j < t - 1; ++j) {
            newChild->keys[j] = fullChild->keys[j + t];
        }

        // 更新父節點的鍵值
        for (int j = parent->n; j > i; --j) {
            parent->children[j + 1] = parent->children[j];
        }
        parent->children[i + 1] = newChild;
        parent->keys[i] = fullChild->keys[t - 1];

        // 更新節點的鍵值數量
        fullChild->n = t - 1;

        // 更新父節點的鍵值數量
        parent->n++;
    }

    // 顯示操作
    void display(BTreeNode* node, int depth = 0) {
        if (!node) return;

        // 顯示當前層級的鍵值，使用 '-' 連接
        for (int i = 0; i < depth; ++i) cout << "  "; // 進行縮排顯示層級
        for (int i = 0; i < node->n; ++i) {
            if (i > 0) cout << "-"; // 添加 "-" 來連接鍵
            cout << node->keys[i];
        }
        cout << endl;

        // 遞歸顯示子節點
        for (int i = 0; i <= node->n; ++i) {
            display(node->children[i], depth + 1);
        }
    }

    void display() { display(root); }

    // 刪除操作
    void deleteKey(int key) {
        if (!root) return;
        deleteKeyFromNode(root, key);
    }

    // 刪除節點中的鍵
    void deleteKeyFromNode(BTreeNode* node, int key) {
        int i = 0;
        while (i < node->n && node->keys[i] < key) i++;

        if (i < node->n && node->keys[i] == key) {
            if (node->isLeaf) {
                // 直接刪除葉子節點中的鍵
                for (int j = i; j < node->n - 1; ++j) {
                    node->keys[j] = node->keys[j + 1];
                }
                node->n--;
            }
            else {
                // 內部節點刪除
                if (node->children[i]->n >= t) {
                    // 從前一子樹中借
                    BTreeNode* pred = node->children[i];
                    while (!pred->isLeaf) {
                        pred = pred->children[pred->n];
                    }
                    node->keys[i] = pred->keys[pred->n - 1];
                    deleteKeyFromNode(pred, node->keys[i]);
                }
                else if (node->children[i + 1]->n >= t) {
                    // 從後一子樹中借
                    BTreeNode* succ = node->children[i + 1];
                    while (!succ->isLeaf) {
                        succ = succ->children[0];
                    }
                    node->keys[i] = succ->keys[0];
                    deleteKeyFromNode(succ, node->keys[i]);
                }
                else {
                    // 合併
                    mergeChildren(node, i);
                    deleteKeyFromNode(node->children[i], key);
                }
            }
        }
        else if (!node->isLeaf) {
            deleteKeyFromNode(node->children[i], key);
        }
    }

    // 合併子節點
    void mergeChildren(BTreeNode* parent, int i) {
        BTreeNode* leftChild = parent->children[i];
        BTreeNode* rightChild = parent->children[i + 1];

        leftChild->keys[leftChild->n] = parent->keys[i];
        for (int j = 0; j < rightChild->n; ++j) {
            leftChild->keys[leftChild->n + 1 + j] = rightChild->keys[j];
        }
        for (int j = 0; j <= rightChild->n; ++j) {
            leftChild->children[leftChild->n + 1 + j] = rightChild->children[j];
        }
        leftChild->n += rightChild->n + 1;

        // 更新父節點的鍵
        for (int j = i; j < parent->n - 1; ++j) {
            parent->keys[j] = parent->keys[j + 1];
        }
        for (int j = i + 1; j < parent->n; ++j) {
            parent->children[j] = parent->children[j + 1];
        }
        parent->n--;
        delete rightChild;
    }
};

// 主程式
int main() {
    int m, t;
    cout << "輸入 m-way tree 的 m 值: ";
    cin >> m;
    MWayTree mWayTree(m);

    cout << "輸入 m-way tree 的初始值 (以空格分隔，輸入 -1 結束): ";
    int value;
    while (cin >> value && value != -1) {
        mWayTree.insert(value);
    }
    cout << "輸入 B-tree 的 t 值 (最小度數): ";
    cin >> t;
    BTree bTree(t);

    cout << "輸入 B-tree 的初始值 (以空格分隔，輸入 -1 結束): ";
    while (cin >> value && value != -1) {
        bTree.insert(value);
    }

    int choice;
    while (true) {
        cout << "\n1. 插入到 m-way tree\n"
            << "2. 插入到 B-tree\n"
            << "3. 刪除 m-way tree 中的值\n"
            << "4. 刪除 B-tree 中的值\n"
            << "5. 顯示 m-way tree\n"
            << "6. 顯示 B-tree\n"
            << "7. 結束\n"
            << "選擇操作: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "輸入插入值: ";
            cin >> value;
            mWayTree.insert(value);
            break;
        case 2:
            cout << "輸入插入值: ";
            cin >> value;
            bTree.insert(value);
            break;
        case 3:
            cout << "輸入刪除值: ";
            cin >> value;
            mWayTree.deleteKey(value);
            break;
        case 4:
            cout << "輸入刪除值: ";
            cin >> value;
            bTree.deleteKey(value);
            break;
        case 5:
            cout << "m-way tree 結構: \n";
            mWayTree.display();
            break;
        case 6:
            cout << "B-tree 結構: \n";
            bTree.display();
            break;
        case 7:
            cout << "結束程式。\n";
            return 0;
        default:
            cout << "無效選擇，請重試。\n";
        }
    }
}
