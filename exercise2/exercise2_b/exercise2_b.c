#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Función para crear una matriz de espines
int **crear_matriz_espines(int L)
{
	int **espines = (int **)malloc(L * sizeof(int *));
	for (int i = 0; i < L; i++)
	{
		espines[i] = (int *)malloc(L * sizeof(int));
	}
	return espines;
}

// Función para liberar la memoria de la matriz
void liberar_matriz_espines(int **espines, int L)
{
	for (int i = 0; i < L; i++)
	{
		free(espines[i]);
	}
	free(espines);
}

// Función para inicializar la matriz de espines aleatoriamente
void inicializar_espines_aleatorios(int **espines, int L)
{
	for (int i = 0; i < L; i++)
	{
		for (int j = 0; j < L; j++)
		{
			// Con probabilidad del 50% reciben +1, de lo contrario -1
			espines[i][j] = (rand() < RAND_MAX / 2) ? 1 : -1;
		}
	}
}

// Cálculo del cambio de energía al invertir un único espín
double cambio_energia(int **espines, int L, int i, int j, double B)
{
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
double magnetizacion_media(int **espines, int L)
{
	double suma = 0.0;
	for (int i = 0; i < L; i++)
	{
		for (int j = 0; j < L; j++)
		{
			suma = suma + espines[i][j];
		}
	}
	return fabs(suma / (L * L));
}
// Implementación del algoritmo de Metropolis
double* algoritmo_metropolis(int L, double T, double B, int pasos, long intervalo_registros)
{
	double delta_H, r;
	int i, j;
	long intervalo_registro = 10000;
	// Creación de la matriz de espines
	int **espines = crear_matriz_espines(L);

    inicializar_espines_aleatorios(espines, L);

	// Crear un arreglo para almacenar la magnetización en cada punto de registro
    long num_registros = pasos / intervalo_registro;
    double* magnetizaciones = (double*)malloc(num_registros * sizeof(double));
	
	for (long paso = 0; paso < pasos; paso++)
	{
		// Selección de un espín aleatorio
		i = rand() % L;
		j = rand() % L;

		// Cálculo del cambio de energía si invertimos el espín (ΔH)
		delta_H = cambio_energia(espines, L, i, j, B);

		// Si el cambio de energía es negativo o se cumple la condición probabilística, aceptamos el cambio
		if (delta_H <= 0.0)
		{
			espines[i][j] = -espines[i][j]; // Inversión del espín
		}
		else
		{
			r = (double)rand() / RAND_MAX; // Condición probabilistica [0,1)
			if (r < exp(-delta_H / T))
			{
				espines[i][j] = -espines[i][j]; // Inversión del espín
			}
		}
		// Registro de la magnetización cada 'intervalo_registro' pasos
        if (paso % intervalo_registro == 0) {
            double magnetizacion = magnetizacion_media(espines, L);
            magnetizaciones[paso / intervalo_registro] = magnetizacion;
        }
	}
	// Liberación de memoria
	liberar_matriz_espines(espines, L);
	return magnetizaciones;
}
void promedio_simulaciones(int L, double T, double B, int pasos, long nSimulaciones, const char *nombre_archivo)
{
    // Intervalo para el registro de datos
    const long intervalo_registro = 10000;
    long num_registros = pasos / intervalo_registro;
    
    // Arreglo para almacenar la suma de magnetizaciones de todas las simulaciones
    double* magnetizacion_promedio = (double*)calloc(num_registros, sizeof(double));
    
    printf("Realizando %ld simulaciones para B = %.2f ...\n", nSimulaciones, B);
    
    for (int sim = 0; sim < nSimulaciones; sim++) {
        if (sim % 10 == 0) {
            printf("Simulación %d de %ld\n", sim, nSimulaciones);
        }
        
        // Ejecutar una simulación y obtener las magnetizaciones
        double* magnetizaciones = algoritmo_metropolis(L, T, B, pasos, intervalo_registro);
        
        // Sumar los resultados para calcular el promedio
        for (long i = 0; i < num_registros; i++) {
            magnetizacion_promedio[i] += magnetizaciones[i];
        }
        
        // Liberar la memoria
        free(magnetizaciones);
    }
    
    // Calcular el promedio sobre el ruido
    for (long i = 0; i < num_registros; i++) {
        magnetizacion_promedio[i] /= nSimulaciones;
    }
    
    // Guardar los resultados en un archivo
    FILE* archivo = fopen(nombre_archivo, "w");
    if (archivo == NULL) {
        printf("Error al abrir el archivo %s\n", nombre_archivo);
        free(magnetizacion_promedio);
        return;
    }
    
    // Escribir los datos
    for (long i = 0; i < num_registros; i++) {
        fprintf(archivo, "%ld %lf\n", (i + 1) * intervalo_registro, magnetizacion_promedio[i]);
    }
    
    fclose(archivo);
    free(magnetizacion_promedio);
    
    printf("Promedio de simulaciones completado. Resultados guardados en %s\n", nombre_archivo);
}

// Función main
int main()
{
	int L, A;
	double T, B;
	printf("Introduzca tamaño del sistema (L): ");
	scanf("%d", &L);

	printf("\nIntroduzca la temperatura (T): ");
	scanf("%lf", &T);

	printf("\nIntroduzca el valor del campo magnético (B): ");
	scanf("%lf", &B);

	// Cálculo del número de pasos
	long pasos = pow(10, 8);
	int nSimulaciones = 100;

	// Inicialización del generador de números aleatorios
	srand(time(NULL));

	// Creación del nombre de archivo con los parámetros
	char nombre_archivo[100];
	sprintf(nombre_archivo, "ising_magnetizacion_promedio_L%d_T%.2f_B%.2f_Nsim%d.dat", 
            L, T, B, nSimulaciones);
	

	// Ejecución de n simulaciones
	promedio_simulaciones(L, T, B, pasos, nSimulaciones, nombre_archivo);

	return 0;
}
