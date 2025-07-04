# ⚔️ Pokémon Battle System

Simulador de combates Pokémon desarrollado en C++ con interfaz gráfica, inspirado en la primera generación de Pokémon y utilizando SFML.

## Descarga e instalación

1. **Clona el repositorio**
   ```bash
   git clone https://github.com/Assured360/pkmnBattle.git
   cd pkmnBattle
   ```

2. **Instala las dependencias**
   - Descarga e instala [SFML 2.5.0](https://www.sfml-dev.org/download.php) y configúralo según tu IDE.
   - Asegúrate de tener un compilador de C++ compatible (Visual Studio 2019+ recomendado).

3. **Compila el proyecto**
   - Abre el proyecto en Visual Studio o tu IDE favorito.
   - Configura las rutas de inclusión y enlace para SFML (consulta la documentación de SFML si tienes dudas).
   - Compila la solución/proyecto.

4. **Ejecuta el simulador**
   - Tras compilar, ejecuta el archivo generado.  
   - Si tienes problemas con DLLs de SFML, asegúrate de copiarlas en la misma carpeta del ejecutable.

## Estructura del proyecto

```cpp
PokemonBattle.cpp    // Punto de entrada y loop principal
├── Pokedex.cpp      // Base de datos de Pokémon
├── combate.cpp      // Motor de combate
├── PCMenu.cpp       // Selección de equipos
└── StartScreen.cpp  // Pantalla inicial
```

## Tecnologías

- **C++17**
- **SFML 2.5.x**
- **Visual Studio 2019+** (o cualquier IDE/CMake compatible)
- **Pixel Art estilo Pokémon**

## Notas

- El juego está diseñado para Windows 10 o superior.
- Resolución mínima recomendada: 800x600 (el escalado es automático).
- Si tienes dudas o encuentras errores, puedes abrir un issue en este repositorio.

---

¡Disfruta tu experiencia de combate Pokémon!
