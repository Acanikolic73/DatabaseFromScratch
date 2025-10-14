#pragma once
#include "pager.hpp"
#include "row.hpp"

//type
// 0 is internal node 1 is leaf
const short NODE_INTERNAL = 0;
const short NODE_LEAF = 1;

//header 
// contain type of node, weather root, pointer to parent
const int NODE_TYPE_SIZE = sizeof(short);
const int IS_ROOT_SIZE = sizeof(short);
const int PARENT_POINTER_SIZE = sizeof(int);

//offsets
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
const int LEAF_MAX_CELLS = LEAF_SPACE / LEAF_CELL_SIZE;

//functions

short is_root(char* node);
void set_root(char* node, short value);
short get_type(char* node);
void set_type(char* node, short value);

void init_leaf(char* node);
int* leaf_get_num_cells(char* node);
int* leaf_get_next(char* node);

void* leaf_get_cell(char* node, int cell_id);
int* leaf_get_key(char* node, int cell_id);
void* leaf_get_value(char* node, int cell_id);
void insert_leaf(char* node, int key, row& Row);