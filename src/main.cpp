#include <iostream>
#include <sstream>
#include "pager.hpp"
#include "row.hpp"
#include "table.hpp"
#include "btree.hpp"

using namespace std;

int main() {

  Pager pager("mydatabase.db");
  Table table(&pager);

  char* root = pager.get_page(0);
  short type = get_type(root);
  if (type != NODE_LEAF) {
      init_leaf(root);
      set_root(root, 1);
      table.root_id = 0;
  }

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
