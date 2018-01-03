#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <getopt.h>
#include <mcrypt.h>

char* encryption;
char* fileEncryption;
MCRYPT td;
int encryptionKeyLength = 16;
int kfd;
char* Z;
int shellFlag = 1;
int socketFlag = 0;
int toShell[2];
int fromShell[2];
pid_t pid;
char* arg[] = {NULL};
int portFlag = 0;
int encryptFlag = 0;

//Print error message                                                                  
void messageFunc(int num) {
  fprintf(stderr, "Error: %s\n", strerror(num));
  exit(1);
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

//Execution shell
void exeShell() {
    int temp = errno;
    if(close(toShell[1]) == -1) {
        messageFunc(temp);
    }
    
    temp = errno;
    if(close(fromShell[0]) == -1) {
        messageFunc(temp);
    }
    
    temp = errno;
    if(dup2(toShell[0], STDIN_FILENO) == -1) {
        messageFunc(temp);
    }
    
    temp = errno;
    if(close(toShell[0]) == -1) {
        messageFunc(temp);
    }
    
    temp = errno;
    if(dup2(fromShell[1], STDOUT_FILENO) == -1) {
        messageFunc(temp);
    }
    
    temp = errno;
    if(dup2(fromShell[1], STDERR_FILENO) == -1) {
        messageFunc(temp);
    }
    
    temp = errno;
    if(close(fromShell[1]) == -1) {
        messageFunc(temp);
    }
    
    temp = errno;
    if(execvp("/bin/bash", arg) == -1) {
        messageFunc(temp);
    }

}

//Read from buffer                                                                
int readFunc(int FD, char* buf, int bytes) {
  int status = read(FD, buf, bytes);
  int temp = errno;
  if(status == -1) {
    messageFunc(temp);
  }

  return status;
}

//Write to buffer                                                                 
int writeFunc(int FD, char* buf, int bytes) {
  int status = write(FD, buf, bytes);
  int temp = errno;
  if(status == -1) {
    messageFunc(temp);
  }

  return status;
}

//Write char by char                                                              
void readNWrite(char* buf, int outputFD, int bytes, int inputFD) {
  int i;
  for(i = 0; i < bytes; i++) {
      if (*(buf + i) == 0x03) {
          if(inputFD == shellFlag) {
              //Kill the shell process, but keep on processing input in transit
              int test0 = kill(pid, SIGINT);
              int status0 = errno;
              if(test0 == -1) {
                  messageFunc(status0);
              }
          }
      } else if (*(buf + i) == 0x04) {
          if(inputFD == shellFlag) {
              int test1 = close(toShell[1]);
              int status1 = errno;
              if(test1 == -1) {
                  messageFunc(status1);
              }
          }
      } else {
          writeFunc(outputFD, buf + i, 1);
      }
  }
}

void saveTerminal() {
  int status;
  pid_t waited = waitpid(pid, &status, 0);
  int test = errno;
  if(waited == -1) {
    messageFunc(test);
  }

  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
  exit(0);
}

//Signal handler
void signalHandler(int signum) {
  if(signum == SIGPIPE || signum == SIGINT || signum == SIGTERM) {
    saveTerminal();
  }
}

int main(int argc, char** argv) {
  int option;
  int portNum;
  int socketID;
  int newSocketID;
    
  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"encrypt", required_argument, 0, 'e'},
    {0, 0, 0, 0}
  };

  while((option = getopt_long(argc, argv, "p:e:", long_options, NULL)) != -1) \
    {
      switch(option) {
      case 'p':
              portNum = atoi(optarg);
              portFlag = 1;
              break;
      case 'e':
              encryptFlag = 1;
              fileEncryption = optarg;
              break;
      default:
              fprintf(stderr, "Correct Usage: ./lab1b-client --port=portnumber [--log=logfile] [--encrypt=keyfile]\n");
              exit(1);
      }
    }

    
  if(portFlag == 0) {
      fprintf(stderr, "Correct Usage: ./lab1b-client --port=portnumber [--log=logfile] [--encrypt=keyfile]\n");
      exit(1);
  }
    
  if(encryptFlag) {
    encryptionInitializer();
  }
    
  signal(SIGINT, signalHandler);
  signal(SIGPIPE, signalHandler);
  signal(SIGTERM, signalHandler);
    
  struct sockaddr_in serv_addr, cli_addr;
  socketID = socket(AF_INET, SOCK_STREAM, 0);
  
  int status17 = errno;
  if(socketID == -1) {
    messageFunc(status17);
  }

  memset((char*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portNum);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
    
  int optval = 1;
  int t20 = setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  int s20 = errno;

  if(t20 == -1) {
    messageFunc(s20);
  }
  
  int test18 = bind(socketID, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  int status18 = errno;
  if(test18 == -1) {
    messageFunc(status18);
  }

  int test19 = listen(socketID, 5);
  int status19 = errno;
  if(test19 == -1) {
    messageFunc(status19);
  }

  unsigned int clilen = sizeof(cli_addr);

  newSocketID = accept(socketID, (struct sockaddr*) &cli_addr, &clilen);
  int status20 = errno;
  if(newSocketID == -1) {
    messageFunc(status20);
  }
    
  int test0 = pipe(toShell);
  int status0 = errno;
  if(test0 == -1) {
    messageFunc(status0);
  }

  int test1 = pipe(fromShell);
  int status1 = errno;
  if(test1 == -1) {
    messageFunc(status1);
  }
    
  pid = fork();
  int status2 = errno;
  if(pid == -1) {
    messageFunc(status2);
  }
    
  if(pid == 0) {
      exeShell();
  } else {
    
    int temp = errno;
    if(close(toShell[0]) == -1) {
      messageFunc(temp);
    }

    temp = errno;
    if(close(fromShell[1]) == -1) {
      messageFunc(temp);
    }
      
    struct pollfd polls[2];
      
    polls[0].fd = newSocketID;
    polls[0].events = POLLIN | POLLHUP | POLLERR;
      
    polls[1].fd = fromShell[0];
    polls[1].events = POLLIN | POLLHUP | POLLERR;
      
    while(1) {
      int test13 = poll(polls, 2, 0);
      int status13 = errno;
      if(test13 == -1) {
	messageFunc(status13);
      }
        
      if(test13 == 0) {
	continue;
      }
        
      if(polls[0].revents & POLLIN) {
	char fromSocket[256];
	int bytes = read(newSocketID, fromSocket, 256);
          
	if(bytes == 0 || bytes == -1) {
	  int test15 = close(toShell[1]);
	  int status15 = errno;

	  if(test15 <= -1) {
	    messageFunc(status15);
	  }
        
	  saveTerminal();
	  break;
	}
          
	if(encryptFlag) {
	  int k;
	  for(k = 0; k < bytes; k++) {
	    if(fromSocket[k] != '\n' && fromSocket[k] != '\r') {
	      int test17 = mdecrypt_generic(td, &fromSocket[k], 1);
	      int status17 = errno;
	      if(test17 == -1) {
		messageFunc(status17);
	      }
	    }
	  }
	}
          
	readNWrite(fromSocket, toShell[1], bytes, shellFlag);
      }
        
      if(polls[1].revents & POLLIN) {
	char inputBuf[256];
	int bytes = readFunc(fromShell[0], inputBuf, 256);
	if(bytes == 0) {
	  saveTerminal();
	  break;
	}
          
	if(encryptFlag) {
	  int test18 = mcrypt_generic(td, &inputBuf, bytes);
	  int status18 = errno;
	  if(test18 != 0) {
	    messageFunc(status18);
	  }
	}
          
	readNWrite(inputBuf, newSocketID, bytes, socketFlag);
      }
        
      if(polls[1].revents & (POLLHUP | POLLERR)) {
	int test16 = close(polls[1].fd);
	int status16 = errno;
	if(test16 == -1) {
	  messageFunc(status16);
	}

	saveTerminal();
	break;
      }
    }
  }
  exit(0);
}
