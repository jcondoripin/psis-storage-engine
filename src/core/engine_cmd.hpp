#pragma once

#include "../storage/database_node.hpp"
#include "../util/record.hpp"
#include <string>
#include <vector>
#include <ostream>

/**
 * @brief Resultado de la ejecución de un comando.
 * Incluye éxito/fallo, mensaje detallado y datos opcionales (por ejemplo en SELECT/GET).
 */
struct CommandResult
{
  bool success{false};
  std::string detail;
  std::vector<Record> data;

  static CommandResult Ok(const std::string &msg = "")
  {
    return CommandResult{true, msg, {}};
  }
  static CommandResult Fail(const std::string &msg)
  {
    return CommandResult{false, msg, {}};
  }
  static CommandResult WithData(std::vector<Record> d, const std::string &msg = "")
  {
    return CommandResult{true, msg, std::move(d)};
  }

  /**
   * @brief Serializa el resultado a JSON.
   * @return std::string con el JSON.
   */
  std::string toJson() const
  {
    std::ostringstream os;
    os << "{";
    os << "\"success\":" << (success ? "true" : "false");
    if (!detail.empty())
    {
      os << ", \"detail\":\"";
      for (char c : detail)
      {
        if (c == '"')
          os << "\\\"";
        else if (c == '\\')
          os << "\\\\";
        else
          os << c;
      }
      os << "\"";
    }
    if (!data.empty())
    {
      os << ", \"data\":[";
      bool firstRec = true;
      for (const auto &rec : data)
      {
        if (!firstRec)
          os << ",";
        firstRec = false;
        os << "{";
        bool firstField = true;
        for (const auto &field : rec.values)
        {
          if (!firstField)
            os << ",";
          firstField = false;
          os << "\"" << field.column << "\":\"";
          for (char c : field.value)
          {
            if (c == '"')
              os << "\\\"";
            else if (c == '\\')
              os << "\\\\";
            else
              os << c;
          }
          os << "\"";
        }
        os << "}";
      }
      os << "]";
    }
    os << "}";
    return os.str();
  }
};

inline std::ostream &operator<<(std::ostream &os, const CommandResult &r)
{
  os << (r.success ? "Success" : "Failure");
  if (!r.detail.empty())
    os << ": " << r.detail;
  return os;
}

/**
 * @brief Interfaz base para todos los comandos del motor.
 */
class EngineCommand
{
public:
  virtual ~EngineCommand() = default;

  /**
   * @brief Ejecuta la lógica del comando sobre la base de datos.
   * @return CommandResult con estado, detalle y datos (si aplica).
   */
  virtual CommandResult execute(DatabaseNode &db) const = 0;

  /**
   * @brief Serializa información de auditoría o log en el stream.
   * Solo se invoca si execute() fue exitoso.
   */
  virtual void log(std::ostream &os) const = 0;
};
