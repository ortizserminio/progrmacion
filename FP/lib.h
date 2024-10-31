#ifndef _lib_H_
#define _lib_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


typedef enum {
    TEXTO,
    NUMERICO,
    FECHA
}TipoDato;

typedef struct {

    char nombre[30];
    TipoDato tipo;
    void *datos;
    unsigned char *esNulo;
    int numFilas;
} Columna;

typedef struct {

    Columna *columna;
    int numColumnas;
    int numFilas;
    int *indice;
} Dataframe;


typedef struct tm Fecha;


typedef struct NodoLista{
    Dataframe *df;
    struct NodoLista *siguiente;
}Nodo;

typedef struct {
    int numDFs;
    Nodo *primero;
}Lista;

void Datos_alumno();
int Validar_decimal(char *cadena);
int Validar_entero (char *cad);
void Trim(char *cadena);
Nodo* Load(char *nombre);
int NumCampos (char *lin);
char* ObtenerCampo(char *lin, int id, char *val);
TipoDato Tipo(char *val);
int FechaValidarAMD (char* fecha);
Dataframe* CrearDataFrame(int filas, int columnas, TipoDato *t, char* cab);
Columna* CrearColumna(char *nombre, TipoDato t, int filas);
Fecha* AFecha(char *cadena);
int anyadirNodo(Nodo *nuevo, Lista *laGranLista);
int numNulos(Columna col);
char* printElemento(Dataframe* df, int col, int fila);
Columna* BuscarColumna(Dataframe *df, char *nombre);
void SortCol(Dataframe *df, Columna *col, int asc);
int compararValores(void *dato1, void *dato2, TipoDato tipo);
void intercambiar(int *a, int *b);
Columna* CopiarColumna(const Columna *colOriginal);
int Save(Dataframe *df, char *nombre);
int EliminarColumna(Dataframe *df, int indiceColumna);
void LiberarColumna(Columna *columna);
void LiberarDataframe(Dataframe *df);
int AgregarColumna(Dataframe *df, char *nombre , TipoDato t);
int Quarter(Dataframe *df, Columna *col, char* nombre);
int EliminarNulos (Nodo* nodo, Dataframe *df, Columna* col);
int Filtrar(Nodo* nodo, Dataframe* df, Columna *col, char *comp, char *valor);




#endif

