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
            printf("\n");
            scanf("%s", inputtext);
            errorflag=stat(inputtext,&inode);
            if (errorflag<0)
                fprintf(stderr, "Not found! (Not a file or directory)\n");
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

unsigned char* TestArray(int *NumCh)
{
    unsigned int randnum;
    int i = 0;
    char* string = "Tesztszoveg";
    *NumCh = strlen(string);
    unsigned char* array = (unsigned char*) malloc((*NumCh)*3);
    for (int i=0; i<(*NumCh)*3; i++)
    {
        array[i] = 0;
    }

    while (i != (*NumCh)*3)
    {
        srand(time(NULL));
        if (i%3 == 0 || i%3 == 3)
        {
            randnum = rand()%64;
            array[i] = randnum << 2;
        }
        else
        {
            randnum = rand()%32;
            array[i] = randnum << 3;
        }
        i++;
    }
    i=0;
    while (i != (*NumCh)*3)
    {
        for (int j=0; j<(*NumCh); j++)
        {
            unsigned char kar = string[j];
            array[i] = array[i] | ((0b11000000 & kar)>>6);
            array[i+1] = array[i+1] | ((0b00111000 & kar)>>3);
            array[i+2] = array[i+2] | (0b00000111 & kar);
            i += 3;
        }

    }
    return array;
}

int main(int argc, char* argv[])
{
    int in;
    int NumCh;
    char filename[1000];
    if (argc == 1)
    {
        BrowseForOpen();
        return 0;
    }
    else
    {
        if (strcmp(argv[1], "--version")==0)
        {
            printf("v. 0.01\nDate:2019.02.18.\nName");
            return 0;
        }
        if (strcmp(argv[1], "--help")==0)
        {
            printf("Help");
            return 0;
        }
        strcpy(filename, argv[1]);
        printf("%s\n",filename);
        unsigned char* abc_array= TestArray(&NumCh);
        unsigned char* stringarray = Unwrap(abc_array, NumCh);
        printf("%s\n", stringarray);
        free(stringarray);

        in = open(filename,O_RDONLY);
        unsigned char* filearray = ReadPixels(in, &NumCh);
        unsigned char* textarray = Unwrap(filearray, NumCh);
        printf("%s", textarray);
        return 0;
    }
}
