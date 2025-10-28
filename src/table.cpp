#include "table.hpp"
#include "btree.hpp"

Table::Table(Pager* p) {
  pager = p;
  num_rows = 0;
  root_id = 0; 
  /*if (!p->is_declared(root_id)) {
      cout << "NEMA SILE" << endl;
      char* root = p->get_page(root_id);
      init_leaf(root);
      set_type(root, NODE_LEAF);
      set_root(root, 1);
  }*/
}

char* row_location(Pager &pager, int row_id) {
  int where = row_id / ROWS_PER_PAGE;
  char* page = pager.get_page(where);
  int row_offset = row_id % ROWS_PER_PAGE;
  int byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

int create_page(Pager* pager) {
    pager->get_page(pager->num_pages);
    return pager->num_pages - 1;
}

void create_root(Table* table, int left_child_page, int right_child_page, int key) {
    int root_page = create_page(table->pager);
    char* root_node = table->pager->get_page(root_page);
    cout << "NOVI ROOT " << left_child_page << " " << right_child_page << endl;

    // Initialize as internal node
    init_internal(root_node);
    set_root(root_node, 1);
    set_type(root_node, NODE_INTERNAL);

    // Add first (child, key) pair
    char* left_child = table->pager->get_page(left_child_page);
    set_parent(left_child, root_page);
    *internal_get_child(root_node, 0) = left_child_page;
    *internal_get_key(root_node, 0) = key;
    *internal_get_num_keys(root_node) = 1;

    // Set right child
    char* right_child = table->pager->get_page(right_child_page);
    set_parent(right_child, root_page);
    //cout << "ovde setujem right_child_page" << endl;
    set_internal_right_child(root_node, right_child_page);
    //cout << *internal_get_right_child(root_node) << " odje" << endl;

    // Old left child is no longer root
    char* left_node = table->pager->get_page(left_child_page);
    set_root(left_node, 0);

    // Update table root
    table->root_id = root_page;

    // Flush all pages
    table->pager->flush_page(left_child_page);
    table->pager->flush_page(right_child_page);
    table->pager->flush_page(root_page);
}

void Split(Table* table, char* node, row& Row) {
    int id = create_page(table->pager);
    int n = *leaf_get_num_cells(node);
    int half = n / 2;
    char* new_node = table->pager->get_page(id);
    init_leaf(new_node);
    for (int i = half; i < n; i++) {
        int cur_key;
        row cur;
        memcpy(&cur_key, leaf_get_key(node, i), LEAF_KEY_SIZE);
        cur = decompress_row((char*)leaf_get_value(node, i));
        insert_leaf(new_node, cur_key, cur);
    }
    //merge 
    int val = *leaf_get_next(node);
    *leaf_get_next(new_node) = val;
    *leaf_get_next(node) = id;
    *leaf_get_num_cells(node) = half;
    //check where to insert current
    int last_left = *leaf_get_key(node, half - 1);
    if (Row.id > last_left) {
        insert_leaf(new_node, Row.id, Row);
    }
    else {
        insert_leaf(node, Row.id, Row);
    }
    //is node is root we need to create new root 
    if (is_root(node)) {
        int separator_key = *leaf_get_key(new_node, 0);
        create_root(table, table->root_id, id, separator_key);
    }
    else {
        int separator_key = *leaf_get_key(new_node, 0);
        insert_into_parent(table, get_parent(node), separator_key, id);
    }
    table->pager->flush_page(id);
}

void insert_row(Table* table, row& Row) {
    char* root = table->pager->get_page(table->root_id);
    char* leaf = find_leaf(table->pager, table->root_id, Row.id);
    if (get_type(leaf) == NODE_LEAF) {
        int n = *leaf_get_num_cells(leaf);
        if (n == LEAF_MAX_CELLS) {
            cout << "Sefe sad splitujem " << endl;
            Split(table, leaf, Row);
        }
        else {
            cout << "radi " << table->root_id << endl;
            insert_leaf(leaf, Row.id, Row);
            table->pager->flush_page(table->root_id);
        }
    }
    else {
        //it is never a case
    }
}

void print(Pager* pager, int page_num) {
    char* node = pager->get_page(page_num);
    short type = get_type(node);

    if (type == NODE_LEAF) {
        int num_cells = *leaf_get_num_cells(node);
        for (int i = 0; i < num_cells; i++) {
            row Row = decompress_row((char*)leaf_get_value(node, i));
            std::cout << "(" << Row.id << ", " << Row.username << ", " << Row.email << ")\n";
        }
    }
    else if (type == NODE_INTERNAL) {
        cout << "correct" << endl;
        int num_keys = *internal_get_num_keys(node);
        for (int i = 0; i < num_keys; i++) {
            int child_page = *internal_get_child(node, i);
            print(pager, child_page);
        }
        int right_page = *internal_get_right_child(node);
        cout << "evo " << right_page << endl;
        print(pager, right_page);
    }
}

void print(Table* table) {
    //FIX AKO ZABORAVIM
    print(table->pager, table->root_id);
}

void Load(Table* table) {
    int root_id = table->root_id;
    char* root = table->pager->get_page(root_id);
    cout << "DAL " << is_root(root) << endl;
    while (!is_root(root)) {
        root_id = get_parent(root);
        root = table->pager->get_page(root_id);
    }
    table->root_id = root_id;
}

//internal functions

void split_internal(Table* table, int parent_id, int index, int child_id, int key) {
    char* parent = table->pager->get_page(parent_id);
    int n = *internal_get_num_keys(parent);
    int new_node_id = create_page(table->pager);
    char* new_node = table->pager->get_page(new_node_id);
    init_internal(new_node);
    int *keys = new int[n + 2];
    int* child = new int[n + 3];
    for (int i = 0; i < n; i++) {
        child[i] = *internal_get_child(parent, i);
        keys[i] = *internal_get_key(parent, i);
    }
    child[n] = *internal_get_right_child(parent);
    for (int i = n; i > index; i--) {
        child[i] = child[i - 1];
        keys[i - 1] = keys[i - 2];
    }
    keys[index] = key;
    child[index + 1] = child_id;
    int half = (n + 1) / 2;
    *internal_get_num_keys(parent) = half;
    for (int i = 0; i < half; i++) {
        *internal_get_child(parent, i) = child[i];
        *internal_get_key(parent, i) = keys[i];
    }
    set_internal_right_child(parent, child[half]);
    int rest = n - half;
    *internal_get_num_keys(new_node) = rest;
    for (int i = 0; i < rest; i++) {
        *internal_get_child(new_node, i) = child[i + half + 1];
        *internal_get_key(new_node, i) = keys[i + half + 1];
    }
    set_internal_right_child(new_node, child[n]);
    //set parent for right 
    for (int i = 0; i < rest; i++) {
        int id = *internal_get_child(new_node, i);
        char* ch = table->pager->get_page(id);
        set_parent(ch, new_node_id);
    }
    int right_id = *internal_get_right_child(new_node);
    char* right_node = table->pager->get_page(right_id);
    set_parent(right_node, new_node_id);
    int split_key = keys[half];
    insert_into_parent(table, parent_id, split_key, right_id);
    delete[] keys;
    delete[] child;
    //set parent for left
    for (int i = 0; i < half; i++) {
        int id = *internal_get_child(parent, i);
        char* ch = table->pager->get_page(id);
        set_parent(ch, parent_id);
    }
    right_id = *internal_get_right_child(parent);
    right_node = table->pager->get_page(right_id);
    set_parent(right_node, parent_id);
    //flush pages
    table->pager->flush_page(parent_id);
    table->pager->flush_page(new_node_id);
}

void insert_into_parent(Table* table, int left_id, int key, int right_id) {
    char* left = table->pager->get_page(left_id);
    if (is_root(left)) {
        create_root(table, left_id, right_id, key);
        return;
    }
    int parent_id = get_parent(left);
    char* parent = table->pager->get_page(parent_id);
    int where = internal_find_child_index(parent, left_id);
    if (*internal_get_num_keys(parent) < INTERNAL_MAX_CELLS) {
        internal_insert_at(parent, where, right_id, key);
        char* right = table->pager->get_page(right_id);
        set_parent(right, parent_id);
        table->pager->flush_page(parent_id);
        table->pager->flush_page(right_id);
    }
    else {
        split_internal(table, parent_id, where, right_id, key);
    }
}

void dfs_btree(Pager* pager, int page_id) {
    char* node = pager->get_page(page_id);
    short type = get_type(node);

    cout << "\n[ Node Page ID = " << page_id << " ]" << endl;
    if (type == NODE_LEAF) {
        cout << "Type: Leaf Node" << endl;
        int num = *leaf_get_num_cells(node);
        cout << "Keys: ";
        for (int i = 0; i < num; i++) {
            row Row = decompress_row((char*)leaf_get_value(node, i));
            cout << "(" << Row.id << "," << Row.username << "," << Row.email << ") ";
        }
        cout << endl;
    }
    else if (type == NODE_INTERNAL) {
        cout << "Type: Internal Node" << endl;
        int num = *internal_get_num_keys(node);
        cout << "Keys: ";
        for (int i = 0; i < num; i++) {
            cout << *internal_get_key(node, i) << " ";
        }
        cout << "\nChildren: ";
        for (int i = 0; i < num; i++) {
            cout << *internal_get_child(node, i) << " ";
        }
        cout << " | Right Child: " << *internal_get_right_child(node) << endl;

        // DFS on children
        for (int i = 0; i < num; i++) {
            dfs_btree(pager, *internal_get_child(node, i));
        }
        dfs_btree(pager, *internal_get_right_child(node));
    }
}