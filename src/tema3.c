#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_DIGIT_SIZE 3
#define MESSAGE_SIZE 64

/* function that returns the minimum from two elements */
int minn(int x, int y)
{
    if (x < y)
        return x;
    return y;
}

/* function that return a char* that contains the integer number */
char* get_string_form(int number)
{
    char* string = malloc(NUMBER_DIGIT_SIZE * sizeof(char));
    sprintf(string, "%d", number);
    return string;
}

/* function that returns a message that contains a manager and its workers
   in the expected format */
char* append_workers_to_message(int index, int worker_count, int* workers)
{
    char* message = malloc(MESSAGE_SIZE * sizeof(char));
    strcpy(message, "\0");
    strcat(message, " ");
    char* string_index = get_string_form(index);
    strcat(message, string_index);
    free(string_index);
    strcat(message, ":");
    for (int i = 0; i < worker_count - 1; i++)
    {
        char* get_string_worker = get_string_form(workers[i]);
        strcat(message, get_string_worker);
        free(get_string_worker);
        strcat(message, ",");
    }
    strcat(message, get_string_form(workers[worker_count - 1]));
    return message;
}

/* function that returns a message containing all managers and their workers
   in the expected format */
char* get_message(int worker_count0, int worker_count1, int worker_count2,
                     int worker_count3, int* workers0, int* workers1,
                     int* workers2, int* workers3)
{
    char* message = malloc(MESSAGE_SIZE * sizeof(char));
    strcpy(message, "\0");
    char* message_workers0 = append_workers_to_message(0, worker_count0, workers0);
    char* message_workers1 = append_workers_to_message(1, worker_count1, workers1);
    char* message_workers2 = append_workers_to_message(2, worker_count2, workers2);
    char* message_workers3 = append_workers_to_message(3, worker_count3, workers3);
    strcat(message, message_workers0);
    strcat(message, message_workers1);
    strcat(message, message_workers2);
    strcat(message, message_workers3);
    free(message_workers0);
    free(message_workers1);
    free(message_workers2);
    free(message_workers3);
    strcat(message, "\n");
    return message;
}

int main(int argc, char *argv[])
{
    int numtasks, rank;

    /* initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* declare variables that many variables independently use, so that
       you do not have to declare them everywhere. Note that the variables
       are initialized by every process that uses them, so they are not
       shared or anything like that */
    int* workers0, *workers1, *workers2, *workers3;
    int worker_count0, worker_count1, worker_count2, worker_count3;
    int n, w, *v;

    int* calculation_array_count, *calculation_array0, *calculation_array1;
    int* calculation_array2, *calculation_array3;
    int* computed_vector0, *computed_vector1;
    int* computed_vector2, *computed_vector3;

    /* the actions of manager 0 */
    if (rank == 0)
    {
        /* opens its file and reads its workers */
        FILE* pFile = fopen("cluster0.txt", "r");
        fscanf(pFile, "%d", &worker_count0);
        int worker;
        /* saves its workers */
        workers0 = malloc(worker_count0 * sizeof(int));
        for (int i = 0; i < worker_count0; i++)
        {
            fscanf(pFile, "%d", &worker);
            workers0[i] = worker;
        }

        /* operation 6, operation 7, operation 8, operation 9, operation 10,
           operation 11: manager 3 sends manager 0 the workers of managers 1,
           2 and 3 */
        MPI_Status status;
        MPI_Recv(&worker_count1, 1, MPI_INT, 3, 6, MPI_COMM_WORLD, &status);
        workers1 = malloc(worker_count1 * sizeof(int));
        MPI_Recv(workers1, worker_count1, MPI_INT, 3, 7, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count2, 1, MPI_INT, 3, 8, MPI_COMM_WORLD, &status);
        workers2 = malloc(worker_count2 * sizeof(int));
        MPI_Recv(workers2, worker_count2, MPI_INT, 3, 9, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count3, 1, MPI_INT, 3, 10, MPI_COMM_WORLD, &status);
        workers3 = malloc(worker_count3 * sizeof(int));
        MPI_Recv(workers3, worker_count3, MPI_INT, 3, 11, MPI_COMM_WORLD, &status);

        /* operation 12, operation 13: manager 0 sends manager 3 the workers of
           manager 0 */
        printf("M(%d,%d)\n", 0, 3);
        MPI_Send(&worker_count0, 1, MPI_INT, 3, 12, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 0, 3);
        MPI_Send(workers0, worker_count0, MPI_INT, 3, 13, MPI_COMM_WORLD);

        /* manager 0 prints topology */
        char* message = get_message(worker_count0, worker_count1,
                                    worker_count2, worker_count3,
                                    workers0, workers1,
                                    workers2, workers3);
        printf("%d ->%s", rank, message);
        free(message);

        /* operation 24, operation 25, operation 26, operation 27: manager 0
           sends its workers its topology. Every worker process expects 4
           messages tagged 24-27 with every manager's topology, which it gets
           in its entirety from its manager */
        for (int i = 0; i < worker_count0; i++)
        {
            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(&worker_count0, 1, MPI_INT,
                     workers0[i], 24, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(workers0, worker_count0, MPI_INT,
                     workers0[i], 24, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(&worker_count1, 1, MPI_INT,
                     workers0[i], 25, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(workers1, worker_count1, MPI_INT,
                     workers0[i], 25, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(&worker_count2, 1, MPI_INT,
                     workers0[i], 26, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(workers2, worker_count2, MPI_INT,
                     workers0[i], 26, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(&worker_count3, 1, MPI_INT,
                     workers0[i], 27, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(workers3, worker_count3, MPI_INT,
                     workers0[i], 27, MPI_COMM_WORLD);
        }

        /* manager 0 generates v array */
        w = worker_count0 + worker_count1 + worker_count2 + worker_count3;
        n = atoi(argv[1]);
        v = malloc(n * sizeof(int));
        for (int k = 0; k < n; k++)
            v[k] = n - k - 1;

        /* manager 0 computes how to best distribute multiplications among
           all managers, keeping in mind the managers' amount of workers.
           (e.g. if a manager has 3 times more workers than another manager,
           it will get ~3 times more multiplications)*/
        int unit = n / w;
        int greater_worker_count = n % w;
        calculation_array_count = malloc(4 * sizeof(int));

        calculation_array_count[0] = unit * worker_count0;
        calculation_array_count[0] += minn(worker_count0, greater_worker_count);
        greater_worker_count -= minn(worker_count0, greater_worker_count);
        calculation_array_count[1] = unit * worker_count1;
        calculation_array_count[1] += minn(worker_count1, greater_worker_count);
        greater_worker_count -= minn(worker_count1, greater_worker_count);
        calculation_array_count[2] = unit * worker_count2;
        calculation_array_count[2] += minn(worker_count2, greater_worker_count);
        greater_worker_count -= minn(worker_count2, greater_worker_count);
        calculation_array_count[3] = unit * worker_count3;
        calculation_array_count[3] += minn(worker_count3, greater_worker_count);
        greater_worker_count -= minn(worker_count3, greater_worker_count);

        /* after determining which manager gets what, splits the array
           accordingly and saves the parts in 4 different arrays */
        calculation_array0 = malloc(calculation_array_count[0] * sizeof(int));
        calculation_array1 = malloc(calculation_array_count[1] * sizeof(int));
        calculation_array2 = malloc(calculation_array_count[2] * sizeof(int));
        calculation_array3 = malloc(calculation_array_count[3] * sizeof(int));

        int pos = 0;

        for (int i = 0; i < calculation_array_count[0]; i++)
            calculation_array0[i] = v[i + pos];
        pos += calculation_array_count[0];

        for (int i = 0; i < calculation_array_count[1]; i++)
            calculation_array1[i] = v[i + pos];
        pos += calculation_array_count[1];

        for (int i = 0; i < calculation_array_count[2]; i++)
            calculation_array2[i] = v[i + pos];
        pos += calculation_array_count[2];

        for (int i = 0; i < calculation_array_count[3]; i++)
            calculation_array3[i] = v[i + pos];
        pos += calculation_array_count[3];

        /* operation 28, operation 29, operation 30, operation 31: manager 0
           sends manager 3 the lengths of the 4 split arrays, as well as the
           arrays that managers 1, 2 and 3 need to multiply with their workers */
        printf("M(%d,%d)\n", 0, 3);
        MPI_Send(calculation_array_count, 4,
                 MPI_INT, 3, 28, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 0, 3);
        MPI_Send(calculation_array1, calculation_array_count[1],
                 MPI_INT, 3, 29, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 0, 3);
        MPI_Send(calculation_array2, calculation_array_count[2],
                 MPI_INT, 3, 30, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 0, 3);
        MPI_Send(calculation_array3, calculation_array_count[3],
                 MPI_INT, 3, 31, MPI_COMM_WORLD);

        /* do the same algorithm that we used to split the array fairly among
           managers, but do it with this manager's array and split it
           fairly among their workers */
        int worker_unit = calculation_array_count[0] / worker_count0;
        int worker_greater_worker_count = calculation_array_count[0] % worker_count0;

        /* operation 37: each manager splits the array it has to multiply
           evenly among its workers and sends one split array to every worker
           it has */
        int worker_pos = 0;
        for (int i = 0; i < worker_count0; i++)
        {
            int vector_length = worker_unit;
            if (worker_greater_worker_count > 0)
            {
                vector_length += 1;
                worker_greater_worker_count--;
            }
            int* to_compute_vector = malloc(vector_length * sizeof(int));
            for (int i = 0; i < vector_length; i++)
                to_compute_vector[i] = calculation_array0[i + worker_pos];

            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(&vector_length, 1,
                     MPI_INT, workers0[i], 37, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 0, workers0[i]);
            MPI_Send(to_compute_vector, vector_length,
                     MPI_INT, workers0[i], 37, MPI_COMM_WORLD);

            worker_pos += vector_length;
        }

        /* after each worker multiplied its array, it sends it back
           to its manager */
        int computed_vector_length = calculation_array_count[0];
        int* computed_vector = malloc(computed_vector_length * sizeof(int));
        int computed_pos = 0;
        for (int i = 0; i < worker_count0; i++)
        {
            int tmp_length;
            MPI_Recv(&tmp_length, 1,
                     MPI_INT, workers0[i], 37, MPI_COMM_WORLD, &status);
            int* tmp_vector = malloc(tmp_length * sizeof(int));
            MPI_Recv(tmp_vector, tmp_length,
                     MPI_INT, workers0[i], 37, MPI_COMM_WORLD, &status);
            for (int j = 0; j < tmp_length; j++)
                computed_vector[j + computed_pos] = tmp_vector[j];
            computed_pos += tmp_length;
        }

        /* operation 41, operation 42, operation 43: manager 3 sends manager 0
           the multiplied split arrays of manager 1, manager 2 and manager 3 */
        computed_vector1 = malloc(calculation_array_count[1] * sizeof(int));
        MPI_Recv(computed_vector1, calculation_array_count[1],
                 MPI_INT, 3, 41, MPI_COMM_WORLD, &status);
        computed_vector2 = malloc(calculation_array_count[2] * sizeof(int));
        MPI_Recv(computed_vector2, calculation_array_count[2],
                 MPI_INT, 3, 42, MPI_COMM_WORLD, &status);
        computed_vector3 = malloc(calculation_array_count[3] * sizeof(int));
        MPI_Recv(computed_vector3, calculation_array_count[3],
                 MPI_INT, 3, 43, MPI_COMM_WORLD, &status);

        /* manager 0 reassembls the split arrays it got from its workers and
           managers 1, 2 and 3 */
        computed_vector0 = malloc(calculation_array_count[0] * sizeof(int));
        for (int i = 0; i < calculation_array_count[0]; i++)
            computed_vector0[i] = computed_vector[i];

        int* result = malloc(n * sizeof(int));
        int result_pos = 0;
        for (int i = 0; i < calculation_array_count[0]; i++)
            result[i + result_pos] = computed_vector0[i];
        result_pos += calculation_array_count[0];
        for (int i = 0; i < calculation_array_count[1]; i++)
            result[i + result_pos] = computed_vector1[i];
        result_pos += calculation_array_count[1];
        for (int i = 0; i < calculation_array_count[2]; i++)
            result[i + result_pos] = computed_vector2[i];
        result_pos += calculation_array_count[2];
        for (int i = 0; i < calculation_array_count[3]; i++)
            result[i + result_pos] = computed_vector3[i];
        result_pos += calculation_array_count[3];

        /* manager 0 prints the reassembled, multiplied array */
        printf("Rezultat:");
        for (int i = 0; i < n; i++)
            printf(" %d", result[i]);
        printf("\n");

        /* free the memory */
        free(workers0);
        free(workers1);
        free(workers2);
        free(workers3);
        free(calculation_array_count);
        free(calculation_array0);
        free(calculation_array1);
        free(calculation_array2);
        free(calculation_array3);
        free(computed_vector);
        free(computed_vector0);
        free(computed_vector1);
        free(computed_vector2);
        free(computed_vector3);
        free(result);
    }
    /* the actions of manager 1 */
    else if (rank == 1)
    {
        FILE* pFile = fopen("cluster1.txt", "r");
        fscanf(pFile, "%d", &worker_count1);
        int worker;
        workers1 = malloc(worker_count1 * sizeof(int));
        for (int i = 0; i < worker_count1; i++)
        {
            fscanf(pFile, "%d", &worker);
            workers1[i] = worker;
        }

        /* operation 0, operation 1: manager 1 sends manager 2 the workers of
           manager 1 */
        printf("M(%d,%d)\n", 1, 2);
        MPI_Send(&worker_count1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 1, 2);
        MPI_Send(workers1, worker_count1, MPI_INT, 2, 1, MPI_COMM_WORLD);

        /* operation 18, operation 19, operation 20, operation 21,
           operation 22, operation 23: manager 2 sends manager 1 the workers
           of manager 0, 2 and 3 */
        MPI_Status status;
        MPI_Recv(&worker_count0, 1,
                 MPI_INT, 2, 18, MPI_COMM_WORLD, &status);
        workers0 = malloc(worker_count0 * sizeof(int));
        MPI_Recv(workers0, worker_count0,
                 MPI_INT, 2, 19, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count2, 1,
                 MPI_INT, 2, 20, MPI_COMM_WORLD, &status);
        workers2 = malloc(worker_count2 * sizeof(int));
        MPI_Recv(workers2, worker_count2,
                 MPI_INT, 2, 21, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count3, 1,
                 MPI_INT, 2, 22, MPI_COMM_WORLD, &status);
        workers3 = malloc(worker_count3 * sizeof(int));
        MPI_Recv(workers3, worker_count3,
                 MPI_INT, 2, 23, MPI_COMM_WORLD, &status);

        /* manager 1 prints topology */
        char* message = get_message(worker_count0, worker_count1,
                                    worker_count2, worker_count3,
                                    workers0, workers1,
                                    workers2, workers3);
        printf("%d ->%s", rank, message);
        free(message);

        /* operation 24, operation 25, operation 26, operation 27: manager 1
           sends its workers its topology. Every worker process expects 4
           messages tagged 24-27 with every manager's topology, which it gets
           in its entirety from its manager */
        for (int i = 0; i < worker_count1; i++)
        {
            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(&worker_count0, 1,
                     MPI_INT, workers1[i], 24, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(workers0, worker_count0,
                     MPI_INT, workers1[i], 24, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(&worker_count1, 1,
                     MPI_INT, workers1[i], 25, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(workers1, worker_count1,
                     MPI_INT, workers1[i], 25, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(&worker_count2, 1,
                     MPI_INT, workers1[i], 26, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(workers2, worker_count2,
                     MPI_INT, workers1[i], 26, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(&worker_count3, 1,
                     MPI_INT, workers1[i], 27, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(workers3, worker_count3,
                     MPI_INT, workers1[i], 27, MPI_COMM_WORLD);
        }

        /* operation 35, operation 36: manager 2 sends manager 1 the array of
           elements its workers have to multiply and its length */
        calculation_array_count = malloc(4 * sizeof(int));
        MPI_Recv(calculation_array_count, 4,
                 MPI_INT, 2, 35, MPI_COMM_WORLD, &status);

        calculation_array1 = malloc(calculation_array_count[1] * sizeof(int));
        MPI_Recv(calculation_array1, calculation_array_count[1],
                 MPI_INT, 2, 36, MPI_COMM_WORLD, &status);

        int worker_unit = calculation_array_count[1] / worker_count1;
        int worker_greater_worker_count = calculation_array_count[1] % worker_count1;

        /* operation 37: each manager splits the array it has to multiply
           evenly among its workers and sends one split array to every worker
           it has */
        int worker_pos = 0;
        for (int i = 0; i < worker_count1; i++)
        {
            int vector_length = worker_unit;
            if (worker_greater_worker_count > 0)
            {
                vector_length += 1;
                worker_greater_worker_count--;
            }
            int* to_compute_vector = malloc(vector_length * sizeof(int));
            for (int i = 0; i < vector_length; i++)
                to_compute_vector[i] = calculation_array1[i + worker_pos];

            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(&vector_length, 1, MPI_INT,
                     workers1[i], 37, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 1, workers1[i]);
            MPI_Send(to_compute_vector, vector_length,
                     MPI_INT, workers1[i], 37, MPI_COMM_WORLD);

            worker_pos += vector_length;
        }

        /* after each worker multiplied its array, it sends it back
           to its manager */
        int computed_vector_length = calculation_array_count[1];
        int* computed_vector = malloc(computed_vector_length * sizeof(int));
        int computed_pos = 0;
        for (int i = 0; i < worker_count1; i++)
        {
            int tmp_length;
            MPI_Recv(&tmp_length, 1,
                     MPI_INT, workers1[i], 37, MPI_COMM_WORLD, &status);
            int* tmp_vector = malloc(tmp_length * sizeof(int));
            MPI_Recv(tmp_vector, tmp_length,
                     MPI_INT, workers1[i], 37, MPI_COMM_WORLD, &status);
            for (int j = 0; j < tmp_length; j++)
                computed_vector[j + computed_pos] = tmp_vector[j];
            computed_pos += tmp_length;
        }

        /* operation 38: manager 1 sends manager 2 the split array
           multiplied by the workers of manager 1 */
        computed_vector1 = malloc(calculation_array_count[1] * sizeof(int));
        for (int i = 0; i < calculation_array_count[1]; i++)
            computed_vector1[i] = computed_vector[i];
        printf("M(%d,%d)\n", 1, 2);
        MPI_Send(computed_vector1, calculation_array_count[1],
                 MPI_INT, 2, 38, MPI_COMM_WORLD);

        /* free the memory */
        free(workers0);
        free(workers1);
        free(workers2);
        free(workers3);
        free(calculation_array_count);
        free(calculation_array1);
        free(computed_vector);
        free(computed_vector1);
    }
    /* the actions of manager 2 */
    else if (rank == 2)
    {
        FILE* pFile = fopen("cluster2.txt", "r");
        fscanf(pFile, "%d", &worker_count2);
        int worker;
        workers2 = malloc(worker_count2 * sizeof(int));
        for (int i = 0; i < worker_count2; i++)
        {
            fscanf(pFile, "%d", &worker);
            workers2[i] = worker;
        }

        /* operation 0, operation 1: manager 1 sends manager 2 the workers of
           manager 1 */
        MPI_Status status;
        MPI_Recv(&worker_count1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        workers1 = malloc(worker_count1 * sizeof(int));
        MPI_Recv(workers1, worker_count1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);

        /* operation 2, operation 3, operation 4, operation 5: manager 2 sends
           manager 3 the workers of managers 1 and 2 */
        printf("M(%d,%d)\n", 2, 3);
        MPI_Send(&worker_count1, 1, MPI_INT, 3, 2, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 2, 3);
        MPI_Send(workers1, worker_count1, MPI_INT, 3, 3, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", 2, 3);
        MPI_Send(&worker_count2, 1, MPI_INT, 3, 4, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 2, 3);
        MPI_Send(workers2, worker_count2, MPI_INT, 3, 5, MPI_COMM_WORLD);

        /* operation 14, operation 15, operation 16, operation 17: manager 3
           sends manager 2 the workers of managers 0 and 3*/
        MPI_Recv(&worker_count0, 1, MPI_INT, 3, 14, MPI_COMM_WORLD, &status);
        workers0 = malloc(worker_count0 * sizeof(int));
        MPI_Recv(workers0, worker_count0, MPI_INT, 3, 15, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count3, 1, MPI_INT, 3, 16, MPI_COMM_WORLD, &status);
        workers3 = malloc(worker_count3 * sizeof(int));
        MPI_Recv(workers3, worker_count3, MPI_INT, 3, 17, MPI_COMM_WORLD, &status);

        /* operation 18, operation 19, operation 20, operation 21,
           operation 22, operation 23: manager 2 sends manager 1 the workers
           of manager 0, 2 and 3 */
        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(&worker_count0, 1, MPI_INT, 1, 18, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(workers0, worker_count0, MPI_INT, 1, 19, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(&worker_count2, 1, MPI_INT, 1, 20, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(workers2, worker_count2, MPI_INT, 1, 21, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(&worker_count3, 1, MPI_INT, 1, 22, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(workers3, worker_count3, MPI_INT, 1, 23, MPI_COMM_WORLD);

        /* manager 2 prints topology */
        char* message = get_message(worker_count0, worker_count1,
                                    worker_count2, worker_count3,
                                    workers0, workers1,
                                    workers2, workers3);
        printf("%d ->%s", rank, message);
        free(message);

        /* operation 24, operation 25, operation 26, operation 27: manager 2
           sends its workers its topology. Every worker process expects 4
           messages tagged 24-27 with every manager's topology, which it gets
           in its entirety from its manager */
        for (int i = 0; i < worker_count2; i++)
        {
            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(&worker_count0, 1,
                     MPI_INT, workers2[i], 24, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(workers0, worker_count0,
                     MPI_INT, workers2[i], 24, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(&worker_count1, 1,
                     MPI_INT, workers2[i], 25, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(workers1, worker_count1,
                     MPI_INT, workers2[i], 25, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(&worker_count2, 1,
                     MPI_INT, workers2[i], 26, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(workers2, worker_count2,
                     MPI_INT, workers2[i], 26, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(&worker_count3, 1,
                     MPI_INT, workers2[i], 27, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(workers3, worker_count3,
                     MPI_INT, workers2[i], 27, MPI_COMM_WORLD);
        }

        /* operation 32, operation 33, operation 34: manager 3 sends manager 2
           the lengths of the 4 split arrays, as well as the arrays that managers
           1 and 2 need to multiply with their workers */
        calculation_array_count = malloc(4 * sizeof(int));
        MPI_Recv(calculation_array_count, 4,
                 MPI_INT, 3, 32, MPI_COMM_WORLD, &status);

        calculation_array1 = malloc(calculation_array_count[1] * sizeof(int));
        MPI_Recv(calculation_array1, calculation_array_count[1],
                 MPI_INT, 3, 33, MPI_COMM_WORLD, &status);

        calculation_array2 = malloc(calculation_array_count[2] * sizeof(int));
        MPI_Recv(calculation_array2, calculation_array_count[2],
                 MPI_INT, 3, 34, MPI_COMM_WORLD, &status);

        /* operation 35, operation 36: manager 2 sends manager 1 the array of
           elements its workers have to multiply and its length */
        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(calculation_array_count, 4,
                 MPI_INT, 1, 35, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 2, 1);
        MPI_Send(calculation_array1, calculation_array_count[1],
                 MPI_INT, 1, 36, MPI_COMM_WORLD);

        int worker_unit = calculation_array_count[2] / worker_count2;
        int worker_greater_worker_count = calculation_array_count[2] % worker_count2;

        /* operation 37: each manager splits the array it has to multiply
           evenly among its workers and sends one split array to every worker
           it has */
        int worker_pos = 0;
        for (int i = 0; i < worker_count2; i++)
        {
            int vector_length = worker_unit;
            if (worker_greater_worker_count > 0)
            {
                vector_length += 1;
                worker_greater_worker_count--;
            }
            int* to_compute_vector = malloc(vector_length * sizeof(int));
            for (int i = 0; i < vector_length; i++)
                to_compute_vector[i] = calculation_array2[i + worker_pos];

            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(&vector_length, 1,
                     MPI_INT, workers2[i], 37, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 2, workers2[i]);
            MPI_Send(to_compute_vector, vector_length,
                     MPI_INT, workers2[i], 37, MPI_COMM_WORLD);

            worker_pos += vector_length;
        }

        /* after each worker multiplied its array, it sends it back
           to its manager */
        int computed_vector_length = calculation_array_count[2];
        int* computed_vector = malloc(computed_vector_length * sizeof(int));
        int computed_pos = 0;
        for (int i = 0; i < worker_count2; i++)
        {
            int tmp_length;
            MPI_Recv(&tmp_length, 1,
                     MPI_INT, workers2[i], 37, MPI_COMM_WORLD, &status);
            int* tmp_vector = malloc(tmp_length * sizeof(int));
            MPI_Recv(tmp_vector, tmp_length,
                     MPI_INT, workers2[i], 37, MPI_COMM_WORLD, &status);
            for (int j = 0; j < tmp_length; j++)
                computed_vector[j + computed_pos] = tmp_vector[j];
            computed_pos += tmp_length;
        }

        /* operation 38: manager 1 sends manager 2 the split array
           multiplied by the workers of manager 1 */
        computed_vector1 = malloc(calculation_array_count[1] * sizeof(int));
        MPI_Recv(computed_vector1, calculation_array_count[1],
                 MPI_INT, 1, 38, MPI_COMM_WORLD, &status);

        /* operation 39, operation 40: manager 2 sends manager 3 the split arrays
           multiplied by the workers of managers 1 and 2 */
        computed_vector2 = malloc(calculation_array_count[2] * sizeof(int));
        for (int i = 0; i < calculation_array_count[2]; i++)
            computed_vector2[i] = computed_vector[i];

        printf("M(%d,%d)\n", 2, 3);
        MPI_Send(computed_vector1, calculation_array_count[1],
                 MPI_INT, 3, 39, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 2, 3);
        MPI_Send(computed_vector2, calculation_array_count[2],
                 MPI_INT, 3, 40, MPI_COMM_WORLD);

        /* free the memory */
        free(workers0);
        free(workers1);
        free(workers2);
        free(workers3);
        free(calculation_array_count);
        free(calculation_array1);
        free(calculation_array2);
        free(computed_vector);
        free(computed_vector1);
        free(computed_vector2);
    }
    /* the actions of manager 3 */
    else if (rank == 3)
    {
        FILE* pFile = fopen("cluster3.txt", "r");
        fscanf(pFile, "%d", &worker_count3);
        int worker;
        workers3 = malloc(worker_count3 * sizeof(int));
        for (int i = 0; i < worker_count3; i++)
        {
            fscanf(pFile, "%d", &worker);
            workers3[i] = worker;
        }

        /* operation 2, operation 3, operation 4, operation 5: manager 2 sends
           manager 3 the workers of managers 1 and 2 */
        MPI_Status status;
        MPI_Recv(&worker_count1, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        workers1 = malloc(worker_count1 * sizeof(int));
        MPI_Recv(workers1, worker_count1, MPI_INT, 2, 3, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count2, 1, MPI_INT, 2, 4, MPI_COMM_WORLD, &status);
        workers2 = malloc(worker_count2 * sizeof(int));
        MPI_Recv(workers2, worker_count2, MPI_INT, 2, 5, MPI_COMM_WORLD, &status);

        /* operation 6, operation 7, operation 8, operation 9, operation 10,
           operation 11: manager 3 sends manager 0 the workers of managers 1,
           2 and 3 */
        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(&worker_count1, 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(workers1, worker_count1, MPI_INT, 0, 7, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(&worker_count2, 1, MPI_INT, 0, 8, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(workers2, worker_count2, MPI_INT, 0, 9, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(&worker_count3, 1, MPI_INT, 0, 10, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(workers3, worker_count3, MPI_INT, 0, 11, MPI_COMM_WORLD);

        /* operation 12, operation 13: manager 0 sends manager 3 the workers of
           manager 0 */
        MPI_Recv(&worker_count0, 1, MPI_INT, 0, 12, MPI_COMM_WORLD, &status);
        workers0 = malloc(worker_count0 * sizeof(int));
        MPI_Recv(workers0, worker_count0, MPI_INT, 0, 13, MPI_COMM_WORLD, &status);

        /* operation 14, operation 15, operation 16, operation 17: manager 3
           sends manager 2 the workers of managers 0 and 3*/
        printf("M(%d,%d)\n", 3, 2);
        MPI_Send(&worker_count0, 1, MPI_INT, 2, 14, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 2);
        MPI_Send(workers0, worker_count0, MPI_INT, 2, 15, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", 3, 2);
        MPI_Send(&worker_count3, 1, MPI_INT, 2, 16, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 2);
        MPI_Send(workers3, worker_count3, MPI_INT, 2, 17, MPI_COMM_WORLD);

        /* manager 3 prints topology */
        char* message = get_message(worker_count0, worker_count1,
                                    worker_count2, worker_count3,
                                    workers0, workers1,
                                    workers2, workers3);
        printf("%d ->%s", rank, message);
        free(message);

        /* operation 24, operation 25, operation 26, operation 27: manager 0
           sends its workers its topology. Every worker process expects 4
           messages tagged 24-27 with every manager's topology, which it gets
           in its entirety from its manager */
        for (int i = 0; i < worker_count3; i++)
        {
            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(&worker_count0, 1,
                     MPI_INT, workers3[i], 24, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(workers0, worker_count0,
                     MPI_INT, workers3[i], 24, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(&worker_count1, 1,
                     MPI_INT, workers3[i], 25, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(workers1, worker_count1,
                     MPI_INT, workers3[i], 25, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(&worker_count2, 1,
                     MPI_INT, workers3[i], 26, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(workers2, worker_count2,
                     MPI_INT, workers3[i], 26, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(&worker_count3, 1,
                     MPI_INT, workers3[i], 27, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(workers3, worker_count3,
                     MPI_INT, workers3[i], 27, MPI_COMM_WORLD);
        }

        /* operation 28, operation 29, operation 30, operation 31: manager 0
           sends manager 3 the lengths of the 4 split arrays, as well as the
           arrays that managers 1, 2 and 3 need to multiply with their workers */
        calculation_array_count = malloc(4 * sizeof(int));
        MPI_Recv(calculation_array_count, 4,
                 MPI_INT, 0, 28, MPI_COMM_WORLD, &status);

        calculation_array1 = malloc(calculation_array_count[1] * sizeof(int));
        MPI_Recv(calculation_array1, calculation_array_count[1],
                 MPI_INT, 0, 29, MPI_COMM_WORLD, &status);

        calculation_array2 = malloc(calculation_array_count[2] * sizeof(int));
        MPI_Recv(calculation_array2, calculation_array_count[2],
                 MPI_INT, 0, 30, MPI_COMM_WORLD, &status);

        calculation_array3 = malloc(calculation_array_count[3] * sizeof(int));
        MPI_Recv(calculation_array3, calculation_array_count[3],
                 MPI_INT, 0, 31, MPI_COMM_WORLD, &status);

        /* operation 32, operation 33, operation 34: manager 3 sends manager 2
           the lengths of the 4 split arrays, as well as the arrays that managers
           1 and 2 need to multiply with their workers */
        printf("M(%d,%d)\n", 3, 2);
        MPI_Send(calculation_array_count, 4,
                 MPI_INT, 2, 32, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 2);
        MPI_Send(calculation_array1, calculation_array_count[1],
                 MPI_INT, 2, 33, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 2);
        MPI_Send(calculation_array2, calculation_array_count[2],
                 MPI_INT, 2, 34, MPI_COMM_WORLD);

        int worker_unit = calculation_array_count[3] / worker_count3;
        int worker_greater_worker_count = calculation_array_count[3] % worker_count3;

        /* operation 37: each manager splits the array it has to multiply
           evenly among its workers and sends one split array to every worker
           it has */
        int worker_pos = 0;
        for (int i = 0; i < worker_count3; i++)
        {
            int vector_length = worker_unit;
            if (worker_greater_worker_count > 0)
            {
                vector_length += 1;
                worker_greater_worker_count--;
            }
            int* to_compute_vector = malloc(vector_length * sizeof(int));
            for (int i = 0; i < vector_length; i++)
                to_compute_vector[i] = calculation_array3[i + worker_pos];

            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(&vector_length, 1,
                     MPI_INT, workers3[i], 37, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", 3, workers3[i]);
            MPI_Send(to_compute_vector, vector_length,
                     MPI_INT, workers3[i], 37, MPI_COMM_WORLD);

            worker_pos += vector_length;
        }

        /* after each worker multiplied its array, it sends it back
           to its manager */
        int computed_vector_length = calculation_array_count[3];
        int* computed_vector = malloc(computed_vector_length * sizeof(int));
        int computed_pos = 0;
        for (int i = 0; i < worker_count3; i++)
        {
            int tmp_length;
            MPI_Recv(&tmp_length, 1,
                     MPI_INT, workers3[i], 37, MPI_COMM_WORLD, &status);
            int* tmp_vector = malloc(tmp_length * sizeof(int));
            MPI_Recv(tmp_vector, tmp_length,
                     MPI_INT, workers3[i], 37, MPI_COMM_WORLD, &status);
            for (int j = 0; j < tmp_length; j++)
                computed_vector[j + computed_pos] = tmp_vector[j];
            computed_pos += tmp_length;
        }

        /* operation 39, operation 40: manager 2 sends manager 3 the split arrays
           multiplied by the workers of managers 1 and 2 */
        computed_vector1 = malloc(calculation_array_count[1] * sizeof(int));
        MPI_Recv(computed_vector1, calculation_array_count[1],
                 MPI_INT, 2, 39, MPI_COMM_WORLD, &status);
        computed_vector2 = malloc(calculation_array_count[2] * sizeof(int));
        MPI_Recv(computed_vector2, calculation_array_count[2],
                 MPI_INT, 2, 40, MPI_COMM_WORLD, &status);

        /* operation 41, operation 42, operation 43: manager 3 sends manager 0
           the multiplied split arrays of manager 1, manager 2 and manager 3 */
        computed_vector3 = malloc(calculation_array_count[3] * sizeof(int));
        for (int i = 0; i < calculation_array_count[3]; i++)
            computed_vector3[i] = computed_vector[i];

        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(computed_vector1, calculation_array_count[1],
                 MPI_INT, 0, 41, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(computed_vector2, calculation_array_count[2],
                 MPI_INT, 0, 42, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", 3, 0);
        MPI_Send(computed_vector3, calculation_array_count[3],
                 MPI_INT, 0, 43, MPI_COMM_WORLD);

        free(workers0);
        free(workers1);
        free(workers2);
        free(workers3);
        free(calculation_array_count);
        free(calculation_array1);
        free(calculation_array2);
        free(calculation_array3);
        free(computed_vector);
        free(computed_vector1);
        free(computed_vector2);
        free(computed_vector3);
    }
    /* the actions of worker processes */
    else
    {
        /* operation 24, operation 25, operation 26, operation 27: every worker
           process gets the topology from its manager */
        MPI_Status status;
        MPI_Recv(&worker_count0, 1,
                 MPI_INT, MPI_ANY_SOURCE, 24, MPI_COMM_WORLD, &status);
        workers0 = malloc(worker_count0 * sizeof(int));
        MPI_Recv(workers0, worker_count0,
                 MPI_INT, MPI_ANY_SOURCE, 24, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count1, 1,
                 MPI_INT, MPI_ANY_SOURCE, 25, MPI_COMM_WORLD, &status);
        workers1 = malloc(worker_count1 * sizeof(int));
        MPI_Recv(workers1, worker_count1,
                 MPI_INT, MPI_ANY_SOURCE, 25, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count2, 1,
                 MPI_INT, MPI_ANY_SOURCE, 26, MPI_COMM_WORLD, &status);
        workers2 = malloc(worker_count2 * sizeof(int));
        MPI_Recv(workers2, worker_count2,
                 MPI_INT, MPI_ANY_SOURCE, 26, MPI_COMM_WORLD, &status);

        MPI_Recv(&worker_count3, 1,
                 MPI_INT, MPI_ANY_SOURCE, 27, MPI_COMM_WORLD, &status);
        workers3 = malloc(worker_count3 * sizeof(int));
        MPI_Recv(workers3, worker_count3,
                 MPI_INT, MPI_ANY_SOURCE, 27, MPI_COMM_WORLD, &status);

        /* every worker process prints topology */
        char* message = get_message(worker_count0, worker_count1,
                                    worker_count2, worker_count3,
                                    workers0, workers1,
                                    workers2, workers3);
        printf("%d ->%s", rank, message);
        free(message);

        /* operation 37: every worker gets an array from its manager, it
           multiplies all the elements of the array by 5, and sends it back
           to its manager (the process that send it the array aka
           status.MPI_SOURCE) */
        int vector_length;
        MPI_Recv(&vector_length, 1,
                 MPI_INT, MPI_ANY_SOURCE, 37, MPI_COMM_WORLD, &status);
        int* to_compute_vector = malloc(vector_length * sizeof(int));
        MPI_Recv(to_compute_vector, vector_length,
                 MPI_INT, MPI_ANY_SOURCE, 37, MPI_COMM_WORLD, &status);

        for (int i = 0; i < vector_length; i++)
            to_compute_vector[i] *= 5;

        printf("M(%d,%d)\n", rank, status.MPI_SOURCE);
        MPI_Send(&vector_length, 1,
                 MPI_INT, status.MPI_SOURCE, 37, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, status.MPI_SOURCE);
        MPI_Send(to_compute_vector, vector_length,
                 MPI_INT, status.MPI_SOURCE, 37, MPI_COMM_WORLD);

        free(workers0);
        free(workers1);
        free(workers2);
        free(workers3);
        free(to_compute_vector);
    }

    /* close MPI */
    MPI_Finalize();
}
