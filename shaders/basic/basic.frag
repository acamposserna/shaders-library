#version 460 core

/**
 * Shader Básico
 *
 * Aplica el color.
 */

// Color del fragmento
layout(location = 0) out vec4 FragColor;

// Color de entrada
uniform vec3 uColor;

void main() {
    // Aplicamos el color al fragmento.
    FragColor = vec4(uColor, 1.0);
}
