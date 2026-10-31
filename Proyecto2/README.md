# Proyecto 2: Simulador de gestión de memoria paginada

**Estudiante:** Patrick Zúñiga Arroyo (carné 2025104944)
**Curso:** Estructuras de Datos
**Lenguaje:** C++ (estándar C++14)

Simulador de consola que administra la memoria de una computadora dividida en páginas. Las páginas físicas libres se guardan en un **árbol AVL**, la tabla de páginas de cada proceso es un **Splay Tree**, y las páginas usadas recientemente se guardan en dos cachés **LRU** (uno local por proceso y uno global).

Además están implementadas las cuatro extensiones opcionales del enunciado:

- visualización del estado de la memoria y de los árboles;
- exportación de métricas a CSV;
- simulación de reemplazo de páginas (swap);
- pruebas unitarias por módulo.

El informe técnico, el manual de usuario y los casos de prueba están en `Proyecto 2 - Informe técnico, manual y casos de prueba.docx` (y su versión `.pdf`).

---

## Requisitos

- Windows
- MinGW con g++ compatible con C++14 (el código no usa nada más nuevo).
- Opcional: Visual Studio Code para leer y editar los archivos.

---

## Cómo compilar

Doble clic en **`compilar.bat`**. Si la compilación sale bien, se crea **`proyecto.exe`**.

El mismo comando, desde una terminal:

```
g++ -std=c++14 -Wall -Wextra -g main.cpp OperationParser.cpp MemoryManager.cpp Process.cpp Page.cpp Cache.cpp -o proyecto.exe
```

---

## Cómo ejecutar

Hay tres formas de usar el programa.

**1. Con el menú.** Ejecutar `proyecto.exe` sin argumentos. Aparece el menú:

- `1`: lee las operaciones desde un archivo. Si se presiona ENTER sin escribir nombre, usa `entrada_prueba.txt`.
- `2`: modo consola. Pide la configuración y luego lee operaciones línea por línea hasta `SALIR`.
- `3`: salir.

**2. Con un archivo directamente.** Desde la terminal:

```
proyecto.exe entrada_prueba.txt
```

Para guardar la salida en un archivo:

```
proyecto.exe entrada_prueba.txt > salida.txt
```

**3. Con las pruebas de evidencia.** Doble clic en **`ejecutar_pruebas.bat`**. Compila el programa, procesa los cinco archivos de entrada y genera `salida_prueba.txt`, `salida_casos.txt`, `salida_error_config.txt`, `salida_extras.txt`, `salida_reemplazo.txt` y `metricas.csv`.

---

## Formato del archivo de entrada

Una operación por línea, con los datos separados por espacios.

- La configuración va **antes de cualquier operación**, en cualquier orden. Si un parámetro obligatorio aparece repetido, la simulación se detiene.
- Lo que está después de `#` es un comentario.
- Las líneas vacías se ignoran.
- Los comandos aceptan mayúsculas o minúsculas (`access` = `ACCESS`). Los id de proceso sí distinguen mayúsculas: `P1` y `p1` son procesos distintos.
- Los archivos guardados en Windows (con `\r` o con el BOM del Bloc de notas) se leen sin problema.

Ejemplo (es el mismo de `entrada_prueba.txt`):

```
MEMORIA_TOTAL 65536
TAMANO_PAGINA 1024
TAMANO_CACHE_GLOBAL 8

NEW_PROCESS P1 4096
NEW_PROCESS P2 8192
ACCESS P1 100
ACCESS P1 2500
ACCESS P2 7000
ALLOCATE P1 2048
FREE P2 2048 2048
END_PROCESS P1
END_PROCESS P2
```

### Configuración

| Línea | Qué hace |
|---|---|
| `MEMORIA_TOTAL n` | Bytes de memoria física. |
| `TAMANO_PAGINA n` | Bytes por página. `MEMORIA_TOTAL` debe ser múltiplo de este valor. |
| `TAMANO_CACHE_GLOBAL n` | Cantidad de entradas del caché global. |
| `PAGINAS_SWAP n` | **Opcional (extra).** Páginas de memoria secundaria. Si `n > 0`, activa el reemplazo de páginas. |

### Operaciones

| Línea | Qué hace |
|---|---|
| `NEW_PROCESS id bytes` | Crea un proceso con `bytes / TAMANO_PAGINA` páginas, redondeando hacia arriba. |
| `END_PROCESS id` | Termina el proceso y libera todas sus páginas. |
| `ACCESS id direccion` | Referencia una dirección virtual del proceso. |
| `ALLOCATE id bytes` | Le da más páginas al proceso. |
| `FREE id direccionInicial bytes` | Libera las páginas virtuales del rango. |

### Comandos de consulta

| Línea | Qué hace |
|---|---|
| `ESTADO` | Muestra el estado completo de la memoria. |
| `METRICAS` | Muestra las métricas acumuladas. |
| `AYUDA` | Muestra la lista de comandos. |
| `SALIR` | Termina la lectura. Lo que venga después no se ejecuta. |
| `ARBOL AVL` | **Extra.** Dibuja el AVL de páginas físicas libres. |
| `ARBOL SPLAY idProceso` | **Extra.** Dibuja la tabla de páginas (Splay Tree) de un proceso. |
| `EXPORTAR archivo.csv` | **Extra.** Guarda las métricas y una fila por proceso en un archivo CSV. |

---

## Extras

### Visualización de los árboles (`ARBOL`)

`ARBOL` no cambia la memoria: no hace splay, solo dibuja. Los árboles se dibujan de lado. La raíz es la línea sin sangría, los hijos de la derecha van arriba y los de la izquierda abajo. En el Splay Tree, la raíz es la última página que usó el proceso.

### Exportación a CSV (`EXPORTAR`)

Crea el archivo en la carpeta desde la que se ejecuta el programa. El CSV tiene dos tablas: las métricas generales y, después de una línea vacía, una fila por cada proceso activo. Se puede abrir con Excel o LibreOffice. Si Excel muestra todo en una sola columna, usar **Datos → Desde texto/CSV** y elegir la coma como separador.

### Reemplazo de páginas (`PAGINAS_SWAP`)

Sin este extra, si no quedan páginas físicas libres, `NEW_PROCESS` y `ALLOCATE` dan *Memoria insuficiente*. Con `PAGINAS_SWAP n`, el simulador tiene además una memoria secundaria (swap) de `n` páginas:

- **Reemplazo:** cuando se necesita una página física y no hay libres, la página en memoria física usada hace más tiempo (LRU, de cualquier proceso) pasa a swap. Sale de los dos cachés y su página física queda libre.
- **Fallo de página:** si `ACCESS` llega a una página que está en swap, se cuenta un fallo de página y la página vuelve a memoria física, sacando otra si hace falta.
- **Salida:** cada reemplazo y cada fallo se informa en su propia línea. El estado muestra el uso del swap y qué páginas de cada proceso están en swap. `METRICAS` y el CSV agregan fallos de página, páginas reemplazadas y páginas en swap.

Reglas:

- `PAGINAS_SWAP` va antes de la primera operación. En modo consola se escribe como una línea más, después de la configuración y antes de operar.
- Sin esa línea (o con `PAGINAS_SWAP 0`), el simulador funciona exactamente igual que sin el extra.
- Las páginas nuevas siempre entran a memoria física, así que una operación no puede pedir más páginas de las que tiene la memoria física.
- Si `PAGINAS_SWAP` es inválido, repetido o llega tarde, se informa el error y la línea se ignora; la simulación sigue.

Ejemplo completo y comentado en `entrada_reemplazo.txt`.

---

## Formato de la salida

Cada línea se muestra con su número y su resultado. Si la operación sale bien, se muestra el estado de la memoria después de ella:

- **Páginas físicas**: total, libres y ocupadas.
- **Páginas libres**: el AVL en orden.
- **Procesos activos**: memoria solicitada, páginas asignadas y el contenido de su caché local.
- **Cache global**: entradas usadas y capacidad. `P1:2->5` significa proceso P1, página virtual 2, página física 5. La primera entrada es la más reciente.
- **Hits y misses** locales y globales.

Al final del archivo se imprime un **resumen**: operaciones exitosas, errores y métricas.

Si una operación falla, se muestra `ERROR: <tipo>: <detalle>` seguido de `(La memoria no cambio.)`.

---

## Errores

Cada mensaje empieza con el nombre del error del enunciado. Los errores de operación no detienen el programa. Los de la configuración obligatoria sí.

| Error | Cuándo ocurre |
|---|---|
| Proceso inexistente | El id no existe (`ACCESS`, `ALLOCATE`, `FREE`, `END_PROCESS`, `ARBOL SPLAY`). |
| Proceso duplicado | `NEW_PROCESS` con un id que ya existe. |
| Memoria insuficiente | No hay suficientes páginas libres. Con reemplazo: tampoco alcanza el swap, o se piden más páginas que las físicas. |
| Dirección inválida | Dirección negativa o fuera del espacio del proceso. |
| Liberación de memoria no asignada | `FREE` con alguna página del rango sin asignar. |
| Acceso a página no válida | La página se liberó con `FREE`. |
| Archivo de entrada incorrecto | Archivo que no se puede abrir, configuración faltante o repetida, datos faltantes, números mal escritos o `PAGINAS_SWAP` inválido. |
| Tamaño de página inválido | `TAMANO_PAGINA` es 0 o negativo, es mayor que la memoria, o no divide a `MEMORIA_TOTAL`. |
| Operación desconocida | El comando no existe. |

En el modo consola, si la configuración es inválida o se escribe una letra donde va un número, el programa vuelve a pedir el dato.

---

## Pruebas

Cada módulo tiene su programa de prueba con su propio `main`. Cada uno se corre con su archivo `.bat`:

| Archivo | Qué prueba | Comprobaciones |
|---|---|---|
| `probar_avl.bat` | AVLTree | 29 |
| `probar_splay.bat` | SplayTree (incluye dibujo y búsqueda sin splay) | 39 |
| `probar_cache.bat` | Cache LRU (incluye la víctima del reemplazo) | 32 |
| `probar_memoria.bat` | MemoryManager: los 12 casos mínimos y el reemplazo de páginas | 93 |
| `probar_parser.bat` | OperationParser: archivo, consola, `ARBOL`, `EXPORTAR` y `PAGINAS_SWAP` | 54 |

Total: **247 comprobaciones**, todas aprobadas. Al final de cada prueba aparece la línea `Resultado: N aprobadas, 0 falladas.`

Las pruebas y el programa también se revisaron con AddressSanitizer y UBSan, sin errores de memoria.

---

## Estructura de la carpeta

**Código del simulador**

| Archivo | Contenido |
|---|---|
| `AVLTree.h` | Árbol AVL genérico (plantilla). Administra las páginas físicas libres. |
| `SplayTree.h` | Splay Tree genérico (plantilla). Es la tabla de páginas de cada proceso. |
| `Cache.h`, `Cache.cpp` | Caché LRU con `std::list` y `std::unordered_map`. Sirve para el caché local, el global y la lista LRU del reemplazo. |
| `Page.h`, `Page.cpp` | Una página: virtual, física, proceso, bits y última referencia. |
| `Process.h`, `Process.cpp` | Un proceso: id, memoria solicitada, tabla de páginas y caché local. |
| `MemoryManager.h`, `MemoryManager.cpp` | El simulador: operaciones, métricas, estado y extras. |
| `OperationParser.h`, `OperationParser.cpp` | Lee operaciones de un archivo o de la consola y las ejecuta. |
| `main.cpp` | Menú principal y ejecución directa con un archivo. |

**Pruebas**

| Archivo | Contenido |
|---|---|
| `pruebaAVL.cpp`, `pruebaSplay.cpp`, `pruebaCache.cpp`, `pruebaMemoria.cpp`, `pruebaParser.cpp` | Programas de prueba, cada uno con su `main`. |

**Scripts (`.bat`)**

| Archivo | Para qué sirve |
|---|---|
| `compilar.bat` | Compila `proyecto.exe`. |
| `ejecutar_pruebas.bat` | Compila y genera todas las salidas de evidencia. |
| `probar_avl.bat`, `probar_splay.bat`, `probar_cache.bat`, `probar_memoria.bat`, `probar_parser.bat` | Compilan y corren las pruebas de cada módulo. |

**Archivos de entrada y evidencias**

| Archivo | Contenido |
|---|---|
| `entrada_prueba.txt` | Ejemplo del enunciado. |
| `entrada_casos.txt` | Casos mínimos y errores, con un comentario por línea que dice qué se espera. |
| `entrada_error_config.txt` | Configuración inválida (`TAMANO_PAGINA 1000`). |
| `entrada_extras.txt` | Ejemplo de `ARBOL` y `EXPORTAR`. |
| `entrada_reemplazo.txt` | Ejemplo del reemplazo de páginas, con comentarios. |
| `salida_*.txt`, `metricas.csv` | Evidencias de ejecución (las genera `ejecutar_pruebas.bat`). |

---

## Limitaciones

- No se simulan escrituras en memoria. El campo de bit modificado existe, pero ningún comando del enunciado escribe.
- El reemplazo de páginas usa solo LRU y es global (la víctima puede ser de cualquier proceso).
- El swap se configura antes de la primera operación y no se puede cambiar después.
- Un id de proceso no puede contener `#`, porque ese símbolo marca el inicio de un comentario.
- `ESTADO` muestra la lista completa de páginas libres. Con memorias grandes la salida es larga.
- La interfaz es solo de consola.
