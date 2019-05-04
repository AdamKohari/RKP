#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


int BrowseForOpen()
{
    int filepointer;
    struct stat inode;
    int errorflag = -1;
    char inputtext[1000];
    char *homedir = getenv("HOME");
    struct dirent *entry;
    int breaker = 0;

    DIR *d;
    chdir(homedir);
    d=opendir(".");

    while ((entry = readdir(d)) != NULL)
    {
            printf("%s\n",(*entry).d_name);
    }

    while(1)
    {
        while (errorflag<0)
        {
            printf("###########################################\n");
            scanf("%s", inputtext);
            printf("###########################################\n");
            errorflag=stat(inputtext,&inode);
            if (errorflag<0)
                fprintf(stderr, "Nem található! (Nem fájl vagy könyvtár)\n");
        }
        errorflag = -1;

        if (inode.st_mode & S_IFDIR)
        {
            chdir(inputtext);
            closedir(d);
            d = opendir(".");
            while ((entry = readdir(d)) != NULL)
            {
                printf("%s\n",(*entry).d_name);
            }
        }
        if (inode.st_mode & S_IFREG)
            break;
    }

    filepointer = open(inputtext, O_RDONLY);
    if (filepointer < 0)
    {
    	fprintf(stderr, "%s\n", "Fájlmegnyitás sikertelen!");
       	exit(1);
    }

   return filepointer;

}

unsigned char* ReadPixels(int f, int* NumCh)
{
	lseek(f, 2, SEEK_SET);
	unsigned int size;
    unsigned int arraystart;
	read(f, &size, 4);
	read(f, NumCh, 4);
	read(f, &arraystart, 4);
    lseek(f, arraystart, SEEK_SET);
	unsigned char *array = (unsigned char *) malloc(size);
    if (array == NULL)
    {
        fprintf(stderr, "Memoriafoglalas sikertelen");
        exit(2);
    }
    read(f, array, size);
    return array;

}

unsigned char* Unwrap(char* Pbuff, int NumCh)
{
    int i = 0;
    int j = 0;
    unsigned char* string = (unsigned char*) malloc(NumCh*3);

    if (string == NULL)
    {
        fprintf(stderr, "Memoriafoglalas sikertelen");
        exit(2);
    }
    while(i != NumCh)
    {
        string[i] = 0;
        string[i] = Pbuff[j] <<6;
        string[i] = string[i] | ((Pbuff[j+1] & 0b00000111) << 3);
        string[i] = string[i] | (Pbuff[j+2] & 0b00000111);

        j += 3;
        i++;
    }
    string[i] = '\0';
    free(Pbuff);
    return string;
}


int main(int argc, char* argv[])
{
    int in;
    int NumCh;
    char filename[1000];
    if (argc == 1)
    {
        in = BrowseForOpen();
    }
    else
    {
        if (strcmp(argv[1], "--version")==0)
        {
            printf("v. 0.1\nDate:2019.02.18.\nName");
            return 0;
        }
        if (strcmp(argv[1], "--help")==0)
        {
            printf("Help");
            return 0;
        }
        strcpy(filename, argv[1]);

        in = open(filename,O_RDONLY);
        if (in < 0)
        {
        	fprintf(stderr, "%s\n", "Fájlmegnyitás sikertelen!");
        	exit(1);
        }
    }
    	unsigned char* filearray = ReadPixels(in, &NumCh);
        unsigned char* textarray = Unwrap(filearray, NumCh);
        printf("%s", textarray);

        return 0;
}
