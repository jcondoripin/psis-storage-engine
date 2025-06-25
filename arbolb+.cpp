#include <iostream>
#include <vector>
#include <memory>

const int ORDEN = 4; // Orden del árbol B+

// Clase base: Nodo genérico
template<typename K>
class Node {
public:
    bool esHoja;
    std::vector<K> claves;
    int maxClaves;

    Node(bool hoja, int orden) : esHoja(hoja), maxClaves(orden - 1) {}
    virtual ~Node() = default;
};

// Nodo hoja
template<typename K, typename V>
class LeafNode : public Node<K> {
public:
    std::vector<V> valores;
    std::shared_ptr<LeafNode<K, V>> siguiente;

    LeafNode(int orden) : Node<K>(true, orden), siguiente(nullptr) {}

    // Método para eliminar clave de la hoja
    bool eliminarClave(const K& clave) {
        for (size_t i = 0; i < this->claves.size(); ++i) {
            if (this->claves[i] == clave) {
                this->claves.erase(this->claves.begin() + i);
                valores.erase(valores.begin() + i);
                return true; // Eliminado
            }
        }
        return false; // No se encontró la clave
    }
};

// Nodo interno
template<typename K>
class InternalNode : public Node<K> {
public:
    std::vector<std::shared_ptr<Node<K>>> hijos;

    InternalNode(int orden) : Node<K>(false, orden) {}
};

// Árbol B+
template<typename K, typename V>
class BPlusTree {
private:
    std::shared_ptr<Node<K>> raiz;

public:
    BPlusTree() : raiz(nullptr) {}

    void remove(const K& clave) {
        if (!raiz) {
            std::cout << "Árbol vacío.\n";
            return;
        }

        if (raiz->esHoja) {
            auto hoja = std::dynamic_pointer_cast<LeafNode<K, V>>(raiz);
            if (hoja->eliminarClave(clave)) {
                std::cout << "Clave eliminada de hoja.\n";
                if (hoja->claves.empty()) {
                    raiz = nullptr;
                    std::cout << "Árbol vacío después de eliminar.\n";
                }
            } else {
                std::cout << "Clave no encontrada.\n";
            }
        } else {
            std::cout << "Eliminación de nodos internos aún no implementada.\n";
        }
    }

    // Método auxiliar para pruebas: fijar raíz
    void setRaiz(std::shared_ptr<Node<K>> nuevaRaiz) {
        raiz = nuevaRaiz;
    }

    // Imprimir hojas
    void imprimir() {
        if (!raiz) {
            std::cout << "Árbol vacío.\n";
            return;
        }

        if (raiz->esHoja) {
            auto hoja = std::dynamic_pointer_cast<LeafNode<K, V>>(raiz);
            std::cout << "Contenido de hoja:\n";
            for (size_t i = 0; i < hoja->claves.size(); ++i) {
                std::cout << hoja->claves[i] << ": " << hoja->valores[i] << "\n";
            }
        } else {
            std::cout << "Impresión solo implementada para hojas.\n";
        }
    }
};

// Prueba en main
int main() {
    BPlusTree<int, std::string> arbol;

    // Crear una hoja con datos de prueba
    auto hoja = std::make_shared<LeafNode<int, std::string>>(ORDEN);
    hoja->claves = {10, 20, 30};
    hoja->valores = {"Ana", "Luis", "Carlos"};

    arbol.setRaiz(hoja);

    arbol.imprimir();

    std::cout << "\n--- Eliminando 20 ---\n";
    arbol.remove(20);
    arbol.imprimir();

    std::cout << "\n--- Eliminando 50 (no existe) ---\n";
    arbol.remove(50);
    arbol.imprimir();

    std::cout << "\n--- Eliminando 10 ---\n";
    arbol.remove(10);
    arbol.imprimir();

    std::cout << "\n--- Eliminando 30 ---\n";
    arbol.remove(30);
    arbol.imprimir();

    return 0;
}
