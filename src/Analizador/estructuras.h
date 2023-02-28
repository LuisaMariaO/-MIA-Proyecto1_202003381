#include <time.h>
using namespace std;

typedef struct 
{
    char part_status; //Indica si la partición está activa o no A->Activa, I->Inactiva
    char part_type; //P->Primaria E->Extendida
    char part_fit; //B->BestFit F->FirstFit W->WorstFit
    int part_start; //Indica en qué byte del disco inicia la partición
    int part_s=0; //Contiene el tamaño de la partición en bytes
    char part_name[16]; //Nombre de la partición

}Particion;


typedef struct
{
    int mbr_tamanio; //En bytes
    time_t mbr_fecha_creacion; //Fecha y hora de creación
    int mbr_dsk_signature; //Número random que identifica al disco de forma única
    char dsk_fit; //B->BestFit F->FirstFist W->WorstFit
    Particion particiones[4]; //Particiones primarias y extendidas que puede tener el disco
}MBR;

typedef struct 
{
    char part_status; //Indica si la partición está activa o no
    char part_fit; //B->BestFit F->FirstFit W->WorstFit
    int part_start; //Indica en qué byte del disco inicia la partición
    int part_s=0; //Contiene el tamaño de la partición en bytes
    int part_next=-1; //Byte en el que está el próximo EBR. -1 si no hay siguiente
    char part_name[16]; //Nombre de la partición

}EBR;

typedef struct
{
    int s_filesystem_type; //Guarda el número que identifica el sistema de archivos utilizado
    int s_inodes_count; //Guarda el número total de inodos
    int s_blocks_count; //Guarda el número total de bloques
    int s_free_blocks_count; //Contiene el número de bloques libres
    int s_free_inodes_count; //Contiene dl número de inodos libres
    time_t s_mtime; //Última fecha en el que el sistema fue montado
    time_t s_umtime; //Última fecha en el que el sistema fue desmontado
    int s_mnt_count; //Indica cúantas veces se ha montado el sistema
    int s_magic; //Valor que identifica al sistema de archivos, tendrá el valor 0xEF53
    int s_inode_s; //Tamaño del inodo
    int s_block_s; //Tamaño del bloque
    int s_first_ino; //Primer inodo libre
    int s_first_blo; //Primer bloque libre
    int s_bm_inode_start; //Guardará el inicio del bitmap de inodos
    int s_bm_block_start; //Guardará el inicio del bitmap de bloques
    int s_inode_start; //Guardará el inicio de la tabla de inodos
    int s_block_start; //Guardará el inicio de la tabla de bloques
}SuperBloque;
/*Se utiliará un inodo por carpeta o archivo*/
typedef struct
{
    int i_uid; //UID del usuario propietario del archivo o carpeta
    int i_gid; //GID del grupo al que pertenece el archivo o carpeta
    int i_s; //Tamaño del archivo en bytes
    time_t i_atime; //Última fecha en que se leyó el inodo sin modificarlo
    time_t i_ctime; //Fecha en que se creó el inodo
    time_t i_mtime; //Última fecha en que se modifica el inodo
    int i_block[15]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; /*Array en que los primeros 12 registros son bloques directos*/
    /*El 13 será el número del bloque simple indirecto*/
    /*El 14 será el número del bloque doble indirecto*/
    /*El 15 será el número del bloque triple indirecto. Si no son utilizados tendrán el valor de -1*/
    char i_type; //Indica si es archivo o carpeta //1->Archivo 0->Carpeta
    int i_perm; //Guardará los permisos del archivo o carpeta. 
    /*Los primeros tres bits serán para el Usuario i_uid. Los siguientes
    tres bits serán para el Grupo al que pertenece el usuario. Y los
    últimos tres bits serán para los permisos de Otros usuarios.
    Cada grupo de tres bits significa lo siguiente: El primer bit indica el
    permiso de lectura R. El segundo bit indica el permiso de escritura
    W. El tercer bit indica el permiso de ejecución X.*/
}Inodo;

typedef struct
{
    char b_name[12]; //Nombre de la carpeta o archivo
    int b_inodo; //Apuntador hacia un inodo asociado al archivo o carpeta
}Content;

typedef struct
{
    Content b_content[4]; //Array con el contenido de la carpeta
}BloqueCarpeta;

typedef struct
{
    char b_content[64]; //Array con el contenido del archivo
}BloqueArchivos;

typedef struct
{
    int b_content[16]; //Array con los apuntadores a bloques (de archivo o carpeta)
}BloqueApuntadores;