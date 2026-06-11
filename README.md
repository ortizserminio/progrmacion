<div align="center">

# DataframeShell

**Una shell interactiva para análisis y manipulación de datos CSV, escrita en C**

![Language](https://img.shields.io/badge/language-C-blue?style=flat-square&logo=c)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey?style=flat-square&logo=windows)
![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)

</div>

---

## ¿Qué es?

DataframeShell es una shell de línea de comandos que permite cargar, explorar y transformar ficheros CSV directamente desde la terminal. Inspirada en la interfaz de pandas pero implementada en C puro, ofrece un prompt interactivo con soporte para múltiples dataframes simultáneos.

```
[?]:> load titanic.csv
[DF0: 891,12]:> view 5
PassengerId         Survived            Pclass              Name
1                   0                   3                   Braund, Mr. Owen Harris
2                   1                   1                   Cumings, Mrs. John Bradley
...
[DF0: 891,12]:> filter Survived == 1
[DF0: 342,12]:> sort Age asc
[DF0: 342,12]:>
```

---

## Características

| Funcionalidad | Descripción |
|--------------|-------------|
| **Múltiples DFs** | Carga y gestiona varios dataframes en memoria |
| **Tipos automáticos** | Detecta columnas de texto, numéricas y de fecha |
| **Filtrado** | Filtra filas por condiciones sobre columnas |
| **Ordenación** | Ordena ascendente o descendente por cualquier columna |
| **Limpieza** | Elimina filas nulas y columnas completas |
| **Fechas** | Extrae el trimestre (quarter) de columnas de fecha |
| **Persistencia** | Guarda el resultado transformado a un nuevo CSV |

---

## Compilación

Necesitas GCC con soporte para Windows (`windows.h`).

```bash
gcc lib.c main.c -o main.exe
```

---

## Uso

Ejecuta el binario y usa el prompt interactivo:

```bash
./main.exe
```

El prompt muestra el dataframe activo y sus dimensiones (`[DF0: filas,columnas]:>`).

---

## Referencia de comandos

### Gestión de archivos

```
load <archivo.csv>        Carga un CSV y lo establece como dataframe activo
save <archivo.csv>        Guarda el dataframe activo en un nuevo CSV
```

### Exploración

```
meta                      Muestra nombre, tipo y valores nulos de cada columna
view [n]                  Muestra las primeras n filas (por defecto 10)
```

### Transformación

```
sort <columna> [asc|des]  Ordena por columna (ascendente por defecto)
filter <col> <op> <val>   Filtra filas: operadores =, !=, <, >, <=, >=
delnull <columna>         Elimina filas con valor nulo en la columna indicada
delcolum <columna>        Elimina una columna completa del dataframe
quarter <col_fecha> <nueva_col>  Crea columna con el trimestre de una fecha
```

### Navegación entre dataframes

```
df<n>                     Cambia al dataframe número n (ej: df0, df1, df2...)
quit                      Sale del programa y libera la memoria
```

---

## Ejemplo de sesión

```bash
[?]:> load titanic.csv
[DF0: 891,12]:> meta
Columna 0: PassengerId    Tipo: NUMERICO    Filas Nulas: 0
Columna 1: Survived       Tipo: NUMERICO    Filas Nulas: 0
Columna 2: Pclass         Tipo: NUMERICO    Filas Nulas: 0
...

[DF0: 891,12]:> filter Age > 18
[DF0: 677,12]:> sort Age des
[DF0: 677,12]:> view 3
PassengerId         Survived            Age
631                 1                   80
852                 0                   74
...

[DF0: 677,12]:> save adultos.csv
[DF0: 677,12]:> quit
EXIT PROGRAM
```

---

## Estructura del proyecto

```
FP/
├── main.c          Bucle principal e interpretación de comandos
├── lib.h           Definición de estructuras (Lista, Nodo, Dataframe, Columna)
├── lib.c           Implementación de todas las operaciones
├── titanic.csv     Dataset de ejemplo
└── Libro1.csv      Dataset adicional de ejemplo
```

---

## Tipos de datos soportados

| Tipo | Descripción | Ejemplo |
|------|-------------|---------|
| `NUMERICO` | Enteros y decimales | `42`, `3.14` |
| `TEXTO` | Cadenas de caracteres | `"Braund, Mr. Owen"` |
| `FECHA` | Fechas (para `quarter`) | `2023-01-15` |

---

<div align="center">

Desarrollado como práctica de Fundamentos de Programación

</div>
