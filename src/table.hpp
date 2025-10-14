#pragma once
#include "pager.hpp"
#include "row.hpp"

const int ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const int TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

struct Table {
  Pager* pager;
  int num_rows, root_id;

  Table(Pager* p);
};

char* row_location(Pager &pager, int row_id);
void insert_row(Table* table, row& Row);
void print(Table* table);