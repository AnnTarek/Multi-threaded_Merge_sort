#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int* arr;
    int left;
    int right;
} MergeArgs;


void merge(int* arr, int left, int mid, int right) {
    int i, j, k;
    int size1 = mid - left + 1;
    int size2 = right - mid;

    int left_array[size1], right_array[size2];

    for (i = 0; i < size1; i++)
        left_array[i] = arr[left + i];
    for (j = 0; j < size2; j++)
        right_array[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    //merge 2 arrays until at least one is empty
    while (i < size1 && j < size2) {
        if (left_array[i] <= right_array[j]) {
            arr[k] = left_array[i];
            i++;
        } else {
            arr[k] = right_array[j];
            j++;
        }
        k++;
    }
    //remaining elements in left_array
    while (i < size1) {
        arr[k] = left_array[i];
        i++;
        k++;
    }

    //remaining elements in right_array
    while (j < size2) {
        arr[k] = right_array[j];
        j++;
        k++;
    }
}

void* mergeSort(void* arg) {
    MergeArgs* args = (MergeArgs*)arg;
    int* arr = args->arr;
    int left = args->left;
    int right = args->right;

    if (left < right) {
        int mid = left + (right - left) / 2;

        pthread_t tid1, tid2;

        //defining the left and right subarrays using the struct
        MergeArgs args1 = {arr, left, mid};
        MergeArgs args2 = {arr, mid + 1, right};

        //creating 2 threads to sort each sub array concurrently
        pthread_create(&tid1, NULL, mergeSort, &args1);
        pthread_create(&tid2, NULL, mergeSort, &args2);

        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        //merge 2 sorted sub arrays
        merge(arr, left, mid, right);
    }

    pthread_exit(NULL);
}



void printSorted(int* arr, int size) {
    int i;
    printf("Sorted array: ");
    for (i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}
//reading size and elements of the array from the file
int getArray(char *filename, int *array_size, int **arr){
    FILE* fptr = fopen(filename,"r");

    if(fptr == NULL) {
    return -1;
    }
    //scan size of array
    fscanf(fptr, "%d", array_size);
    //allocate memory of the given size
    *arr = (int *)malloc((*array_size) * sizeof(int));

    //scan  nnnnnnnnnnnn  nnnnnnnnnelements of the array
    for(int i=0;i<(*array_size);i++)
    {
        fscanf(fptr, "%d", &(*arr)[i]);
    }

    fclose(fptr);
    return 0;
}

int main(int argc, char *argv[]) {
    //checking if the input file is not given
    if(argc!=2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    int array_size, *arr;
    if(getArray(argv[1], &array_size, &arr) == -1){
        printf("Error while opening the file\n");
        exit(1);
    }

    MergeArgs args = {arr, 0, array_size - 1};
    //create thread
    pthread_t tid;
    pthread_create(&tid, NULL, mergeSort, &args);
    //make main thread wait
    pthread_join(tid, NULL);

    printSorted(arr, array_size);
    free(arr);
    //pthread_exit(NULL);
    return 0;
}

