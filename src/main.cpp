/**
 * @brief Programa básico en OpenGL para probar shaders.
 * 
 * @details
 * Este programa se utiliza para probar los distintos shaders que
 * se aplica a un cuadrado naranja.
 * El shader a probar se pasa por parámetro.
 * 
 * @author Antonio Campos (a.campos.serna@gmail.com)
 * @version 0.1
 * @date 2026-05-28
 * @copyright GNU General Public License v3 (see https://www.gnu.org/licenses/gpl-3.0.html)
 */


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>   // glm::value_ptr → puntero raw para uniform

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>

#include <Logger/Logger.hpp>

/**
 * CONSTANTES
 */

// Tamaño de la ventana 
static constexpr int   kWidth    = 800;
static constexpr int   kHeight   = 600;

// Ubicación y extensiones de los shaders
static constexpr std::string    kShadersPath = "shaders/";
static constexpr std::string    kVertExt     = ".vert";
static constexpr std::string    kFragExt     = ".frag";

/**
 * VARIABLES GLOBALES
 */

// Logger de consola
static Logger console_log(LogLevel::TRACE, LogMode::CONSOLE, "");

/**
 * Geometría del cuadrado
 */

// Los cuatro vértices del cuadrado 
static constexpr float kVertices[] = {
     0.5f,  0.5f, 0.0f,  // Arriba a la derecha
     0.5f, -0.5f, 0.0f,  // Abajo a la derecha
    -0.5f, -0.5f, 0.0f,  // Abajo a la izquierda
    -0.5f,  0.5f, 0.0f   // Arriba a la izquierda
};

// Se definien los indice de los dos triángulos que forman el cuadrado
static constexpr unsigned int kIndices[] = {
    0, 1, 3,  // Primer triángulo
    1, 2, 3   // Segundo triángulo
};


/**
 * Utilidades para la gestión de los shaders:
 * 
 * 1. readFile: Leer fichero de texto con el código del shader.
 * 2. compileShader: Compila el shader.
 * 3. createProgram: Enlaza los shaders compilados y crea el programa para la GPU.
 */

/**
 * @brief Leer fichero de texto.
 * 
 * @details
 * Lee el fichero de texto que se le pasa por parámetro y devuelve todo el contenido en
 * una cadena <code>std::string</code>.
 * 
 * Si se produce un error en la lectura del fichero devuelve un objeto vacío.
 * 
 * @param path Fichero de texto para leer.
 * @return std::string Contenido del fichero.
 */
static std::string readFile(const char* path) {
    std::ifstream f(path);
    if (!f) {
        console_log.log(LogLevel::ERROR, "Error abriendo el fichero: " + std::string(path));
        return {};
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

/**
 * @brief Compilación de un shader.
 * 
 * @details
 * Se lee el código del shader del fichero que se pasa por parámetro y se compila.
 * 
 * Se comprueba si ha habido algún error en la copilación con <code>glGetShaderiv</code>.
 * 
 * @param type Tipo del shader GL_VERTEX_SHADER o GL_FRAGMENT_SHADER.
 * @param path Fichero con el código del shader.
 * @return GLuint Identificador del shader compilado.
 */
static GLuint compileShader(GLenum type, const char* path) {
    std::string src  = readFile(path);
    const char* csrc = src.c_str();

    GLuint shader = glCreateShader(type);      // reserva objeto en la GPU
    glShaderSource(shader, 1, &csrc, nullptr); // carga el código fuente
    glCompileShader(shader);                   // compila en la GPU

    // Comprobamos errores de compilación
    int ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        console_log.log(LogLevel::ERROR, "Error en el shader ("
            + std::string(path) + "): "
            + std::string(log)
        );
    }
    return shader;
}

/**
 * @brief Crea el programa para ejecutar en la GPU.
 * 
 * @details
 * Enlaza vertex shader + fragment shader en un programa ejecutable por la GPU.
 * 
 * Tras el enlazado, los objetos shader individuales ya no son necesarios.
 * 
 * @param vertPath Fichero con el código fuente del vertex shader.
 * @param fragPath Fichero con el código fuente del fragment shader.
 * @return GLuint ID del programa.
 */
static GLuint createProgram(const char* vertPath, const char* fragPath) {
    GLuint vert = compileShader(GL_VERTEX_SHADER,   vertPath);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragPath);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);   // conecta salidas del vert con entradas del frag

    int ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        console_log.log(LogLevel::ERROR, "Error en el linkado: " + std::string(log));
    }

    // Los shaders ya están linkados en el programa; liberamos los objetos.
    glDeleteShader(vert);
    glDeleteShader(frag);

    return prog;
}


int main(int argc, char* argv[]) {

    console_log.log(LogLevel::INFO, "Inicio del programa.");

    // Comprobación de parámetros
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <nombre_shader>\n";
        return -1;
    }

    // Path de los shaders (vertex y fragment)
    std::string pathShader = kShadersPath + argv[1] + "/";
    std::string pathVertexShader = pathShader + argv[1] + kVertExt;
    std::string pathFragmentShader = pathShader + argv[1] + kFragExt;  
 
    // Inicializamos GLFW
    if (!glfwInit()) {
        console_log.log(LogLevel::ERROR, "Error inicializando GLFW.");
        return -1;
    }

    // Creamos el contexto OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creamos la ventana
    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Shaders",
                                          nullptr, nullptr);   

    if (!window) {
        console_log.log(LogLevel::ERROR, "Error creando la ventana de GLFW.");
        glfwTerminate();
        return -1;
    }

    // Hacemos que las llamadas OpenGL siguientes operen sobre esta ventana
    glfwMakeContextCurrent(window);

    // Inicializamos GLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        console_log.log(LogLevel::ERROR, "Error inicializando GLAD.");
        return -1;
    }

    // Área de dibujo para OpenGL
    glViewport(0, 0, kWidth, kHeight);


    // Crear el programa para la GPU. Carga, compila y linka los shaders.
    GLuint program = createProgram(pathVertexShader.c_str(),
                                   pathFragmentShader.c_str());

    // Crear y configurar VBO / EBO / VAO
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Vinculamos el VAO. A partir de aquí "recuerda" todo lo que hacemos.
    glBindVertexArray(vao);

    // Cargamos los vértices en la VRAM (GL_STATIC_DRAW = datos inmutables).
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);

    // Cargamos los índices en la VRAM. El EBO queda vinculado al VAO.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);

    // El vértice tiene este layout en memoria (stride = 6 floats = 24 B):
    //   [x  y  z
    //    └──┬──┘
    //   aPos(loc=0)
    //   offset 0

    constexpr GLsizei stride = 3 * sizeof(float);

    // Atributo 0 -> aPos: 3 floats, offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Desvinculamos el VAO para evitar modificaciones accidentales.
    glBindVertexArray(0);

    // Las matrices Model, View y Projection son unitarias y constantes para
    // toda la ejecución
    glm::mat4 model      = glm::mat4(1.0f);
    glm::mat4 view       = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Color naranja
    glm::vec3 color      = glm::vec3(1.0f, 0.5f, 0.0f);

    // Obtenemos la localizacion de las variables "uniform"
    GLint uModel         = glGetUniformLocation(program, "uModel");
    GLint uView          = glGetUniformLocation(program, "uView");
    GLint uProjection    = glGetUniformLocation(program, "uProjection");
    GLint uColor         = glGetUniformLocation(program, "uColor");

    // Bucle principal de renderizado
    while (!glfwWindowShouldClose(window)) {

        // Limpiamos el color buffer (fondo gris oscuro) y el depth buffer.
        glClearColor(0.15f, 0.15f, 0.15f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Envío de matrices a los shaders. Se envían en los "uniforms".
        glUseProgram(program);
        glUniformMatrix4fv(uModel,      1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uView,       1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(uColor,            1, glm::value_ptr(color));

        // Dibujamos el cuadrado
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // Intercambio de buffers de visualización
        glfwSwapBuffers(window);

        // Procesamiento de eventos del S.O.
        glfwPollEvents();
    }

    // Limpiamos de recursos
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(program);

    // Cerramos la ventana
    glfwDestroyWindow(window);
    glfwTerminate();

    // Fin del programa
    console_log.log(LogLevel::INFO, "Fin del programa.");
    return 0;
}