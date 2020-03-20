#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "No file input\n");
        return 1;
    }

    int fd1[2];
    double totalSum = 0.0;
    int totalNums = 0;

    // Create children processes
    for (int i = 1; i < argc; i++) {
        if (pipe(fd1) == -1) {
            fprintf(stderr, "Pipe Failed");
            return 1;
        }

        pid_t p = fork();

        if (p < 0) {
            fprintf(stderr, "fork Failed");
            return 1;
        }

        // Child process
        if (p == 0) {
            FILE *fp = fopen(argv[i], "r");
            double sum = 0.0;
            int numOfNumbers = 0;

            // Close reading end of the pipe
            close(fd1[0]);

            // Check if the file successfully opened for reading
            if (fp != NULL) {
                double input;

                // Print a message for an empty file
                if (fscanf(fp, "%lf", &input) == EOF) {
                    fprintf(stderr,"No numbers in %s\n", argv[i]);
                }

                    // Find sum and numOfNumbers of current file
                else {
                    fseek(fp, 0, SEEK_SET);

                    while (fscanf(fp, "%lf", &input) != EOF) {
                        sum += input;
                        numOfNumbers++;
                    }
                }
            }

            // If the file couldn't be opened
            else {
                fprintf(stderr, "Cannot open %s\n", argv[i]);
            }

            // Write the number of numbers and sum to the parent
            write(fd1[1], &numOfNumbers, sizeof(numOfNumbers));
            write(fd1[1], &sum, sizeof(sum));

            // Close writing end of the pipe and the file
            close(fd1[1]);
            fclose(fp);

            // Exit child
            exit(0);
        }
    }


    for (int j = 1; j > argc; j++) {
        // Close writing end of the pipe
        close(fd1[1]);

        // Wait for child
        wait(NULL);

        // Read the sum and numOfNumbers of the current file from the pipe
        double sum;
        int numOfNumbers;
        read(fd1[0], &numOfNumbers, sizeof(numOfNumbers));
        read(fd1[0], &sum, sizeof(sum));


        // Add the sum and the total number of numbers from the current file to the totalSum and totalNums
        totalSum += sum;
        totalNums += numOfNumbers;
    }

    // Close reading end of the pipe
    close(fd1[0]);

    if (totalNums == 0) {
        printf("Files entered have no numbers\n");
    }
    else {
        printf("Average: %.6f\n", totalSum/totalNums);
    }

    return 0;
}
