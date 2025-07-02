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
  SEARCH_MISS     // Clave no encontrada
};

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
