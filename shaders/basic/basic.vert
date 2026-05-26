#version 460 core

/**
 * Shader Básico
 *
 * Convierte las coordenadas del modelo (locales)
 * a las cordenadas de clip mediante las matrices
 * de transformación Model, View y Projection
 */ 

// Posición del vértice en model space (local)
layout(location = 0) in vec3 aPosition;

// Matriz Model → World
// Lleva los vértices de model space a world space
uniform mat4 uModel;

// Matriz World → View
// Lleva los vértices de model space a view space (camara)
uniform mat4 uView;

// Matriz de proyección perspectiva/ortográfica
// Lleva de view space a clip space
uniform mat4 uProjection;

void main() {
    // Transformamos a clip space para la rasterización.
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}