#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    char newString[20][20];
    int i,j,ctr;

    fp = fopen("cmdfile.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {

        printf("---------------------------------------\n");

        printf("\n\n Split string by space into words :\n");

        printf("Retrieved line of length %zu:\n", read);
        printf("%s", line);



        j=0; ctr=0;
        for(i=0;i<=(strlen(line));i++)
        {
            // if space or NULL found, assign NULL into newString[ctr]
            if(line[i]==' '||line[i]=='\0')
            {
                newString[ctr][j]='\0';
                ctr++;  //for next word
                j=0;    //for next word, init index to 0
            }
            else
            {
                newString[ctr][j]=line[i];
                j++;
            }
        }
        printf("\n Strings or words after split by space are :\n");
        for(i=0;i < ctr;i++)
            printf(" %s\n",newString[i]);
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}