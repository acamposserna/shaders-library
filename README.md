# Biblioteca de Shaders

## Introducción

Este proyecto es una biblioteca de shaders programados en GLSL. Se incluyen desde el shader más básico a otros más complejos, todos ellos programados con fines didácticos. 

El objetivo principal es comprender el funcionamiento de los shaders dentro del pipeline de renderizado, como se comunican los datos de las geometría entre la CPU y la GPU y cómo los shaders son necesarios para producir los píxeles que se muestran en la pantalla.

También se incluye en este proyecto una pequeña aplicación que renderiza un cubo multiculor y que se puede utilizar para probar el efecto que producen los distintos shaders en dicho cubo.

---

## Stack Tecnológico

Para la programación de shaders utilizamos el lenguaje [GLSL 4.60](https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.html).

El stack tecnológico completo es el siguiente:

| Componente          | Tecnología       | Rol en el proyecto                                              |
|---------------------|------------------|-----------------------------------------------------------------|
| Lenguaje            | C++23            | Lógica de la aplicación, gestión de recursos                    |
| API gráfica         | OpenGL 4.6       | Comunicación con la GPU                                         |
| Lenguaje shaders    | GLSL 4.60        | Programas que se ejecutan en la GPU |
| Ventana y contexto  | GLFW             | Creación de ventana multiplataforma y contexto OpenGL           |
| Carga de extensiones| GLAD             | Resolución en tiempo de ejecución de las funciones de OpenGL    |
| Matemáticas         | GLM              | Vectores y matrices compatibles con GLSL (column-major)         |
| Sistema de build    |  Make            | Configuración y compilación                                     |

---
## ¿Qué es un Shader?

Un **shader** es un programa que se ejecuta directamente en la GPU (*Graphics Processing Unit*) dentro del pipeline de renderizado. A diferencia del código que corre en la CPU, los shaders operan de forma masivamente paralela: la GPU lanza miles de instancias simultáneas del mismo programa, cada una procesando un vértice, un fragmento u otro tipo de primitiva de forma independiente.

Para saber más sobre Shaders puedes consultar el fichero [SHADERS.md](SHADERS.md).

## Listado de Shaders

| Nombre | Tipo(s) | Descripción |
|--------|---------|-------------|
| [Básico](/shaders/basic/BASIC.md) | vertex y fragment | Posiciona cada vertice con su color en coordenadas de clip |

