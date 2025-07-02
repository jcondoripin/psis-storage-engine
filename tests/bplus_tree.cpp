// test insert, search and print method for bplustree
#include "../src/index/bplustree.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <optional>
int main()
{
  try
  {
    BPlusTree<int, std::string> tree(5); // Orden 4 para test

    // Insertar claves (40 claves) con bucle for
    for (int i = 0; i < 21; ++i)
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

    bool deleted14 = tree.remove(14);
    assert(deleted14);
    assert(!tree.search(14).has_value());

    bool deleted16 = tree.remove(16);
    assert(deleted16);
    assert(!tree.search(16).has_value());

    bool deleted13 = tree.remove(13);
    assert(deleted13);
    assert(!tree.search(13).has_value());

    bool deleted0 = tree.remove(0);
    assert(deleted0);
    assert(!tree.search(0).has_value());

    assert(tree.remove(8));
    assert(tree.remove(5));
    assert(tree.remove(1));

    std::cout << "Árbol tras eliminar 14, 16, 13, 0: ";
    tree.print();

    for (auto i : {8, 1, 0, 22, 70, 69, 71, 49})
    {
      tree.insert(i, "Valor" + std::to_string(i));
    }
    for (int i = 80; i < 200; ++i)
    {
      tree.insert(i, "Valor" + std::to_string(i));
    }
    assert(tree.remove(81));
    assert(tree.remove(82));

    std::cout << "Probando actualización de clave 10...\n";
    bool updated = tree.update(10, "NuevoValor10");
    assert(updated);
    auto updatedVal = tree.search(10);
    assert(updatedVal.has_value());
    assert(updatedVal.value() == "NuevoValor10");

    std::cout << "Probando actualización de clave inexistente (999)...\n";
    bool updatedInexistente = tree.update(999, "NoExiste");
    assert(!updatedInexistente);

    tree.print();

    tree.printLinked();

    std::cout << "✅ Todos los tests pasaron correctamente.\n";
  }
  catch (const std::exception &e)
  {
    std::cerr << "Excepción atrapada: " << e.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Excepción desconocida atrapada." << std::endl;
  }

  return 0;
}