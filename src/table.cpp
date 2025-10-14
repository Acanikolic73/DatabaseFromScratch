#include "table.hpp"
#include "btree.hpp"

Table::Table(Pager* p) {
  pager = p;
  num_rows = 0;
  root_id = 0;
}

char* row_location(Pager &pager, int row_id) {
  int where = row_id / ROWS_PER_PAGE;
  char* page = pager.get_page(where);
  int row_offset = row_id % ROWS_PER_PAGE;
  int byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

void insert_row(Table* table, row& Row) {
    char* root = table->pager->get_page(table->root_id);
    int num_cells = *leaf_get_num_cells(root);

    insert_leaf(root, Row.id, Row);
    table->pager->flush_page(table->root_id);
}


void print(Table* table) {
    char* root = table->pager->get_page(table->root_id);
    int num_cells = *leaf_get_num_cells(root);

    for (int i = 0; i < num_cells; i++) {
        int key;
        memcpy(&key, root + LEAF_NODE_HEADER_SIZE + i * LEAF_CELL_SIZE, LEAF_KEY_SIZE);
        row Row = decompress_row(root + LEAF_NODE_HEADER_SIZE + i * LEAF_CELL_SIZE + LEAF_KEY_SIZE);
        std::cout << "(" << Row.id << ", " << Row.username << ", " << Row.email << ")\n";
    }
}