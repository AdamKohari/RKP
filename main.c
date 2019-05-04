#include "TheProgram.h"

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
    signal(SIGINT, WhatToDo);
    signal(SIGALRM, WhatToDo);
    alarm(1);
    unsigned char* filearray = ReadPixels(in, &NumCh);
    unsigned char* textarray = Unwrap(filearray, NumCh);
    alarm(0);

    switch (Post("Z8MVK2", textarray, NumCh))
    {
        case 0: printf("A dekódolt szöveg sikeresen postolva lett a webszerverre!\n"); break;
        case 2: fprintf(stderr, "Socket létrehozási hiba!\n"); break;
    	case 3: fprintf(stderr, "Csatlakozási hiba!\n"); break;
    	case 4: fprintf(stderr, "Küldési hiba!\n"); break;
        case 5: fprintf(stderr, "Fogadási hiba!\n"); break;
        case 6: fprintf(stderr, "A szervernek nem sikerült feldolgoznia a kérést!");
    }

    close(in);
     return 0;
}