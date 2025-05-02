#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
 
// Función para crear una matriz de espines
int** crear_matriz_espines(int L) {
	int** espines = (int**)malloc(L * sizeof(int*));
	for (int i = 0; i < L; i++) {
    	espines[i] = (int*)malloc(L * sizeof(int));
	}
	return espines;
}
 
// Función para liberar la memoria de la matriz
void liberar_matriz_espines(int** espines, int L) {
	for (int i = 0; i < L; i++) {
    	free(espines[i]);
	}
	free(espines);
}
 
// Función para inicializar la matriz de espines aleatoriamente
void inicializar_espines_aleatorios(int** espines, int L) {
	for (int i = 0; i < L; i++) {
    	for (int j = 0; j < L; j++) {
        	// Con probabilidad del 50% reciben +1, de lo contrario -1
        	espines[i][j] = (rand() < RAND_MAX / 2) ? 1 : -1;
    	}
	}
}
 
// Cálculo del cambio de energía al invertir un único espín
double cambio_energia(int** espines, int L, int i, int j, double B) {
	// Implementación de condiciones de contorno periódicas
	int izquierda = (j == 0) ? L - 1 : j - 1;
	int derecha = (j == L - 1) ? 0 : j + 1;
	int arriba = (i == 0) ? L - 1 : i - 1;
	int abajo = (i == L - 1) ? 0 : i + 1;
   
	// Cálculo de la suma de los vecinos (con condiciones de contorno periódicas)
	int suma_vecinos = espines[arriba][j] + espines[abajo][j] + espines[i][izquierda] + espines[i][derecha];
   
	// Cálculo del cambio de energía según la fórmula (9)
	return 2.0 * espines[i][j] * (suma_vecinos + B);
}
 
// Implementación del algoritmo de Metropolis
void algoritmo_metropolis(int** espines, int L, double T, double B, long pasos) {
	double delta_E, r;
	int i, j;
   
	for (long paso = 0; paso < pasos; paso++) {
    	// Selección de un espín aleatorio
    	i = rand() % L;
    	j = rand() % L;
       
    	// Cálculo del cambio de energía si invertimos el espín
    	delta_E = cambio_energia(espines, L, i, j, B);
       
    	// Si el cambio de energía es negativo o se cumple la condición probabilística, aceptamos el cambio
    	if (delta_E <= 0.0) {
        	espines[i][j] = -espines[i][j];  // Inversión del espín
    	} else {
        	r = (double)rand() / RAND_MAX;
 	       if (r < exp(-delta_E / T)) {
            	espines[i][j] = -espines[i][j];  // Inversión del espín
        	}
    	}
	}
}
 
// Escritura del estado de la red a un archivo
void escribir_espines_a_archivo(int** espines, int L, const char* nombre_archivo) {
	FILE* archivo = fopen(nombre_archivo, "w");
	if (archivo == NULL) {
    	printf("Error al abrir el archivo %s\n", nombre_archivo);
    	return;
	}
   
	for (int i = 0; i < L; i++) {
    	for (int j = 0; j < L; j++) {
        	fprintf(archivo, "%d ", espines[i][j]);
    	}
    	fprintf(archivo, "\n");
	}
   
	fclose(archivo);
}
 
// Función main
int main() {
    int L, A;
    double T, B;
    printf("Introduzca tamaño del sistema (L): ");
    scanf("%d", &L); 

    printf("\nIntroduzca exponente para el número de pasos (A): ");
    scanf("%d", &A);

    printf("\nIntroduzca la temperatura (T): ");
    scanf("%lf", &T);

    printf("\nIntroduzca el valor del campo magnético (B): ");
    scanf("%lf", &B);

   
	// Cálculo del número de pasos
	long pasos = pow(10, A);
   
	// Inicialización del generador de números aleatorios
	srand(time(NULL));
   
	// Creación de la matriz de espines
	int** espines = crear_matriz_espines(L);
   
	// Inicialización aleatoria de los espines
	inicializar_espines_aleatorios(espines, L);
   
	// Ejecución del algoritmo de Metropolis
	algoritmo_metropolis(espines, L, T, B, pasos);
   
	// Creación del nombre de archivo con los parámetros
	char nombre_archivo[100];
	sprintf(nombre_archivo, "ising_L%d_A%d_T%.2f_B%.2f.dat", L, A, T, B);
   
	// Escritura de los resultados al archivo
	escribir_espines_a_archivo(espines, L, nombre_archivo);
   
	printf("Simulación completada. Los resultados se guardaron en el archivo %s\n", nombre_archivo);
   
	// Liberación de memoria
	liberar_matriz_espines(espines, L);
   
	return 0;
}
