#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
using namespace std;

const int PAGE_SIZE = 4096;
const int TABLE_MAX_PAGES = 100;
const int PAGER_HEADER_SIZE = sizeof(int);

struct Pager {
  fstream file;
  string filename;
  int file_length, num_pages;
  vector<char*> pages;

  Pager(const string& fileName);
  char* get_page(int page_id);
  void flush_page(int page_id);
  bool is_declared(int page_id);
  ~Pager();
};
