#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include "pokedex.h"

// Declaraci�n de la funci�n de combate
void mostrarCombate(const std::vector<int>& equipoJugador, const std::vector<int>& equipoRival);
void testCombate();

// Estructura para manejar sprites animados de Pok�mon
struct MiniaturaPokemon {
    sf::Sprite sprite;
    sf::IntRect frame1;  // Primer frame de animaci�n
    sf::IntRect frame2;  // Segundo frame de animaci�n
    float animTimer = 0.f;
    bool showingFrame1 = true;
    int index; // �ndice del Pok�mon (1 a 151)
};

// Estructura para slots del equipo del jugador
struct EquipoSlot {
    bool ocupado = false;
    int indexPokemon = -1;
};

// Coordenadas predefinidas para extraer sprites del spritesheet
const std::vector<std::pair<int, int>> coordenadasColumnas = {
    {0, 34}, {134, 168}, {268, 302}, {402, 436}, {536, 570},
    {670, 704}, {804, 838}, {938, 972}, {1071, 1105}, {1205, 1239},
    {1339, 1373}, {1473, 1508}, {1608, 1641}, {1741, 1775}, {1875, 1909}
};

const std::vector<int> coordenadasFilas = {
    0, 168, 337, 505, 674, 842, 1011, 1179, 1347, 1516, 1684
};

// Carga todas las miniaturas de Pokémon desde el spritesheet
std::vector<MiniaturaPokemon> cargarMiniaturas(sf::Texture& texturePokemon, float escala, float offsetX, float offsetY, float espacioX, float espacioY) {
    std::vector<MiniaturaPokemon> miniaturas;
    const int spriteWidth = 32;
    const int spriteHeight = 32;
    int totalPokemons = 151;
    int columnas = 15;

    for (int i = 0; i < totalPokemons; ++i) {
        int fila = i / columnas;
        int columna = i % columnas;

        if (fila >= (int)coordenadasFilas.size() || columna >= (int)coordenadasColumnas.size())
            continue;

        MiniaturaPokemon m;
        m.sprite.setTexture(texturePokemon);

        // Configurar los dos frames para la animación
        int x1 = coordenadasColumnas[columna].first;
        int x2 = coordenadasColumnas[columna].second;
        int y = coordenadasFilas[fila];

        m.frame1 = sf::IntRect(x1, y, spriteWidth, spriteHeight);
        m.frame2 = sf::IntRect(x2, y, spriteWidth, spriteHeight);
        m.sprite.setTextureRect(m.frame1);

        // Posicionar sprite en pantalla de forma proporcional
        m.sprite.setScale(escala, escala);
        m.sprite.setPosition(offsetX + columna * (spriteWidth * escala + espacioX), offsetY + fila * (spriteHeight * escala + espacioY));
        m.index = i + 1; // Índices van de 1 a 151
        miniaturas.push_back(m);
    }

    return miniaturas;
}

// Funci�n para dibujar texto usando spritesheet de letras
void dibujarNombre(sf::RenderWindow& window, sf::Texture& letrasTexture, const std::string& nombre, float x, float y) {
    // Coordenadas x para may�sculas A-Z
    const int coordsMayus[26] = { 171, 178, 185, 192, 199, 206, 213, 220, 226, 233, 240, 247, 254, 261, 268, 275, 282, 289, 296, 302, 309, 316, 323, 330, 337, 343 };

    // Coordenadas x para min�sculas a-z
    const int coordsMinus[26] = { 170, 177, 184, 192, 199, 206, 213, 220, 226, 231, 238, 246, 253, 260, 267, 274, 280, 287, 294, 301, 308, 315, 322, 329, 336, 343 };

    float posX = x;
    for (size_t i = 0; i < nombre.size(); ++i) {
        char c = nombre[i];
        if (!isalpha(c)) continue;

        bool mayus = isupper(c);
        int letraIndex = toupper(c) - 'A';

        // Verificar que el �ndice est� en rango v�lido
        if (letraIndex < 0 || letraIndex > 25) continue;

        int letraX = mayus ? coordsMayus[letraIndex] : coordsMinus[letraIndex];
        int letraY = mayus ? 123 : 142;
        int letraHeight = mayus ? 9 : 10;

        // Verificar l�mites del spritesheet
        sf::Vector2u textureSize = letrasTexture.getSize();
        if (letraX + 5 > (int)textureSize.x || letraY + letraHeight > (int)textureSize.y) {
            continue;
        }

        sf::Sprite letra(letrasTexture, sf::IntRect(letraX, letraY, 5, letraHeight));
        letra.setScale(2.0f, 2.0f);
        letra.setPosition(posX, y);
        window.draw(letra);

        // Avanzar posici�n para la siguiente letra
        posX += 5 * 2.0f + 3.f;
    }
}

// Obtiene el nombre del Pok�mon desde la pok�dex
inline std::string getPokemonName(int index) {
    if (index >= 1 && index <= static_cast<int>(pokedex.size())) {
        return pokedex[index - 1].nombre;
    }
    else {
        return "???";
    }
}

// Genera equipo rival con Pok�mon legendarios (IDs 130-151)
std::vector<int> generarEquipoRival() {
    std::vector<int> pokemonsFuertes;
    // Llenar con Pok�mon legendarios y muy fuertes
    for (int i = 130; i <= 151; ++i) {
        pokemonsFuertes.push_back(i);
    }

    // Mezclar aleatoriamente
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(pokemonsFuertes.begin(), pokemonsFuertes.end(), gen);

    // Seleccionar 6 Pok�mon
    std::vector<int> equipoRival(pokemonsFuertes.begin(), pokemonsFuertes.begin() + 6);
    return equipoRival;
}

// Verifica si el equipo tiene 6 Pok�mon
bool equipoCompleto(const std::vector<EquipoSlot>& equipo) {
    int contador = 0;
    for (const auto& slot : equipo) {
        if (slot.ocupado) contador++;
    }
    return contador == 6;
}

// Convierte los slots del equipo a vector de �ndices
std::vector<int> obtenerEquipoJugador(const std::vector<EquipoSlot>& equipo) {
    std::vector<int> equipoJugador;
    for (const auto& slot : equipo) {
        if (slot.ocupado) {
            equipoJugador.push_back(slot.indexPokemon);
        }
    }
    return equipoJugador;
}

// Verifica si un Pok�mon ya est� en el equipo
bool pokemonYaEnEquipo(const std::vector<EquipoSlot>& equipo, int indexPokemon) {
    for (const auto& slot : equipo) {
        if (slot.ocupado && slot.indexPokemon == indexPokemon) {
            return true;
        }
    }
    return false;
}

void inicializarPokedex();

// Funci�n principal del men� de selecci�n de Pok�mon
void mostrarPCMenu() {
    // Configurar ventana en pantalla completa
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "PC Pokemon", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    // Cargar texturas necesarias
    sf::Texture texturePokemons, textureUI, textureUIBTT;
    if (!texturePokemons.loadFromFile("C:\\Proyectos\\PokemonBattle\\x64\\Debug\\resources\\sprites\\pc_pokemon.png") ||
        !textureUI.loadFromFile("C:\\Proyectos\\PokemonBattle\\x64\\Debug\\resources\\sprites\\Interfaces.png") ||
        !textureUIBTT.loadFromFile("C:\\Proyectos\\PokemonBattle\\x64\\Debug\\resources\\sprites\\uibtt.png")) {
        std::cerr << "Error cargando texturas\n";
        return;
    }

    // Configurar fondo escalado a pantalla completa
    sf::Sprite fondo(textureUI, sf::IntRect(246, 1, 240, 160));
    sf::Vector2u screenSize = window.getSize();
    fondo.setScale((float)screenSize.x / 240.f, (float)screenSize.y / 160.f);

    // Calcular escala y posiciones proporcionales para miniaturas
    const int spriteWidth = 32;
    const int spriteHeight = 32;
    int columnas = 15;
    int filas = (151 + columnas - 1) / columnas;
    float margenX = screenSize.x * 0.04f;
    float margenY = screenSize.y * 0.04f;
    float areaWidth = screenSize.x * 0.7f;
    float areaHeight = screenSize.y * 0.85f;
    float escalaMini = std::min(
        (areaWidth / columnas - 10) / spriteWidth,
        (areaHeight / filas - 10) / spriteHeight
    );
    float espacioX = 10.f * escalaMini;
    float espacioY = 10.f * escalaMini;

    // Cargar miniaturas de Pokémon y configurar cursor
    auto miniaturas = cargarMiniaturas(texturePokemons, escalaMini, margenX, margenY, espacioX, espacioY);
    sf::Sprite cursor(textureUI, sf::IntRect(512, 2, 24, 24));
    cursor.setScale(2.f * escalaMini, 2.f * escalaMini);

    // Configurar slots del equipo (6 slots) de forma proporcional
    std::vector<EquipoSlot> equipo(6);
    float slotsAreaX = screenSize.x * 0.8f;
    float slotsAreaY = screenSize.y * 0.15f;
    float espacioVertical = (screenSize.y * 0.7f) / 6.f;
    sf::Vector2f posSlots(slotsAreaX, slotsAreaY);

    // Flecha para iniciar combate (visible solo con equipo completo)
    sf::Sprite flecha(textureUI, sf::IntRect(549, 9, 10, 14));
    float escalaFlecha = escalaMini * 2.5f;
    flecha.setScale(escalaFlecha, escalaFlecha);
    sf::Vector2f posFlecha(posSlots.x + 15.f, posSlots.y + 6 * espacioVertical + 10.f * escalaMini);
    flecha.setPosition(posFlecha);

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Cerrar ventana con ESC o bot�n cerrar
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();

            // CLIC IZQUIERDO: Seleccionar Pok�mon o iniciar combate
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mouse((float)event.mouseButton.x, (float)event.mouseButton.y);

                // Verificar clic en flecha de combate
                if (equipoCompleto(equipo) && flecha.getGlobalBounds().contains(mouse)) {
                    // Generar equipos y mostrar informaci�n
                    std::vector<int> equipoRival = generarEquipoRival();
                    std::vector<int> equipoJugador = obtenerEquipoJugador(equipo);

                    std::cout << "Equipo jugador seleccionado:\n";
                    for (int id : equipoJugador) std::cout << id << " ";
                    std::cout << "\n";

                    std::cout << "Equipo rival generado:\n";
                    for (int id : equipoRival) std::cout << id << " ";
                    std::cout << "\n";

                    // Cerrar ventana e iniciar combate
                    window.close();
                    mostrarCombate(equipoJugador, equipoRival);
                    return;
                }

                // Verificar clic en Pok�mon para agregar al equipo
                for (const auto& m : miniaturas) {
                    if (m.sprite.getGlobalBounds().contains(mouse)) {
                        // Solo agregar si hay espacio y no est� duplicado
                        if (!equipoCompleto(equipo) && !pokemonYaEnEquipo(equipo, m.index)) {
                            for (auto& slot : equipo) {
                                if (!slot.ocupado) {
                                    slot.ocupado = true;
                                    slot.indexPokemon = m.index;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }

            // CLIC DERECHO: Remover Pok�mon del equipo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                sf::Vector2f mouse((float)event.mouseButton.x, (float)event.mouseButton.y);

                // Verificar clic en slots del equipo (área más precisa)
                for (int i = 0; i < 6; ++i) {
                    float y = posSlots.y + i * espacioVertical;
                    sf::FloatRect slotBounds(posSlots.x - 5.f, y - 5.f, 250.f * escalaMini, espacioVertical);
                    if (slotBounds.contains(mouse) && equipo[i].ocupado) {
                        equipo[i].ocupado = false;
                        equipo[i].indexPokemon = -1;
                        break;
                    }
                }
            }
        }

        // Actualizar animaciones de Pok�mon
        float delta = clock.restart().asSeconds();
        for (auto& m : miniaturas) {
            m.animTimer += delta;
            if (m.animTimer >= 0.5f) {
                m.animTimer = 0.f;
                m.showingFrame1 = !m.showingFrame1;
                m.sprite.setTextureRect(m.showingFrame1 ? m.frame1 : m.frame2);
            }
        }

        // Actualizar posici�n del cursor
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        cursor.setPosition((float)mousePos.x, (float)mousePos.y);

        // RENDERIZADO
        window.clear();
        window.draw(fondo);

        // Dibujar todas las miniaturas de Pok�mon
        for (const auto& m : miniaturas) {
            window.draw(m.sprite);
        }

        // Dibujar slots del equipo
        for (int i = 0; i < 6; ++i) {
            float y = posSlots.y + i * espacioVertical;
            if (equipo[i].ocupado) {
                sf::Sprite pokebola(textureUIBTT, sf::IntRect(132, 64, 9, 9));
                pokebola.setScale(3.f * escalaMini, 3.f * escalaMini);
                pokebola.setPosition(posSlots.x, y);
                window.draw(pokebola);
            }
            else {
                sf::Sprite circulo(textureUIBTT, sf::IntRect(122, 64, 9, 9));
                circulo.setScale(3.f * escalaMini, 3.f * escalaMini);
                circulo.setPosition(posSlots.x, y);
                window.draw(circulo);
            }
        }

        // Dibujar nombres de Pokémon en el equipo
        for (int i = 0; i < 6; ++i) {
            float y = posSlots.y + i * espacioVertical;
            if (equipo[i].ocupado) {
                std::string nombre = getPokemonName(equipo[i].indexPokemon);
                dibujarNombre(window, textureUIBTT, nombre, posSlots.x + 50.f * escalaMini, y);
            }
        }

        // Dibujar flecha de combate si el equipo está completo
        if (equipoCompleto(equipo)) {
            window.draw(flecha);
        }

        // Dibujar cursor del mouse
        window.draw(cursor);
        window.display();
    }
}