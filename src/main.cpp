#include <stdio.h>
#include <iostream>
#include "./Analizador/analizador.h"
using namespace std;

int main()
{
  cout << "*****************************************" << endl;
  cout << "******** LUISA MARIA ORTIZ ROMERO *******" << endl;
  cout << "*********** PROYECTO 1 - MIA ************" << endl;
  cout << "************** 202003381 ****************" << endl;
  cout << "*****************************************" << endl;

  char comando[500];
  while(true){
    //Solicitando el comando
    cout<< "> ";
    scanf(" %[^\n]",comando);
   
    analizar(comando);
  } 
  return 0;
}