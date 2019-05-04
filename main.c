#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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

int Post(char *neptunID, char *message, int NumCh)
{
   int s;
   int flag;
   int bytes;
   int err;
   unsigned int server_size;
   char on;
   char buffer[1024];
   char rcvbuffer[1024];
   struct sockaddr_in server;

   on   = 1;
   flag = 0;
   server.sin_family      = AF_INET;
   server.sin_addr.s_addr = inet_addr("193.6.135.148");
   server.sin_port        = htons(80);
   server_size = sizeof server;

   s = socket(AF_INET, SOCK_STREAM, 0 );
   if ( s < 0 ) 
		return 2;

   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
   setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);

   err = connect( s, (struct sockaddr *) &server, server_size);
   if ( err < 0 ) 
   {
		close(s);
		return 3;
   }
   

   int contentl = strlen("NeptunID=ABC123&PostedText=") + NumCh;
   snprintf(buffer, sizeof(buffer), 
   	"POST /~vargai/post.php HTTP/1.1\r\nHost: irh.inf.unideb.hu\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nNeptunID=%s&PostedText=%s", 
   contentl, neptunID, message);

   bytes = send(s, buffer, strlen(buffer)+1, flag);
   if ( bytes <= 0 ) 
   {
		close(s);
		return 4;
   }

   bytes = recv(s, rcvbuffer, 1024, flag);
   if ( bytes < 0 ) 
   {
		close(s);
		return 5;
   }

   if (strstr(rcvbuffer, "The message has been received.") == NULL)
   {
   		close(s);
   		return 6;
   }

   close(s);
   return 0;
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
