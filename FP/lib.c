#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <windows.h>
#include "lib.h"

void Datos_alumno(){

    printf("XXX, XXX XXX\n");
    printf("XXX@xx.xx\n");

}

// 1 OK, 0 MAL
int Validar_decimal(char *cadena){

    int i, p=0;

    if(cadena[0] == '+' || cadena[0] == '-')
        i=1;
    else    
        i=0;

    if(strlen(cadena) == 1 && i==1){ // caso en el que solo escriben + o -
        return 0;
    }

    if(strlen(cadena) == 0){
        return 0;
    }

    if(i == 1 && strlen(cadena) == 2 && cadena[1] == '.' ){ //caso en el que nos escriben +. o -.
        return 0;
    }

    if(strlen(cadena) == 1 && i == 0 && cadena[0] == '.'){ // .
        return 0;
    }


    for(i; i<strlen(cadena); i++){
        if(cadena[i] < '0' || cadena[i] > '9'){
            if(cadena[i] == '.'){
                p++;
            }
            else{
                return 0;
            }
        }

        if(p == 2){
            return 0;
        }
    }

    return 1;
}

// 1 OK, 0 MAL
int Validar_entero (char *cad)
{
	int i, ini;
	
	if(cad[0]=='+')
		ini=1;
	else 
		ini=0;
	for (i=ini; i<strlen(cad); i++)
		{
			if (cad[i]<'0' || cad[i]>'9')
				return 0;
		}
	if (ini==1 && strlen(cad)==1)
		return 0;
	if (ini==0 && strlen(cad)==0)
		return 0;
return 1;
}

//Esta funcion elimina los espacios en blanco del token
void Trim(char *cadena){

    int c, i, j, n=strlen(cadena);

    if(cadena == NULL || n<1){
        return;
    }

    n--;

    while(n>=0 && ((c=cadena[n]) == ' ' || c=='\t' || c=='\n' || c =='\r')){
        n--;
    }

    cadena[n+1] = '\0';

    i=0;

    while(i<=n && ((c=cadena[i]) ==' ' || c == '\t' || c=='\n' || c=='\r')){
        i++;
    }

    for(j=0;j <= n-i+1; j++){
        cadena[j] = cadena[j+i];
    }
}

Nodo* Load(char *nombre){

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    Nodo *nuevo = (Nodo *)malloc(sizeof(Nodo));
    nuevo->siguiente=NULL;
    char aux[50] = "./";

    FILE *fichero;
    TipoDato *t;
    char cab[1000], lin[1000], val[1000], *token, *nombreVar, *numero;
    int filas=1, columnas, i, j, contador;

    strcat(aux, nombre);

    if(strchr(nombre, '.') == NULL){ //Entramos en el caso de que no tenga .txt
        strcat(aux, ".csv");
    }

    fichero = fopen(aux, "r"); //abrimos fichero en modo lectura

    if (fichero == NULL) {
        SetConsoleTextAttribute(hConsole, 4);
        printf("Error al abrir el fichero.\n");
        SetConsoleTextAttribute(hConsole, 7);
        return NULL;
    }
    fgets(cab,1000,fichero);
	Trim(cab);
	columnas=NumCampos(cab);
	
    if (columnas==0)
	{
        SetConsoleTextAttribute(hConsole, 4);
		printf("	## ERROR la cabecera no tiene campos\n");
        SetConsoleTextAttribute(hConsole, 7);
		return NULL;
	}

    fgets (lin, 1000,fichero);
	t=malloc(sizeof(TipoDato)*columnas);
	for(i=0; i<columnas; i++)
	{
		ObtenerCampo(lin, i, val);
		Trim(val);
		t[i]=Tipo(val);
	}

    while(!feof(fichero))
	{	
		if (fgets(lin,1000,fichero)==NULL)
			break;
		Trim(lin);
		filas++;
	}

    nuevo->df=CrearDataFrame(filas, columnas, t, cab);


    fseek(fichero,0,SEEK_SET);
	fgets(lin,1000,fichero);

    filas=0;

    while (!feof(fichero))
	{
		if(fgets(lin,1000,fichero)==NULL)
			break;

        for(i=0;i<columnas;i++){
            ObtenerCampo(lin,i,val);
            Trim(val);
            if (t[i]==Tipo(val) || t[i]==TEXTO){
                nuevo->df->columna[i].esNulo[filas]=0;
                if (t[i]==TEXTO){
                    if(strlen(val)==0){
                        nuevo->df->columna[i].esNulo[filas]=1;
                    }
                    ((char**)nuevo->df->columna[i].datos)[filas]= strdup(val);
                }
                else if(t[i]==NUMERICO){
                    ((double*)nuevo->df->columna[i].datos)[filas]=atof(val);
                }
                else if(t[i]==FECHA){
                    ((Fecha*)nuevo->df->columna[i].datos)[filas]=*AFecha(val);
                }
            }
            else{
                nuevo->df->columna[i].esNulo[filas]=1;
            }
        }
        filas++;
	}
    return nuevo;
}

Fecha* AFecha(char *cadena){

    Fecha *f = (Fecha *)malloc(sizeof(Fecha));

    char *token;
    token = strtok(cadena, "-/");
    f->tm_year = atoi(token);
    token = strtok(NULL, "-/");
    f->tm_mon = atoi(token)-1;
    token = strtok(NULL, "-/");
    f->tm_mday = atoi(token);

    return f;
}

Dataframe* CrearDataFrame(int filas, int columnas, TipoDato *t, char* cab){
    
    char* val = (char *)malloc(sizeof(char)*40);
    Dataframe *df = (Dataframe *)malloc(sizeof(Dataframe));
    df->numColumnas=columnas;
    df->numFilas=filas;
    df->indice = (int *)malloc(sizeof(int)*filas);
    if(df->indice == NULL)
        return NULL;
    for (int i=0; i<filas; i++){
        df->indice[i] = i;
    }
    df->columna = (Columna *)malloc(sizeof(Columna)*columnas);
    for (int i=0; i<columnas; i++){
        ObtenerCampo(cab,i, val);
        df->columna[i] = *CrearColumna(val, t[i], filas);
    }
    return df;
}

Columna* CrearColumna(char *nombre, TipoDato t, int filas){
    Columna *c = (Columna *)malloc(sizeof(Columna));
    
    strcpy(c->nombre, nombre);
    c->esNulo = (unsigned char *) malloc(sizeof(unsigned char)*filas);

    c->tipo = t;
    if (t == TEXTO){
        c->datos = (char **)malloc(sizeof(char*)*filas);
    }
    else if(t == NUMERICO){
        c->datos = (double *)malloc(sizeof(double)*filas);
    }
    else if(t == FECHA){
        c->datos = (Fecha *)malloc(sizeof(Fecha)*filas);
    }
    c->numFilas=filas;
    return c;
}

int NumCampos (char *lin){
    char *sep = strdup(",");
	int x=1;
	int tamSep=strlen(sep);
	char *p1, *p2;
	
	if (lin==NULL || lin[0]=='\0')
		return 0;
	p2=strstr(lin, sep);
	while (p2!=NULL)
	{
		x++;
		p1=p2+tamSep;
		p2=strstr(p1, sep);
	}
	
	return x;
}
		
char* ObtenerCampo(char *lin, int id, char *val){
    char *sep = strdup(",");
	int i, tamSep=strlen(sep);
	char *p1, *p2;
	
	if (lin==NULL || lin[0]=='\0')
		return NULL;
	
	p1=lin;
	p2=strstr(lin, sep);
	
	for(i=0; i<id;i++)
	{
		p1=p2+tamSep;
		p2=strstr(p1, sep);
	}
	if(p2==NULL)
		strcpy(val,p1);
	else
	{
		strncpy(val, p1, p2-p1);
		val [p2-p1]='\0';
	}
	return val;
}

TipoDato Tipo(char *val){
    // printf("VAL: '%s' ->", val);
    if (FechaValidarAMD(val)){
        // printf("FECHA\n");
    	return FECHA;
    }
    else if (Validar_decimal(val)){
        // printf("NUMERICO\n");
        return NUMERICO;
    }
    else{
        // printf("TEXTO\n");
        return TEXTO;
    }
}

int FechaValidarAMD (char* fecha) {

    int i, d, m, a;

    if (strlen(fecha)!=10){
        return 0;
    }
    //Comprobamos que el formato de la fecha sea correcto
    if(fecha[4] != '-' && fecha[4] != '/' || fecha[4] != fecha[7]){ 
    //Da igual que sea barra o guion, pero tiene que ser el mismo caracter    
    //2021/12/31 o 2021-12-31 FORMATO SIEMPRE EN AAAA/MM/DD o AAAA-MM-DD
       return 0;
    }else{
        for (i = 0; i < 10; i++){
            if (i==4 || i==7){
                continue;
            }
            if(fecha[i] < '0' || fecha[i] > '9'){
                return 0;
            }
            
        }
        if(i==10){
            d=(fecha[8]-'0')*10+(fecha[9]-'0');
            m=(fecha[5]-'0')*10+(fecha[6]-'0');
            a=(fecha[0]-'0')*1000+(fecha[1]-'0')*100+(fecha[2]-'0')*10+(fecha[3]-'0');


            if (a < 1  || m < 1 || m > 12 || d < 1 || d > 31){
                return 0;
            }else if(m==4 || m==6 || m==9 || m==11 && d > 30){
                return 0;
            }
            else{
                if(m==2){
                    if(a%400==0 || (a%100!=0 && a%4==0)){ //Comprobación de que el año sea bisiesto
                        if(d >= 29){
                            return 0;
                        }
                    }else if(d > 28){
                        return 0;
                    }  
                    else{
                        return 1;
                    }
                }
                else{
                    return 1;
                }
            } 
        }
    }
}

int anyadirNodo(Nodo *nuevo, Lista *laGranLista){
    Nodo *aux;
    if (laGranLista->primero == NULL){
        laGranLista->primero = nuevo;
        laGranLista->numDFs++;
        return 1;
    }
    else{
        aux = laGranLista->primero;
        while (aux->siguiente != NULL){
            printf("a");
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
        laGranLista->numDFs++;
        return 1;
    }
}

int numNulos(Columna col){

    int i, contador=0;
    for(i=0; i<col.numFilas; i++){
        if(col.esNulo[i] == 1){
            contador++;
        }
    }
    return contador;
}

char* printElemento(Dataframe* df, int col, int fila) {
    char *resultado = (char *)malloc(100 * sizeof(char)); 
    if (!resultado)
        return NULL; 

    if (df->columna[col].esNulo[df->indice[fila]]==1)
        strcpy(resultado, "#N/A");
    else if (df->columna[col].tipo == TEXTO) {
        strcpy(resultado, ((char **)df->columna[col].datos)[df->indice[fila]]);
    }
    else if (df->columna[col].tipo == NUMERICO) {
        sprintf(resultado, "%f", ((double *)df->columna[col].datos)[df->indice[fila]]);
    }
    else if (df->columna[col].tipo == FECHA) {
        sprintf(resultado, "%d/%d/%d", 
            ((Fecha *)df->columna[col].datos)[df->indice[fila]].tm_year, 
            ((Fecha *)df->columna[col].datos)[df->indice[fila]].tm_mon+1, 
            ((Fecha *)df->columna[col].datos)[df->indice[fila]].tm_mday);
    }
    return resultado;
}

Columna* BuscarColumna(Dataframe *df, char *nombre){
    int i;
    for(i=0; i<df->numColumnas; i++){
        if(strcmp(df->columna[i].nombre, nombre) == 0){
            return &df->columna[i];
        }
    }
    return NULL;
}

void intercambiar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

//Si dato1 > dato2 return + 
// Si dato1 < dato2 return - 
// Si dato1 = dato2 return 0
int compararValores(void *dato1, void *dato2, TipoDato tipo) {
    if (tipo == TEXTO) {
        // printf("Comparando %s con %s\n", (char *)dato1, (char *)dato2);
        return strcmp((char *)dato1, (char *)dato2);
    } else if (tipo == NUMERICO) {
        // printf("Comparando %f con %f\n", *(double *)dato1, *(double *)dato2);
        return (*(double *)dato1 > *(double *)dato2) - (*(double *)dato1 < *(double *)dato2);
    } else if (tipo == FECHA) {
        // printf("Comparando %d/%d/%d con %d/%d/%d\n",
        //    ((Fecha *)dato1)->tm_mday, ((Fecha *)dato1)->tm_mon + 1, ((Fecha *)dato1)->tm_year,
        //    ((Fecha *)dato2)->tm_mday, ((Fecha *)dato2)->tm_mon + 1, ((Fecha *)dato2)->tm_year);
        Fecha *f1 = (Fecha *)dato1;
        Fecha *f2 = (Fecha *)dato2;
        if (f1->tm_year != f2->tm_year)
            return f1->tm_year - f2->tm_year;
        else if (f1->tm_mon != f2->tm_mon)
            return f1->tm_mon - f2->tm_mon;
        else 
            return f1->tm_mday - f2->tm_mday;
    }
    return 0; // Por defecto, consideramos iguales si no se puede comparar
}

void SortCol(Dataframe *df, Columna *col, int asc) {
    int n = df->numFilas;
    int *indice = df->indice;  // El índice a ordenar
    unsigned char *esNulo = col->esNulo;  // Arreglo de si es nulo o no
    
    // size_t sizeOfData;
    // // Determinar el tamaño de los datos según el tipo
    // if (col->tipo == TEXTO) {
    //     sizeOfData = sizeof(char *);  // Texto: punteros a cadenas
    // } else if (col->tipo == NUMERICO) {
    //     sizeOfData = sizeof(double);  // Numérico: asumimos double
    // } else if (col->tipo == FECHA) {
    //     sizeOfData = sizeof(Fecha);   // Fecha: struct tm
    // }

    // Algoritmo de burbuja
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int idx1 = indice[j];
            int idx2 = indice[j + 1];

            // Si el primer elemento es nulo, lo dejamos al final
            if (esNulo[idx1]) {
                intercambiar(&indice[j], &indice[j + 1]);
                continue;
            }
            // Si el segundo elemento es nulo, lo movemos al final
            if (esNulo[idx2]) {
                continue;
            }
            void *dato1;
            void *dato2;
            
            // void *dato1 = col->datos + (idx1 * sizeOfData);
            // void *dato2 = col->datos + (idx2 * sizeOfData);
            

            if (col->tipo == NUMERICO){
                dato1 = &((double *)col->datos)[idx1];
                dato2 = &((double *)col->datos)[idx2];
            }
            else if (col->tipo == FECHA) {
                dato1 = &((Fecha *)col->datos)[idx1];
                dato2 = &((Fecha *)col->datos)[idx2];
            }
            else if (col->tipo == TEXTO){
                dato1 = ((char **)col->datos)[idx1];
                dato2 = ((char **)col->datos)[idx2];
            }

            // Comparar los valores no nulos
            int cmp = compararValores(dato1, dato2, col->tipo);

            if ((asc && cmp > 0) || (!asc && cmp < 0)) {
                intercambiar(&indice[j], &indice[j + 1]);
            }
        }
    }
}

Columna* CopiarColumna(const Columna *colOriginal) {
    Columna *nuevaCol = (Columna *)malloc(sizeof(Columna));
    if (!nuevaCol) {
        printf("Error al asignar memoria para la nueva columna.\n");
        return NULL;
    }

    strncpy(nuevaCol->nombre, colOriginal->nombre, sizeof(nuevaCol->nombre));
    nuevaCol->tipo = colOriginal->tipo;
    nuevaCol->numFilas = colOriginal->numFilas;

    // Asignar memoria para los datos y copiar según el tipo de dato
    if (colOriginal->tipo == TEXTO) {
        nuevaCol->datos = malloc(nuevaCol->numFilas * sizeof(char *));
        if (!nuevaCol->datos) {
            printf("Error al asignar memoria para los datos de la columna.\n");
            free(nuevaCol);
            return NULL;
        }
        for (int i = 0; i < nuevaCol->numFilas; i++) {
            ((char **)nuevaCol->datos)[i] = strdup(((char **)colOriginal->datos)[i]);  // Copia profunda de cada cadena
        }
    } else if (colOriginal->tipo == NUMERICO) {
        nuevaCol->datos = malloc(nuevaCol->numFilas * sizeof(double));
        if (!nuevaCol->datos) {
            printf("Error al asignar memoria para los datos de la columna.\n");
            free(nuevaCol);
            return NULL;
        }
        memcpy(nuevaCol->datos, colOriginal->datos, nuevaCol->numFilas * sizeof(double));
    } else if (colOriginal->tipo == FECHA) {
        nuevaCol->datos = malloc(nuevaCol->numFilas * sizeof(Fecha));
        if (!nuevaCol->datos) {
            printf("Error al asignar memoria para los datos de la columna.\n");
            free(nuevaCol);
            return NULL;
        }
        memcpy(nuevaCol->datos, colOriginal->datos, nuevaCol->numFilas * sizeof(Fecha));
    }

    // Asignar memoria para esNulo y copiar
    nuevaCol->esNulo = (unsigned char *)malloc(nuevaCol->numFilas * sizeof(unsigned char));
    if (!nuevaCol->esNulo) {
        printf("Error al asignar memoria para esNulo.\n");
        free(nuevaCol->datos);
        free(nuevaCol);
        return NULL;
    }
    memcpy(nuevaCol->esNulo, colOriginal->esNulo, nuevaCol->numFilas * sizeof(unsigned char));

    return nuevaCol;
}

int Save(Dataframe *df, char *nombre) {
    FILE *fichero;

    char aux[50]="./";


    strcat(aux, nombre);

    if(strchr(nombre, '.') == NULL){ //Entramos en el caso de que no tenga .csv
        strcat(aux, ".csv");
    }

    fichero = fopen(aux, "w");

    if (fichero!=NULL) {

        for (int i = 0; i < df->numColumnas; i++){
            if (i>0)
                fprintf(fichero, ",");

            fprintf(fichero, "%s", df->columna[i].nombre);
        }

        for (int i = 0; i < df->numFilas; i++){
            fprintf(fichero, "\n");
            for (int j = 0; j < df->numColumnas; j++){
                if(j>0)
				    fprintf(fichero, ",");
                
                if (df->columna[j].esNulo[df->indice[i]] == 0){
                    fprintf(fichero, "%s", printElemento(df, j, i));
                }
            }
        }

    }
    else {
        return -1;
    }
    fclose(fichero);
    return 1;
}

int AgregarColumna(Dataframe *df, char *nombre , TipoDato t) {
    Columna *nuevaMem = (Columna *)realloc(df->columna, (df->numColumnas + 1) * sizeof(Columna));
    
    if (nuevaMem == NULL) {
        return -1;  // Error al reasignar memoria
    }
    df->columna = nuevaMem;
    
    df->columna[df->numColumnas] = *CrearColumna(nombre, t, df->numFilas);
    
    df->numColumnas++;
    
    return 1;
}

int Quarter(Dataframe *df, Columna *col, char* nombre) {
    if (AgregarColumna(df, nombre, TEXTO) == -1) {
        return -1;
    }



    for (int i = 0; i < df->numFilas; i++) {

        df->columna[df->numColumnas - 1].esNulo[i] = 0;
        if (col->esNulo[df->indice[i]] == 1) {
            ((char **)df->columna[df->numColumnas - 1].datos)[df->indice[i]] = strdup("#N/A");
            continue;
        }
        int month = ((Fecha *)col->datos)[df->indice[i]].tm_mon + 1;
        char *quarter;

        if (month >= 1 && month <= 3) {
            quarter = "Q1";
        } else if (month >= 4 && month <= 6) {
            quarter = "Q2";
        } else if (month >= 7 && month <= 9) {
            quarter = "Q3";
        } else {
            quarter = "Q4";
        }

        ((char **)df->columna[df->numColumnas - 1].datos)[df->indice[i]] = strdup(quarter);
    }

    return 1;

}

void LiberarColumna(Columna *columna) {
    if (columna->tipo == TEXTO) {
        for (int i = 0; i < columna->numFilas; i++) {
            if (((char **)columna->datos)[i] != NULL && columna->esNulo[i]==0) {
                free(((char **)columna->datos)[i]);
            }
        }
    }
    if (columna->datos != NULL) {  
        free(columna->datos);
    }
    if (columna->esNulo != NULL) {  
        free(columna->esNulo);
    }
}

void LiberarDataframe(Dataframe *df) {
    if (df != NULL) {
        for (int i = 0; i < df->numColumnas; i++) {
            LiberarColumna(&df->columna[i]);
        }
        if (df->columna != NULL) {
            free(df->columna);
        }
        if (df->indice != NULL) {
            free(df->indice);
        }
        free(df);  
    }
}

int EliminarColumna(Dataframe *df, int indiceColumna) {

    LiberarColumna(&df->columna[indiceColumna]);
    
    // Mover las columnas
    for (int i = indiceColumna; i < df->numColumnas - 1; i++) {
        df->columna[i] = df->columna[i + 1];  // Copiar la siguiente columna
    }
    
    df->numColumnas--;
    
    // Reasignar memoria
    Columna *nueva_memoria = (Columna *)realloc(df->columna, df->numColumnas * sizeof(Columna));
    
    if (nueva_memoria == NULL && df->numColumnas > 0) {
        return -1;  // Error al reasignar memoria
    }
    
    df->columna = nueva_memoria;
    
    return 1;  // Columna eliminada con éxito
}

int EliminarNulos (Nodo* nodo, Dataframe *df, Columna *col) {
    Dataframe *dfNuevo;
    int numFilas = df->numFilas;
    int n=0;
    char cab[1000]="";
    TipoDato *t;
    t = (TipoDato *)malloc(df->numColumnas * sizeof(TipoDato));

    for (int i = 0; i < df->numFilas; i++) {
        if (col->esNulo[i] == 1) {
            numFilas--;
        }
    }

    if (numFilas == df->numFilas) {
        printf("No hay valores nulos\n");
        return 1;
    }

    for (int i = 0; i < df->numColumnas; i++) {
        t[i] = df->columna[i].tipo;
        strcat(cab, df->columna[i].nombre);
        if (i < df->numColumnas - 1)
            strcat(cab, ",");
    }

    dfNuevo = CrearDataFrame(numFilas, df->numColumnas, t, cab);

    if (dfNuevo == NULL)
        return -1;

    for (int i = 0; i < df->numFilas; i++) {

        if (col->esNulo[df->indice[i]] == 0) {

            for (int j = 0; j < df->numColumnas; j++) {

                if(df->columna[j].esNulo[df->indice[i]]==1) {
                    dfNuevo->columna[j].esNulo[n] = 1;
                }
                else {
                    dfNuevo->columna[j].esNulo[n] = 0;
                    if (df->columna[j].tipo == TEXTO) {
                        ((char **)dfNuevo->columna[j].datos)[n] = strdup(((char **)df->columna[j].datos)[df->indice[i]]);
                    }
                    else if (df->columna[j].tipo == NUMERICO) {
                        ((double *)dfNuevo->columna[j].datos)[n] = ((double *)df->columna[j].datos)[df->indice[i]];
                    }
                    else if (df->columna[j].tipo == FECHA) {
                        ((Fecha *)dfNuevo->columna[j].datos)[n] = ((Fecha *)df->columna[j].datos)[df->indice[i]];
                    }
                }
            }
            n++;
        }
    }
    nodo->df = dfNuevo;

    LiberarDataframe(df);

    return 1;
}

int Filtrar(Nodo* nodo, Dataframe* df, Columna *col, char *comp, char *valor){
    Dataframe *dfNuevo;
    int numFilas = df->numFilas;
    int n=0;
    char cab[1000]="";
    TipoDato *t, tipo=col->tipo;
    t = (TipoDato *)malloc(df->numColumnas * sizeof(TipoDato));
    void *val;

    if (tipo == TEXTO) {
        val = strdup(valor);
        printf("Val: %s\n", (char *)val);
    }
    else if (tipo == NUMERICO) {
        val=malloc(sizeof(double));
        *(double *)val = atof(valor);
        printf("Val: %f\n", *(double *)val);
    }
    else if (tipo == FECHA) {
        val=malloc(sizeof(Fecha));
        val=AFecha(valor);
        printf("Val: %d/%d/%d\n", ((Fecha *)val)->tm_year, ((Fecha *)val)->tm_mon+1, ((Fecha *)val)->tm_mday);
    }

    printf("Valor: %s\n", valor);
    printf("Comp: %s\n", comp);
    printf("Tipo: %d\n", tipo);



    for (int i = 0; i < df->numFilas; i++) {
        void *dato;
        if (tipo == NUMERICO){
            dato = &((double *)col->datos)[i];
        }
        else if (tipo == FECHA) {
            dato = &((Fecha *)col->datos)[i];
        }
        else if (tipo == TEXTO){
            dato = ((char **)col->datos)[i];
        }

        if (strcmp(comp,"gt")==0) {
            if (compararValores(dato, val, tipo) < 0){
                numFilas--;
            }
            
        }
        else if (strcmp(comp, "lt")==0) {
            if (compararValores(dato, val, tipo) > 0){
                numFilas--;
            }
        }
        else if (strcmp(comp, "eq")==0) {
            if (compararValores(dato, val, tipo) != 0){
                numFilas--;
            }

        }
        else if (strcmp(comp, "neq")==0) {
            if (compararValores(dato, val, tipo) == 0){
                numFilas--;
            }
        }
    }

    for (int i = 0; i < df->numColumnas; i++) {
        t[i] = df->columna[i].tipo;
        strcat(cab, df->columna[i].nombre);
        if (i < df->numColumnas - 1)
            strcat(cab, ",");
    }

    dfNuevo = CrearDataFrame(numFilas, df->numColumnas, t, cab);

    if (dfNuevo == NULL)
        return -1;
    for (int i = 0; i < df->numFilas; i++) {
        void *dato;
        if (tipo == NUMERICO){
            dato = &((double *)col->datos)[df->indice[i]];
        }
        else if (tipo == FECHA) {
            dato = &((Fecha *)col->datos)[df->indice[i]];
        }
        else if (tipo == TEXTO){
            dato = ((char **)col->datos)[df->indice[i]];
        }
        if (strcmp(comp, "gt")==0) {
            if (compararValores(dato, val, tipo) > 0) {
                for (int j = 0; j < df->numColumnas; j++) {
                    if(df->columna[j].esNulo[df->indice[i]]==1) {
                        dfNuevo->columna[j].esNulo[n] = 1;
                    }
                    else{
                        dfNuevo->columna[j].esNulo[n] = 0;
                        if (df->columna[j].tipo == TEXTO) {
                            ((char **)dfNuevo->columna[j].datos)[n] = strdup(((char **)df->columna[j].datos)[df->indice[i]]);
                        }
                        else if (df->columna[j].tipo == NUMERICO) {
                            ((double *)dfNuevo->columna[j].datos)[n] = ((double *)df->columna[j].datos)[df->indice[i]];
                        }
                        else if (df->columna[j].tipo == FECHA) {
                            ((Fecha *)dfNuevo->columna[j].datos)[n] = ((Fecha *)df->columna[j].datos)[df->indice[i]];
                        }
                    }
                }
                n++;
            }
        }
        else if (strcmp(comp, "lt")==0) {
            if (compararValores(dato, val, tipo) < 0) {
                for (int j = 0; j < df->numColumnas; j++) {
                    if(df->columna[j].esNulo[df->indice[i]]==1) {
                        dfNuevo->columna[j].esNulo[n] = 1;
                    }
                    else{
                        dfNuevo->columna[j].esNulo[n] = 0;
                        if (df->columna[j].tipo == TEXTO) {
                            ((char **)dfNuevo->columna[j].datos)[n] = strdup(((char **)df->columna[j].datos)[df->indice[i]]);
                        }
                        else if (df->columna[j].tipo == NUMERICO) {
                            ((double *)dfNuevo->columna[j].datos)[n] = ((double *)df->columna[j].datos)[df->indice[i]];
                        }
                        else if (df->columna[j].tipo == FECHA) {
                            ((Fecha *)dfNuevo->columna[j].datos)[n] = ((Fecha *)df->columna[j].datos)[df->indice[i]];
                        }
                    }
                }
                n++;
            }
        }
        else if (strcmp(comp, "eq")==0) {
            if (compararValores(dato, val, tipo) == 0) {
                for (int j = 0; j < df->numColumnas; j++) {
                    if(df->columna[j].esNulo[df->indice[i]]==1) {
                        dfNuevo->columna[j].esNulo[n] = 1;
                    }
                    else{
                        dfNuevo->columna[j].esNulo[n] = 0;
                        if (df->columna[j].tipo == TEXTO) {
                            ((char **)dfNuevo->columna[j].datos)[n] = strdup(((char **)df->columna[j].datos)[df->indice[i]]);
                        }
                        else if (df->columna[j].tipo == NUMERICO) {
                            ((double *)dfNuevo->columna[j].datos)[n] = ((double *)df->columna[j].datos)[df->indice[i]];
                        }
                        else if (df->columna[j].tipo == FECHA) {
                            ((Fecha *)dfNuevo->columna[j].datos)[n] = ((Fecha *)df->columna[j].datos)[df->indice[i]];
                        }
                    }
                }
                n++;
            }
        }
        else if (strcmp(comp, "neq")==0) {
            if (compararValores(dato, val, tipo) != 0) {
                for (int j = 0; j < df->numColumnas; j++) {
                    if(df->columna[j].esNulo[df->indice[i]]==1) {
                        dfNuevo->columna[j].esNulo[n] = 1;
                    }
                    else{
                        dfNuevo->columna[j].esNulo[n] = 0;
                        if (df->columna[j].tipo == TEXTO) {
                            ((char **)dfNuevo->columna[j].datos)[n] = strdup(((char **)df->columna[j].datos)[df->indice[i]]);
                        }
                        else if (df->columna[j].tipo == NUMERICO) {
                            ((double *)dfNuevo->columna[j].datos)[n] = ((double *)df->columna[j].datos)[df->indice[i]];
                        }
                        else if (df->columna[j].tipo == FECHA) {
                            ((Fecha *)dfNuevo->columna[j].datos)[n] = ((Fecha *)df->columna[j].datos)[df->indice[i]];
                        }
                    }
                }
                n++;
            }
        }
    }

    nodo->df = dfNuevo;

    LiberarDataframe(df);

    return 1;

}