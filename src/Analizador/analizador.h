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
MBR ajustar(MBR mbr, int psize, char name[16], char type, char fit, FILE *archivo);
void ajustar(char* ruta, int psize, char name[16], char type, char fit);
void ajustarP(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit);
void ajustarE(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit);
void ajustarL(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit);

MBR firstFitP(MBR mbr, int pzise, char name[16], char fit);
MBR firstFitE(MBR mbr, int pzise, char name[16], char fit, FILE *archivo);
MBR bestFitP(MBR mbr, int pzise, char name[16], char fit);
MBR bestFitE(MBR mbr, int pzise, char name[16], char fit, FILE *archivo);
MBR worstFitP(MBR mbr, int pzise, char name[16], char fit);
MBR worstFitE(MBR mbr, int pzise, char name[16], char fit, FILE *archivo);
void firstFitL(EBR inicial, int psize, char name[16], char fit, int limite,FILE *archivo);
MBR bestFit(MBR mbr, int psize, char name[16], char type);
MBR worstFit(MBR mbr, int psize, char name[16], char type);
void rep();