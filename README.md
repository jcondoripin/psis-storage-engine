ES

Se propone lo siguiente:
Tenemos un nodo de árbol b+,
cada nodo contiene tres arrays, uno de claves, otro de valores y otro de nodos hijos
las claves son almacenadas en un vector especial definido para búsquedas binarias y con método 
lower_bound para inserción en posiciones específicas
los otros dos arrays son vectores normales

```
mydb_rt/                        
├── include/                    # Headers públicos
│   └── mydb_rt/                
│       ├── engine.hpp          # API principal
│       ├── storage.hpp         # Persistencia genérica
│       ├── network.hpp         # Cliente/servidor
│       └── events.hpp          # Pub/Sub
├── src/                        
│   ├── core/                   
│   │   └── engine.cpp          # Orquesta storage + network + events
│   ├── index/                  
│   │   └── bplustree.cpp       # Estructura B+ en memoria
│   ├── storage/                
│   │   ├── kv_store.cpp        # Wrapper de RocksDB o simple mmap+file
│   │   └── snapshot.cpp        # Dump y restore rápido
│   ├── network/                
│   │   └── server.cpp          # Boost.Asio TCP, acepta comandos
│   ├── events/                 
│   │   └── pubsub.cpp          # Envío de notificaciones a clientes suscritos
│   └── util/                   
│       ├── logger.cpp          # Logging minimalista (spdlog/otros)
│       └── config.cpp          # Lee parámetros (puerto, paths)
├── tests/                      
│   ├── core_tests.cpp          
│   └── storage_tests.cpp       
├── examples/                   
│   └── simple_client.cpp       # Demo de inserción + suscripción
├── CMakeLists.txt              
└── README.md                   
```