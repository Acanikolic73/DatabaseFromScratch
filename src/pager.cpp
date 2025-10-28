#include "pager.hpp"

Pager::Pager(const string& fileName) {
  filename = fileName;
  file.open(filename, ios::in | ios::out | ios::binary);
  // If file doesn't exist, create and write header
  if(!file.is_open()) {
      ofstream newFile(filename, ios::binary);
      int zero = 0;
      newFile.write(reinterpret_cast<char*>(&zero), PAGER_HEADER_SIZE);
      newFile.close();
      file.open(filename, ios::in | ios::out | ios::binary);
  }
  file.seekg(0, ios::end);
  file_length = file.tellg();
  // If file shorter than header, fix it, add header
  if(file_length < PAGER_HEADER_SIZE) {
      int zero = 0;
      file.seekp(0, ios::beg);
      file.write(reinterpret_cast<char*>(&zero), PAGER_HEADER_SIZE);
      file.flush();
      file_length = PAGER_HEADER_SIZE;
  }
  num_pages = (file_length - PAGER_HEADER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
  pages.resize(TABLE_MAX_PAGES, nullptr);
  cout << "Opened database file " << filename << " (" << num_pages << " pages)\n";
}

bool Pager::is_declared(int page_id) {
    return (pages[page_id] != nullptr);
}

char* Pager::get_page(int page_id) {
  if(page_id >= TABLE_MAX_PAGES) {
    cout << "Tried to fetch page number out of bounds (" << page_id << ")\n";
    exit(EXIT_FAILURE);
  }
  if(pages[page_id] == nullptr) {
    char* page = new char[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    if(page_id < num_pages) {
      file.seekg(PAGER_HEADER_SIZE + PAGE_SIZE * page_id, ios::beg);
      file.read(page, PAGE_SIZE);
    }
    pages[page_id] = page;
    if(page_id >= num_pages) num_pages = page_id + 1;
  }
  return pages[page_id];
}

void Pager::flush_page(int page_id) {
  if(pages[page_id] == nullptr) return;
  file.seekp(PAGER_HEADER_SIZE + PAGE_SIZE * page_id, ios::beg);
  file.write(pages[page_id], PAGE_SIZE);
  file.flush();
}

Pager::~Pager() {
  for(int i = 0; i < num_pages; i++) {
    flush_page(i);
    delete[] pages[i];
  }
  file.close();
}
