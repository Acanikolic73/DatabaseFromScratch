#include <iostream>
#include <cstring>
#include "btree.hpp"

using namespace std;

int get_parent(char* node) {
    int parent;
    memcpy(&parent, node + PARENT_POINTER_OFFSET, PARENT_POINTER_SIZE);
    return parent;
}

void set_parent(char* node, int parent) {
    memcpy(node + PARENT_POINTER_OFFSET, &parent, PARENT_POINTER_SIZE);
}

short is_root(char* node) {
    short result;
    memcpy(&result, node + IS_ROOT_OFFSET, IS_ROOT_SIZE);
    return result;
}

void set_root(char* node, short is_root) {
    memcpy(node + IS_ROOT_OFFSET, &is_root, IS_ROOT_SIZE);
}

short get_type(char* node) {
    short result;
    memcpy(&result, node + NODE_TYPE_OFFSET, NODE_TYPE_SIZE);
    return result;
}

void set_type(char* node, short type) {
    memcpy(node + NODE_TYPE_OFFSET, &type, NODE_TYPE_SIZE);
}

int* leaf_get_num_cells(char* node) {
    return reinterpret_cast<int*>(node + LEAF_NUMBER_CELLS_OFFSET);
}

int* leaf_get_next(char* node) {
    return reinterpret_cast<int*>(node + LEAF_NEXT_POINTER_OFFSET);
}

void init_leaf(char* node) {
    set_root(node, 0);
    set_type(node, NODE_LEAF);
    *leaf_get_num_cells(node) = 0;
    *leaf_get_next(node) = 0;
}

// return pointer to the start of a cell
void* leaf_get_cell(char* node, int cell_id) {
    return node + LEAF_NODE_HEADER_SIZE + cell_id * LEAF_CELL_SIZE;
}

// key is at the start of the cell
int* leaf_get_key(char* node, int cell_id) {
    return reinterpret_cast<int*>(leaf_get_cell(node, cell_id));
}

// value is after the key inside the cell
void* leaf_get_value(char* node, int cell_id) {
    return (char*)leaf_get_cell(node, cell_id) + LEAF_KEY_SIZE;
}

void insert_leaf(char* node, int key, row& Row) {
    int n = *leaf_get_num_cells(node);
    if (n >= LEAF_MAX_CELLS) {
        cout << "Error: leaf node full\n";
        return;
    }

    // Append at the end (no sorting / shifting yet)
    *leaf_get_num_cells(node) = n + 1;
    *leaf_get_key(node, n) = key;
    void* value_pointer = leaf_get_value(node, n);
    compress_row(Row, reinterpret_cast<char*>(value_pointer));
}

/// internal functions 

int* internal_get_num_keys(char* node) {
    return reinterpret_cast<int*>(node + INTERNAL_NUM_KEYS_OFFSET);
}

void set_internal_num_keys(char* node, int value) {
    memcpy(node + INTERNAL_NUM_KEYS_OFFSET, &value, INTERNAL_NUM_KEYS_SIZE);
}

int* internal_get_right_child(char* node) {
    return reinterpret_cast<int*>(node + INTERNAL_RIGHT_CHILD_OFFSET);
}

void set_internal_right_child(char* node, int value) {
    memcpy(node + INTERNAL_RIGHT_CHILD_OFFSET, &value, INTERNAL_RIGHT_CHILD_SIZE);
}

void* internal_get_cell(char* node, int id) {
    return node + INTERNAL_HEADER_SIZE + id * INTERNAL_CELL_SIZE;
}

int* internal_get_child(char* node, int id) {
    return reinterpret_cast<int*>(internal_get_cell(node, id));
}

int* internal_get_key(char* node, int id) {
    return reinterpret_cast<int*>((char*)internal_get_cell(node, id) + INTERNAL_CHILD_SIZE);
}

void init_internal(char* node) {
    set_type(node, NODE_INTERNAL);
    set_root(node, 0);
    set_internal_num_keys(node, 0);
    set_internal_right_child(node, 0);
}

int find_child_index(char* node, int key) {
    int n = *internal_get_num_keys(node);
    int l = 0, r = n - 1, ans = n - 1;
    while (l <= r) {
        int mid = (l + r) / 2;
        int cur_key = *internal_get_key(node, mid);
        if (key >= cur_key) {
            ans = mid;
            r = mid - 1;
        }
        else {
            l = mid + 1;
        }
    }
    return ans;
}

void insert_internal(char* node, int child, int key) {
    int n = *internal_get_num_keys(node);
    if (n >= INTERNAL_MAX_CELLS) {
        cout << "Error: internal node is full\n";
        return;
    }
    memcpy(internal_get_child(node, n), &child, INTERNAL_CHILD_SIZE);
    memcpy(internal_get_key(node, n), &key, INTERNAL_KEY_SIZE);
    *internal_get_num_keys(node) = n + 1;
}

char* find_leaf(Pager* pager, int node_id, int key) {
    char* node = pager->get_page(node_id);
    short type = get_type(node);
    if (type == NODE_LEAF) {
        return node;
    }
    int n = *internal_get_num_keys(node);
    for (int i = 0; i < n; i++) {
        int cur_key = *internal_get_key(node, i);
        if (cur_key > key) {
            return find_leaf(pager, *internal_get_child(node, i), key);
        }
    }
    return find_leaf(pager, *internal_get_right_child(node), key);
}
//internal

void internal_insert_at(char* parent, int index, int id, int key) {
    int n = *internal_get_num_keys(parent);
    for (int i = n; i > index; i--) { // right shift
        memcpy(internal_get_cell(parent, i), internal_get_cell(parent, i - 1), INTERNAL_CELL_SIZE);
    }
    *internal_get_child(parent, index) = id;
    *internal_get_key(parent, index) = key;
    *internal_get_num_keys(parent) = n + 1;
}

int internal_find_child_index(char* parent, int id) {
    int n = *internal_get_num_keys(parent);
    for (int i = 0; i < n; i++) {
        if (*internal_get_child(parent, i) == id) return i;
    }
    if (*internal_get_right_child(parent) == id) return n;
    //not found 
    return -1;
}
