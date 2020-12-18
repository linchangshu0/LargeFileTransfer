#include <iostream>

#include <stdio.h>
#include <WinSock2.h>

//缓存大小设置不能超过2M
#define BUFF_SIZE (1024 * 1024)

using namespace std;

/*
 * Client Main.
 */
int main(int argc, char** argv)
{
    WSADATA wsadata;
    WSAStartup(0x202, &wsadata);

    unsigned short port;       /* port client will connect to         */
    char buf[BUFF_SIZE];              /* data buffer for sending & receiving */
    struct hostent *hostnm;    /* server host name information        */
    struct sockaddr_in server; /* server address                      */
    int s;                     /* client socket                       */

    /*
     * Check Arguments Passed. Should be hostname and port.
     */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    /*
     * The host name is the first argument. Get the server address.
     */
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *) 0)
    {
        fprintf(stderr, "Gethostbyname failed\n");
        exit(2);
    }

    /*
     * The port is the second argument.
     */
    port = (unsigned short) atoi(argv[2]);

    /*
     * Put a message into the buffer.
     */
    strcpy(buf, "the message");

    /*
     * Put the server information into the server structure.
     * The port must be put into network byte order.
     */
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    /*
     * Get a stream socket.
     */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket error\n");
        exit(3);
    }

    /*
     * Connect to the server.
     */
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Connect error\n");
        exit(4);
    }

    //先接收文件大小
    int iRecv = 0;
    memset(buf, 0, BUFF_SIZE);
    iRecv = recv(s, buf, BUFF_SIZE, 0);
    if (iRecv < 0) {
        printf("recv error\n");
        exit(5);
    }
    off64_t totalFileSize = atoll(buf);
    printf("totalFileSize:%lld\n", totalFileSize);
    //再接收文件名

    //接收文件 将文件保存到指定位置
    char *filePath = "D:\\client\\test.exe";
    FILE *f = NULL;
    f = fopen(filePath, "ab");
    if (f == NULL) {
        printf("file:%s doesn't exist and failed to create\n", filePath);
        exit(5);
    }

    off64_t fileRecv = 0;
    while (fileRecv < totalFileSize) {
        memset(buf, 0, BUFF_SIZE);
        iRecv = recv(s, buf, BUFF_SIZE, 0);
        printf("iRecv:%d\n", iRecv);
        if (iRecv < 0)
        {
            printf("Recv error\n");
            exit(6);
        }
        if (iRecv == 0) {
            break;
        }
        fileRecv += iRecv;
        printf("totalFileSize:%lld recv file size:%lld\n", totalFileSize, fileRecv);
        fwrite(buf, sizeof(char), iRecv, f);
    }
    fclose(f);

    printf("Client Ended Successfully\n");
    exit(0);

}
