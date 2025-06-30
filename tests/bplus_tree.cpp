// test insert, search and print method for bplustree
#include "../src/index/bplustree.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <optional>
int main()
{
  BPlusTree<int, std::string> tree(4); // Orden 4 para test

  // Insertar claves (40 claves) con bucle for
  for (int i = 0; i < 40; ++i)
  {
    tree.insert(i, "Valor" + std::to_string(i));
  }

  std::cout << "Árbol tras inserciones: ";
  tree.print();

  // Buscar claves existentes
  assert(tree.search(10).has_value());
  assert(tree.search(20).value() == "Valor20");
  assert(tree.search(5).value() == "Valor5");

  // Buscar clave inexistente
  assert(!tree.search(99).has_value());

  // Eliminar clave
  // bool deleted = tree.remove(20);
  // assert(deleted);
  // assert(!tree.search(20).has_value());

  // std::cout << "Árbol tras eliminar 20: ";
  // tree.print();

  std::cout << "✅ Todos los tests pasaron correctamente.\n";

  return 0;
}