#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include"timer.h"

int qtdPrimos; //guarda a quantidade de numeros primos
int nthreads; //numero de threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua

typedef struct{
   int inicio; //inicio da sequencia
   int fim; //fim da sequencia
} tArgs;

int ehPrimo(long long int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
        for (i=3; i<sqrt(n)+1; i+=2)
        if(n%i==0) return 0;
}

//funcao que as threads executarao
void * tarefa(void *arg) {
    tArgs *args = (tArgs*) arg;
    int i = args->inicio;

     while (i < args->fim){
        if (ehPrimo(i)){
            pthread_mutex_lock(&mutex);
            qtdPrimos++;
            pthread_mutex_unlock(&mutex);
        }
        i++;
    }
    
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    tArgs *args;
    long int tamSequencia;
    pthread_t *tid; //identificadores das threads no sistema
    double inicio, fim, delta;

    GET_TIME(inicio);

    if (argc < 3){
      printf("Digite: %s <tamanho da sequencia> <numero de threads>\n", argv[0]);
    }

    tamSequencia = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    if (nthreads > tamSequencia) nthreads=tamSequencia;

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo inicializacao:%lf\n", delta);

    GET_TIME(inicio);

   //alocacao das estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 1;}
    args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
    if(args==NULL) {puts("ERRO--malloc"); return 1;}

    pthread_mutex_init(&mutex, NULL); // inicializa o lock

    //criacao das threads
    for(int i=0; i<nthreads; i++) {
        (args+i)->inicio = (tamSequencia/nthreads) * i;
        if(i + 1  >= nthreads) (args+i)->fim = tamSequencia; //manda o resto da divisao para última thread
        else (args+i)->fim = (tamSequencia/nthreads) * (i + 1);
    
        if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
            puts("ERRO--pthread_create"); return 2;
        }
    } 

    //--espera todas as threads terminarem
    for (int i=0; i<nthreads; i++) {
        if (pthread_join(tid[i], NULL)) {
            printf("--ERRO: pthread_join() \n");
        } 
    }

    pthread_mutex_destroy(&mutex);

    GET_TIME(fim)   
    delta = fim - inicio;
    printf("Tempo de execucao (tamanho sequencia %d) (nthreads %d): %lf\n", tamSequencia, nthreads, delta);

    free(args);
    free(tid);
    printf("Quantidade de numeros primos: %d", qtdPrimos);
}