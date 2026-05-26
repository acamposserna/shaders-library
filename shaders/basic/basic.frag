#version 460 core

/**
 * Shader Básico
 *
 * Aplica el color.
 */

// Color
uniform vec3 color;

void main() {
    // Aplicamos el color al fragmento.
    gl_FragColor = vec4(color, 1.0);
}
