/*
 * Nombre del equipo: S.O. AGREVAL
 * Fecha: 25/09/2024
 * Versión: 1.2.0
 * Programa para simular la sincronización de procesos usando memoria compartida y turnos.
 *
 * Descripción:
 * Este programa utiliza memoria compartida (IPC) para coordinar dos procesos que se alternan
 * en una sección crítica. Cada proceso espera su turno antes de entrar en la región crítica
 * y, una vez que la ha terminado, cede el turno al otro proceso. Se utiliza una variable
 * compartida (`turno`) para controlar qué proceso está en la sección crítica en cada momento.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>  // Incluir para funciones de memoria compartida
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHMKEY 75  // Definir la clave de la memoria compartida (sin el punto y coma)
#define TRUE 1     // Definir el valor lógico TRUE (sin el punto y coma)

// Prototipos de las funciones
int region_critica(int pid);
int region_no_critica(int pid);
int esperando_turno(int pid);

int main() {

    int shmid, pid;
    char *addr, *turno;  // Puntero para acceder a la memoria compartida

    // Obtener el ID del proceso actual
    pid = getpid();

    // Crear el segmento de memoria compartida
    shmid = shmget(SHMKEY, 10, 0777 | IPC_CREAT);
    if (shmid < 0) {
        perror("Error al crear la memoria compartida");
        exit(1);
    }

    // Adjuntar el segmento de memoria compartida al espacio de direcciones del proceso
    addr = (char *)shmat(shmid, 0, 0);
    if (addr == (char *)-1) {
        perror("Error al adjuntar la memoria compartida");
        exit(1);
    }

    // Inicializar el puntero `turno` para controlar el turno de los procesos
    turno = addr;
    *turno = '0';  // Establecer turno inicial

    // Mostrar mensaje inicial
    printf("\n[I] Variables Declaradas");
    sleep(5);
    printf("\n[I] Inicia proceso de turnos con turno inicial: %c", *turno);

    // Bucle principal del proceso
    while (TRUE) {
        // Esperar el turno
        while (*turno != '0') {
            esperando_turno(pid);
        }

        // Entrar a la región crítica
        region_critica(pid);

        // Simular un retardo antes de ceder el turno
        sleep(1);

        // Cambiar el turno al proceso 1
        *turno = '1';

        // Ejecutar la región no crítica
        region_no_critica(pid);
    }

    // Desvincular el segmento de memoria compartida
    shmdt(addr);

    // Eliminar el segmento de memoria compartida
    shmctl(shmid, IPC_RMID, 0);

    return 0;
}

/*
 * Función que simula la espera por el turno en la región crítica.
 */
int esperando_turno(int pid) {
    printf("\n[I] Proceso %d está esperando su turno", pid);
    sleep(2);
    return 0;
}

/*
 * Función que simula la ejecución en la región crítica.
 * El proceso modifica variables de manera segura en esta sección.
 */
int region_critica(int pid) {
    printf("\n\n[O] Proceso %d entrando en la región crítica y cambiando variables", pid);
    for (int i = 0; i < 5; i++) {
        printf("\n[O] Proceso %d - Turno %d", pid, i + 1);
        sleep(1);
    }
    return 0;
}

/*
 * Función que simula la ejecución en la región no crítica.
 */
int region_no_critica(int pid) {
    printf("\n[I] Proceso %d está en la región no crítica", pid);
    sleep(2);
    return 0;
}

