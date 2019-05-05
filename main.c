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
            printf("v. 1.0\nDate:2019.05.06.\nKohari Adam\n");
            return 0;
        }
        if (strcmp(argv[1], "--help")==0)
        {
            printf("A program futtatasa ketfelekepp lehetseges:\n- A dekodolando fajl eleresi utjat parancssori argumentumkent megadva\n- Megadott argumentum nelkul inditva, a beepitett fajltallozo alprogrammal\n");
            return 0;
        }
        strcpy(filename, argv[1]);

        in = open(filename,O_RDONLY);
        if (in < 0)
        {
        	fprintf(stderr, "%s\n", "Fajlmegnyitas sikertelen!");
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
        case 0: printf("A dekodolt szoveg sikeresen postolva lett a webszerverre!\n"); break;
        case 2: fprintf(stderr, "Socket letrehozasi hiba!\n"); break;
    	case 3: fprintf(stderr, "Csatlakozasi hiba!\n"); break;
    	case 4: fprintf(stderr, "Kuldesi hiba!\n"); break;
        case 5: fprintf(stderr, "Fogadasi hiba!\n"); break;
        case 6: fprintf(stderr, "A szervernek nem sikerult feldolgoznia a kerest!");
    }

    close(in);
    free(textarray);
    return 0;
}
