#include "analizador.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <cstdlib>
//#include "estructuras.h"

/*<id,ruta>*/
map <string,string> montadas;
map <string,string>::iterator it;

/*<id,nombreParticion>*/
map <string, string> nombres;

/*<nombreDisco,ruta>*/
map <string,string> rutas;


//lista.push_back("Hola","Adios");
//montadas["Hola"] = lista;
//map <string,list<pair<string,string>>>::iterator it;

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

        if(!ffit){
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
    //fstream script;
    //script.open(nombre,fstream::in);
    string comand;
    ifstream script(nombre);
    while(getline(script, comand)){
        cout<<"\n"<<endl;
        
        cout<<comand<<endl;
        if(comand.length()>5){ //En caso de saltos de linea
        char ccomand[500];
        strcpy(ccomand, comand.c_str());
        
        analizar(ccomand); //Analiza cada comando del script
        }
        
        
    }
    script.close();
    }catch(exception e ){
        cout<<"Error: No es posible leer el script"<<endl;
    }
}


void execute(char* parametros){
    //Pasamos a la siguiente posicion
    parametros = strtok(NULL, " ");
    list<string> lista_param;
    while(parametros!=NULL){

        lista_param.push_back(parametros);
        parametros = strtok(NULL, " ");
        //TODO: REVISAR EL PROBLEMA DE LAS LÓGICAS
    }
    string path;
    while(!lista_param.empty()){

        string tmp = lista_param.front();
        string tipo = get_tipo_parametro(tmp);
        string valor = get_valor_parametro(tmp);

        if(tipo == ">path"){
            
            path=regresarEspacio(valor);
            leerScript(path);
        }
        else if(tipo[0] == '#'){
            //Si viene un comentario, no pasa nada
            break;
        }
        else{
            cout<<"Error: Parámetro no válido"<<endl;
        }

        lista_param.pop_front();
    }
}





void ajustarP(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit){
    FILE *archivo= fopen(ruta,"rb+");
    fseek(archivo,0,SEEK_SET);//Coloco el puntero al inicio del archivo
    MBR mbr;
    fread(&mbr, sizeof(MBR),1,archivo);

    Particion particion;
    particion.part_status='1';
    particion.part_type='P';
    particion.part_fit=fit;
    particion.part_s=psize;
    strcpy(particion.part_name,name);

    int ocupado=sizeof(MBR);
    int pos=-1;

    int mejor, peor=0;
    int start=0;

    bool crear=true;

    if(mbr.particiones[0].part_status=='1' && mbr.particiones[1].part_status=='1' && mbr.particiones[2].part_status=='1' && mbr.particiones[3].part_status=='1'){
            cout<<"Error: Límite de 4 particiones primarias y extendidas alcanzado"<<endl;
            crear=false;
    }
    else{

    for(int i=0; i<4;i++){ //Encontrando el espacio ocupado actualmente
        if(mbr.particiones[i].part_status=='1'){
            ocupado+=mbr.particiones[i].part_s;
        }
    }    

    
    if(dsk_fit=='F'){

        //First Fit
        for(int i=0; i<4;i++){
            if(mbr.particiones[i].part_status=='0'){//Si la particion está libre
            if(mbr.particiones[i].part_s==0){//Si nunca ha sido utilizada
                if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    pos=i;
                   
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                
            }
            else{
                if(mbr.particiones[i].part_s>=particion.part_s){ //Si ya ha sido utilizada y cabe la nueva particion
                    if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    particion.part_start=mbr.particiones[i].part_start;
                    pos=i;
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                }
            }

            

            }
           
        }


        


    }
    else if(dsk_fit=='B'){
        //Best Fit
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
     
        for(int i=0; i<4;i++){
            if(mbr.particiones[i].part_status=='0'){//Si la particion está libre
            if(mbr.particiones[i].part_s==0){//Si nunca ha sido utilizada
                if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    pos=i;
                   
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                
            }
            else{
                if(mejor==0){
                    mejor=mbr.particiones[i].part_s-particion.part_s;
                    //particion.part_start=mbr.particiones[i].part_start;
                    //pos=i;
                }
                if(mbr.particiones[i].part_s>=particion.part_s){ //Si ya ha sido utilizada y cabe la nueva particion
                    if((mbr.particiones[i].part_s-particion.part_s)<=mejor){ //Si a esta posición le sobre menos
                    if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    mejor=mbr.particiones[i].part_s-particion.part_s;
                    particion.part_start=mbr.particiones[i].part_start;
                    pos=i;
                    
                    }
                    
                
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                }
            }
            }

            

            }
          
        }

        }
        else{
            cout<<"Error: Espacio insuficiente para la partición"<<endl;
            crear=false;
        }

    }
    else{
        //Worst Fit
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
     
        for(int i=0; i<4;i++){
            if(mbr.particiones[i].part_status=='0'){//Si la particion está libre
            if(mbr.particiones[i].part_s==0){//Si nunca ha sido utilizada
                if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    pos=i;
                   
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                
            }
            else{
                if(mejor==0){
                    peor=mbr.particiones[i].part_s-particion.part_s;
                    //particion.part_start=mbr.particiones[i].part_start;
                    //pos=i;
                }
                if(mbr.particiones[i].part_s>=particion.part_s){ //Si ya ha sido utilizada y cabe la nueva particion
                    if((mbr.particiones[i].part_s-particion.part_s)>=peor){ //Si a esta posición le sobre menos
                    if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    peor=mbr.particiones[i].part_s-particion.part_s;
                    particion.part_start=mbr.particiones[i].part_start;
                    pos=i;
                    
                    }
                    
                
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                }
            }
            }

            

            }
          
        }

        }
        else{
            cout<<"Error: Espacio insuficiente para la partición"<<endl;
            crear=false;
        }
    }

}

    if(crear){
        if(particion.part_start==0){
        particion.part_start=ocupado;
        }
        mbr.particiones[pos]=particion;
        cout<<"¡Partición primaria <"<<name<<"> creada!"<<endl;
        fseek(archivo,0,SEEK_SET); //Escribo el mbr actualizado
        fwrite(&mbr,sizeof(MBR),1,archivo); 

    }

    fclose(archivo);
}

void ajustarE(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit){
    bool extendida=false;
    FILE *archivo= fopen(ruta,"rb+");
    fseek(archivo,0,SEEK_SET);//Coloco el puntero al inicio del archivo
    MBR mbr;
    fread(&mbr, sizeof(MBR),1,archivo);
    //Verifico si ya existe una partición extendida
    for(int i=0; i<4; i++){
        if(mbr.particiones[i].part_type=='E'){
            extendida=true;
        }
    }
    Particion particion;
    particion.part_status='1';
    particion.part_type='E';
    particion.part_fit=fit;
    particion.part_s=psize;
    strcpy(particion.part_name,name);

    int ocupado=sizeof(MBR);
    int pos=-1;

    int mejor, peor=0;
    int start=0;

    bool crear=true;

    if(mbr.particiones[0].part_status=='1' && mbr.particiones[1].part_status=='1' && mbr.particiones[2].part_status=='1' && mbr.particiones[3].part_status=='1'){
            cout<<"Error: Límite de 4 particiones primarias y extendidas alcanzado"<<endl;
            crear=false;
    }
    else{
    if(!extendida){    
    for(int i=0; i<4;i++){ //Encontrando el espacio ocupado actualmente
        if(mbr.particiones[i].part_status=='1'){
            ocupado+=mbr.particiones[i].part_s;
        }
    }    

    
    if(dsk_fit=='F'){

        //First Fit
        for(int i=0; i<4;i++){
            if(mbr.particiones[i].part_status=='0'){//Si la particion está libre
            if(mbr.particiones[i].part_s==0){//Si nunca ha sido utilizada
                if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    pos=i;
                   
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                
            }
            else{
                if(mbr.particiones[i].part_s>=particion.part_s){ //Si ya ha sido utilizada y cabe la nueva particion
                    if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    particion.part_start=mbr.particiones[i].part_start;
                    pos=i;
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                }
            }

            

            }
           
        }


        


    }
    else if(dsk_fit=='B'){
        //Best Fit
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
     
        for(int i=0; i<4;i++){
            if(mbr.particiones[i].part_status=='0'){//Si la particion está libre
            if(mbr.particiones[i].part_s==0){//Si nunca ha sido utilizada
                if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    pos=i;
                   
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                
            }
            else{
                if(mejor==0){
                    mejor=mbr.particiones[i].part_s-particion.part_s;
                    //particion.part_start=mbr.particiones[i].part_start;
                    //pos=i;
                }
                if(mbr.particiones[i].part_s>=particion.part_s){ //Si ya ha sido utilizada y cabe la nueva particion
                    if((mbr.particiones[i].part_s-particion.part_s)<=mejor){ //Si a esta posición le sobre menos
                    if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    mejor=mbr.particiones[i].part_s-particion.part_s;
                    particion.part_start=mbr.particiones[i].part_start;
                    pos=i;
                    
                    }
                    
                
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                }
            }
            }

            

            }
          
        }

        }
        else{
            cout<<"Error: Espacio insuficiente para la partición"<<endl;
            crear=false;
        }

    }
    else{
        //Worst Fit
        if((ocupado+particion.part_s)<mbr.mbr_tamanio){
     
        for(int i=0; i<4;i++){
            if(mbr.particiones[i].part_status=='0'){//Si la particion está libre
            if(mbr.particiones[i].part_s==0){//Si nunca ha sido utilizada
                if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    pos=i;
                   
                    break;
                }
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                
            }
            else{
                if(mejor==0){
                    peor=mbr.particiones[i].part_s-particion.part_s;
                    //particion.part_start=mbr.particiones[i].part_start;
                    //pos=i;
                }
                if(mbr.particiones[i].part_s>=particion.part_s){ //Si ya ha sido utilizada y cabe la nueva particion
                    if((mbr.particiones[i].part_s-particion.part_s)>=peor){ //Si a esta posición le sobre menos
                    if((ocupado+particion.part_s)<mbr.mbr_tamanio){ //Si el tamaño ocupado cabe en el disco
                    peor=mbr.particiones[i].part_s-particion.part_s;
                    particion.part_start=mbr.particiones[i].part_start;
                    pos=i;
                    
                    }
                    
                
                else{
                    cout<<"Error: Espacio insuficiente para la partición"<<endl;
                    crear=false;
                    break;
                }
                }
            }
            }

            

            }
          
        }

        }
        else{
            cout<<"Error: Espacio insuficiente para la partición"<<endl;
            crear=false;
        }
    }
    }
    else{
        cout<<"Error: Solo puede existir una partición extendida en el disco duro"<<endl;
        crear=false;
    }
}

    if(crear){
        if(particion.part_start==0){
        particion.part_start=ocupado;
        }
        mbr.particiones[pos]=particion;
        
        fseek(archivo,0,SEEK_SET); //Escribo el mbr actualizado
        fwrite(&mbr,sizeof(MBR),1,archivo); 

        EBR ebr;
        ebr.part_start=particion.part_start;

        
        fseek(archivo,particion.part_start,SEEK_SET);
        fwrite(&ebr,sizeof(EBR),1,archivo);
        cout<<"¡Partición extendida <"<<name<<"> creada!"<<endl;

    }

    fclose(archivo);
}

void ajustarL(char* ruta, int psize, char name[16], char type, char fit, char dsk_fit){
    bool extendida, crear=false;
    int ocupado=0;
    FILE *archivo= fopen(ruta,"rb+");
    fseek(archivo,0,SEEK_SET);//Coloco el puntero al inicio del archivo
    MBR mbr;
    fread(&mbr, sizeof(MBR),1,archivo);

    Particion ext;

    EBR logica; //Nueva particióm
    logica.part_fit=fit;
    strcpy(logica.part_name,name);
    logica.part_status='1';
    logica.part_s=psize;

    //Verifico si ya existe una partición extendida
    for(int i=0; i<4; i++){
        if(mbr.particiones[i].part_type=='E'){
            extendida=true;
            ext = mbr.particiones[i];
            break;
        }
    }

    if(extendida){
        //Si existe una partición extendida, puedo crear lógicas

        
        EBR tmp, ultimo;
        fseek(archivo,ext.part_start,SEEK_SET); //Me muevo al inicio de la partición extendida para leer el EBR
        fread(&tmp, sizeof(EBR),1,archivo);

        fseek(archivo,ext.part_start,SEEK_SET);
        fread(&ultimo,sizeof(EBR),1,archivo);

          
        ocupado+=tmp.part_s;

        if(tmp.part_status=='0'){//Si el primer ebr no está usado, se inserta la nueva partición
            /*Actualizo el ebr*/
            if(ocupado+logica.part_s<=ext.part_s){ //Si cabe la partición
                logica.part_start=ext.part_start;
                fseek(archivo,ext.part_start,SEEK_SET);
                fwrite(&logica,sizeof(EBR),1,archivo);
                cout<<"¡Partición lógica <"<<name<<"> creada!"<<endl;
            }
            else{
                cout<<"Error: Espacio insuficiente para la partición"<<endl;
            }
        }
        else{
        
        while(tmp.part_next!=-1){
           
            fseek(archivo, tmp.part_next,SEEK_SET);
            fread(&tmp,sizeof(EBR),1,archivo); //Cambio a a la siguiente partición lógica
            ocupado+=tmp.part_s;
            if(tmp.part_next==-1){//Si encontró otro ebr
                memcpy(&ultimo,&tmp,sizeof(EBR));
                
            }
        }
        if(ocupado+logica.part_s<=ext.part_s){
        //Ya que llegué al final de la lista enlazada, puedo colocar la nueva partición
        logica.part_start = ultimo.part_start+ultimo.part_s; //Inicio de la nueva
        ultimo.part_next = logica.part_start; //Apuntador en el anterior

        fseek(archivo,ultimo.part_start,SEEK_SET);//Actualizo el último
        fwrite(&ultimo,sizeof(EBR),1,archivo);

        fseek(archivo,logica.part_start,SEEK_SET); //Escribo el nuevo
        fwrite(&logica,sizeof(EBR),1,archivo);
        cout<<"¡Partición lógica <"<<name<<"> creada!"<<endl;
        }
        else{
            cout<<"Error: Espacio insuficiente para crear la partición"<<endl;
        }
    }

    }
    else{
        cout<<"Error: No se puede crear una partición lógica si no existe una extendida"<<endl;
        crear=false;
    }

    fclose(archivo);
}

void ajustar(char* ruta, int psize, char name[16], char type, char fit){
    bool colocar=true;
    char dsk_fit;
    FILE *archivo= fopen(ruta,"rb+");
    fseek(archivo,0,SEEK_SET);//Coloco el puntero al inicio del archivo
    MBR mbr;
    fread(&mbr, sizeof(MBR),1,archivo);
    dsk_fit=mbr.dsk_fit;        

    //Primero verifico que no se repita el nombre de la partición en el disco
    for(int i=0; i<4; i++){
        if (strcasecmp(name,mbr.particiones[i].part_name)==0){
           
            cout<<"Error: Ya existe una partición llamada <"<<name<<">"<<endl;
            colocar=false;
            break;
        }
        if(mbr.particiones[i].part_type=='E'){//Si la partición es extendida, reviso en las particiones lógicas
            EBR tmp;
            fseek(archivo,mbr.particiones[i].part_start,SEEK_SET); //Me muevo al inicoi de la partición extendida para leer el EBR
            fread(&tmp, sizeof(EBR),1,archivo);

            if(strcasecmp(name,tmp.part_name)==0){
                    cout<<"Error: Ya existe una partición llamada <"<<name<<">"<<endl;
                    colocar=false;
                    break;
                
            }

            while(tmp.part_next!=-1){
                if(strcasecmp(name,tmp.part_name)==0){
                    cout<<"Error: Ya existe una partición llamada <"<<name<<">"<<endl;
                    colocar=false;
                    break;
                }
                fseek(archivo, tmp.part_next,SEEK_SET);
                fread(&tmp,sizeof(EBR),1,archivo); //Cambio a a la siguiente partición lógica

                if(tmp.part_next==-1){
                    if(strcasecmp(name,tmp.part_name)==0){
                    cout<<"Error: Ya existe una partición llamada <"<<name<<">"<<endl;
                    colocar=false;
                    break;
                }
                }
            }
        }
    }
    fclose(archivo); //Cierro el archivo para evitar conflictos

    if(colocar){
       
        if(type=='P'){
            ajustarP(ruta,psize,name,type,fit,dsk_fit);
        }
        else if(type=='E'){
            ajustarE(ruta,psize,name,type,fit,dsk_fit);
        }
        else if(type=='L'){
            ajustarL(ruta,psize,name,type,fit,dsk_fit);
        }
        
    }

    
}

void fdiskDelete(char* ruta, char name[16]){
    bool flogica=false; //Banderas para determinar si se encontró la partición y si es primaria, extendida o lógica
    bool encontrada=false;
    bool fprExt = false;
    Particion prExt; //Si la partición a eliminar es primaria o extendida
    EBR logica,ultima;
    MBR mbr;
    int pos;
    int iniEx;
    

    FILE *archivo= fopen(ruta,"rb+");
    fseek(archivo,0,SEEK_SET);
    fread(&mbr, sizeof(MBR),1,archivo);



    for(int i=0; i<4; i++){
        if (strcasecmp(name,mbr.particiones[i].part_name)==0){
           
            
            encontrada=true;
            fprExt=true;
            memcpy(&prExt,&mbr.particiones[i],sizeof(Particion));
            //prExt = mbr.particiones[i];
            pos=i;
            break;
        }
        if(mbr.particiones[i].part_type=='E'){//Si la partición es extendida, reviso en las particiones lógicas
            EBR tmp;
            
            fseek(archivo,mbr.particiones[i].part_start,SEEK_SET); //Me muevo al inicoi de la partición extendida para leer el EBR
            fread(&tmp, sizeof(EBR),1,archivo);

            fseek(archivo,mbr.particiones[i].part_start,SEEK_SET); //Me muevo al inicoi de la partición extendida para leer el EBR
            fread(&ultima, sizeof(EBR),1,archivo);

            if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    flogica=true;
                    //logica = tmp;
                    memcpy(&logica,&tmp,sizeof(EBR));
                    
                    iniEx = mbr.particiones[i].part_start;
                    break;
                
            }

            while(tmp.part_next!=-1){
               
                if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    flogica=true;
                    //logica = tmp;
                     memcpy(&logica,&tmp,sizeof(EBR));
                    iniEx = mbr.particiones[i].part_start;
                    break;
                }
                memcpy(&ultima,&tmp,sizeof(EBR));
                fseek(archivo, tmp.part_next,SEEK_SET);
                fread(&tmp,sizeof(EBR),1,archivo); //Cambio a a la siguiente partición lógica
                
                if(tmp.part_next==-1){
                    //memcpy(&ultima,&tmp,sizeof(EBR));
                    if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    flogica=true;
                    //logica = tmp;
                     memcpy(&logica,&tmp,sizeof(EBR));
                    iniEx = mbr.particiones[i].part_start;
                    break;
                }
                }
            }
        }
    }

    //Operaciones a realizar si se contró la partición
    if(encontrada){
    char op;
    cout<<"¿Desea eliminar la partición <"<<name<<">? [S/N] "<<endl;
    cin>>op;

    if(op=='S'||op=='s'){
        //Procedo a la eminicación
        if(fprExt){
            char nombre[16];
            strcpy(prExt.part_name,nombre);
            prExt.part_status='0';
            mbr.particiones[pos] = prExt;
            fseek(archivo,0,SEEK_SET);
            fwrite(&mbr,sizeof(MBR),1,archivo);
            

            char cero=0;
            fseek(archivo,prExt.part_start,0); //Me ubico al inicio de la partición
            for (int i=0 ; i<prExt.part_s; i++){ //Lleno de 0s hasta el final de la partición
                fwrite(&cero, sizeof(cero),1,archivo); //Escribo los 0s en el archivo
            
            }
            cout<<"Partición <"<<name<<"> eliminada!"<<endl;

        }
        else if(flogica){
            char cero=0;
            //Actualizo el puntero de la partición anterior a la eliminada
            ultima.part_next = logica.part_next;
            fseek(archivo,ultima.part_start,SEEK_SET);
            fwrite(&ultima,sizeof(EBR),1,archivo);
            

            /*Si el inicio de la partición coincide con la de la extendida, no elimino el ebr, solo el contenido de la particón lógica*/
            if(logica.part_start!=iniEx){
                fseek(archivo,logica.part_start,SEEK_SET);
                for (int i=0 ; i<logica.part_s; i++){ //Lleno de 0s hasta el final de la partición
                    fwrite(&cero, sizeof(cero),1,archivo); //Escribo los 0s en el archivo

                
                }
            }
            else{
                fseek(archivo,(logica.part_start+sizeof(EBR)),SEEK_SET);

                for (int i=0 ; i<(logica.part_s); i++){ //Lleno de 0s hasta el final de la partición
                fwrite(&cero, sizeof(cero),1,archivo); //Escribo los 0s en el archivo
            
                }
            }
            cout<<"Partición <"<<name<<"> eliminada!"<<endl;

    }
    }
    else if(op=='n' || op=='N'){
        cout<<"Elimincación de partición cancelada"<<endl;
    }
    else{
        cout<<"Error: Opción inválida"<<endl;
    }
    }
    else{
        cout<<"Error: No se encontró la partición"<<endl;
    }

    fclose(archivo);



}

void fdiskAdd(char* ruta, char name[16], int add){
    bool flogica=false; //Banderas para determinar si se encontró la partición y si es primaria, extendida o lógica
    bool encontrada=false;
    bool fprExt = false;
    Particion prExt; //Si la partición a ,odificar es primaria o extendida
    EBR logica,ultima;
    MBR mbr;
    int pos;
    int iniEx;


    FILE *archivo= fopen(ruta,"rb+");
    fseek(archivo,0,SEEK_SET);
    fread(&mbr, sizeof(MBR),1,archivo);



    for(int i=0; i<4; i++){
        if (strcasecmp(name,mbr.particiones[i].part_name)==0){
           
            
            encontrada=true;
            fprExt=true;
            memcpy(&prExt,&mbr.particiones[i],sizeof(Particion));
            //prExt = mbr.particiones[i];
            pos=i;
            break;
        }
        if(mbr.particiones[i].part_type=='E'){//Si la partición es extendida, reviso en las particiones lógicas
            EBR tmp;
            
            fseek(archivo,mbr.particiones[i].part_start,SEEK_SET); //Me muevo al inicoi de la partición extendida para leer el EBR
            fread(&tmp, sizeof(EBR),1,archivo);

            fseek(archivo,mbr.particiones[i].part_start,SEEK_SET); //Me muevo al inicoi de la partición extendida para leer el EBR
            fread(&ultima, sizeof(EBR),1,archivo);

            if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    flogica=true;
                    //logica = tmp;
                    pos=i;
                    memcpy(&logica,&tmp,sizeof(EBR));
                    iniEx = mbr.particiones[i].part_start;
                    break;
                
            }

            while(tmp.part_next!=-1){
               
                if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    flogica=true;
                    //logica = tmp;
                    pos=i;
                     memcpy(&logica,&tmp,sizeof(EBR));
                    iniEx = mbr.particiones[i].part_start;
                    break;
                }
                memcpy(&ultima,&tmp,sizeof(EBR));
                fseek(archivo, tmp.part_next,SEEK_SET);
                fread(&tmp,sizeof(EBR),1,archivo); //Cambio a a la siguiente partición lógica
                
                if(tmp.part_next==-1){
                    //memcpy(&ultima,&tmp,sizeof(EBR));
                    if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    flogica=true;
                    pos=i;
                    //logica = tmp;
                     memcpy(&logica,&tmp,sizeof(EBR));
                    iniEx = mbr.particiones[i].part_start;
                    break;
                }
                }
            }
        }
    }

    if(encontrada){
        
        if(fprExt){ //Si es primaria o extendida
            int disponible;
            if((prExt.part_s+add)>=0){ //Si el tamaño resultante es positivo
            
            if((pos+1)<=2){ //Si todavía no es la última posición
                if(mbr.particiones[pos+1].part_status=='1'){
                    cout<<"Error: No hay espacio contiguo para agregar espacio"<<endl;
                }
                else{
                    if(mbr.particiones[pos+1].part_s!=0){
                    //La partición de la derecha está libre pero ya ha sido ocupada
                    if((prExt.part_start+prExt.part_s+add)<=(mbr.particiones[pos+1].part_start+mbr.particiones[pos+1].part_s))
                    { //si cabe en la partición siguiente
                        prExt.part_s = prExt.part_s+add; //Sumo el nuevo espacio
                        mbr.particiones[pos] = prExt;
                        
                        
                        mbr.particiones[pos+1].part_start = (mbr.particiones[pos+1].part_start+add); //Corro el inicio de esta partición
                        mbr.particiones[pos+1].part_s = (mbr.particiones[pos+1].part_s-add); //Resto el espacio

                        fseek(archivo,0,SEEK_SET); //Actualizo el mbr
                        fwrite(&prExt,sizeof(MBR),1,archivo);
                        cout<<"¡Tamaño de la partición actualizado!"<<endl;
                    }
                    else{
                        cout<<"No hay espacio libre suficiente"<<endl;
                    }
                }
                    else{
                        //La partición de la derecha nunca ha sido utilizada
                        if((prExt.part_start+prExt.part_s+add)<(mbr.mbr_tamanio)){
                            prExt.part_s = prExt.part_s+add; //Sumo el nuevo espacio
                            mbr.particiones[pos] = prExt;
                            fseek(archivo,0,SEEK_SET); //Actualizo el mbr
                            fwrite(&prExt,sizeof(MBR),1,archivo);
                            cout<<"¡Tamaño de la partición actualizado!"<<endl;
                        }
                        else{
                            cout<<"Error No hay espacio suficiente en el disco"<<endl;
                        }
                    }
                }
            }
        
            else{
             //Si ya es la última partición
                if((prExt.part_start+prExt.part_s+add)<(mbr.mbr_tamanio)){
                prExt.part_s = prExt.part_s+add;
                mbr.particiones[pos] = prExt;
                fseek(archivo,0,SEEK_SET); //Actualizo el mbr
                fwrite(&prExt,sizeof(MBR),1,archivo);
                cout<<"¡Tamaño de la partición actualizado!"<<endl;
                }
                else{
                    cout<<"Error: No hay espacio suficiente en el disco"<<endl;
                }
            }

            }
            else{
            cout<<"Error: No puede existir tamaño negativo"<<endl;
            }
        }
        else if(flogica){
            //Si es lógica la partición a modificar
            if((logica.part_s+add)>=0){ //Si el tamaño resultante es positivo
            if(logica.part_next!=-1){ //Si tiene siguiente
            EBR siguiente;
            fseek(archivo,logica.part_next,SEEK_SET);
            fread(&siguiente,sizeof(EBR),1,archivo);

            if((logica.part_start+logica.part_s+add)<siguiente.part_start){ //Si no se topa con la siguiente partición
                logica.part_s=logica.part_s+add;
                fseek(archivo,logica.part_start,SEEK_SET);
                fwrite(&logica,sizeof(EBR),1,archivo);
                cout<<"¡Tamaño de la partición actualizado!"<<endl;

            }
            else{
                cout<<"Espacio insuficiente para modificar la partición"<<endl;
            }
            }
            else{
                if((logica.part_start+logica.part_s+add)<=(mbr.particiones[pos].part_s+mbr.particiones[pos].part_start)){ //Si el nuevo tamaño cabe en la extendida
                    logica.part_s=logica.part_s+add;
                    fseek(archivo,logica.part_start,SEEK_SET);
                    fwrite(&logica,sizeof(EBR),1,archivo);
                    cout<<"¡Tamaño de la partición actualizado!"<<endl;
                }
                else{
                    cout<<"Error: Espacio insuficiente para modificar la partición"<<endl;
                }
            }

        }
        }
        
    }
    else{
        cout<<"Error: No se encontró la partición"<<endl;
    }

    fclose(archivo);
}

void fdisk(char* parametros){
    parametros = strtok(NULL, " ");
    bool fsize, funit, fpath, fname, ftype, ffit, fdelete, fadd = false;
    fsize=false;
    funit=false;
    fpath=false;
    fname=false;
    ftype=false;
    ffit=false;
    fdelete=false;
    fadd=false;
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
            strcpy(aj, valor.c_str());
            if(strcasecmp(aj,"bf")==0){
                fit='B';
                ffit=true;
            }
            else if(strcasecmp(aj,"ff")==0){
                ffit='F';
                ffit=true;
            }
            else if(strcasecmp(aj,"wf")==0){
                ffit='W';
                ffit=true;
            }
            else{
                cout<<"Error: Valor de ajuste inválido"<<endl;
            }
        }

        else if(tipo==">delete"){
            char * del = new char[valor.length()];
            strcpy(del,valor.c_str());
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
            add = add*1024;
        }
        else{
            if(unit=='k'){
                size = size*1024;

            }
            else if(unit=='m'){
                size = size*1024*1024;

            } //Si la unidad es b, no hay más acciones que tomar
        }

        if(!ftype){
            type='P';
        }
        if(!ffit){
            fit='W';
        }

        char *ruta = new char[path.length()];
        
        strcpy(ruta,path.c_str());
        if(existsFile(path)){

            ajustar(ruta,size,name, type, fit);

            
        }
        else{
            cout<<"Error: No se encontró el disco duro"<<endl;
        }


    }
    else if(fdelete && fname && fpath){
        char *ruta = new char[path.length()];
        strcpy(ruta,path.c_str());
        fdiskDelete(ruta,name);
    }
    else if(fadd && fname && fpath){
        if(!funit){ //Si falta el parámetro, se toma k
            add = add*1024;
        }
        else{
            if(unit=='k'){
                add = add*1024;
            }
            else if(unit=='m'){
                add = add*1024*1024;
            } //Si la unidad es b, no hay más acciones que tomar
        }

        char *ruta = new char[path.length()];
        
        strcpy(ruta,path.c_str());
        if(existsFile(path)){

            fdiskAdd(ruta,name,add);

            
        }
        else{
            cout<<"Error: No se encontró el disco duro"<<endl;
        }

    }
    else{
        cout<<"Error: Parámetros no suficientes para realizar una acción"<<endl;
    }
    cout<<"\n"<<endl;
}

string getFileName(string ruta){
  
    ruta=ruta.substr(ruta.find_last_of("\\/")+1);

    string name = ruta.substr(0,ruta.find_last_of("."));

    return name;
}

void montarParticion(char* ruta, char name[16]){
    MBR mbr;
    bool encontrada = false;
    FILE *archivo= fopen(ruta,"rb+");
    fseek(archivo,0,SEEK_SET);
    fread(&mbr, sizeof(MBR),1,archivo);



    for(int i=0; i<4; i++){
        if (strcasecmp(name,mbr.particiones[i].part_name)==0){
            encontrada=true;
            break;
        }
        if(mbr.particiones[i].part_type=='E'){//Si la partición es extendida, reviso en las particiones lógicas
            EBR tmp;
            
            fseek(archivo,mbr.particiones[i].part_start,SEEK_SET); //Me muevo al inicoi de la partición extendida para leer el EBR
            fread(&tmp, sizeof(EBR),1,archivo);

            

            if(strcasecmp(name,tmp.part_name)==0){
                    encontrada=true;
                    break;
            }

            while(tmp.part_next!=-1){
               
                if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    break;
                }
                
                fseek(archivo, tmp.part_next,SEEK_SET);
                fread(&tmp,sizeof(EBR),1,archivo); //Cambio a a la siguiente partición lógica
                
                if(tmp.part_next==-1){
                    //memcpy(&ultima,&tmp,sizeof(EBR));
                    if(strcasecmp(name,tmp.part_name)==0){
                    
                    encontrada=true;
                    break;
                }
                }
            }
        }
    }


    if(encontrada){
        string id="";
        id+=digitosCarnet;
            
           
            
            string nombreDisco = getFileName(ruta);
            int ult = 0;

            for(it = montadas.begin(); it!=montadas.end(); it++){
                if(it->second==ruta){
                    ult++;
                }
            }
            ult++;
            id+=to_string(ult);
            id+=nombreDisco;
            
            montadas[id] = ruta; //Agrego el par <id,rutaDisco> 
            nombres[id] = name; //Agrego el par <id,nombreParticion>

           
           
            cout<<"Partición <"<<name<<"> montada! ID: "<<id<<endl;
            //TODO: Hacer prueba con debugger
        
    }
    else{
        cout<<"Error: No se encontró la partición"<<endl;
    }
    fclose(archivo);
    
}
void verMontadas(){
    if(montadas.size()==0){
        cout<<"No hay particiones montadas :("<<endl;
    }
    else{
        cout<<"Particiones montadas actualmente"<<endl;
        for(it=montadas.begin(); it!=montadas.end(); it++){
            cout<<"-"<<it->first<<endl;
        }
    }
}

void mount(char* parametros){
    bool fpath=false;
    bool fname=false;
    bool finfo = false;
    parametros = strtok(NULL," ");

    string path;
    char name[16];
    while (parametros!=NULL){
        string tmp = parametros;
        string tipo = get_tipo_parametro(tmp);
        string valor = get_valor_parametro(tmp);

        if(tipo==">path"){
            valor = regresarEspacio(valor);
            if(existsFile(valor)){
                path=valor;
                fpath=true;
            }
            else{
                cout<<"Error: No se encontró el disco duro"<<endl;
                break;
            }
        }
        else if(tipo==">name"){
            valor=regresarEspacio(valor);
            strcpy(name,valor.c_str());
            fname=true;
        }
        else if(tipo==">info"){
            verMontadas();
            finfo=true;
            break;
        }
        else if(tipo[0] == '#'){
            //Si viene un comentario, no pasa nada
            break;
        }
        else{
            
            cout<<"Parámetro inválido"<<endl;
        }
        parametros = strtok(NULL," ");
        
    }
    //Trabajando con los parámetros
    if(fname && fpath){
        char* ruta = new char[path.length()];
        strcpy(ruta,path.c_str());
        montarParticion(ruta,name);
    }
    else if(finfo){
        //No pasa nada
    }
    else{
        cout<<"Parámetros insuficientes para realizar una acción"<<endl;
    }
    cout<<"\n";
    
}

void desmontarParticion(string id){
    it = montadas.find(id);
    if(it!=montadas.end()){
        //Si la encuentro, la elimino
        montadas.erase(it);
        it=nombres.find(id);
        if(it!=nombres.end()){
            nombres.erase(it);
        }
        cout<<"¡Partición desmontada!"<<endl;

    }
    else{
        cout<<"Error: No se encontró el id"<<endl;
    }
}

void unmount(char* parametros){
    bool fid = false;
    parametros = strtok(NULL," ");

    string path;
    string id;
    while (parametros!=NULL){
        string tmp = parametros;
        string tipo = get_tipo_parametro(tmp);
        string valor = get_valor_parametro(tmp);

        if(tipo==">id"){
            valor = regresarEspacio(valor);
            id=valor;
            fid=true;
        }
        else if(tipo[0] == '#'){
            //Si viene un comentario, no pasa nada
            break;
        }
        else{
            
            cout<<"Parámetro inválido"<<endl;
        }
        parametros = strtok(NULL," ");
        
    }
    //Trabajando con los parámetros
    if(fid){
        desmontarParticion(id);
    }
    else{
        cout<<"Parámetros insuficientes para realizar una acción"<<endl;
    }
    cout<<"\n";
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
    else if(strcasecmp(token,"mount")==0){
        mount(token);
    }
    else if(strcasecmp(token,"unmount")==0){
        unmount(token);
    }
    else if (token[0]=='#'){
        //Si es un comentario, no pasa nada

    }
    else {
        cout << "Error: Comando no aceptado" << endl;
    }
    
}