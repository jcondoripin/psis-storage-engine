#include <iostream>
#include <string>
#include "../src/util/event.hpp"
#include "../src/index/bplustree_event.hpp"
#include "../src/index/bplustree.hpp"

// Helper para imprimir el tipo de evento
const char* to_string(BPlusTreeEventType t) {
  switch (t) {
    case BPlusTreeEventType::INSERT_STARTED:        return "INSERT_STARTED";
    case BPlusTreeEventType::INSERT_COMPLETED:      return "INSERT_COMPLETED";
    case BPlusTreeEventType::INSERT_FAILED_EXISTS:  return "INSERT_FAILED_EXISTS";
    case BPlusTreeEventType::REMOVE_STARTED:        return "REMOVE_STARTED";
    case BPlusTreeEventType::REMOVE_COMPLETED:      return "REMOVE_COMPLETED";
    case BPlusTreeEventType::REMOVE_FAILED_NOT_FOUND: return "REMOVE_FAILED_NOT_FOUND";
    case BPlusTreeEventType::NODE_SPLIT_LEAF:       return "NODE_SPLIT_LEAF";
    case BPlusTreeEventType::NODE_SPLIT_INTERNAL:   return "NODE_SPLIT_INTERNAL";
    case BPlusTreeEventType::NODE_MERGED_LEAF:      return "NODE_MERGED_LEAF";
    case BPlusTreeEventType::NODE_MERGED_INTERNAL:  return "NODE_MERGED_INTERNAL";
    case BPlusTreeEventType::BORROW_FROM_LEFT:      return "BORROW_FROM_LEFT";
    case BPlusTreeEventType::BORROW_FROM_RIGHT:     return "BORROW_FROM_RIGHT";
    case BPlusTreeEventType::ROOT_REPLACED:         return "ROOT_REPLACED";
    case BPlusTreeEventType::TREE_EMPTY:            return "TREE_EMPTY";
    case BPlusTreeEventType::SEARCH_STARTED:        return "SEARCH_STARTED";
    case BPlusTreeEventType::SEARCH_HIT:            return "SEARCH_HIT";
    case BPlusTreeEventType::SEARCH_MISS:           return "SEARCH_MISS";
    default:                                        return "UNKNOWN";
  }
}

int main() {
  // 1) Creamos un BPlusTree de orden 3 para <int, std::string>
  BPlusTree<int, std::string> tree(4);

  // 2) Nos suscribimos al canal de eventos:
  tree.events.subscribe([](const BPlusTreeEvent<int,std::string>& evt) {
    std::cout
      << "[EVENT] " << to_string(evt.type)
      << " | key=" << (evt.key ? std::to_string(*evt.key) : "-")
      << " | msg=" << evt.message
      << std::endl;
  });

  // 3) Ejecutamos varias operaciones y veremos todos los eventos:
  std::cout << "=== INSERTS ===\n";
  tree.insert(10, "dieciseis");
  tree.insert(20, "veinte");
  tree.insert(5,  "cinco");
  tree.insert(15, "quince");  // forzará splits

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
