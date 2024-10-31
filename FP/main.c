#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <dirent.h>
#include "lib.h"

// gcc lib.c main.c -o main.exe

int main()
{

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Variables
    char prompt[100] = "?";
    char com[100];
    char aux[100];
    char *numeroDf;

    char *comtok[100];
    char separadores[10] = " \n\t\r";

    int numarg, n, asc, i, numDF = 0;

    Lista *laGranLista = NULL;
    Nodo *nodoActivo = NULL, *nodoAux = NULL;
    Dataframe *dfActivo = NULL;
    Columna *col1 = NULL, *col2 = NULL, *colRes = NULL;

    laGranLista = malloc(sizeof(Lista));
    laGranLista->numDFs = 0;
    laGranLista->primero = NULL;

    // Logica
    Datos_alumno();

    while (1)
    {

        printf("[%s]:>", prompt);
        gets(com);

        numarg = 0;

        comtok[numarg] = strtok(com, separadores);

        numarg++;

        while ((comtok[numarg] = strtok(NULL, separadores)) != NULL)
        {
            numarg++;
        }

        if (comtok[0] == NULL)
        {
            continue;
        }

        // tokenice
        //  for(int i = 0; i < numarg; i++){
        //      SetConsoleTextAttribute(hConsole, 2);
        //      printf("%s\n", comtok[i]);
        //      SetConsoleTextAttribute(hConsole, 7);
        //  }

        if (strcmp(comtok[0], "quit") == 0)
        {

            nodoAux = laGranLista->primero;
            for (int i = 0; i < laGranLista->numDFs; i++)
            {
                LiberarDataframe(nodoAux->df);
                nodoAux = nodoAux->siguiente;
            }

            SetConsoleTextAttribute(hConsole, 2);
            printf("EXIT PROGRAM\n");
            SetConsoleTextAttribute(hConsole, 7);

            return 0;
        }
        else if (strcmp(comtok[0], "load") == 0)
        {

            if (numarg != 2)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: load necesita un argumento\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }

            nodoAux = Load(comtok[1]);

            if (nodoAux == NULL)
                continue;
            else
            {
                anyadirNodo(nodoAux, laGranLista);
                numDF = laGranLista->numDFs - 1;
                sprintf(aux, "DF%d: %d,%d", numDF, nodoAux->df->numFilas, nodoAux->df->numColumnas);
                strcpy(prompt, aux);
                dfActivo = nodoAux->df;
                nodoActivo = nodoAux;
                nodoAux = NULL;
            }
            // cargar el CSV
            // printf("Entrando en CSV\n");
        }
        else if (strcmp(comtok[0], "meta") == 0)
        {

            // TO DO
            printf("Entrando en meta\n");
            SetConsoleTextAttribute(hConsole, 2);

            for (int i = 0; i < dfActivo->numColumnas; i++)
            {
                printf("Columna %d: %s\t", i, dfActivo->columna[i].nombre);
                if (dfActivo->columna[i].tipo == 0)
                {
                    printf("Tipo: TEXTO\t");
                }
                else if (dfActivo->columna[i].tipo == 1)
                {
                    printf("Tipo: NUMERICO\t");
                }
                else if (dfActivo->columna[i].tipo == 2)
                {
                    printf("Tipo: FECHA\t");
                }
                printf("Filas Nulas: %d\n", numNulos(dfActivo->columna[i]));
            }

            SetConsoleTextAttribute(hConsole, 7);
        }
        else if (strcmp(comtok[0], "view") == 0)
        {
            n = 10;
            if (numarg > 2)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: view no acepta mas de 2 argumentos\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (numarg == 2)
            {
                if (Validar_entero(comtok[1]) == 0)
                {
                    SetConsoleTextAttribute(hConsole, 4);
                    printf("Error: view necesita un argumento numerico\n");
                    SetConsoleTextAttribute(hConsole, 7);
                    continue;
                }
                n = atoi(comtok[1]);
            }

            if (dfActivo == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No hay dataframe activo\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }

            if (n > dfActivo->numFilas)
            {
                n = dfActivo->numFilas;
            }

            SetConsoleTextAttribute(hConsole, 2);
            for (int i = 0; i < dfActivo->numColumnas; i++)
            {
                printf("%-20s", dfActivo->columna[i].nombre);
            }
            printf("\n");
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < dfActivo->numColumnas; j++)
                {
                    printf("%-20s", printElemento(dfActivo, j, i));
                }
                printf("\n");
            }
            SetConsoleTextAttribute(hConsole, 7);
        }
        else if (strcmp(comtok[0], "sort") == 0)
        {

            asc = 1;

            if (numarg > 3)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: view no acepta mas de 3 argumentos\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (numarg == 3)
            {
                if (strcmp(comtok[2], "asc") == 0)
                {
                    asc = 1;
                }
                else if (strcmp(comtok[2], "des") == 0)
                {
                    asc = 0;
                }
                else
                {
                    SetConsoleTextAttribute(hConsole, 4);
                    printf("Error: el segundo argumento tiene que ser asc o des\n");
                    SetConsoleTextAttribute(hConsole, 7);
                    continue;
                }
            }
            if (dfActivo == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No hay dataframe activo\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            col1 = BuscarColumna(dfActivo, comtok[1]);
            if (BuscarColumna(dfActivo, comtok[1]) == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: Columna no encontrada\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            for (int i = 0; i < dfActivo->numFilas; i++)
            {
                printf("%d\n", dfActivo->indice[i]);
            }
            SortCol(dfActivo, col1, asc);
        }
        else if (strcmp(comtok[0], "save") == 0)
        {

            if (numarg != 2)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: save necesita un argumento\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (dfActivo == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No hay dataframe activo\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (Save(dfActivo, comtok[1]) == -1)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No se ha podido abrir el archivo\n");
                SetConsoleTextAttribute(hConsole, 7);
            }
        }
        else if (strcmp(comtok[0], "filter") == 0)
        {

            if (numarg != 4)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: filter necesita tres argumentos\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (dfActivo == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No hay dataframe activo\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (BuscarColumna(dfActivo, comtok[1]) == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: Columna no encontrada\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (BuscarColumna(dfActivo, comtok[1])->tipo != Tipo(comtok[3]))
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: El tipo de columna no corresponde con el valor\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (Filtrar(nodoActivo, dfActivo, BuscarColumna(dfActivo, comtok[1]), comtok[2], comtok[3]) == -1)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No se ha podido filtrar\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            else
            {
                dfActivo = nodoActivo->df;
                sprintf(aux, "DF%d: %d,%d", numDF, dfActivo->numFilas, dfActivo->numColumnas);
                strcpy(prompt, aux);
            }
        }
        else if (strcmp(comtok[0], "delnull") == 0)
        {

            if (numarg != 2)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: delnull no necesita argumentos\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (dfActivo == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No hay dataframe activo\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (BuscarColumna(dfActivo, comtok[1]) == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: Columna no encontrada\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }

            if (EliminarNulos(nodoActivo, dfActivo, BuscarColumna(dfActivo, comtok[1])) == -1)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No se ha podido eliminar los nulos\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            else
            {
                dfActivo = nodoActivo->df;
                sprintf(aux, "DF%d: %d,%d", numDF, dfActivo->numFilas, dfActivo->numColumnas);
                strcpy(prompt, aux);
            }
        }
        else if (strcmp(comtok[0], "delcolum") == 0)
        {

            if (numarg != 2)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: delcolum necesita un argumento\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (dfActivo == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No hay dataframe activo\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (BuscarColumna(dfActivo, comtok[1]) == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: Columna no encontrada\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }

            for (i = 0; i < dfActivo->numColumnas; i++)
            {
                if (strcmp(dfActivo->columna[i].nombre, comtok[1]) == 0)
                {
                    break;
                }
            }

            if (EliminarColumna(dfActivo, i) == -1)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No se ha podido eliminar la columna\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            else
            {
            }
        }
        else if (strcmp(comtok[0], "quarter") == 0)
        {

            if (numarg != 3)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: quarter necesita un argumento\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (dfActivo == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: No hay dataframe activo\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (BuscarColumna(dfActivo, comtok[1]) == NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: Columna no encontrada\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (BuscarColumna(dfActivo, comtok[1])->tipo != FECHA)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: La columna no es tipo fecha\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (BuscarColumna(dfActivo, comtok[2]) != NULL)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: Columna ya existente\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }

            Quarter(dfActivo, BuscarColumna(dfActivo, comtok[1]), comtok[2]);
        }
        else if (strncmp(comtok[0], "df", 2) == 0)
        {
            numeroDf = comtok[0] + 2;
            if (Validar_entero(numeroDf) == 0)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: df necesita un argumento numerico\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (numarg > 1)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: df no acepta argumentos\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            if (atoi(numeroDf) > laGranLista->numDFs)
            {
                SetConsoleTextAttribute(hConsole, 4);
                printf("Error: df no existe\n");
                SetConsoleTextAttribute(hConsole, 7);
                continue;
            }
            nodoAux = laGranLista->primero;
            for (int i = 0; i < laGranLista->numDFs; i++)
            {
                if (atoi(numeroDf) == i)
                {
                    numDF = i;
                    sprintf(aux, "DF%d: %d,%d", i, nodoAux->df->numFilas, nodoAux->df->numColumnas);
                    strcpy(prompt, aux);
                    dfActivo = nodoAux->df;
                    nodoActivo = nodoAux;
                    nodoAux = NULL;
                    break;
                }
                nodoAux = nodoAux->siguiente;
            }
        }
        else
        {

            SetConsoleTextAttribute(hConsole, 4);
            printf("Error: Comando no reconocido\n");
            SetConsoleTextAttribute(hConsole, 7);
        }
    }
    return 0;
}