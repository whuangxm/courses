// By Jianming Huang, Kuan Zhu, Grade 2014, Software School, Xiamen University
// Jan. 2015

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define PROTOPORT       5194            /* default protocol port number */
#define QUENUM 2
#define INITIALIZE closemark = 0;\
		   outputmark = 0;\
		   outputmarkc = 0;\
		   jud = 0;\
		   Rjud = 0;
struct position
{
    int i, j;
};
//
struct  hostent  *ptrh;  /* pointer to a host table entry       */
struct  protoent *ptrp;  /* pointer to a protocol table entry   */
struct  sockaddr_in sad; /* structure to hold server's address  */
struct  sockaddr_in cad; /* structure to hold client's address  */
int     sd, sd2;         /* socket descriptors                  */
int     port;            /* protocol port number                */
int     alen;
char    host[100];
int     option;
int     table[4][4];
int     Rtable[4][4];
char    closemark, outputmark, outputmarkc;
char    jud, Rjud;
//

int OutputTable(int table[][4])
{
    int i, j;
    for (i = 0; i <= 3; i++)
    {
        for (j = 0; j <= 3; j++)
        {
            if (table[i][j] < 0)
                table[i][j] = -table[i][j];
            printf("%5d ", table[i][j]);
        }
        printf("\n");
        printf("\n");
    }
    return 0;
}
int RanInsert(int table[][4], int race4)
{
    int k = 0, re = 0, i, j, bloNum;
    struct position blaBlo[16];
    for (i = 0; i <= 15; i++)
    {
        blaBlo[i].i = 0;
        blaBlo[i].j = 0;
    }
    for (i = 0; i <= 3; i++)
        for (j = 0; j <= 3; j++)
            if (table[i][j] == 0)
            {
                blaBlo[k].i = i;
                blaBlo[k].j = j;
                k++;
            }
    if (k > 1)
    {
        re = randomer(k--);
        if (race4 != 0 && randomer(race4) == race4) bloNum = 4;
        else bloNum = 2;
        if (table[blaBlo[re].i][blaBlo[re].j] != 0)
        {
            if (re + 1 < k)
                table[blaBlo[re + 1].i][blaBlo[re + 1].j] = bloNum;
            else table[blaBlo[re - 1].i][blaBlo[re - 1].j] = bloNum;
        }
        if (table[blaBlo[re].i][blaBlo[re].j] == 0)
            table[blaBlo[re].i][blaBlo[re].j] = bloNum;
    }
    if (k == 1)
        table[blaBlo[0].i][blaBlo[0].j] = 2;
    if (k < 1)
        printf("No position to insert.\n");
    return 0;
}
int randomer(int n)
{
    int i, j;
    srand((unsigned)(time(NULL)));
    i = ((rand() % 1000) / 100 + (rand() % 100) / 10 + (rand() % 10)) % n;
    return i;
}
int Rightmove(int table[][4])
{
    int k, i, j;
    for (k = 0; k <= 3; k++)  //check loop.
        for (j = 2; j >= 0; j--)
            for (i = 3; i >= 0; i--)  //upmove every available block;
                if (table[i][j] != 0)
                {
                    if (table[i][j + 1] == 0)
                    {
                        table[i][j + 1] = table[i][j];
                        table[i][j] = 0;///*debug:*/printf("upmove:%d %d\n",i,j);
                    }
                    if ((table[i][j + 1] > 0) && (table[i][j + 1] == table[i][j]) && (table[i][j] > 0))
                    {
                        table[i][j + 1] = -(table[i][j + 1] + table[i][j]);
                        table[i][j] = 0;///*debug:*/printf("absorb:%d %d\n",i,j);
                    }
                }
    return 0;
}
int Leftmove(int table[][4])
{
    int k, i, j;
    for (k = 0; k <= 3; k++)  //check loop.
        for (j = 1; j <= 3; j++)
            for (i = 3; i >= 0; i--)  //upmove every available block;
                if (table[i][j] != 0)
                {
                    if (table[i][j - 1] == 0)
                    {
                        table[i][j - 1] = table[i][j];
                        table[i][j] = 0;//debug:printf("upmove:%d %d\n",i,j);
                    }
                    if ((table[i][j - 1] > 0) && (table[i][j - 1] == table[i][j]) && (table[i][j] > 0))
                    {
                        table[i][j - 1] = -(table[i][j - 1] + table[i][j]);
                        table[i][j] = 0;//debug:printf("absorb:%d %d\n",i,j);
                    }
                }
    return 0;
}
int Downmove(int table[][4])
{
    int k, i, j;
    for (k = 0; k <= 3; k++)  //check loop.
        for (i = 2; i >= 0; i--)
            for (j = 3; j >= 0; j--)  //downmove every available block;
                if (table[i][j] != 0)
                {
                    //debug:printf("check:%d %d\n",i,j);
                    if (table[i + 1][j] == 0)
                    {
                        table[i + 1][j] = table[i][j];
                        table[i][j] = 0;//debug:printf("upmove:%d %d\n",i,j);
                    }
                    if ((table[i + 1][j] > 0) && (table[i + 1][j] == table[i][j]) && (table[i][j] > 0))
                    {
                        table[i + 1][j] = -(table[i + 1][j] + table[i][j]);
                        table[i][j] = 0;//debug:printf("absorb:%d %d\n",i,j);
                    }
                }
    return 0;
}
int Upmove(int table[][4])
{
    int k, i, j;
    for (k = 0; k <= 3; k++)  //check loop.
        for (i = 1; i <= 3; i++)
            for (j = 3; j >= 0; j--)  //upmove every available block;
                if (table[i][j] != 0)
                {
                    if (table[i - 1][j] == 0)
                    {
                        table[i - 1][j] = table[i][j];
                        table[i][j] = 0;//debug:printf("upmove:%d %d\n",i,j);
                    }
                    if ((table[i - 1][j] > 0) && (table[i - 1][j] == table[i][j]) && (table[i][j] > 0))
                    {
                        table[i - 1][j] = -(table[i - 1][j] + table[i][j]);
                        table[i][j] = 0;//debug:printf("absorb:%d %d\n",i,j);
                    }
                }
    return 0;
}
int Judgement(int table[][4])
{
    int jud = 0, i, j;
    for (i = 0; i <= 3; i++)
        for (j = 0; j <= 3; j++)
        {
            if (table[i][j] == 2048)
            {
                jud = 2;//Seek for 2048 block.
                break;
            }
            if (table[i][j] == 0)
            {
                jud = 1;
                break;//Seek for blank block.
            }

            if (((table[i][j] == table[i - 1][j]) && (i - 1 >= 0)) || ((table[i][j] == table[i + 1][j]) && (i + 1 <= 3)) || ((table[i][j] == table[i][j - 1]) && (j - 1 >= 0)) || ((table[i][j] == table[i][j + 1]) && (j + 1 <= 3)))
            {
                jud = 1;
                break;//Seek for possible absorbing.
            }
        }
    return jud;
}


// Socket.
int Server()
{
    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;         /* set family to Internet     */
    sad.sin_addr.s_addr = INADDR_ANY; /* set the local IP address   */
    port = PROTOPORT;
    printf("Port:(Can be ignored):");
    if (scanf("%d", &port) == 1)
    {
        if (port <= 0)                   /* test for illegal value       */
        {                          /* print error message and exit */
            fprintf(stderr, "bad port number %d\n", port);
            return -1;
        }
    }
    else fflush(stdin);
    sad.sin_port = htons((u_short)port);
    /*if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
    {
    fprintf(stderr, "cannot map \"tcp\" to protocol number");
    return -2;
    }*/
    ptrp = getprotobyname("tcp");
    sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sd < 0)
    {
        fprintf(stderr, "socket creation failed\n");
        return -3;
    }
    if (bind(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        fprintf(stderr, "bind failed\n");
        return -4;
    }
    if (listen(sd, QUENUM) < 0)
    {
        fprintf(stderr, "listen failed\n");
        return -5;
    }

    while (1)
    {
        alen = sizeof(cad);
        if ((sd2 = accept(sd, (struct sockaddr *)&cad, &alen)) < 0)
        {
            fprintf(stderr, "accept failed\n");
            return -6;
        }
        else break;
        /*                 pthread_t ID;
                         pthread_create(&ID,NULL,sendmessage,sd2);*/
    }
}
void TableRecv()
{
    do{
        if (recv(option == 1 ? sd2 : sd, Rtable, sizeof(Rtable), 0) > 0)
        {
            outputmarkc = 1;
        }
        if (recv(option == 1 ? sd2 : sd, &Rjud, sizeof(Rjud), 0) > 0)
        {
            outputmarkc = 1;
        }
        if (closemark)
            break;
    } while (1);
}
void TableOut()
{
    int i, j;
    char co[100];
    while (1)
    {
        if (outputmark || outputmarkc)
        {
            TableSend();
            system("clear");
            printf("	localplayer				remoteplayer\n");
            for (i = 0; i < 4; ++i)
            {
                if (jud == 0)
                {
                    for (j = 0; j < 4; ++j)
                    {
                        if (table[i][j] < 0)
                            table[i][j] = -table[i][j];
                        printf("%5d", table[i][j]);
                    }
                    printf("			");
                }
                else if (jud == -1)
                {
                    printf("	    You lose!						  	    ");
                    break;
                }
                else
                {
                    printf("	    You win!							    ");
                    break;
                }
                if (Rjud == 0)
                {
                    for (j = 0; j < 4; ++j)
                    {
                        printf("%5d", Rtable[i][j] > 0 ? Rtable[i][j] : (-Rtable[i][j]));
                    }
                }
                else if (jud == -1)
                {
                    printf("Opponent lose!");
                    break;
                }
                else
                {
                    printf("Opponent win!");
                    break;
                }
                printf("\n\n");
                if (jud || Rjud)
                    closemark = 1;
            }
            outputmark = outputmarkc = 0;
            printf("Please enter 'w' to upmove,'s'to downmove,'a'to leftmove,'d'to rightmove,'q'to quit.\n");
            //printf("Command:");
            co[0] = 0;
            /*if (fscanf(stdin,"%s",co) == 1)
                printf("%s",co);*/
        }
        if (closemark)
            break;
    }
}
int Client()
{
    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;         /* set family to Internet     */
    sad.sin_addr.s_addr = INADDR_ANY; /* set the local IP address   */
    port = PROTOPORT;
    printf("Port:(Can be ignored):");
    if (scanf("%d", &port) == 1)
    {
        if (port <= 0)                   /* test for illegal value       */
        {                          /* print error message and exit */
            fprintf(stderr, "bad port number %d\n", port);
            return -1;
        }
    }
    else fflush(stdin);
    sad.sin_port = htons((u_short)port);
    printf("IP:");
    scanf("%s", host);
    ptrh = gethostbyname(host);
    if (((char *)ptrh) == NULL) {
        fprintf(stderr, "invalid host: %s\n", host);
        return -2;
    }
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
    /*if ( ((int)(ptrp = getprotobyname("tcp"))) == 0) {
                fprintf(stderr, "cannot map \"tcp\" to protocol number");
                return -3;
                }*/
    ptrp = getprotobyname("tcp");
    sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sd < 0) {
        fprintf(stderr, "socket creation failed\n");
        return -4;
    }

    /* Connect the socket to the specified server. */

    if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        fprintf(stderr, "connect failed\n");
        return -5;
    }

    /* Repeatedly read data from socket and write to user's screen. */
    /*        pthread_t ID;
            pthread_create(&ID,NULL,sendmessage,sd);*/
    /* Close the socket. */
}
int game2048()
{
    char command = 0; int i, j, k = 0, re = 0, loop, race4 = 5, temp;
    for (i = 0; i <= 3; ++i)
        for (j = 0; j <= 3; j++)
            table[i][j] = 0;
    //Give numbers to two blocks randomly.
    for (i = 1; i <= 2; ++i)
        RanInsert(table, 0);
    //output the table.
    //OutputTable(table);
    outputmark = 1;
    //Ask to enter command.
    //printf("Please enter 'w' to upmove,'s'to downmove,'a'to leftmove,'d'to rightmove,'q'to quit.\n");
    //Game.
    do
    {
        //printf("Command:");	
        command = getchar();
        if (command == '\n') command = getchar();
        if ((command != 'w') && (command != 'a') && (command != 's') && (command != 'd') && (command != 'q') && (command != '4'))
        {
            printf("invalid instruction\n");
            continue;
        }
        //Movements.
        if (command == 'w')//upmove.
            Upmove(table);
        if (command == 'a')//leftmove.
            Leftmove(table);
        if (command == 'd')//rightmove.
            Rightmove(table);
        if (command == 's')//downmove.
            Downmove(table);
        if (command == 'q') return 0;
        if (command == '4')
        {
            scanf("%d", &race4);
            if (race4 < 2)
                race4 = 5;
        }
        //insert a new block.
        RanInsert(table, race4);
        //output the table.
        //PrintHead();
        outputmark = 1;
        TableSend();
        //W or L judgement
        if ((temp = Judgement(table)) == 0)
        {
            jud = -1;
            outputmark = 1;
            return 0;
        }
        if (temp == 2)
        {
            jud = 1;
            outputmark = 1;
            return 0;
        }
    } while (command != 'q');
    while (1)
        if (closemark)
            return 0;
}
int TableSend()
{
    if (option == 1)
    {
        send(sd2, table, sizeof(table), 0);
        send(sd2, &jud, sizeof(jud), 0);
    }
    else
    {
        send(sd, table, sizeof(table), 0);
        send(sd, &jud, sizeof(jud), 0);
    }
}
int main()
{
    printf("1 - server.\n2 - client.\nTo choose a mode.\n");
    scanf("%d", &option);
    switch (option)
    {
    case 1:Server();
        break;
    case 2:Client();
    }
    pthread_t TID[2];
    pthread_create(&TID[0], NULL, TableRecv, NULL);
    pthread_create(&TID[1], NULL, TableOut, NULL);
    INITIALIZE
        game2048();
    close(sd);
    if (option == 1)
        close(sd2);
}
