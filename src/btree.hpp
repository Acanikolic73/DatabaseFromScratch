#pragma once
#include "pager.hpp"
#include "row.hpp"
#include "table.hpp"

//type
// 0 is internal node 1 is leaf
const short NODE_INTERNAL = 0;
const short NODE_LEAF = 1;

// leaf node header 
// contain type of node, weather root, pointer to parent
const int NODE_TYPE_SIZE = sizeof(short);
const int IS_ROOT_SIZE = sizeof(short);
const int PARENT_POINTER_SIZE = sizeof(int);

//leaf node header offsets
const int NODE_TYPE_OFFSET = 0;
const int IS_ROOT_OFFSET = NODE_TYPE_OFFSET + NODE_TYPE_SIZE;
const int PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const int HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

//leaf node header
// contain number of cells(rows), pointer to next
const int LEAF_NUMBER_CELLS_SIZE = sizeof(int);
const int LEAF_NEXT_POINTER_SIZE = sizeof(int);

//offsets
const int LEAF_NUMBER_CELLS_OFFSET = HEADER_SIZE;
const int LEAF_NEXT_POINTER_OFFSET = LEAF_NUMBER_CELLS_OFFSET + LEAF_NUMBER_CELLS_SIZE;
const int LEAF_NODE_HEADER_SIZE = HEADER_SIZE + LEAF_NUMBER_CELLS_SIZE + LEAF_NEXT_POINTER_SIZE;

//leaf node body
// contain key, cells
// in one cell we store one row 
const int LEAF_KEY_SIZE = sizeof(int);
const int LEAF_VALUE_SIZE = ROW_SIZE;
const int LEAF_CELL_SIZE = LEAF_KEY_SIZE + LEAF_VALUE_SIZE;

//offsets
const int LEAF_KEY_OFFSET = 0;
const int LEAF_VALUE_OFFSET = LEAF_KEY_OFFSET + LEAF_KEY_SIZE;
//cells per page
const int LEAF_SPACE = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
//const int LEAF_MAX_CELLS = LEAF_SPACE / LEAF_CELL_SIZE;
const int LEAF_MAX_CELLS = 2;

//functions

short is_root(char* node);
void set_root(char* node, short value);
short get_type(char* node);
void set_type(char* node, short value);
int get_parent(char* node);
void set_parent(char* node, int parent);

void init_leaf(char* node);
int* leaf_get_num_cells(char* node);
int* leaf_get_next(char* node);

void* leaf_get_cell(char* node, int cell_id);
int* leaf_get_key(char* node, int cell_id);
void* leaf_get_value(char* node, int cell_id);
void insert_leaf(char* node, int key, row& Row);


/// internal node header
const int INTERNAL_NUM_KEYS_SIZE = sizeof(int);
const int INTERNAL_RIGHT_CHILD_SIZE = sizeof(int);
//offset
const int INTERNAL_NUM_KEYS_OFFSET = HEADER_SIZE;
const int INTERNAL_RIGHT_CHILD_OFFSET = INTERNAL_NUM_KEYS_OFFSET + INTERNAL_NUM_KEYS_SIZE;
const int INTERNAL_HEADER_SIZE = HEADER_SIZE + INTERNAL_NUM_KEYS_SIZE + INTERNAL_RIGHT_CHILD_SIZE;
/// internal node body
const int INTERNAL_KEY_SIZE = sizeof(int);
const int INTERNAL_CHILD_SIZE = sizeof(int);
const int INTERNAL_CELL_SIZE = INTERNAL_KEY_SIZE + INTERNAL_CHILD_SIZE;
const int INTERNAL_SPACE = PAGE_SIZE - INTERNAL_HEADER_SIZE;
//const int INTERNAL_MAX_CELLS = INTERNAL_SPACE / INTERNAL_CELL_SIZE;
const int INTERNAL_MAX_CELLS = 2;
//internal functions 
int* internal_get_num_keys(char* node);
void set_internal_num_keys(char* node, int value);
int* internal_get_right_child(char* node);
void set_internal_right_child(char* node, int value);
void* internal_get_cell(char* node, int id);
int* internal_get_child(char* node, int id);
int* internal_get_key(char* node, int id);
void init_internal(char* node);
int find_child_index(char* node, int key);
void insert_internal(char* node, int child, int key);

void insert_into_parent(Table* table, int left_page, int key, int right_page);

// internal node helpers
void internal_insert_at(char* parent_node, int index, int child_page, int key);
void split_internal(Table* table, int parent_page, int index, int child_page, int key);
int internal_find_child_index(char* parent_node, int child_page);


//search btree
char* find_leaf(Pager* pager, int root_id, int key);