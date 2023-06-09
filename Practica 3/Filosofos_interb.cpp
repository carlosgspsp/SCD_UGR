// Compilar con: mpicxx -std=c++11 -o filosofos-interb filosofos-interb.cpp
// Ejecutar con: mpirun -np (num_procesos_esperado) ./filosofos-interb
// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Pr�ctica 3. Implementaci�n de algoritmos distribuidos con MPI
//
// Archivo: filosofos-interb.cpp
// Implementaci�n del problema de los fil�sofos (sin camarero).
// Plantilla para completar.
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------


#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
   num_filosofos = 5 ,
   num_procesos  = 2*num_filosofos,
   etiq_solicitar = 0,
   etiq_liberar = 1 ;


//**********************************************************************
// plantilla de funci�n para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilaci�n)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

// ---------------------------------------------------------------------

void funcion_filosofos( int id )
{
  int id_ten_izq = (id+1)              % num_procesos, //id. tenedor izq.
      id_ten_der = (id+num_procesos-1) % num_procesos; //id. tenedor der.

  while ( true )
  {
  	// Uno de los fil�sofos (por ejemplo, el 0) debe coger los tenedores al 
  	// rev�s porque puede darse situaci�n de interbloqueo si no es as�
    if (id == 0)
    {
    	cout <<"Fil�sofo " <<id << " solicita ten. der." <<id_ten_izq <<endl;
	    // solicitar tenedor der
	    MPI_Ssend(NULL, 0, MPI_INT, id_ten_der, etiq_solicitar, MPI_COMM_WORLD);

	    cout <<"Fil�sofo " <<id <<" solicita ten. izq." <<id_ten_der <<endl;
	    // solicitar tenedor izq
	    MPI_Ssend(NULL, 0, MPI_INT, id_ten_izq, etiq_solicitar, MPI_COMM_WORLD);
    } 
    else 
    {
    	cout <<"Fil�sofo " <<id << " solicita ten. izq." <<id_ten_izq <<endl;
	    // solicitar tenedor izquierdo
	    MPI_Ssend(NULL, 0, MPI_INT, id_ten_izq, etiq_solicitar, MPI_COMM_WORLD);

	    cout <<"Fil�sofo " <<id <<" solicita ten. der." <<id_ten_der <<endl;
	    // solicitar tenedor derecho
	    MPI_Ssend(NULL, 0, MPI_INT, id_ten_der, etiq_solicitar, MPI_COMM_WORLD);
    }
    

    cout <<"Fil�sofo " <<id <<" comienza a comer" <<endl ;
    sleep_for( milliseconds( aleatorio<10,100>() ) );

    cout <<"Fil�sofo " <<id <<" suelta ten. izq. " <<id_ten_izq <<endl;
    // soltar el tenedor izquierdo
    MPI_Ssend(NULL, 0, MPI_INT, id_ten_izq, etiq_liberar, MPI_COMM_WORLD);

    cout<< "Fil�sofo " <<id <<" suelta ten. der. " <<id_ten_der <<endl;
    // soltar el tenedor derecho
    MPI_Ssend(NULL, 0, MPI_INT, id_ten_der, etiq_liberar, MPI_COMM_WORLD);

    cout << "Filosofo " << id << " comienza a pensar" << endl;
    sleep_for( milliseconds( aleatorio<10,100>() ) );
 }
}
// ---------------------------------------------------------------------

void funcion_tenedores( int id )
{
  int valor, id_filosofo ;  // valor recibido, identificador del fil�sofo
  MPI_Status estado ;       // metadatos de las dos recepciones

  while ( true )
  {
     // recibir petici�n de cualquier fil�sofo
  	 MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, etiq_solicitar, MPI_COMM_WORLD, &estado);
     // guardar en 'id_filosofo' el id. del emisor
     id_filosofo = estado.MPI_SOURCE;
     cout <<"Ten. " <<id <<" ha sido cogido por filo. " <<id_filosofo <<endl;

     // recibir liberaci�n de fil�sofo 'id_filosofo'
     MPI_Recv(NULL, 0, MPI_INT, id_filosofo, etiq_liberar, MPI_COMM_WORLD, &estado);
     cout <<"Ten. "<< id<< " ha sido liberado por filo. " <<id_filosofo <<endl ;
  }
}
// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int id_propio, num_procesos_actual ;

   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
   MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


   if ( num_procesos == num_procesos_actual )
   {
      // ejecutar la funci�n correspondiente a 'id_propio'
      if ( id_propio % 2 == 0 )          // si es par
         funcion_filosofos( id_propio ); //   es un fil�sofo
      else                               // si es impar
         funcion_tenedores( id_propio ); //   es un tenedor
   }
   else
   {
      if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
      { cout << "el n�mero de procesos esperados es:    " << num_procesos << endl
             << "el n�mero de procesos en ejecuci�n es: " << num_procesos_actual << endl
             << "(programa abortado)" << endl ;
      }
   }

   MPI_Finalize( );
   return 0;
}

// ---------------------------------------------------------------------
