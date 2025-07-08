# PSIS-DB: Motor de Base de Datos en C++

**PSIS-DB** es un motor de base de datos educativo escrito en C++, que implementa estructuras tipo B+Tree, almacenamiento en archivos binarios (`.tbl`, `.meta`), y ejecución de comandos a través de un intérprete interno y una interfaz CLI.

## 🧠 Funcionalidades principales

- Estructura de índices basada en **B+Trees**  
- Almacenamiento persistente en archivos `.tbl` y `.meta`  
- Separación de lógica en módulos: `core`, `index`, `network`, `storage`, `util`  
- Ejecución de comandos `CREATE`, `INSERT`, `GET`, `UPDATE` y `DELETE`
- Soporte para logs (`.log`) y backups automáticos  
- Arquitectura extensible y portable  

---

## 🗂️ Estructura del Proyecto

```plaintext
.
├── main.cpp                 # Ejecutable principal del servidor
├── src/
│   ├── core/                # Motor de ejecución e interpretación de comandos
│   ├── index/               # Implementación de B+Tree
│   ├── network/             # Cliente/Servidor TCP
│   ├── storage/             # Almacenamiento binario (datos y metadatos)
│   └── util/                # Estructuras auxiliares (columnas, tablas, operaciones)
├── build/                   # Archivos binarios y tests compilados (Windows)
├── include/                 # Cabeceras públicas
├── tests/                   # Código de pruebas (unitarias)
└── README.md                # Documentación del proyecto
````

---

## ⚙️ Compilación

### 📦 Requisitos

* C++17 o superior
* Compilador GCC o Clang (Linux), MinGW o MSVC (Windows)
* Make (Linux) o compilación manual (Windows)

---

### 💻 Linux

#### 🔧 Compilar el servidor (`main.cpp`)

```bash
g++ main.cpp -std=c++17 -o psisdb_server
```

> Para facilitar la compilación, puedes añadir un `Makefile` con los siguientes objetivos:
>
> ```makefile
> all: server cli
>
> server:
> 	g++ main.cpp -std=c++17 -o psisdb_server
>
> clean:
> 	rm -f psisdb_server psisdb_cli
> ```

---

### 🪟 Windows (MinGW)

1. Abre MSYS2 o una terminal con MinGW configurado.

2. Verifica el compilador:

   ```bash
   g++ --version
   ```

3. Compila el servidor:

   ```bash
   g++ main.cpp -std=c++17 -lws2_32 -o server.exe
   ```

---

## ▶️ Ejecución

### Servidor

```bash
./psisdb_server       # Linux
server.exe            # Windows
```
---

## 🧪 Pruebas

En la carpeta `tests/` encontrarás archivos .cpp de pruebas listos para compilar.

---

## 💾 Archivos Generados

* **`.tbl`** → Datos binarios de la tabla
* **`.meta`** → Metadatos de la estructura de la tabla
* **`.log`** → Registro de comandos mutables

---

## 🧰 Módulos clave

* **`engine.hpp`**      : Ejecuta comandos parseados
* **`interpreter.hpp`** : Interpreta tokens y estructura consultas
* **`database_node.hpp`** : Manejo de almacenamiento de filas
* **`bplustree.hpp`**   : Implementación principal de B+Tree

---

## 📝 Licencia

Este proyecto se distribuye bajo la **MIT License**. Puedes usar, modificar y distribuir libremente.

---