#include <iostream>
#include <list>
#include "estructuras.h"
using namespace std;

void analizar(char *comando);
bool verify_name(string name);
string get_path_raid(string path);
void verifyDirectory(string path);
char* espacioCadena(char* comando);
string regresarEspacio(string ruta);
bool crearArchivo(string ruta, int tamano);
bool existsFile(string ruta);

void mkdisk(char* parametros);
void rmdisk(char* parametros);
void removeFile(string path);
void execute(char* parametros);
void leerScript(string nombre);
void fdisk(char* parametros);

void ajustar(char* ruta, int psize, char name[16], char type, char fit);
void ajustarP(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit);
void ajustarE(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit);
void ajustarL(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit);

void fdiskDelete(char* ruta, char name[16]);
void fdiskAdd(char* ruta, char name[16], int add);


void rep();