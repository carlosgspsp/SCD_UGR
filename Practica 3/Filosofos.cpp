#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

using namespace std;

#define coger_tenedor 0
#define soltar_tenedor 1

void Filosofo( int id, int nprocesos);
void Tenedor ( int id, int nprocesos);

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   if( size!=10)
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 10" << endl << flush ;
      MPI_Finalize( ); 
      return 0; 
   }
   
   if ((rank%2) == 0)  
      Filosofo(rank,size); // Los pares son Filosofos 
   else 
      Tenedor(rank,size);  // Los impares son Tenedores 
   
   MPI_Finalize( );
   return 0;
}  
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
   int izq = (id+1) % nprocesos;
   int der = ((id+nprocesos)-1) % nprocesos; 
   int fi_comiendo = 0; 
   
   while(1)
   {

      if(fi_comiendo < 5){

        fi_comiendo++;

        // Solicita tenedor izquierdo
        cout << "Filosofo "<<id<< " solicita tenedor izq ..." << izq << endl << flush;
        MPI_Ssend( NULL, 0, MPI_INT, izq, coger_tenedor, MPI_COMM_WORLD );

        // Solicita tenedor derecho
        cout <<"Filosofo "<<id<< " coge tenedor der ..." << der << endl << flush;
        MPI_Ssend( NULL, 0, MPI_INT, der, coger_tenedor, MPI_COMM_WORLD );

        cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
        sleep((rand() % 3)+1);  //comiendo
        
        // Suelta el tenedor izquierdo
        cout <<"Filosofo "<<id<< " suelta tenedor izq ..." << izq << endl << flush;
        MPI_Ssend( NULL, 0, MPI_INT, izq, soltar_tenedor, MPI_COMM_WORLD );
        
        // Suelta el tenedor derecho
        cout <<"Filosofo "<<id<< " suelta tenedor der ..." << der << endl << flush;
        MPI_Ssend( NULL, 0, MPI_INT, der, soltar_tenedor, MPI_COMM_WORLD );
        
        fi_comiendo--;

        // Piensa (espera bloqueada aleatorio del proceso)
        cout << "Filosofo " << id << " PENSANDO" << endl << flush;
        
        // espera bloqueado durante un intervalo de tiempo aleatorio 
        // (entre una d�cima de segundo y un segundo)
        usleep( 1000U * (100U+(rand()%900U)) );

      }
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  MPI_Status status; 
  int Filo;
  
  while( true )
  {
    // Espera un peticion desde cualquier filosofo vecino ...
    MPI_Recv( NULL, 0, MPI_INT, MPI_ANY_SOURCE, coger_tenedor, MPI_COMM_WORLD, &status);

    // Recibe la peticion del filosofo ...
    Filo = status.MPI_SOURCE;    
    cout << "Ten. " << id << " recibe petic. de " << Filo << endl << flush;
    
    // Espera a que el filosofo suelte el tenedor...
    MPI_Recv( NULL, 0, MPI_INT, Filo, soltar_tenedor, MPI_COMM_WORLD, &status );
    cout << "Ten. " << id << " recibe liberac. de " << Filo << endl << flush; 
  }
}
// -------------------------------------------------------------------