#include <stdio.h> // printf...
#include <sys/socket.h> // socket, bind...
#include <netinet/in.h> // struct sockaddr_in...
#include <arpa/inet.h>  // htons...
#include <sys/types.h>  // setsockopt...
#include <fcntl.h>      // fcntl()
#include <stdlib.h>     // exit(1);
#include <string.h>     // memset
#include <unistd.h>     // close()
#include <errno.h>
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
    // kiem tra doi so dau vao xem co thoa man hay khong 
    if(argc != 3)
    {
        setTextColor(stdout, TC_RED);
        printf("Usage: ./client <Serverip> <Serverport>");
        exit(1);
    }

    // khoi tai socket 
    setTextColor(stdout, TC_BLACK);
    printf("Creating socket...\n");
    SOCKET sockfd_client;
    struct sockaddr_in addr;
    if((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Creating fail!");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Creating success...\n");

    // check socket open or not
    setTextColor(stdout, TC_BLACK);
    printf("Checking socket...\n");
    int flag;
    if((flag = fcntl(sockfd_client, F_GETFL, 0)) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Socket not open!");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Checking success...\n");

    // set socket in block mode
    setTextColor(stdout, TC_BLACK);
    printf("Setting mode for socket...\n");
    if(fcntl(sockfd_client, F_SETFL, flag & (~O_NONBLOCK)) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Setting mode fail!");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Setting mode success...\n");

    // assign struct of socket server
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    // connect to server 
    setTextColor(stdout, TC_BLACK);
    printf("Connecting to server...\n");
    int flags;
    flags = connect(sockfd_client, (struct sockaddr*) &addr, sizeof(addr));
    if(flags < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Connecting fail!");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Connecting success...\n\n\n");
    setTextColor(stdout, TC_BLACK);
    char buffer[1024];
    char check;
    while(1)
    {
        printf("Ban co muon gui tin nhan cho server không? Y/N ?");
        printf("\nNhap cau tra loi cua ban: ");
        scanf("%c", &check);
        if(check == 'N')
        {
            printf("End success...");
            close(sockfd_client);
            exit(0);
        }
        else if(check == 'Y')
        {
            while(getchar() != '\n');
            printf("Nhap tin nhan de gui: ");
            fgets(buffer, 1024, stdin);
            buffer[strlen(buffer) - 1] = '\0';
            printf("\nData is sending...\n");
            send(sockfd_client, buffer, sizeof(buffer), 0);
            printf("Data sent success...\n");
            if(strcmp(buffer, "break") == 0)
            {
                close(sockfd_client);
                exit(0);
            }
        }
    }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
    return 0;
}