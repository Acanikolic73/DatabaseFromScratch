#include <iostream>
#include <cstring>
#include "btree.hpp"

using namespace std;

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
