# Shader Básico

Este shader básico es fundamental para entender cómo funciona el pipeline de renderizado en OpenGL. Se compone de dos etapas: el **vertex shader** y el **fragment shader**.

---

## ¿Qué hace este shader?

Su objetivo es el más fundamental posible: **situar una geometría en pantalla con un color sólido**. Sin texturas, sin iluminación, sin efectos. Solo posición y color.

---

## Vertex Shader (`basic.vert`)

El vertex shader se ejecuta **una vez por cada vértice** de la malla. Su misión es decirle a la GPU dónde debe aparecer ese vértice en la pantalla.

```glsl
layout(location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
```

### Variables de entrada

| Variable | Tipo | Descripción |
|---|---|---|
| `aPosition` | `in vec3` | Coordenadas en Model Space del vétice |
| `uModel` | `uniform mat4` | Matriz de transformación Model |
| `uView` | `uniform mat4` | Matriz de transformación View |
| `uProjection` | `uniform mat4` | Matriz de transformación Projection |


### ¿Cómo llega un vértice a la pantalla?

Un vértice recorre tres espacios de coordenadas antes de llegar a la pantalla. Cada transformación se aplica mediante una matriz 4×4:

```
Model Space → [uModel] → World Space → [uView] → View Space → [uProjection] → Clip Space
```

| Espacio | Qué representa |
|---|---|
| **Model Space** | Coordenadas locales del objeto, tal y como vienen del archivo 3D. |
| **World Space** | Posición del objeto dentro de la escena (traslación, rotación, escala). |
| **View Space** | La escena vista desde la cámara. El eje -Z apunta hacia la escena. |
| **Clip Space** | Sistema de coordenadas que la GPU usa para recortar y proyectar en pantalla. |

La multiplicación se lee **de derecha a izquierda**: primero se aplica `uModel`, luego `uView` y por último `uProjection`.

### Salida: `gl_Position`

`gl_Position` es la variable de salida reservada de GLSL. Asignarle el vértice transformado es lo que indica a la GPU dónde dibujarlo. Si no se escribe, el vértice no aparece.

---

## Fragment Shader (`basic.frag`)

El fragment shader se ejecuta **una vez por cada fragmento** (píxel candidato) que cubre la geometría tras la rasterización. Su misión es calcular el color final de ese píxel.

```glsl
layout(location = 0) out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
```

### Variables de entrada

| Variable | Tipo | Descripción |
|---|---|---|
| `uColor` | `uniform vec3` | Color RGB que llega desde la aplicación C++. |

### Salida: `FragColor`

Color RGBA final del fragmento que se escribe en el framebuffer.

El cuarto componente del `vec4` de salida es el canal **alpha** (opacidad). Al fijarlo en `1.0` el objeto se dibuja completamente opaco.

---


## Uniforms que hay que enviar desde C++

| Uniform | Tipo GLSL | Función OpenGL |
|---|---|---|
| `uModel` | `mat4` | `glUniformMatrix4fv` |
| `uView` | `mat4` | `glUniformMatrix4fv` |
| `uProjection` | `mat4` | `glUniformMatrix4fv` |
| `uColor` | `vec3` | `glUniform3f` |

---

## Limitaciones de este shader

Este shader es deliberadamente minimalista. No implementa:

- **Iluminación**: todos los polígonos se ven con el mismo color independientemente de la luz.
- **Texturas**: el color es uniforme para toda la malla.
- **Interpolación de color por vértice**: todos los fragmentos reciben el mismo `uColor`.
