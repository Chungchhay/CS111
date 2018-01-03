#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <mcrypt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

char* encryption;
char* fileEncryption;
char* Z;
MCRYPT td;
int encryptionKeyLength = 16;
int kfd;
int toServer = 10;
int fromServer = 5;
int logFD;
int portFlag = 0;
int logFlag = 0;
int encryptFlag = 0;

char buf[256];
struct termios saveDefaultTerminal;
struct termios currentTerminal;

//Print error message
void messageFunc(int num) {
  fprintf(stderr, "Error: %s\n", strerror(num));
  exit(1);
}

//saving terminal
void saveTerminalSetting() {
  int status = tcsetattr(STDIN_FILENO, TCSANOW, &saveDefaultTerminal);
  int temp = errno;
  if(status == -1) {
    messageFunc(temp);
  }
}

//Read from buffer
int readFunc(int file_descriptor, char* buf, int bytes) {
  int status = read(file_descriptor, buf, bytes);
  int temp = errno;
  if(status == -1) {
    messageFunc(temp);
  }

  return status;
}

//Write to buffer
int writeFunc(int file_descriptor, char* buf, int bytes) {
  int status = write(file_descriptor, buf, bytes);
  int temp = errno;
  if(status == -1) {
    messageFunc(temp);
  }

  return status;
}

void readNWrite(char* buf, int file_descriptor,int bytes) {
  int i;
  for(i = 0; i < bytes; i++) {
      if (*(buf + i) == '\r' || *(buf + i) == '\n') {
          if(file_descriptor == STDOUT_FILENO) {
              char tmp[2];
              tmp[0] = '\r';
              tmp[1] = '\n';
              writeFunc(file_descriptor, tmp, 2);
          } else {
              char tmp[1];
              tmp[0] = '\n';
              writeFunc(file_descriptor, tmp, 1);
          }
      } else {
          writeFunc(file_descriptor, buf + i, 1);
      }
  }
}

//Write to the log file                                                                
void logFunc(char* buf, int bytes, int writeThroughSocket) {
  if(writeThroughSocket == toServer) {
    int test0 = dprintf(logFD, "SENT %d bytes: ", bytes);
    int status0 = errno;
    if(test0 == -1) {
      messageFunc(status0);
    }

    readNWrite(buf, logFD, bytes);
    writeFunc(logFD, "\n", 1);
  }

  if(writeThroughSocket == fromServer) {
    int test1 = dprintf(logFD, "RECEIVED %d bytes: ", bytes);
    int status1 = errno;
    if(test1 == -1) {
      messageFunc(status1);
    }

    readNWrite(buf, logFD, bytes);

    writeFunc(logFD, "\n", 1);
  }
}

void input() {
    
    int test = tcgetattr(STDIN_FILENO, &saveDefaultTerminal);
    int temp = errno;
    
    if(test == -1) {
        messageFunc(temp);
    }
    
    atexit(saveTerminalSetting);
    
    int test0 = tcgetattr(STDIN_FILENO, &currentTerminal);
    temp = errno;
    
    if(test0 == -1) {
        messageFunc(temp);
    }
    
    currentTerminal.c_iflag = ISTRIP;
    currentTerminal.c_oflag = 0;
    currentTerminal.c_lflag = 0;
    
    int test1 = tcsetattr(STDIN_FILENO, TCSANOW, &currentTerminal);
    temp = errno;
    
    if(test1 == -1) {
        messageFunc(temp);
    }
}

//Setup the encryption
void encryptionInitializer() {
  td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  int status0 = errno;
  if(td == MCRYPT_FAILED) {
    messageFunc(status0);
  }

  kfd = open(fileEncryption, O_RDONLY);
  int status1 = errno;
  if(kfd == -1) {
    messageFunc(status1);
  }

  encryption = malloc(encryptionKeyLength);
  int status2 = errno;
  if(encryption == NULL) {
    messageFunc(status2);
  }

  int iv_size = mcrypt_enc_get_iv_size(td);
  Z = malloc(iv_size);
  int status3 = errno;
  if(Z == NULL) {
    messageFunc(status3);
  }

  memset(encryption, 0, encryptionKeyLength);
  
  int bytes_read = read(kfd, encryption, encryptionKeyLength);
  int status4 = errno;
  if(bytes_read != encryptionKeyLength) {
    messageFunc(status4);
  }

  int test5 = close(kfd);
  int status5 = errno;
  if(test5 == -1) {
    messageFunc(status5);
  }

  int i;

  for(i = 0; i < iv_size; i++) {
    Z[i] = 0;
  }

  
  int test6 = mcrypt_generic_init(td, encryption, encryptionKeyLength, Z);
  int status6 = errno;
  if(test6 == -1) {
    messageFunc(status6);
  }

  free(encryption);
  free(Z);
}


int main(int argc, char** argv) {
  char* log_file;
  int option;
  int port_number;

  //Array of options structs
  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"log", optional_argument, 0, 'l'},
    {"encrypt", required_argument, 0, 'e'},
    {0, 0, 0, 0}
  };

  while((option = getopt_long(argc, argv, "p:l:e:", long_options, NULL)) != -1) {
    switch(option) {
    case 'p':
            port_number = atoi(optarg);
            portFlag = 1;
            break;
    case 'l':
            log_file = optarg;
            logFlag = 1;
            break;
    case 'e':
            encryptFlag = 1;
            fileEncryption = optarg;
            break;
    default:
            fprintf(stderr, "Correct Usage: client --port=portnumber [--log=logfile] [--encrypt=keyfile]\n");
            exit(1);
    }
  }
    
  if(portFlag == 0) {
      fprintf(stderr, "Correct Usage: client --port=portnumber [--log=logfile] [--encrypt=keyfile]\n");
      exit(1);
  }
    
  if(encryptFlag) {
    encryptionInitializer();
  }
    
  if(logFlag) {
    logFD = creat(log_file, S_IRWXU);
    int status6 = errno;
    if(logFD == -1) {
      messageFunc(status6);
    }
  }
    
    input();
    
  struct sockaddr_in serv_addr;
  struct hostent* server = gethostbyname("localhost");
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int status0 = errno;

  if(sockfd == -1) {
    messageFunc(status0);
  }

  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_number);
  memcpy((char*) &serv_addr.sin_addr.s_addr, (char*) server->h_addr, server->h_length);
    
  int connection = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  int status1 = errno;

  if(connection == -1) {
    messageFunc(status1);
  }
    
  struct pollfd polls[2];
    
  polls[0].fd = STDIN_FILENO;
  polls[0].events = POLLIN | POLLHUP| POLLERR;
    
  polls[1].fd = sockfd;
  polls[1].events = POLLIN | POLLHUP | POLLERR;
    
  while(1) {
    int poll_status = poll(polls, 2, 0);
    int status2 = errno;

    if(poll_status == -1) {
      messageFunc(status2);
    }
      
    if(poll_status == 0) {
      continue;
    }
      
    if(polls[0].revents & POLLIN) {
      char buf2[256];
      int bytes_read = readFunc(STDIN_FILENO, buf2, 256);
      readNWrite(buf2, STDOUT_FILENO, bytes_read);
        
      if(encryptFlag) {
	int k;
	for(k = 0; k < bytes_read; k++) {
	  if(buf2[k] != '\r' && buf2[k] != '\n') {
	    int test5 = mcrypt_generic(td, &buf2[k], 1);
	    int status5 = errno;
	    if(test5 != 0) {
	      messageFunc(status5);
	    }
	  }
	}
      }
        
      if(logFlag) {
	logFunc(buf2, bytes_read, toServer);
      }
        
      readNWrite(buf2, polls[1].fd, bytes_read);
    }
      
    if(polls[1].revents & POLLIN) {
      char buf1[256];
      int bytes_read = readFunc(polls[1].fd, buf1, 256);
        
      if(bytes_read == 0) {
	int test3 = close(sockfd);
      	int status3 = errno;
      	if(test3 == -1) {
      	  messageFunc(status3);
      	} else {
        exit(0);
      	}
      }
        
      if(logFlag) {
	logFunc(buf1, bytes_read, fromServer);
      }
        
      if(encryptFlag) {
	int test6 = mdecrypt_generic(td, &buf1,  bytes_read);
	int status6 = errno;
	if(test6 != 0) {
	  messageFunc(status6);
	}
      }
        
      readNWrite(buf1, STDOUT_FILENO, bytes_read);
      
    }
      
    if(polls[0].revents & (POLLHUP | POLLERR)) {
      int test10 = close(sockfd);
      int status10 = errno;
      if(test10 == -1) {
	messageFunc(status10);
      }
    }
      
    if(polls[1].revents & (POLLHUP | POLLERR)) {
      int test4 = close(sockfd);
      int status4 = errno;
      if(test4 == -1) {
	messageFunc(status4);
      }

      exit(0);
    }
  }
    
  exit(0);  
}
