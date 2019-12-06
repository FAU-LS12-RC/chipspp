/* sort.c */
/* Jonathan P Dawson */
/* 2013-12-23 */

/* Based on the in-place algorithm on the Wikipedia page */
/* http://en.wikipedia.org/wiki/Quicksort#In-place_version */

/*globals*/
const unsigned length = 32;

/* partition subarray */
unsigned partition(
        int array[], 
        unsigned left, 
        unsigned right, 
        unsigned pivot_index)
{

    int temp, pivot_value;
    unsigned i, store_index;

    store_index = left;
    pivot_value = array[pivot_index];

    temp = array[pivot_index];
    array[pivot_index] = array[right];
    array[right] = temp;

    for(i=left; i<right; i++){
        if(array[i] <= pivot_value){
            temp = array[store_index];
            array[store_index] = array[i];
            array[i] = temp;
            store_index++;
        }
    }

    temp = array[store_index];
    array[store_index] = array[right];
    array[right] = temp;

    return store_index;
}

 
/* recursive sort */
void quick_sort(int array[], unsigned left, unsigned right){
    unsigned pivot;

    /* if the subarray has two or more elements */
    if (left < right){

        /* partition sub array into two further sub arrays */
        pivot = (left + right) >> 1;
        pivot = partition(array, left, right, pivot);

        /* push both subarrays onto stack */
        quick_sort(array, left, pivot-1);
        quick_sort(array, pivot+1, right);
    }

}

void main(){
    int array[length];
    unsigned i;

    /* Fill array with zeros */
    for(i=0; i<length; i++){
        array[i] = 0;
    }

    /* Add unsorted data to the array */
    array[0] = 16;
    array[1] = 15;
    array[2] = 14;
    array[3] = 13;
    array[4] = 12;
    array[5] = 11;
    array[6] = 10;
    array[7] = 9;
    array[8] = 8;
    array[9] = 7;
    array[10] = 6;
    array[11] = 5;
    array[12] = 4;
    array[13] = 3;
    array[14] = 2;
    array[15] = 1;

    /* Sort the array */
    quick_sort(array, 0, length-1);

    for(i=0; i<length; i++){
        report(array[i]);
    }

}
