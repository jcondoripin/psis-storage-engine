#include <iostream>
#include <string>
#include "../src/util/event.hpp"
#include "../src/index/bplustree_event.hpp"
#include "../src/index/bplustree.hpp"

int main() {
  // 1) Creamos un BPlusTree de orden 3 para <int, std::string>
  BPlusTree<int, std::string> tree(4);

  // 2) Nos suscribimos al canal de eventos:
  tree.events.subscribe([](const BPlusTreeEvent<int,std::string>& evt) {
    std::cout
      << "[EVENT] " << to_string_bplustree_event(evt.type)
      << " | key=" << (evt.key ? std::to_string(*evt.key) : "-")
      << " | msg=" << evt.message
      << std::endl;
  });

  // 3) Ejecutamos varias operaciones y veremos todos los eventos:
  std::cout << "=== INSERTS ===\n";
  tree.insert(10, "diez");
  tree.insert(20, "veinte");
  tree.insert(5,  "cinco");
  tree.insert(15, "quince");  // forzará splits

  std::cout << "=== UPDATES ===\n";
  tree.update(10, "diez_update");
  tree.update(20, "veinte_update");
  tree.update(5,  "cinco_update");
  tree.update(15, "quince_update");  // forzará splits

  tree.print();

  std::cout << "\n=== SEARCH ===\n";
  auto r1 = tree.search(15);
  auto r2 = tree.search(42);   // miss

  std::cout << "\n=== REMOVES ===\n";
  tree.remove(20);
  tree.remove(5);
  tree.remove(99);             // no existe
  tree.remove(15);             // no existe

  tree.print();

  return 0;
}
