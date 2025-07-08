#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

/**
 * @brief Manejador de callbacks indexados por tabla.
 * @tparam K Tipo de la tabla (p. ej. std::string).
 * @tparam T Tipo del valor que llega al listener.
 */
template <typename K, typename T>
class EventKeyHandler
{
public:
  // Para cada tabla guardamos un vector de listeners
  std::unordered_map<K, std::vector<std::function<void(const T &)>>> listeners;

  /**
   * @brief Suscribe un listener para una tabla concreta.
   * @param key      La tabla del evento.
   * @param listener Callback que recibirá el valor.
   */
  void subscribe(const K &key, const std::function<void(const T &)> &listener)
  {
    listeners[key].push_back(listener);
    std::cout << "[emit] Listener agregado (" << listeners[key].size() << ") a topico: " << key << std::endl;
  }

  /**
   * @brief Emite el valor a todos los listeners suscritos a keyIndex.
   * @param keyIndex La tabla para la que se dispara el evento.
   * @param value    El valor a pasar a cada listener.
   */
  void emit(const K &keyIndex, const T &value)
  {
    auto it = listeners.find(keyIndex);
    if (it != listeners.end())
    {
      for (auto &listener : it->second)
      {
        if (listener)
        {
          listener(value);
          std::cout << "[emit] Evento enviado a topico: " << keyIndex << std::endl;
        }
        else
        {
          std::cerr << "[emit] Listener inválido (null)" << std::endl;
        }
      }
    }
  }
};
