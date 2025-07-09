#pragma once

#include <optional>
#include <string>
#include <chrono>

enum class BPlusTreeEventType
{
  INSERT_STARTED,       // Inicio de inserción
  INSERT_COMPLETED,     // Fin de inserción exitosa
  INSERT_FAILED_EXISTS, // Falla por clave duplicada

  REMOVE_STARTED,          // Inicio de eliminación
  REMOVE_COMPLETED,        // Fin de eliminación exitosa
  REMOVE_FAILED_NOT_FOUND, // Falla por clave inexistente

  UPDATE_STARTED,          // Inicio de actualización
  UPDATE_COMPLETED,        // Fin de actualización exitosa
  UPDATE_FAILED_NOT_FOUND, // Falla por clave inexistente

  NODE_SPLIT_LEAF,      // Nodo hoja dividido
  NODE_SPLIT_INTERNAL,  // Nodo interno dividido
  NODE_MERGED_LEAF,     // Nodos hoja fusionados
  NODE_MERGED_INTERNAL, // Nodos internos fusionados

  BORROW_FROM_LEFT,  // Reequilibrio: se tomó de nodo izquierdo
  BORROW_FROM_RIGHT, // Reequilibrio: se tomó de nodo derecho

  ROOT_REPLACED, // Cambio de raíz (split o merge)
  TREE_EMPTY,    // El árbol ha quedado vacío

  SEARCH_STARTED, // Inicio de búsqueda
  SEARCH_HIT,     // Clave encontrada
  SEARCH_MISS,    // Clave no encontrada

  SEARCH_TRAVERSE // Clave no encontrada
};

const char *to_string_bplustree_event(BPlusTreeEventType t)
{
  switch (t)
  {
  case BPlusTreeEventType::INSERT_STARTED:
    return "INSERT_STARTED";
  case BPlusTreeEventType::INSERT_COMPLETED:
    return "INSERT_COMPLETED";
  case BPlusTreeEventType::INSERT_FAILED_EXISTS:
    return "INSERT_FAILED_EXISTS";
  case BPlusTreeEventType::UPDATE_STARTED:
    return "UPDATE_STARTED";
  case BPlusTreeEventType::UPDATE_COMPLETED:
    return "UPDATE_COMPLETED";
  case BPlusTreeEventType::UPDATE_FAILED_NOT_FOUND:
    return "UPDATE_FAILED_NOT_FOUND";
  case BPlusTreeEventType::REMOVE_STARTED:
    return "REMOVE_STARTED";
  case BPlusTreeEventType::REMOVE_COMPLETED:
    return "REMOVE_COMPLETED";
  case BPlusTreeEventType::REMOVE_FAILED_NOT_FOUND:
    return "REMOVE_FAILED_NOT_FOUND";
  case BPlusTreeEventType::NODE_SPLIT_LEAF:
    return "NODE_SPLIT_LEAF";
  case BPlusTreeEventType::NODE_SPLIT_INTERNAL:
    return "NODE_SPLIT_INTERNAL";
  case BPlusTreeEventType::NODE_MERGED_LEAF:
    return "NODE_MERGED_LEAF";
  case BPlusTreeEventType::NODE_MERGED_INTERNAL:
    return "NODE_MERGED_INTERNAL";
  case BPlusTreeEventType::BORROW_FROM_LEFT:
    return "BORROW_FROM_LEFT";
  case BPlusTreeEventType::BORROW_FROM_RIGHT:
    return "BORROW_FROM_RIGHT";
  case BPlusTreeEventType::ROOT_REPLACED:
    return "ROOT_REPLACED";
  case BPlusTreeEventType::TREE_EMPTY:
    return "TREE_EMPTY";
  case BPlusTreeEventType::SEARCH_STARTED:
    return "SEARCH_STARTED";
  case BPlusTreeEventType::SEARCH_HIT:
    return "SEARCH_HIT";
  case BPlusTreeEventType::SEARCH_MISS:
    return "SEARCH_MISS";
  case BPlusTreeEventType::SEARCH_TRAVERSE:
    return "SEARCH_TRAVERSE";
  default:
    return "UNKNOWN";
  }
}

template <typename Key, typename Value>
struct BPlusTreeEvent
{
  BPlusTreeEventType type;
  std::optional<Key> key;
  std::optional<Value> value;
  std::string message;
  std::chrono::system_clock::time_point timestamp;

  BPlusTreeEvent(
      BPlusTreeEventType type,
      std::optional<Key> key = std::nullopt,
      std::optional<Value> value = std::nullopt,
      const std::string &message = "")
      : type(type), key(key), value(value), message(message), timestamp(std::chrono::system_clock::now())
  {
  }
};
