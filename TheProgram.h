#ifndef THE_PROGRAM_H
#define THE_PROGRAM_H

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
#include <omp.h>
#include <signal.h>

void WhatToDo(int sig)
{
	pid_t pid;
	if (sig == SIGALRM)
	{
		fprintf(stderr, "A program túl sokáig futott!");
		kill(getpid(), SIGKILL);
	}

	if (sig == SIGINT)
	{
		pid = fork();
		if (pid != 0)
		{
			kill(getpid(), SIGKILL);
		}
	}

}

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
    unsigned char* string = (unsigned char*) malloc(NumCh*3);

    if (string == NULL)
    {
        fprintf(stderr, "Memoriafoglalas sikertelen");
        exit(2);
    }

    int i, j;
   	#pragma omp parallel for schedule(guided) private(j)
   	for (i = 0; i < NumCh; i++)
   	{
   		j = i*3;
   		string[i] = 0;
        string[i] = Pbuff[j] <<6;
        string[i] = string[i] | ((Pbuff[j+1] & 0b00000111) << 3);
        string[i] = string[i] | (Pbuff[j+2] & 0b00000111);
   	}
    
    string[NumCh] = '\0';
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

#endif