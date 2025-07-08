#pragma once

#include "../network/server.hpp"
#include "engine.hpp"
#include "interpreter.hpp"

/**
 * @brief Clase principal que representa el servidor de base de datos.
 *
 * Esta clase:
 * - Inicia un servidor de red que escucha comandos en texto.
 * - Interpreta los comandos usando el motor de ejecución (`Engine`).
 * - Devuelve las respuestas serializadas en formato JSON.
 */
class DatabaseServer
{
public:
  /**
   * @brief Constructor del servidor de base de datos.
   *
   * @param dir Ruta al directorio de datos de la base de datos (donde están .meta y .tbl).
   * @param port Puerto en el que el servidor escucha (por defecto: "65535").
   * @param host Dirección IP en la que el servidor se enlaza (por defecto: "127.0.0.1").
   *
   * Este constructor inicializa el motor de base de datos, configura el servidor de red
   * y define el manejador de comandos recibidos.
   */
  DatabaseServer(const std::string &dir,
                 const std::string &port = "65535",
                 const std::string &host = "127.0.0.1")
      : engine_(dir), server_(port, host, 1024, SOMAXCONN)
  {
    server_.setHandler([this](const std::string &msg, SOCKET client)
                       { return handleCommand(msg, client); });
  }

  /**
   * @brief Inicia el servidor y comienza a aceptar conexiones.
   *
   * @return true si el servidor se inició correctamente, false en caso de error.
   */

  bool start()
  {
    return server_.start();
  }

  /**
   * @brief Detiene el servidor y cierra todas las conexiones activas.
   */
  void stop()
  {
    server_.stop();
  }

private:
  /// @brief  Nodo de base de datos, a futuro esto se puede
  /// manejar de manera distribuida std::vector<EngineNode> engine_nodes_;
  EngineNode engine_;
  Server server_;

  /**
   * @brief Procesa un comando recibido por la red.
   *
   * @param msg Texto plano del comando (ej. "GET users 1").
   * @return std::string Respuesta serializada en formato JSON o error en texto plano.
   *
   * Este método:
   * - Parsea el mensaje usando `parser_engine::readCommand`.
   * - Ejecuta el comando en el motor.
   * - Devuelve el resultado en JSON.
   * - Captura y devuelve errores de parsing o ejecución como texto plano.
   */
  std::string handleCommand(const std::string &msg, SOCKET client)
  {
    try
    {
      std::cout << "[Client] Comando recibido por cliente " << std::to_string(client) << std::endl;
      auto args = parser_engine::readCommand(msg, client);
      auto result = engine_.exec(args, [this](const std::string &msg, SOCKET clt)
                                 { send_client_socket(msg, clt); });
      return result.toJson();
    }
    catch (const std::exception &ex)
    {
      return std::string("Error: ") + ex.what();
    }
  }

  void send_client_socket(const std::string &msg, SOCKET client)
  {
    server_.sendToClient(client, msg);
  }
};
