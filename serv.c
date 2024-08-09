#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define SOCKQ 5
#define MAXLINE 1024

int _sendmsg(char * msg, int sockfd);
int _readmsg(int sockfd, char* dest);
int _readnick(int sockfd, char* dest);

int main(int argc, char* argv[]){

    if (argc < 2){
        printf("(-) Usage: %s <NICK>\n", argv[0]);
        exit(1);
    }

    int sfd;
    if( (sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("Error creating socket\n");
        errno = 1;
        exit(1);
    }

    struct sockaddr_in sin;

    sin.sin_port = htons(13);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(sfd, (struct sockaddr *) &sin, sizeof(sin)) == -1){
        printf("Error binding socket\n");
        printf("%d\n", errno);
        exit(2);
    }

    printf("Listening...\n");
    if( listen(sfd, SOCKQ) == -1 ){
        printf("Error listening\n");
        exit(3);
    }

    struct sockaddr clientsin;
    socklen_t sofclient = sizeof(clientsin);

    int clientfd;

    for(; ;){
        if( (clientfd = accept(sfd, &clientsin, &sofclient)) == -1){
            printf("Error accepting conn\n");
            exit(4);
        }
        printf("Accepted a connection, send the first message.\n");
        break;
     }

    char otherNick[MAXLINE] = {0};
    if(_readnick(clientfd, otherNick) < 0) exit(1);

    // send nick
    _sendmsg(argv[1], clientfd);

    char readBuf[MAXLINE] = {0};
    char writeBuf[MAXLINE] = {0};

    printf("%s: ", argv[1]);
    fflush(stdout);
    fgets(writeBuf, MAXLINE, stdin);
    _sendmsg(writeBuf, clientfd);

    while(_readmsg(clientfd, readBuf) > 0){
        printf("%s: %s", otherNick, readBuf);

        printf("%s: ", argv[1]);
        fgets(writeBuf, MAXLINE, stdin);
        _sendmsg(writeBuf, clientfd);
    }

    exit(0);
}

int _sendmsg(char* msg, int sockfd){
    if(msg[strlen(msg) - 1] == '\n') msg[strlen(msg) - 1] = '\0';
    char finalmsg[MAXLINE] = {0};
    snprintf(finalmsg, MAXLINE - 2, "%s\n", msg);
    return write(sockfd, finalmsg, strlen(finalmsg));
}
int _readmsg(int sockfd, char* dest){
    char buf[MAXLINE] = {0};
    int n = read(sockfd, buf, MAXLINE - 1);
    if ( n <= 0){
        return n;
    }
    buf[n] = '\0';
    strncpy(dest, buf, MAXLINE);
    return n;
}
int _readnick(int sockfd, char* dest){
    _readmsg(sockfd, dest);
    dest[strlen(dest) - 1] = '\0';
    // dest[strlen(dest) - 1] = '\0';
    return 1;
}
