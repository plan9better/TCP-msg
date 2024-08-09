#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <unistd.h>

#define MAXLINE 1024

int _sendmsg(char * msg, int sockfd);
int _readmsg(int sockfd, char* dest);
int _readnick(int sockfd, char* dest);

int main(int argc, char **argv){
    int sfd;

    if(argc < 3){
        printf("(-) Usage: %s <IP ADDRESS> <NICK>\n", argv[0]);
        exit(1);
    }

    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
       printf("(-) Error creating socket\n");
       exit(2);
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    // Port to correct format
    servaddr.sin_port = htons(13);

    // IP addr to correct format
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) != 1){
        printf("(-) Error assigning ip addr to struct\n");
        exit(3);
    }

    printf("(+) Connecting...\n");
    if(connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        printf("(-) Error connecting\n");
        exit(4);
    }
    printf("(+) Connection established\n");

    char readBuf[MAXLINE] = {0};
    char writeBuf[MAXLINE] = {0};

    _sendmsg(argv[2], sfd);

    char otherNick[MAXLINE] = {0};
    if (_readnick(sfd, otherNick) <= 0) printf("The other side did not respond\n");

    while(_readmsg(sfd, readBuf) > 0){
        printf("%s: %s", otherNick, readBuf);

        printf("%s: ", argv[2]);
        fgets(writeBuf, MAXLINE, stdin);
        _sendmsg(writeBuf, sfd);
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
    int n = _readmsg(sockfd, dest);
    dest[strlen(dest) - 1] = '\0';
    // dest[strlen(dest) - 1] = '\0';
    return n;
}
