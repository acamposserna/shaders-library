# Shaders en OpenGL

## Definición de Shader

Un **shader** es un programa que se ejecuta directamente en la GPU (*Graphics Processing Unit*) dentro del pipeline de renderizado. A diferencia del código que corre en la CPU, los shaders operan de forma masivamente paralela: la GPU lanza miles de instancias simultáneas del mismo programa, cada una procesando un vértice, un fragmento u otro tipo de primitiva de forma independiente.

El término *shader* tiene origen histórico en el cálculo de iluminación y sombreado (*shading*), pero en la GPU moderna su alcance es mucho más amplio: abarcan cualquier computación por etapa del pipeline, desde la transformación de geometría hasta el cálculo de color final.

### Etapas programables en el canal de renderizado

El pipeline gráfico de OpenGL combina etapas **fijas** (configurables pero no programables) con etapas **programables** mediante shaders. A continuación se describen las etapas principales en orden de ejecución:

| Etapa | Tipo | Descripción |
|---|---|---|
| **Vertex Shader** | Programable | Transforma vértices de espacio objeto a espacio de clip |
| **Tessellation Control Shader** | Programable (opcional) | Controla el nivel de subdivisión de parches (*patches*) |
| **Tessellation Evaluation Shader** | Programable (opcional) | Calcula la posición de los nuevos vértices generados |
| **Geometry Shader** | Programable (opcional) | Opera sobre primitivas completas; puede generar o descartar geometría |
| **Rasterización** | Fija | Convierte primitivas vectoriales en fragmentos discretos; realiza interpolación de atributos |
| **Fragment Shader** | Programable | Determina el color de cada fragmento |
| **Per-Fragment Operations** | Fija | Pruebas de profundidad (*depth test*), *stencil test*, mezcla (*blending*), etc. |

La siguiente figura resume el flujo de datos entre etapas:

```
Vértices (CPU)
     │
     ▼
[Vertex Shader]
     │
     ▼
[Tessellation Control Shader]  ← opcional
     │
     ▼
[Tessellation Evaluation Shader] ← opcional
     │
     ▼
[Geometry Shader]  ← opcional
     │
     ▼
[Rasterización]  ← etapa fija
     │
     ▼
[Fragment Shader]
     │
     ▼
[Per-Fragment Operations]  ← etapa fija
     │
     ▼
Framebuffer
```

> Las únicas etapas obligatorias para un programa OpenGL funcional son el **Vertex Shader** y el **Fragment Shader**. El resto son opcionales y se activan según las necesidades del renderizado.

---

### Lenguaje GLSL

**GLSL** (*OpenGL Shading Language*) es el lenguaje de programación oficial para shaders en OpenGL. Fue introducido en OpenGL 2.0 (2004) y ha evolucionado junto con la especificación de OpenGL. Está definido en la especificación oficial de Khronos Group.

#### Características generales

- **Sintaxis similar a C**: GLSL hereda la sintaxis de C (bucles, condicionales, funciones, punteros excluidos), lo que reduce la curva de aprendizaje para programadores familiarizados con dicho lenguaje.
- **Tipado estático y fuerte**: todas las variables deben declararse con un tipo explícito.
- **Compilación en tiempo de ejecución**: los shaders se compilan por el driver de la GPU cuando la aplicación se ejecuta, a través de las llamadas `glShaderSource`, `glCompileShader` y `glLinkProgram`.

#### Tipos de datos principales

| Tipo | Descripción |
|---|---|
| `float`, `int`, `uint`, `bool` | Escalares primitivos |
| `vec2`, `vec3`, `vec4` | Vectores de floats de 2, 3 y 4 componentes |
| `ivec2/3/4`, `uvec2/3/4`, `bvec2/3/4` | Vectores de enteros, sin signo y booleanos |
| `mat2`, `mat3`, `mat4` | Matrices cuadradas de floats (column-major) |
| `sampler2D`, `samplerCube`, `sampler3D` | Manejadores de texturas 2D, cúbicas y 3D |

#### Calificadores de variables

Los calificadores (*qualifiers*) determinan desde dónde provienen y hacia dónde van los datos de una variable:

| Calificador | Descripción |
|---|---|
| `in` | Dato de entrada a la etapa actual (por vértice o por fragmento) |
| `out` | Dato de salida de la etapa actual, enviado a la siguiente |
| `uniform` | Variable global de solo lectura, igual para todas las invocaciones del shader en una llamada de dibujo; establecida desde la CPU |
| `layout(location = N)` | Especifica explícitamente el índice de atributo o de salida |

#### Funciones matemáticas integradas

GLSL proporciona un conjunto amplio de funciones matemáticas intrínsecas ejecutadas de forma nativa en hardware:

- **Trigonométricas**: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- **Exponenciales**: `pow`, `exp`, `log`, `sqrt`, `inversesqrt`
- **Vectoriales**: `dot`, `cross`, `normalize`, `length`, `distance`, `reflect`, `refract`
- **Interpolación**: `mix` (interpolación lineal), `smoothstep`, `step`, `clamp`
- **Textura**: `texture`, `textureLod`, `textureGrad`

#### Declaración de versión

Todo shader GLSL debe comenzar con una directiva de versión:

```glsl
#version 460 core
```

El número corresponde a la versión de GLSL (460 → GLSL 4.60, asociada a OpenGL 4.6). La palabra clave `core` indica que se usa el perfil *core* de OpenGL, sin funcionalidad *deprecated*.

---

## Principales tipos de Shaders y en qué etapa se utilizan

### Vertex Shader

El **Vertex Shader** es la primera etapa programable del pipeline. Se ejecuta **una vez por cada vértice** que llega al pipeline y su responsabilidad principal es transformar las coordenadas de cada vértice desde el **espacio del objeto** (*object space* u *model space*) hasta el **espacio de clip** (*clip space*).

#### Cadena de transformaciones

La transformación completa de un vértice se expresa habitualmente como la composición de tres matrices:

$$
\mathbf{p}_{\text{clip}} = \mathbf{M}_{\text{proj}} \cdot \mathbf{M}_{\text{view}} \cdot \mathbf{M}_{\text{model}} \cdot \mathbf{p}_{\text{object}}
$$

Donde:

| Símbolo | Nombre | Descripción |
|---|---|---|
| $\mathbf{p}_{\text{object}}$ | Posición en espacio objeto | Coordenada original del vértice tal como se definió en el modelo |
| $\mathbf{M}_{\text{model}}$ | Matriz de modelo (*model matrix*) | Transforma de espacio objeto a **espacio mundo** (*world space*); codifica traslación, rotación y escala del objeto |
| $\mathbf{M}_{\text{view}}$ | Matriz de vista (*view matrix*) | Transforma de espacio mundo a **espacio cámara** (*camera/eye space*); simula la posición y orientación del observador |
| $\mathbf{M}_{\text{proj}}$ | Matriz de proyección (*projection matrix*) | Transforma de espacio cámara a **espacio de clip**; aplica perspectiva u ortografía |
| $\mathbf{p}_{\text{clip}}$ | Posición en espacio de clip | Coordenadas homogéneas 4D antes de la división de perspectiva |

Esta composición se conoce habitualmente como la **transformación MVP** (*Model-View-Projection*).

Tras el Vertex Shader, el hardware realiza automáticamente la **división de perspectiva** (*perspective divide*):

$$
\mathbf{p}_{\text{NDC}} = \frac{1}{w} \cdot (x, y, z)
$$

obteniendo las **Coordenadas de Dispositivo Normalizadas** (*Normalized Device Coordinates*, NDC), en el rango $[-1, 1]^3$ para las tres componentes.

#### Otras responsabilidades habituales del Vertex Shader

Además de la transformación de posición, el Vertex Shader puede:
- Transformar **normales** al espacio necesario para el cálculo de iluminación.
- Pasar **coordenadas de textura** (*texture coordinates* o *UV*) a etapas posteriores.
- Calcular atributos por vértice como color difuso o tangentes para *normal mapping*.

#### Ejemplo de código

```glsl
#version 460 core

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
```

> La variable de salida obligatoria de todo Vertex Shader es `gl_Position`, que debe contener las coordenadas del vértice en espacio de clip.

---

### Fragment Shader

El **Fragment Shader** (también denominado *Pixel Shader* en la terminología de DirectX) es la segunda etapa programable principal del pipeline. Se ejecuta **una vez por cada fragmento** producido por el rasterizador y tiene como responsabilidad determinar el **color final** de ese fragmento.

Un **fragmento** es un candidato a píxel: contiene información como posición en pantalla, profundidad, normales interpoladas y coordenadas UV, pero aún no ha pasado las pruebas *per-fragment* (profundidad, stencil, etc.) que determinan si será escrito al framebuffer.

#### Entradas disponibles

El Fragment Shader recibe:
- Los **atributos interpolados** procedentes del Vertex Shader (normales, UVs, posición en espacio mundo, etc.).
- Variables **uniform** accesibles globalmente (texturas, parámetros de luz, tiempo, etc.).
- Variables de sistema como `gl_FragCoord` (posición del fragmento en espacio de ventana, incluyendo profundidad) y `gl_FrontFacing` (booleano que indica si la cara es frontal).

#### Operaciones habituales

1. **Muestreo de texturas** (*texture sampling*): obtención del texel correspondiente en una o varias texturas mediante las coordenadas UV interpoladas.
2. **Cálculo de iluminación local**: aplicación de modelos como Phong, Blinn-Phong o cálculos PBR (*Physically Based Rendering*) usando la normal interpolada y la posición de la fuente de luz.
3. **Descarte de fragmento** (*discard*): instrucción que elimina el fragmento sin escribir ningún valor (útil para transparencias con *alpha testing*).

#### Ejemplo de código

```glsl
#version 460 core

// Color del fragmento
layout(location = 0) out vec4 FragColor;

// Color de entrada
uniform vec3 uColor;

void main() {
    // Aplicamos el color al fragmento.
    FragColor = vec4(uColor, 1.0);
}
```

#### Salida del Fragment Shader

La salida principal es la variable `out vec4 fragColor` (o el nombre que se declare), que contiene el color RGBA del fragmento. Esta salida pasa a las operaciones *per-fragment* de la siguiente etapa.

#### Puntos clave

- El Fragment Shader determina el color de cada fragmento de forma completamente programable.
- La precisión de la interpolación de normales es crítica para la calidad de la iluminación.
- La instrucción `discard` permite eliminación condicional de fragmentos, pero su uso excesivo puede perjudicar el rendimiento por interferir con la optimización *early-z*.

---

### Geometry Shader

El **Geometry Shader** es una etapa programable **opcional** del pipeline, situada entre el Vertex Shader (o la etapa de teselación, si está activa) y la rasterización. A diferencia del Vertex Shader, que opera sobre un único vértice, el Geometry Shader recibe como entrada una **primitiva completa** (un triángulo, una línea o un punto, junto con sus vértices adyacentes si se configuran) y puede emitir cero o más primitivas de salida.

#### Capacidades características

- **Generación de geometría**: puede emitir más vértices y primitivas de las que recibe, permitiendo por ejemplo expandir puntos en *billboards* (quads orientados a la cámara) o generar geometría procedural.
- **Eliminación de geometría**: puede decidir no emitir una primitiva, actuando como un filtro.
- **Cambio de tipo de primitiva**: puede recibir triángulos y emitir líneas, o viceversa.
- **Acceso a todos los vértices de la primitiva**: a diferencia del Vertex Shader, tiene visibilidad sobre la primitiva completa, lo que permite calcular normales de cara (*face normals*) o determinar adyacencia.

#### Tipos de primitiva de entrada y salida

| Primitiva de entrada | Vértices disponibles |
|---|---|
| `points` | 1 |
| `lines` | 2 |
| `lines_adjacency` | 4 (2 + 2 de adyacencia) |
| `triangles` | 3 |
| `triangles_adjacency` | 6 (3 + 3 de adyacencia) |

La primitiva de salida se especifica con `line_strip`, `triangle_strip` o `points`, junto con el número máximo de vértices emitidos (`max_vertices`).

#### Ejemplo de código: expansión de puntos en quads

El siguiente ejemplo toma cada punto de entrada y emite un quad (dos triángulos en *triangle strip*) centrado en dicho punto:

```glsl
#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float uSize; // Tamaño del quad en NDC

void main() {
    vec4 center = gl_in[0].gl_Position;
    float s = uSize * 0.5;

    // Vértice inferior-izquierdo
    gl_Position = center + vec4(-s, -s, 0.0, 0.0);
    EmitVertex();

    // Vértice inferior-derecho
    gl_Position = center + vec4( s, -s, 0.0, 0.0);
    EmitVertex();

    // Vértice superior-izquierdo
    gl_Position = center + vec4(-s,  s, 0.0, 0.0);
    EmitVertex();

    // Vértice superior-derecho
    gl_Position = center + vec4( s,  s, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
```

Las funciones `EmitVertex()` y `EndPrimitive()` son intrínsecas de GLSL exclusivas del Geometry Shader: la primera emite el vértice con el valor actual de `gl_Position` (y otros `out`), y la segunda finaliza la primitiva actual.

#### Consideraciones de rendimiento

El Geometry Shader introduce una **sincronización implícita** en el pipeline que puede reducir el paralelismo de la GPU. En escenarios de alto rendimiento (como expansión masiva de partículas), se prefiere frecuentemente el uso de **Compute Shaders** con *indirect drawing* o la etapa de **teselación**, que ofrecen mayor eficiencia. El Geometry Shader sigue siendo válido para efectos de baja complejidad geométrica o para renderizado en capas (*layered rendering*) hacia cubemaps.

#### Puntos clave

- Recibe primitivas completas y puede emitir un número variable de primitivas de salida.
- Es la única etapa (junto con el Tessellation Evaluation Shader) que puede cambiar el tipo de primitiva.
- Su uso excesivo puede ser un cuello de botella; se recomienda evaluar alternativas basadas en Compute Shader para volúmenes altos de geometría generada.

---

### Tessellation Shaders

La **teselación** es el proceso de subdividir una primitiva de entrada en un mayor número de triángulos más pequeños, con el objetivo de añadir detalle geométrico de forma dinámica en función de criterios como la distancia a la cámara o la curvatura de la superficie. En OpenGL 4.0 se introdujo como etapa programable a través de dos shaders complementarios.

La unidad de entrada de la teselación es el **parche** (*patch*), declarado como `GL_PATCHES`. Un parche es un conjunto arbitrario de entre 1 y 32 vértices de control que define la forma base de la superficie.

#### Tessellation Control Shader (TCS)

El **Tessellation Control Shader** (TCS) se ejecuta **una vez por cada vértice de control** del parche. Sus responsabilidades son:

1. **Calcular los niveles de teselación** (*tessellation levels*): establece cuántas subdivisiones se aplicarán a cada borde exterior (`gl_TessLevelOuter[]`) y al interior (`gl_TessLevelInner[]`) del parche.
2. **Transformar o pasar los vértices de control** a la siguiente etapa.

Los niveles de teselación determinan el número de subdivisiones. Un valor de `gl_TessLevelOuter[i] = 4.0` indica que el borde $i$ se dividirá en 4 segmentos.

```glsl
#version 460 core

// Número de vértices de control de salida (igual al de entrada en este caso)
layout(vertices = 3) out;

void main() {
    // Pasar el vértice de control sin modificar
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Solo el primer invocation establece los niveles (evita escrituras redundantes)
    if (gl_InvocationID == 0) {
        // Subdividir cada borde exterior en 4 segmentos
        gl_TessLevelOuter[0] = 4.0;
        gl_TessLevelOuter[1] = 4.0;
        gl_TessLevelOuter[2] = 4.0;
        // Subdivisión interior
        gl_TessLevelInner[0] = 4.0;
    }
}
```

#### Tessellation Evaluation Shader (TES)

El **Tessellation Evaluation Shader** (TES) se ejecuta **una vez por cada nuevo vértice** generado por el *tessellator* (la unidad de hardware fija que realiza la subdivisión). Recibe las coordenadas paramétricas del nuevo vértice (en el dominio del parche) y debe calcular su posición final en espacio de clip.

La variable `gl_TessCoord` contiene las coordenadas baricéntricas $(u, v, w)$ del nuevo vértice dentro del parche.

```glsl
#version 460 core

// Dominio triangular con espaciado igual y orientación horaria
layout(triangles, equal_spacing, ccw) in;

uniform mat4 uMVP;

void main() {
    // Interpolación baricéntrica de la posición usando gl_TessCoord
    vec3 p = gl_TessCoord.x * gl_in[0].gl_Position.xyz
           + gl_TessCoord.y * gl_in[1].gl_Position.xyz
           + gl_TessCoord.z * gl_in[2].gl_Position.xyz;

    gl_Position = uMVP * vec4(p, 1.0);
}
```

Los modos de dominio disponibles son `triangles`, `quads` e `isolines`. El espaciado (`equal_spacing`, `fractional_even_spacing`, `fractional_odd_spacing`) controla la distribución de los vértices en los bordes subdivididos.

#### Puntos clave

- La teselación permite implementar superficies de subdivisión, *displacement mapping* y LOD (*Level of Detail*) geométrico de forma eficiente en GPU.
- El TCS determina *cuánto* se subdivide; el TES determina *dónde* se colocan los nuevos vértices.
- A diferencia del Geometry Shader, la teselación escala bien en la GPU moderna, ya que el *tessellator* es una unidad de hardware fija altamente paralelizable.

---

### Compute Shader

El **Compute Shader** es una etapa programable de propósito general que opera **fuera del pipeline de renderizado tradicional**. Introducido en OpenGL 4.3, no forma parte de la cadena vertex → rasterización → fragmento, sino que se despacha de forma independiente mediante la llamada `glDispatchCompute`.

Su propósito es aprovechar la capacidad de cómputo paralelo de la GPU para tareas que no requieren producir píxeles directamente: simulación de partículas, cálculos físicos, postprocesado de imágenes, generación de texturas procedurales, ordenación en GPU, etc.

#### Modelo de ejecución

El Compute Shader se organiza en una jerarquía de dos niveles:

- **Work group** (*grupo de trabajo*): unidad mínima de despacho. Todos los invocations dentro de un work group pueden compartir memoria y sincronizarse entre sí.
- **Invocation**: ejecución individual del shader dentro del work group.

La llamada de despacho especifica cuántos work groups se lanzan en cada dimensión:

```c
// Lanzar (16 × 16 × 1) work groups
glDispatchCompute(16, 16, 1);
```

Dentro del shader, el tamaño local del work group se declara con:

```glsl
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
```

El número total de invocations = work groups × local size por dimensión.

#### Variables de sistema

| Variable | Descripción |
|---|---|
| `gl_GlobalInvocationID` | Índice global único de la invocation: `gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID` |
| `gl_LocalInvocationID` | Índice de la invocation dentro de su work group |
| `gl_WorkGroupID` | Índice del work group actual |
| `gl_WorkGroupSize` | Tamaño del work group (igual al `local_size` declarado) |

#### Memoria compartida y sincronización

Las invocations de un mismo work group pueden compartir datos a través de variables declaradas con el calificador `shared`. El acceso concurrente a esta memoria requiere sincronización explícita mediante barreras:

```glsl
shared float sCache[64]; // Memoria compartida entre invocations del mismo work group

// Barrera: garantiza que todas las escrituras a sCache son visibles
// antes de que cualquier invocation continúe
barrier();
memoryBarrierShared();
```

#### Ejemplo: inversión de imagen en GPU

```glsl
#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Imagen de entrada (solo lectura) y salida (solo escritura)
layout(rgba8, binding = 0) uniform readonly  image2D uInput;
layout(rgba8, binding = 1) uniform writeonly image2D uOutput;

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    vec4 color = imageLoad(uInput, texel);
    // Invertir componentes RGB
    imageStore(uOutput, texel, vec4(1.0 - color.rgb, color.a));
}
```

#### Puntos clave

- El Compute Shader es la herramienta principal para **GPGPU** (*General-Purpose GPU computing*) en OpenGL.
- No produce salida gráfica directa; escribe en buffers (`SSBO`, *Shader Storage Buffer Objects*) o imágenes (`image2D`, etc.) que pueden ser consumidos por el pipeline de renderizado en pasos posteriores.
- La sincronización entre el Compute Shader y el pipeline gráfico requiere barreras de memoria en la CPU (`glMemoryBarrier`) para garantizar la coherencia de los datos.
