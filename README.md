# Proyecto Hundir la Flota

**Autor:** Carlos Lucena Robles  
**Asignatura:** Redes  
**Curso:** 2023 - 2024  
**Universidad:** Universidad de Córdoba  
**Grado:** 4º de Ingeniería Informática  

---

## Descripción

Esta aplicación de terminal implementa el clásico juego "Hundir la Flota" utilizando un modelo cliente-servidor basado en TCP/IP (sockets).

---

## Instrucciones de Compilación

Para compilar el proyecto, sigue estos pasos:

1. Crea y accede al directorio de compilación:
```bash
mkdir build && cd build
```
	
2. Ejecuta CMake y compilar:
```bash
cmake .. && make
```

---
		
##  Ejecución

Una vez compilado, encontrarás dos ejecutables en el directorio 
```bash build/src/app ```

- **cliente**: El ejecutable del cliente (necesita como argumento la IP del servidor)
- **servidor**: Ejecutable del servidor
		
**¡Importante!** Añadir/crear un fichero "usuarios.txt" en el mismo directorio donde se 	
encuentra el ejecutable del servidor (se incluye un fichero de prueba en el directorio raíz del proyecto)
	
---

##  Estructura proyecto

```bash
HundirFlota
├── build
│   └── src
│       └── app
│           ├── cliente
│           ├── servidor
│           └── usuarios.txt
├── CMakeLists.txt
├── README.md
├── src
│   ├── app
│   │   ├── cliente.c
│   │   ├── CMakeLists.txt
│   │   └── servidor.c
│   ├── cliente
│   │   ├── CMakeLists.txt
│   │   ├── funcionesCliente.c
│   │   └── funcionesCliente.h
│   ├── CMakeLists.txt
│   ├── include
│   │   ├── barco.h
│   │   ├── CMakeLists.txt
│   │   ├── macros.h
│   │   ├── partida.h
│   │   └── usuario.h
│   └── servidor
│       ├── CMakeLists.txt
│       ├── funcionesServidor.c
│       └── funcionesServidor.h
└── usuarios.txt
```	
