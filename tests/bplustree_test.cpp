#include "../src/index/bplustree.hpp"
#include <iostream>
#include <cassert>
#include <string>

int main()
{
  BPlusTree<int, std::string> tree(4); // Orden 4 para test

  // Insertar claves
  tree.insert(10, "Diez");
  tree.insert(20, "Veinte");
  tree.insert(5, "Cinco");
  tree.insert(15, "Quince");
  tree.insert(25, "Veinticinco");

  std::cout << "Árbol tras inserciones: ";
  tree.print();

  // Buscar claves existentes
  assert(tree.search(10).has_value());
  assert(tree.search(20).value() == "Veinte");
  assert(tree.search(5).value() == "Cinco");

  // Buscar clave inexistente
  assert(!tree.search(99).has_value());

  // Eliminar clave
  bool deleted = tree.remove(20);
  assert(deleted);
  assert(!tree.search(20).has_value());

  std::cout << "Árbol tras eliminar 20: ";
  tree.print();

  std::cout << "✅ Todos los tests pasaron correctamente.\n";

  return 0;
}
