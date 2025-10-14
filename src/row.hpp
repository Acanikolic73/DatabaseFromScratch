#pragma once
#include <string>
using namespace std;

const int ID_SIZE = sizeof(int);
const int USERNAME_SIZE = 32;
const int EMAIL_SIZE = 256;
const int ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

struct row {
  int id;
  string username, email;
};

void compress_row(const row& Row, char* dest);
row decompress_row(const char* src);
