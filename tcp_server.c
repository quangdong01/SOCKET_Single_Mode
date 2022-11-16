#include <stdio.h> // printf...
#include <sys/socket.h> // socket, bind...
#include <netinet/in.h> // struct sockaddr_in...
#include <arpa/inet.h>  // htons...
#include <sys/types.h>  // setsockopt...
#include <fcntl.h>      // fcntl()
#include <stdlib.h>     // exit(1)
#include <string.h>     // memset
#include <unistd.h>     // close()


#define SOCKET int
typedef enum TextColor
{
    TC_BLACK = 0,
    TC_BLUE = 1,
    TC_GREEN = 2,
    TC_CYAN = 3,
    TC_RED = 4,
    TC_MAGENTA = 5,
    TC_BROWN = 6,
    TC_LIGHTGRAY = 7,
    TC_DARKGRAY = 8,
    TC_LIGHTBLUE = 9,
    TC_LIGHTGREEN = 10,
    TC_LIGHTCYAN = 11,
    TC_LIGHTRED = 12,
    TC_LIGHTMAGENTA = 13,
    TC_YELLOW = 14,
    TC_WHITE = 15
} TextColor;

static const char *ansiColorSequences[] =
{
    "\x1B[0;30m",
    "\x1B[0;34m",
    "\x1B[0;32m",
    "\x1B[0;36m",
    "\x1B[0;31m",
    "\x1B[0;35m",
    "\x1B[0;33m",
    "\x1B[0;37m",
    "\x1B[1;30m",
    "\x1B[1;34m",
    "\x1B[1;32m",
    "\x1B[1;36m",
    "\x1B[1;31m",
    "\x1B[1;35m",
    "\x1B[1;33m",
    "\x1B[1;37m"
};

static const char *ansiColorTerms[] =
{
    "xterm",
    "rxvt",
    "vt100",
    "linux",
    "screen",
    0
    // there are probably missing a few others
};

// get current terminal and check whether it's in our list of terminals
// supporting ANSI colors:
static int isAnsiColorTerm(void)
{
    char *term = getenv("TERM");
    for (const char **ansiTerm = &ansiColorTerms[0]; *ansiTerm; ++ansiTerm)
    {
        int match = 1;
        const char *t = term;
        const char *a = *ansiTerm;
        while (*a && *t)
        {
            if (*a++ != *t++)
            {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

void setTextColor(FILE *stream, TextColor color)
{
    int outfd = fileno(stream);
    if (isatty(outfd) && isAnsiColorTerm())
    {
        // we're directly outputting to a terminal supporting ANSI colors,
        // so send the appropriate sequence:
        fputs(ansiColorSequences[color], stream);
    }
}



int main(int argc, char *argv[])
{   
    // kiem tra doi so dau vao neu khong truyen dia chi va port cho server thi ket thuc 
    if(argc != 3)
    {
        setTextColor(stdout,  TC_RED);
        printf("Usage: ./test <serverip> <serverport>\n");
        exit(1);
    }
    setTextColor(stdout,  TC_BLACK);
    // tao bo mo ta cua socket
    SOCKET sockfd_server, sockfd_client;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    // khoi tao socket
    printf("Socket is creating...\n");
    if((sockfd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Creating fail!\n");
        exit(1);
    }
    setTextColor(stdout,  TC_GREEN);
    printf("Creating success...\n");
    setTextColor(stdout,  TC_BLACK);
    // thiet lap che do chan cho socket
    printf("Setting blocking mode...\n");
    int flag;
    // Checking socket open
    printf("Checking socket open...\n");
    flag = fcntl(sockfd_server, F_GETFL);
    if(flag < 0)
    {
        setTextColor(stdout,  TC_RED);
        printf("Socket not open!\n");
        exit(1);
    }
    flag = fcntl(sockfd_server, F_SETFL, flag & (~O_NONBLOCK));
    if(flag < 0)
    {   
        setTextColor(stdout,  TC_RED);
        printf("Setting mode fail!\n");
        exit(1);
    }
    setTextColor(stdout,  TC_GREEN);
    printf("Setting blocking mode success...\n");

    // thiet lap option cho socket de tranh loi 'address in use'
    setTextColor(stdout,  TC_BLACK);
    printf("Setting option for socket...\n");
    int option_value = 1, option_length;
    option_length = sizeof(option_length);
    if(setsockopt(sockfd_server, SOL_SOCKET, SO_REUSEADDR, &option_value, option_length) <0 )
    {
        setTextColor(stdout,  TC_RED);
        printf("Setting fail!\n");
        exit(1);
    }
    setTextColor(stdout,  TC_GREEN);
    printf("Setting success\n");

    // buoc socket
    setTextColor(stdout,  TC_BLACK);
    printf("Socket is binding...\n");
    if(bind(sockfd_server, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        setTextColor(stdout,  TC_RED);
        printf("Binding fail!\n");
        exit(1);
    }
    setTextColor(stdout,  TC_GREEN);
    printf("Binding success\n");

    // setting listen for socket
    setTextColor(stdout,  TC_BLACK);
    printf("Setting listen...\n");
    int listen_queue = 10;
    if(listen(sockfd_server, listen_queue) < 0)
    {
        setTextColor(stdout,  TC_RED);
        printf("Setting listen fail!\n");
        exit(1);
    }
    setTextColor(stdout,  TC_GREEN);
    printf("Setting listen success...\n");
    // Wating connect
    setTextColor(stdout,  TC_BLACK);
    printf("Waiting connect...\n");
    // khoi tao cau truc dia chi cho client duoc tra ve
    struct sockaddr_in addr_cli;
    // do dai cau truc dia chi
    int cli_length = sizeof(addr_cli);
    // co de kiem tra xem bo mo ta socket client co mo ko
    int flags;
    // chap nhan ket noi
    sockfd_client = accept(sockfd_server, (struct sockaddr*) &addr_cli, &cli_length);
    printf("Success \n");
    if(sockfd_client < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Connecting fail ! \n");
        exit(1);
    }
    // kiem tra xem bo mo ta tra ve co mo ko
    printf("Checking description...\n");
    flags = fcntl(sockfd_client, F_GETFL, 0);
    if(flags < 0)
    {
        setTextColor(stdout,  TC_RED);
        printf("Description not open!\n");
        exit(1);
    }
    setTextColor(stdout,  TC_GREEN);
    printf("Desciption open...\n");
    // bo dem chua dia chi
    char address[1024];
    // lu tru port
    int cli_port;
    // chuyen dia chi tu byte mang sang byte may chu hay dia chi dai dien duoc chi dinh theo kieu dau cham thap phan
    inet_ntop(addr_cli.sin_family, &addr_cli.sin_addr.s_addr, address, 1024);
    // chuyen port tu byte mang sang byte may chu
    cli_port = ntohs(addr_cli.sin_port);
    // Hien thi dia chi va port 
    setTextColor(stdout,  TC_GREEN);
    printf("Connecting to %s and port %d\n", address, cli_port);
    setTextColor(stdout,  TC_BLACK);
    char buffer[1024];
    while(1)
    {
        printf("Data is recving...\n");
        recv(sockfd_client, buffer, 1024, 0);
        printf("Data from client: %s\n", buffer);
        if(strcmp(buffer,"break") == 0)
        {
            printf("End success...\n");
            close(sockfd_client);
            exit(0);
        }
    }

    
    return 0;
}