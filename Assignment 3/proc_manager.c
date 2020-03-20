#include <stdio.h>
#include <time.h>

// A function that terminates when enter key is pressed
void fun()
{
    printf("fun() starts \n");
    printf("Press enter to stop fun \n");
    while(1)
    {
        if (getchar())
            break;
    }
    printf("fun() ends \n");
}

int main(int argc, char *argv[])
{
    // Calculate the time taken by fun()
    struct timespec start, finish;
    double elapsed = 0;

//    clock_t t;
//    t = clock();
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("start %ld\n", start.tv_sec);
    fun();

    clock_gettime(CLOCK_MONOTONIC, &finish);
    printf("finish %ld\n", finish.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Finished at__, runtime duration %f\n", elapsed);
    return 0;
}
