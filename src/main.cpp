#include <iostream>
#include <sstream>
#include "pager.hpp"
#include "row.hpp"
#include "table.hpp"
#include "btree.hpp"

using namespace std;

bool IsExist(string path) {
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        return false;
    }
    //check is empty
    file.seekg(0, ios::end);
    if (file.tellg() == 0) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

int main() {

  bool Is = IsExist("mydatabase.db");

  Pager pager("mydatabase.db");
  Table table(&pager);
  
  if (!Is) {
      char* root = pager.get_page(0);
      init_leaf(root);
      set_type(root, NODE_LEAF);
      set_root(root, 1);
  }
  else {
      Load(&table);
  }

  dfs_btree(&pager, table.root_id);

  string input;
  while(true) {
    cout << "db > ";
    getline(cin, input);
    stringstream ss(input);
    string command;
    ss >> command;

    if(command == "insert") {
      row Row;
      ss >> Row.id >> Row.username >> Row.email;
      insert_row(&table, Row);
      cout << "Row is successfully inserted\n";
    } else if(command == "select") {
      print(&table);
    } else if(command == "exit") {
      //for persistent save
      table.pager->flush_page(table.root_id);
      cout << "Bye\n";
      break;
    } else {
      cout << "Unrecognized command\n";
    }
  }
  return 0;
}
