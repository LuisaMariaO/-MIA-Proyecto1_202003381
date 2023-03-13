#include <iostream>
#include <list>
#include "estructuras.h"
#include <map>
#include <cmath>
using namespace std;


/*<rutaDisco,[<id,nombrePartición>]>*/
//map <string,list<pair<string,string>>> montadas();

const string digitosCarnet = "81"; //202003381


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

void mount(char* parametros);
void montarParticion(char* ruta, char name[16]);
string getFileName(string ruta);
void verMontadas();

void unmount(char* parametros);
void desmontarParticion(string id);

void rep(char* parametros);
void repMbr(string path, string id);
string getPathWName(string path);
string toLower(string valor);
void repDisk(string path, string id);

void pause();

void mkfs(char* parametros);
void formatear(string id, char fs);

void repSb(string path, string id);
void repBmInode(string path, string id);
void repBmBlock(string path, string id);
void repJournaling(string path, string id);
void repFile(string path, string id, string rutaFs);