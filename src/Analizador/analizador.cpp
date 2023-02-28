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
    if(parametro[0]=='#'){
        return parametro;
    }
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
        FILE *arch = fopen(ruta.c_str(),"wb"); //Creo el nuevo archivo
        //char ceros[tamano]; //Lleno mi arreglo de 0
        /*ARREGLO: Se cambia el método pues los arreglos tienen límite, al parecer c:*/
        char cero = 0;
        //cout<<"Hola"<<endl;
        //for (int i=0 ; i<tamano; i++){
          //  ceros[i] = 0;
            
        //}
        //cout<<ceros<<endl;
      for (int i=0 ; i<tamano; i++){
        fwrite(&cero, sizeof(cero),1,arch); //Escribo los 0s en el archivo
      }
        
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

        else if(tipo[0] == '#'){
            //Si viene un comentario, no pasa nada
            break;
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

void rmdisk(char* parametros){
    parametros = strtok(NULL," ");
    string path;
    while(parametros!=NULL){
        string tmp = parametros;
        string tipo = get_tipo_parametro(tmp);
        string valor = get_valor_parametro(tmp);

        if(tipo == ">path"){
            valor=regresarEspacio(valor);
            path=valor;
            
        }
        else if(tipo[0] == '#'){
            //Si viene un comentario, no pasa nada
            break;
        }
        else{
            cout<<"Error: Parámetro no válido"<<endl;
        }
        parametros = strtok(NULL, " ");
    }
    removeFile(path);
}

void removeFile(string path){
    char op;
    cout<<"Desea eliminar el disco "<<path<<"? [S/N] "<<endl;
    cin>>op;

    if(op=='S'||op=='s'){
        //Se elimina
        char*ruta = new char[path.length()];
        strcpy(ruta,path.c_str());
        try{
            if(existsFile(path)){
                remove(ruta);
                cout<<"¡Disco duro eliminado!"<<endl;
            }
            else{
                cout<<"Error: No existe el disco duro"<<endl;
            }
            
        }
        catch(exception e){
            cout<<"Error: No se puede eliminar el disco"<<endl;
        }
    }
    else if(op=='N'|| op=='n'){
        //No se elimina
        cout<<"Eliminación de disco cancelada"<<endl;
    }
    else{
        cout<<"Error: Confirmación no válida"<<endl;

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
        else if(tipo[0] == '#'){
            //Si viene un comentario, no pasa nada
            break;
        }
        else{
            cout<<"Error: Parámetro no válido"<<endl;
        }

        parametros = strtok(NULL," ");
    }
}

MBR ajustar(MBR mbr, int psize, char name[16], char type, char fit, FILE *archivo){
    bool extendida=false;
    int ocupado, iniEbr, sizeEbr;
    for (int i=0; i<4; i++){
        //Si ya existe una particion extendida, no será posible crear otra
        if(mbr.particiones[i].part_type=='E'){
            extendida=true;
            iniEbr=mbr.particiones[i].part_start;
            sizeEbr=mbr.particiones[i].part_s;
        }
    }

    if(type=='P'){
        //Partición primaria
        if(mbr.dsk_fit=='F'){
            mbr = firstFitP(mbr, psize, name, fit);
        }



    }
    else if(type=='E'){
        //Partición extendida
        //Si ya existe una partición extendida, no se puede crear.

        if(extendida){
            cout<<"Error: Ya existe una partición extendida en el disco"<<endl;
        }
        else{
            if(mbr.dsk_fit=='F'){
                firstFitE(mbr, psize, name,fit, archivo);
            }
        }

    }
    else{
        //Si la partición es lógica, se verifica que exista una partición extendida
        if(extendida){
            EBR ebr;
            fseek(archivo,iniEbr,SEEK_SET);
            fread(&ebr, sizeof(ebr),1,archivo);
            //if(mbr.dsk_fit=='F'){
                firstFitL(ebr, psize, name, fit, sizeEbr, archivo);
            //}
        }
        else{
            cout<<"Error: No se puede crear una partición lógica si no existe una partición extendida"<<endl;
        }
    }
    return mbr;
}

MBR firstFitP(MBR mbr, int psize, char name[16], char fit){
    int ocupado = sizeof(mbr);
    int pos=-1; //Posición donde será insertada la partición
    Particion particion;
    particion.part_status='A';
    particion.part_type='P';
    particion.part_fit=fit;
    particion.part_s=psize;
    strcpy(particion.part_name, name);
    for(int i=0; i<4; i++){
        
        if(mbr.particiones[i].part_status=='I'){ //Si la partición está libre
            if(mbr.particiones[i].part_s==0){ //Si el tamaño es 0, es decir, nunca ha sido ocupado
                pos = i;
            }
            
            else if(mbr.particiones[i].part_s>=particion.part_s){ //O si el espacio es mayor o igual al de la nueva partición
                pos = i;
            }
        }
        else{
            ocupado+=mbr.particiones[i].part_s;
        }
    }

    if(pos>=0){
        //Si se pudo encontrar el lugar de la partición
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
            //Si la partición no pasa el límite del disco

            if(mbr.particiones[pos].part_start==0){ //Si el espacio todavía no tiene inicio asignado
            if(pos==0){
                particion.part_start = sizeof(mbr);
            }
            else if(pos==1){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s;
            }
            else if(pos==2){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s;
            }
            else if(pos==3){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s + mbr.particiones[2].part_s;
            }
            }

            mbr.particiones[pos] = particion;
        }
        else{
            cout<<"Error: No hay espacio suficiente para la partición"<<endl;
        }
    }
    else{
        cout<<"Error: No se puede sobrepasar el límite de particiones primarias"<<endl;
    }


    return mbr;
}

MBR firstFitE(MBR mbr, int psize, char name[16], char fit,  FILE *archivo){
    int ocupado = sizeof(mbr);
    int pos=-1; //Posición donde será insertada la partición
    Particion particion;
    particion.part_status='A';
    particion.part_type='E';
    particion.part_fit=fit;
    particion.part_s=psize;
    
    strcpy(particion.part_name, name);
    for(int i=0; i<4; i++){
        
        if(mbr.particiones[i].part_status=='I'){ //Si la partición está libre
            if(mbr.particiones[i].part_s==0){ //Si el tamaño es 0, es decir, nunca ha sido ocupado
                pos = i;
            }
            
            else if(mbr.particiones[i].part_s>=particion.part_s){ //O si el espacio es mayor o igual al de la nueva partición
                pos = i;
            }
        }
        else{
            ocupado+=mbr.particiones[i].part_s;
        }
    }

    if(pos>=0){
        //Si se pudo encontrar el lugar de la partición
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
            //Si la partición no pasa el límite del disco

            if(mbr.particiones[pos].part_start==0){ //Si el espacio todavía no tiene inicio asignado
            if(pos==0){
                particion.part_start = sizeof(mbr);
            }
            else if(pos==1){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s;
            }
            else if(pos==2){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s;
            }
            else if(pos==3){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s + mbr.particiones[2].part_s;
            }
            }

            mbr.particiones[pos] = particion;

            //EBR inicial

            
            EBR ebr;
            ebr.part_status = 'I';
            ebr.part_start=mbr.particiones[pos].part_start;
            ebr.part_next=-1;

            fseek(archivo, ebr.part_start,SEEK_SET);
            fwrite(&ebr, sizeof(ebr),1,archivo);
        }
        else{
            cout<<"Error: No hay espacio suficiente para la partición"<<endl;
        }
    }
    else{
        cout<<"Error: No se puede sobrepasar el límite de 4 particiones primarias y extendidas"<<endl;
    }


    
    return mbr;
}

void firstFitL(EBR inicial, int psize, char name[16], char fit, int limite, FILE *archivo){
    int ocupado = sizeof(inicial);
    int pos=-1; //Posición donde será insertada la partición
    int libre, size;
    EBR particion;
    particion.part_status='A';
    particion.part_fit=fit;
    particion.part_s=psize;
    strcpy(particion.part_name, name);
    if(inicial.part_status='I'){
        //Si el ebr inicial está libre, se utiliza este
        if(ocupado+particion.part_s<=limite){
            inicial = particion;
        }
        else{
            cout<<"Error: No se puede sobrepasar el tamaño de la partición extendida"<<endl;
        }
    }
    else{
        EBR tmp;
        while(tmp.part_next!=-1){

            
            ocupado=ocupado+(tmp.part_s - sizeof(EBR));
            fseek(archivo, tmp.part_next,SEEK_SET);
            fread(&tmp,sizeof(tmp), 1, archivo);
        }

        if(ocupado+particion.part_s<=limite){
            particion.part_start=ocupado;
            inicial = particion;
        }
        else{
            cout<<"Error: No se puede sobrepasar el tamaño de la partición extendida"<<endl;
        }
    }
}

MBR bestFitP(MBR mbr, int psize, char name[16], char fit){
    int ocupado = sizeof(mbr);
    int mejor;
    int pos=-1; //Posición donde será insertada la partición
    Particion particion;
    particion.part_status='A';
    particion.part_type='P';
    particion.part_fit=fit;
    particion.part_s=psize;
    strcpy(particion.part_name, name);
    for(int i=0; i<4; i++){
        
        if(mbr.particiones[i].part_status=='I'){ //Si la partición está libre
            
            if(mbr.particiones[i].part_s==0){ //Si el tamaño es 0, es decir, nunca ha sido ocupado
                pos = i;
                break;
            }
            
            else{ //Si la nueva partición cabe y sobra el menor espacio posible
                if(mbr.particiones[i].part_s>=particion.part_s){
                    if(mejor==0){
                        mejor = mbr.particiones[i].part_s;
                        pos=i;
                    }
                    else if(mbr.particiones[i].part_s<=mejor){
                        pos = i;
                        mejor = mbr.particiones[i].part_s;
                    }

                    
                }
                
            }
        }
        else{
            ocupado+=mbr.particiones[i].part_s;
        }
    }

    if(pos>=0){
        //Si se pudo encontrar el lugar de la partición
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
            //Si la partición no pasa el límite del disco

            if(mbr.particiones[pos].part_start==0){ //Si el espacio todavía no tiene inicio asignado
            if(pos==0){
                particion.part_start = sizeof(mbr);
            }
            else if(pos==1){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s;
            }
            else if(pos==2){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s;
            }
            else if(pos==3){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s + mbr.particiones[2].part_s;
            }
            }

            mbr.particiones[pos] = particion;
        }
        else{
            cout<<"Error: No hay espacio suficiente para la partición"<<endl;
        }
    }
    else{
        cout<<"Error: No se puede sobrepasar el límite de particiones primarias"<<endl;
    }


    return mbr;
}

MBR bestFitE(MBR mbr, int psize, char name[16], char fit,  FILE *archivo){
    int ocupado = sizeof(mbr);
    int mejor;
    int pos=-1; //Posición donde será insertada la partición
    Particion particion;
    particion.part_status='A';
    particion.part_type='E';
    particion.part_fit=fit;
    particion.part_s=psize;
    
    strcpy(particion.part_name, name);
    for(int i=0; i<4; i++){
        
        if(mbr.particiones[i].part_status=='I'){ //Si la partición está libre
            if(mbr.particiones[i].part_s==0){ //Si el tamaño es 0, es decir, nunca ha sido ocupado
                pos = i;
                break;
            }
            
              else{ //Si la nueva partición cabe y sobra el menor espacio posible
                if(mbr.particiones[i].part_s>=particion.part_s){
                    if(mejor==0){
                        mejor = mbr.particiones[i].part_s;
                        pos=i;
                    }
                    else if(mbr.particiones[i].part_s<=mejor){
                        pos = i;
                        mejor = mbr.particiones[i].part_s;
                    }
                
            }
              }
        }
        
        else{
            ocupado+=mbr.particiones[i].part_s;
        }
    }

    if(pos>=0){
        //Si se pudo encontrar el lugar de la partición
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
            //Si la partición no pasa el límite del disco

            if(mbr.particiones[pos].part_start==0){ //Si el espacio todavía no tiene inicio asignado
            if(pos==0){
                particion.part_start = sizeof(mbr);
            }
            else if(pos==1){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s;
            }
            else if(pos==2){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s;
            }
            else if(pos==3){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s + mbr.particiones[2].part_s;
            }
            }

            mbr.particiones[pos] = particion;

            //EBR inicial

            
            EBR ebr;
            ebr.part_status = 'I';
            ebr.part_start=mbr.particiones[pos].part_start;
            ebr.part_next=-1;

            fseek(archivo, ebr.part_start,SEEK_SET);
            fwrite(&ebr, sizeof(ebr),1,archivo);
        }
        else{
            cout<<"Error: No hay espacio suficiente para la partición"<<endl;
        }
    }
    else{
        cout<<"Error: No se puede sobrepasar el límite de 4 particiones primarias y extendidas"<<endl;
    }


    
    return mbr;
}

MBR worstFitP(MBR mbr, int psize, char name[16], char fit){
    int ocupado = sizeof(mbr);
    int mejor;
    int pos=-1; //Posición donde será insertada la partición
    Particion particion;
    particion.part_status='A';
    particion.part_type='P';
    particion.part_fit=fit;
    particion.part_s=psize;
    strcpy(particion.part_name, name);
    for(int i=0; i<4; i++){
        
        if(mbr.particiones[i].part_status=='I'){ //Si la partición está libre
            
            if(mbr.particiones[i].part_s==0){ //Si el tamaño es 0, es decir, nunca ha sido ocupado
                pos = i;
                break;
            }
            
            else{ //Si la nueva partición cabe y sobra el menor espacio posible
                if(mbr.particiones[i].part_s>=particion.part_s){
                    if(mejor==0){
                        mejor = mbr.particiones[i].part_s;
                        pos=i;
                    }
                    else if(mbr.particiones[i].part_s>=mejor){
                        pos = i;
                        mejor = mbr.particiones[i].part_s;
                    }

                    
                }
                
            }
        }
        else{
            ocupado+=mbr.particiones[i].part_s;
        }
    }

    if(pos>=0){
        //Si se pudo encontrar el lugar de la partición
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
            //Si la partición no pasa el límite del disco

            if(mbr.particiones[pos].part_start==0){ //Si el espacio todavía no tiene inicio asignado
            if(pos==0){
                particion.part_start = sizeof(mbr);
            }
            else if(pos==1){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s;
            }
            else if(pos==2){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s;
            }
            else if(pos==3){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s + mbr.particiones[2].part_s;
            }
            }

            mbr.particiones[pos] = particion;
        }
        else{
            cout<<"Error: No hay espacio suficiente para la partición"<<endl;
        }
    }
    else{
        cout<<"Error: No se puede sobrepasar el límite de particiones primarias"<<endl;
    }


    return mbr;
}

MBR worstFitE(MBR mbr, int psize, char name[16], char fit,  FILE *archivo){
    int ocupado = sizeof(mbr);
    int mejor;
    int pos=-1; //Posición donde será insertada la partición
    Particion particion;
    particion.part_status='A';
    particion.part_type='E';
    particion.part_fit=fit;
    particion.part_s=psize;
    
    strcpy(particion.part_name, name);
    for(int i=0; i<4; i++){
        
        if(mbr.particiones[i].part_status=='I'){ //Si la partición está libre
            if(mbr.particiones[i].part_s==0){ //Si el tamaño es 0, es decir, nunca ha sido ocupado
                pos = i;
                break;
            }
            
              else{ //Si la nueva partición cabe y sobra el menor espacio posible
                if(mbr.particiones[i].part_s>=particion.part_s){
                    if(mejor==0){
                        mejor = mbr.particiones[i].part_s;
                        pos=i;
                    }
                    else if(mbr.particiones[i].part_s>=mejor){
                        pos = i;
                        mejor = mbr.particiones[i].part_s;
                    }
                
            }
              }
        }
        
        else{
            ocupado+=mbr.particiones[i].part_s;
        }
    }

    if(pos>=0){
        //Si se pudo encontrar el lugar de la partición
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
            //Si la partición no pasa el límite del disco

            if(mbr.particiones[pos].part_start==0){ //Si el espacio todavía no tiene inicio asignado
            if(pos==0){
                particion.part_start = sizeof(mbr);
            }
            else if(pos==1){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s;
            }
            else if(pos==2){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s;
            }
            else if(pos==3){
                particion.part_start = sizeof(mbr) + mbr.particiones[0].part_s + mbr.particiones[1].part_s + mbr.particiones[2].part_s;
            }
            }

            mbr.particiones[pos] = particion;

            //EBR inicial

            
            EBR ebr;
            ebr.part_status = 'I';
            ebr.part_start=mbr.particiones[pos].part_start;
            ebr.part_next=-1;

            fseek(archivo, ebr.part_start,SEEK_SET);
            fwrite(&ebr, sizeof(ebr),1,archivo);
        }
        else{
            cout<<"Error: No hay espacio suficiente para la partición"<<endl;
        }
    }
    else{
        cout<<"Error: No se puede sobrepasar el límite de 4 particiones primarias y extendidas"<<endl;
    }


    
    return mbr;
}

void fdisk(char* parametros){
    parametros = strtok(NULL, " ");
    bool fsize, funit, fpath, fname, ftype, ffit, fdelete, fadd = false;
    int size, add;
    char unit, type, fit;
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
            valor=regresarEspacio(valor);
            strcpy(name,valor.c_str());
           
            fname=true;
        }

        else if(tipo==">type"){
            if(valor=="p" || valor=="P"){
                type='P';
                ftype=true;
            }
            else if(valor=="e" || valor=="E"){
                type='E';
                ftype=true;
            }
            else if(valor=="l" || valor=="L"){
                type='L';
                ftype=true;
            }
            else{
                cout<<"Error: Tipo de partición inválido"<<endl;
            }
        }

        else if(tipo==">fit"){
            char* aj = new char[valor.length()];
            if(strcasecmp(aj,"BF")==0){
                fit='B';
                ffit=true;
            }
            else if(strcasecmp(aj,"FF")==0){
                ffit='F';
                ffit=true;
            }
            else if(strcasecmp(aj,"WF")==0){
                ffit='W';
                ffit=true;
            }
            else{
                cout<<"Error: Valor de ajuste inválido"<<endl;
            }
        }

        else if(tipo==">delete"){
            char * del = new char[valor.length()];
            if(strcasecmp(del,"full")==0){
                fdelete=true;
            }
            else{
                cout<<"Parámetro delete inválido"<<endl;
            }
        }

        else if(tipo==">add"){
            try{
                add = stoi(valor);
                fadd=true;
            }
            catch(exception e){
                cout<<"Error: Valor de >add no numérico"<<endl;
                
            }
        }

        else if(tipo[0] == '#'){
            //Si viene un comentario, no pasa nada
            break;
        }
        

        else{
            cout<<"Error: Parámetro no válido"<<endl;
        }
        parametros = strtok(NULL, " ");
    }

    /*Para la creación de particiones*/
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
        if(existsFile(path)){
            FILE *archivoe= fopen(ruta,"rb+");
            fseek(archivoe,0,SEEK_SET);//Coloco el puntero al inicio del archivo
            MBR mbr;
            fread(&mbr, sizeof(mbr),1,archivoe);
            ajustar(mbr, size, name, type, fit, archivoe);
            

            fseek(archivoe,0,SEEK_SET);//Coloco el puntero al inicio del archivo
            fwrite(&mbr,sizeof(mbr),1,archivoe);

    
            fclose(archivoe);
        }
        else{
            cout<<"Error: No se encontró el disco duro"<<endl;
        }


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
    else if(strcasecmp(token,"rmdisk")==0){
        rmdisk(token);
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
    else if (token[0]=='#'){
        //Si es un comentario, no pasa nada

    }
    else {
        cout << "Error: Comando no aceptado" << endl;
    }
}