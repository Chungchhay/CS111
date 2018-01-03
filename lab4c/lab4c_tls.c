#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <time.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

double celsius;
double fahrenheit;
const int B = 4275;
const int R0 = 100000;
char tempType = 'F';
struct tm* timeInfo;
const int buffSize = 1024;
char tempBuff[9];
FILE *logFile;
char offOption;
int stopFlag;
int logFlag = 0;
int flag = 0;
char *id;
char *host = "";
int port;
int sockfd;
SSL_CTX *context;
SSL *ssl;

void printTemperature(void);
void messageFunc(int);

int main(int argc, char* argv[]) {
    struct pollfd fds[1];
    int timeout = 1000;
    int pret;
    //int fd = 0;
    //int i = 0;
    char buff[buffSize];
    int longOpt = 0;
    stopFlag = 0;
    
    static struct option longOptions[] = {
        {"log", required_argument, 0, 'l'},
        {"id", required_argument, 0, 'i'},
        {"host", required_argument, 0, 'h'},
        {0,0,0,0}
    };
    
    while (1)
    {
        longOpt = getopt_long(argc, argv, "l:i:h:", longOptions, NULL);
        if (longOpt == -1)
        {
            break;
        }
        
        if (longOpt == 'l')
        {
            logFlag = 1;
            logFile = fopen(optarg, "w");
            int status = errno;
            
            if (!logFile)
            {
                messageFunc(status);
            }
            
        }
        else if (longOpt == 'i')
        {
            id = optarg;
        }
        else if (longOpt == 'h')
        {
            host = optarg;
        }
        else
        {
            perror("Argument Error");
            exit(1);
        }
    }
    
    port = atoi(argv[argc - 1]);
    
    struct sockaddr_in serv_addr;
    struct hostent* server = gethostbyname(host);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int status0 = errno;
    
    if (server == NULL)
    {
        fprintf(stderr, "Error: %s\n", strerror(status0));
        exit(2);
    }
    
    if(sockfd == -1) {
        fprintf(stderr, "Error: %s\n", strerror(status0));
        exit(2);
    }
    
    //memset((char*) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy((char*) &serv_addr.sin_addr.s_addr, (char*) server->h_addr, server->h_length);
    
    int connection = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    int status1 = errno;
    
    if(connection == -1) {
        messageFunc(status1);
    }
    
    if (SSL_library_init() < 0) {
        fprintf(stderr, "Error: initializing OpenSSL library\n");
        exit(2);
    }
    
    if ((context = SSL_CTX_new(SSLv23_client_method())) == NULL)
    {
        fprintf(stderr, "Error: creating SSL context structure\n");
        exit(2);
    }
    
    ssl = SSL_new(context);
    
    if(SSL_set_fd(ssl, sockfd) == 0) {
        fprintf(stderr, "Error: setting sockfd SLL fd\n");
        exit(2);
    }
    
    if(SSL_connect(ssl) != 1) {
        fprintf(stderr, "Error: establishing SSL/TLS\n");
        exit(2);
    }
    
    char bufferID[15];
    sprintf(bufferID, "ID=%s\n", id);
    if(SSL_write(ssl, bufferID, strlen(bufferID)) < 0) {
        fprintf(stderr, "could not write id\n");
        exit(2);
    }
    //SSL_write(ssl, bufferID, strlen(bufferID));
    
    if(logFlag) {
        fprintf(logFile, "ID=%s\n", id);
    }
    
    int temp = 0;
    
    while(1)
    {
        fds[0].fd = sockfd;
        fds[0].events = 0;
        fds[0].events |= POLLIN;
        pret = poll(fds, 1, timeout);
        
        if (pret && !flag)
        {
            char t;
            memset(buff, 0, 1024);
            SSL_read(ssl, buff, 1024);
            buff[strlen(buff) - 1] = '\0';
            //if (t == '\n')
            //{
                //buff[temp] = '\0';
                //temp = 0;
                char subBuff[8];
                memcpy(subBuff, &buff[0], 7);
                subBuff[7] = '\0';
                
                if (strcmp(buff, "OFF") == 0)
                {
                    if (logFlag)
                    {
                        fprintf(logFile, "OFF\n");
                    }
                    stopFlag = 0;
                    offOption = 'F';
                }
                else if (strcmp(buff, "START") == 0)
                {
                    if (logFlag)
                    {
                        fprintf(logFile, "START\n");
                    }
                    
                    offOption = ' ';
                    stopFlag = 0;
                }
                else if (strcmp(buff, "STOP") == 0)
                {
                    if (logFlag)
                    {
                        fprintf(logFile, "STOP\n");
                    }
                    
                    stopFlag = 1;
                    offOption = ' ';
                }
                else if (strcmp(buff, "SCALE=F") == 0)
                {
                    tempType = 'F';
                    if (logFlag)
                    {
                        fprintf(logFile, "SCALE=F\n");
                    }
                    
                }
                else if (strcmp(buff, "SCALE=C") == 0)
                {
                    tempType = 'C';
                    if (logFlag)
                    {
                        fprintf(logFile, "SCALE=C\n");
                    }
                }
                else if (strcmp(subBuff, "PERIOD=") == 0)
                {
                    int periodValid = 1;
                    int sleepTime = 1;
                    
                    char period[1024 - 8];
                    memcpy(period, &buff[7], 1024-7);
                    period[1024-7] = '\0';
                    int j = 0;
                    
                    //Should add button in it
                    while (period[j] != '\0')
                    {
                        if (isdigit(period[j]) == 0)
                        {
                            periodValid = 0;
                            break;
                        }
                        j++;
                    }
                    
                    if (periodValid)
                    {
                        int periodTime = atoi(period);
                        sleepTime = periodTime;
                        timeout = sleepTime * 1000;
                        if (logFlag)
                        {
                            fprintf(logFile, "PERIOD=%d\n", sleepTime);
                        }
                    }
                }
                flag = 1;
            }
            /*else
            {
                buff[temp] = t;
                temp++;
            }
        }*/
        else
        {
            printTemperature();
            flag = 0;
        }
    }
    
    if (logFlag)
    {
        fclose(logFile);
    }
    
    return 0;
}

void printTemperature()
{
    time_t t = time(NULL);
    timeInfo = localtime(&t);
    
    strftime(tempBuff, 9, "%H:%M:%S", timeInfo); //print time
    
    mraa_aio_context temp;
    temp = mraa_aio_init(1);
    //Convert to normal temperature in C
    uint16_t value = mraa_aio_read(temp);
    float R = 1023.0 / (float) value - 1.0;
    R = R0 * R;
    float temperature = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15;
    
    if (tempType == 'F')
    {
        temperature = temperature * 1.8 + 32;
    }
    
    if (stopFlag)
    {/*
      Should not input in the log file nor the stdout
      if (logFlag)
      {
      fprintf(logFile, "%s %0.1f\n", tempBuff, temperature);
      }
      */
    }
    else if (offOption != 'F')
    {
        char tempBuffer[50];
        sprintf(tempBuffer, "%s %0.1f\n",tempBuff, temperature);
        if(SSL_write(ssl, tempBuffer, strlen(tempBuffer)) < 0) {
            fprintf(stderr, "Error: SSL write\n");
            exit(2);
        }
        //SSL_write(ssl, tempBuffer, strlen(tempBuffer));
        
        if (logFlag)
        {
            fprintf(logFile, "%s %0.1f\n",tempBuff, temperature);
        }
    }
    else
    {
        char tempBuffer[50];
        sprintf(tempBuffer, "%s SHUTDOWN\n", tempBuff);
        if(SSL_write(ssl, tempBuffer, strlen(tempBuffer)) < 0) {
            fprintf(stderr, "Error: SSL write\n");
            exit(2);
        }
        //SSL_write(ssl, tempBuffer, strlen(tempBuffer));
        
        if (logFlag)
        {
            fprintf(logFile, "%s SHUTDOWN", tempBuff);
        }
        mraa_aio_close(temp);
        //SSL_free(ssl);
        //SSL_CTX_free(context);
        SSL_shutdown(ssl);
        exit(0);
    }
    
}

//Print error message
void messageFunc(int num) {
    fprintf(stderr, "Error: %s\n", strerror(num));
    exit(1);
}


