#include <iostream>
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
MBR firstFit(MBR mbr, int pzise, char name[16]);
void rep();