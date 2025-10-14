#include "row.hpp"
#include <cstring>

void compress_row(const row& Row, char* dest) {
    memcpy(dest, &Row.id, ID_SIZE);
    memset(dest + ID_SIZE, 0, USERNAME_SIZE);
    strncpy_s(dest + ID_SIZE, USERNAME_SIZE, Row.username.c_str(), _TRUNCATE);
    memset(dest + ID_SIZE + USERNAME_SIZE, 0, EMAIL_SIZE);
    strncpy_s(dest + ID_SIZE + USERNAME_SIZE, EMAIL_SIZE, Row.email.c_str(), _TRUNCATE);
}

row decompress_row(const char* src) {
    row result;
    memcpy(&result.id, src, ID_SIZE);
    result.username = std::string(src + ID_SIZE, USERNAME_SIZE);
    result.email = std::string(src + ID_SIZE + USERNAME_SIZE, EMAIL_SIZE);
    return result;
}

