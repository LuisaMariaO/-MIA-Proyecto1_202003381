#include "analizador.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <cstdlib>
//#include "estructuras.h"

/*Funciones que devuelven el tipo y el valor de un parametro en strings ya en lowercase */
string get_tipo_parametro(string parametro){
    //Iteramos hasta obtener el tipo del parametro
    string tipo = "";
    for(int i = 0; i < parametro.length(); i++){
        if(parametro.at(i) == '=') break;
        char caracter = tolower(parametro.at(i));
        tipo = tipo + caracter;
    }
    //devolvemos el string
    return tipo;
}
string get_valor_parametro(string parametro){
    //Iteramos hasta obtener el valor del parametro
    string valor = "";
    bool concatenar = false;
    for(int i = 0; i < parametro.length(); i++){
        if(concatenar){
            char caracter = parametro.at(i);
            valor = valor + caracter;
        }
        if(parametro.at(i) == '=') concatenar = true;
    }
    //devolvemos el string
    return valor;
}

/*Funcion que busca espacios entre cadenas y los sustituye por $ para evitar separarla*/

char* espacioCadena(char* comando){
    bool cadena=false;
    bool cambio=false;
    for(int i=0; i<100; i++){
        if(comando[i] == '\"'){ //Reviso las comillas para poder identificar las cadenas,pues probablemente traen espacios
            if(cadena){
                cadena=false;
            }
            else{
            cadena=true;
            cambio=true;
            }
            
        }
        if(cadena && comando[i] == ' '){
            comando[i]='$'; //Si me encuentro dentro de una cadena y encuentro un espacio, lo sustituyo temporalmente por un $
        }
    }

    

    return comando;
}

string regresarEspacio(string ruta){
    
    for(int i=0; i<ruta.length(); i++){
        if(ruta.at(i) == '$'){
            ruta.at(i) = ' ';
        }
        
    }
    char *tmpr = new char[ruta.length()];
    strcpy(tmpr,ruta.c_str());
    if(tmpr[0] == '\"'){
        ruta.replace(0,1,"");
        ruta.replace(ruta.length()-1,1,"");

    }
    

    return ruta;
}

void verifyDirectory (string ruta){
    
    ruta=ruta.substr(0, ruta.find_last_of("\\/"));
    char* folder = new char[ruta.length()];
    strcpy(folder, ruta.c_str());

    if (mkdir(folder, 0777) == -1){
        //cerr << "Error :  " << strerror(errno) << endl;
    }
 
    else
        cout << "Aviso: Se creó un directorio nuevo"<<endl;
   
}

bool existsFile(string name){
    ifstream ifile;
    ifile.open(name);
    if (ifile){
        return true;
    }
    return false;
}

bool crearArchivo(string ruta, int tamano){
try{

if(!existsFile(ruta)){
        FILE *arch = fopen(ruta.c_str(),"wb"); //Creo el nuevo archivo de profesores
        char ceros[tamano]; //Lleno mi arreglo de 0
        for (int i=0 ; i<tamano; i++){
            ceros[i] = 0;
            
        }
        fwrite(&ceros, sizeof(ceros),1,arch); //Escribo los 0s en el archivo
        fclose(arch);
        return true;
    }
    else{
        cout<<"Error: Ya existe un disco duro con ese nombre"<<endl;
        return false;
    }
    }catch(exception e){

}
return false;
}

/*Funcion para analizar el comando de mkdisk*/
void mkdisk(char *parametros){
    //Banderas para determinar si los parámetros fueron recibidos
    bool fsize, funit, fpath, ffit = false;

    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, " ");
    //Inicializamos nuestro disco
    MBR mbr;
    int size=0;
    string path;
    char unit, fit;
    while(parametros!=NULL){
        string tmp = parametros;
        string tipo = get_tipo_parametro(tmp);
        string valor = get_valor_parametro(tmp);

        if(tipo == ">size"){
            size = stoi(valor);
            if(size > 0){
                fsize=true;
            }
            else{
                cout<<"Error: Tamaño de disco inválido"<<endl;
                break;
            }
        }
        else if(tipo == ">unit"){
            if(valor=="k" || valor=="K"){
                unit = 'k';
                funit=true;
            }
            else if(valor=="m" || valor=="M"){ 
                unit = 'm';
                funit=true;
            }
            else{
                cout<<"Error: Valor no válido para >unit"<<endl;
            }
        }

        else if(tipo==">fit"){
            char* aj = new char[valor.length()];
            strcpy(aj,valor.c_str());
            if(strcasecmp(aj,"bf")==0){
                //Mejor ajuste
                fit='B';
                ffit=true;
            }
            else if(strcasecmp(aj,"ff")==0){
                //Primer ajuste
                fit='F';
                ffit=true;
            }
            else if(strcasecmp(aj,"wf")==0){
                //Peor ajuste
                fit='W';
                ffit=true;
            }
            else{
                cout<<"Error: Valor de ajuste inválido"<<endl;
            }
        }

        else if(tipo==">path"){
            valor=regresarEspacio(valor); //Regreso el espacio reemplazado al inicio
        
            verifyDirectory(valor); //Verifico si el directorio no existe para crearlo
            path=valor;
            fpath=true;
        }

        else{
            cout<<"Error: Parámetro inválido"<<endl;
        }
        parametros = strtok(NULL, " ");
    }

    if(fsize && fpath){
        if(!funit){
            size = size*1024*1024; //Si no está el parámetro de unidades, se toma mb 
        }
        else{
            if(unit=='k'){
                size = size*1024;
            }
            else{
                size = size*1024*1024; 
            }
        }

        if(!ffit){{
            fit='F';
        }
        mbr.mbr_tamanio=size;
        mbr.dsk_fit=fit;

        time_t t = time(nullptr);
        mbr.mbr_fecha_creacion = t;

        srand(time(0)); //Seed para números realmente aleatorios
        mbr.mbr_dsk_signature = rand();

        if(crearArchivo(path,size)){
            char *ruta = new char[path.length()];
            strcpy(ruta,path.c_str());
            FILE *arch = fopen(ruta,"rb+"); //Abro el archivo
            fseek(arch,0,SEEK_SET);
            fwrite(&mbr, sizeof(mbr),1,arch); //Escribo lel mbr en el archivo
            fclose(arch);

            cout<<"¡Disco creado con éxito!\n"<<endl;
        }


    }
    }
    else{
        cout<<"Error: No es posible crear el disco duro, faltan parámetros obligatorios"<<endl;
    }
  

}




void leerScript(string nombre){
    try{
    fstream script;
    script.open(nombre,ios::in);
    string comand;
    while(getline(script, comand)){
        cout<<comand<<endl;
        char ccomand[100];
        strcpy(ccomand, comand.c_str());

        analizar(ccomand); //Analiza cada comando del script
        
    }
    script.close();
    }catch(exception e ){
        cout<<"Error: No es posible leer el script"<<endl;
    }
}


void execute(char* parametros){
    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, " ");
    string path;
    while(parametros!=NULL){

        string tmp = parametros;
        string tipo = get_tipo_parametro(tmp);
        string valor = get_valor_parametro(tmp);

        if(tipo == ">path"){
            path=valor;
            leerScript(path);
        }
        else{
            cout<<"Error: Parámetro no válido"<<endl;
        }

        parametros = strtok(NULL," ");
    }
}

MBR firstFit(MBR mbr, int psize, char name[16]){
    int ocupado;
    /*
    if(mbr.partition1.part_s>0){
        ocupado+=mbr.partition1.part_s;

        
        if(mbr.partition2.part_s>0){
            ocupado+=mbr.partition2.part_s;


            if(mbr.partition3.part_s>0){
                ocupado+=mbr.partition3.part_s;


                if(mbr.partition4.part_s>0){
                    cout<<"Error: Límite de particiones alcanzado"<<endl;
                }
                else{
                    if((psize+ocupado) < mbr.mbr_tamanio){
                    mbr.partition4.part_s=psize;
                    char* nombre;
                    strcpy(mbr.partition4.part_name, name);
                    cout<<"¡Partición creada con éxito!"<<endl;
                    return mbr;
                    }
                    else{
                        cout<<"Error: Espacio insuficiente en el disco"<<endl;
                    }
                }



            }
            else{
                if((psize+ocupado) < mbr.mbr_tamanio){
                mbr.partition3.part_s=psize;
                char* nombre;
                strcpy(mbr.partition3.part_name, name);
                cout<<"¡Partición creada con éxito!"<<endl;
                return mbr;
                }
                else{
                    cout<<"Error: Espacio insuficiente en el disco"<<endl;
                }
            }


        }
        else{
            if((psize+ocupado) < mbr.mbr_tamanio){
            mbr.partition2.part_s=psize;
            char* nombre;
            strcpy(mbr.partition2.part_name, name);
            cout<<"¡Partición creada con éxito!"<<endl;
            return mbr;
            }
            else{
                cout<<"Error: Espacio insuficiente en el disco"<<endl;
            }
        }

    }
    else{
        if(psize < mbr.mbr_tamanio){
        mbr.partition1.part_s=psize;
        char* nombre;
        strcpy(mbr.partition1.part_name, name);
        cout<<"¡Partición creada con éxito!"<<endl;
        return mbr;
        }
        else{
            cout<<"Error: Espacio insuficiente en el disco"<<endl;
        }
        
    }
    */
    return mbr;
}

void fdisk(char* parametros){
    parametros = strtok(NULL, " ");
    bool fsize, funit, fpath, fname = false;
    int size;
    char unit;
    string path;
    char name[16];
    while(parametros!=NULL){
        string tmp = parametros;
        string tipo = get_tipo_parametro(tmp);
        string valor = get_valor_parametro(tmp);

        if(tipo==">size"){
            size = stoi(valor);
            fsize=true;
        }
        else if(tipo==">unit"){
            if(valor=="b" || valor=="B"){
                unit='b';
                funit=true;
            }
            else if(valor=="k" || valor=="K"){
                unit='k';
                funit=true;
            }
            else if(valor=="m" || valor=="M"){
                unit='m';
                funit=true;
            }
            else{
                cout<<"Error: Parámetro inválido"<<endl;
                break;
            }
        }
        else if(tipo==">path"){
            valor=regresarEspacio(valor); //Regreso el espacio reemplazado al inicio
            if(existsFile(valor)){
              
                path=valor;
                fpath=true;
            }
            else{
                cout<<"Error: No existe el disco duro"<<endl;
                break;
            }
        }

        else if(tipo==">name"){
            strcpy(name,valor.c_str());
           
            fname=true;
        }

        else{
            cout<<"Error: Parámetro no válido"<<endl;
        }
        parametros = strtok(NULL, " ");
    }

    if(fsize && fpath && fname){
        
        if(!funit){
            size = size*1024; //Si falta el parámetro, se toma k
        }
        else{
            if(unit=='k'){
                size = size*1024;
            }
            else{
                size = size*1024*1024;
            } //Si la unidad es b, no hay más acciones que tomar
        }

        char *ruta = new char[path.length()];
        
        strcpy(ruta,path.c_str());
     
        FILE *archivoe= fopen(ruta,"rb+");
        fseek(archivoe,0,SEEK_SET);//Coloco el puntero al inicio del archivo
        MBR mbr;
        fread(&mbr, sizeof(mbr),1,archivoe);
        mbr=firstFit(mbr,size,name);

        fseek(archivoe,0,SEEK_SET);//Coloco el puntero al inicio del archivo
        fwrite(&mbr,sizeof(mbr),1,archivoe);

   
    fclose(archivoe);

    }
}

void rep(){
    ofstream file("rep.dot");
    if(!file){
        cout<<"Error al abrir el archivo"<<endl;
    }
    file<<"digraph G{"<<endl;
    file<<"a0[shape=none label=<"<<endl;
    file<<"<TABLE cellspacing=\"0\" cellpadding=\"0\">"<<endl;
    file<<"<TR>"<<endl;
    file<<"<TD bgcolor=\"#FFF300\">MBR</TD>"<<endl;
    file<<"<TD bgcolor=\"#39F91A\">Partición 1</TD>"<<endl;
    file<<"<TD bgcolor=\"#39F91A\">Partición 2</TD>"<<endl;
    file<<"<TD bgcolor=\"#39F91A\">Partición 3</TD>"<<endl;
    file<<"<TD bgcolor=\"#39F91A\">Partición 4</TD>"<<endl;
    file<<"</TR>"<<endl;
    file<<"</TABLE>"<<endl;
    file<<">]"<<endl;
    file<<"}"<<endl;
    file.close();

    system("dot -Tpng rep.dot -o rep.png");


}


/*Funcion que define que comando es el que hay que ejecutar*/
void analizar(char *comando) {
    comando = espacioCadena(comando); //El comando con las cadenas unidas

    char *token = strtok(comando, " ");
    if(strcasecmp(token, "mkdisk") == 0){
        mkdisk(token);
    }
    else if(strcasecmp(token,"execute")==0){
        execute(token);
    }
    else if(strcasecmp(token,"fdisk")==0){
        fdisk(token);
    }
    else if(strcasecmp(token,"rep")==0){
        rep();
    }
    else {
        cout << "Error: Comando no aceptado" << endl;
    }
}