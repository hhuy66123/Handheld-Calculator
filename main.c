#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "postfix.h"
#include "findroot.h"
#include <pthread.h>
#define NUM_THREADS 3

int found = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
float best_result = 0.0;

typedef struct
{
    Token *postfix;
    float result;
} ThreadData;

void *findrootNewton(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    data->result = newtonRaphson(data->postfix);

    pthread_mutex_lock(&mutex);
    if (!found)
    {
        best_result = data->result;
        found = 1;
        printf("\nNewton Raphson tim duoc nghiem la: %f\n", best_result);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void *findrootBisection(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->result = bisectionMethod(data->postfix); 

    pthread_mutex_lock(&mutex);
    if (!found) { 
        best_result = data->result;
        found = 1;
        printf("\nBisection tim duoc nghiem la: %f\n", best_result);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void *findrootSecant(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    data->result = secantMethod(data->postfix);

    pthread_mutex_lock(&mutex);
    if (!found)
    {
        best_result = data->result;
        found = 1;
        printf("Secant tim duoc nghiem la: %f\n", best_result);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main()
{
   
        struct timespec start, end;
        Token *output;
        char str[MAX];


        printf("\n \n");
        printf("Nhap bieu thuc: ");
        fgets(str, MAX, stdin);
        str[strcspn(str, "\n")] = 0;

        output = infixToPostfix(str);
        int XT = 0;

      

        if (output != NULL)
        { 
            for(int i= 0;i<100;i++){
                if((output[i].value.variable) == 'x'){
                    XT = 1;
                    break;
                }
            }

            printTokens(output);

    

                pthread_t threads[NUM_THREADS];    
                ThreadData threadData[NUM_THREADS]; 

                clock_gettime(CLOCK_MONOTONIC, &start);

                threadData[0].postfix = output;
                pthread_create(&threads[0], NULL, findrootNewton, (void *)&threadData[0]);      

                threadData[1].postfix = output;
                pthread_create(&threads[1], NULL, findrootBisection, (void *)&threadData[1]);      

                threadData[2].postfix = output;
                pthread_create(&threads[2], NULL, findrootSecant, (void *)&threadData[2]);

                for (int i = 0; i < NUM_THREADS; i++)
                {
                    pthread_join(threads[i], NULL);
                }
                clock_gettime(CLOCK_MONOTONIC, &end);
                double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
                printf("Thoi gian tim nghiem: %f giay\n\n", elapsed);
                printf("Ket qua voi nghiem %.5f la: %.3f\n", best_result, evaluatePostfix(output, best_result));
            }
            return 0;
} 
  
    

//
//(x^14-3*x^12+7*x^9)-(5*x^8+2*x^6)+(4*x^5-11*x^3+6*x^2)-(20*x-50)
//x^10+2*x+sin(x^2+2*x-10)