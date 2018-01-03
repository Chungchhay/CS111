//NAME: Chungchhay Kuoch
//EMAIL: chungchhaykuoch@yahoo.com
//ID: 004 843 308

#include <getopt.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>

struct termios saveOriginalTerminal;
struct termios currentTerminal;

char bufferSize[256];
int toChild[2];
int fromChild[2];
pid_t pid;
char* arg[] = {NULL};
int inputPipe;
int outputPipe;
int shellFlag = 0;

const int shell = 1;
const int noShell = 0;

//Just to print error message
void messageFunc(char* message, int num) {
  fprintf(stderr, "Error: %s\n%s\n", message, strerror(num));
  exit(1);
}

//Execution the shell
void exeShell() {
  //Check for every system call
    int temp = errno;
    if (close(toChild[1]) == -1)
    {
        messageFunc("Child process FAILED", temp);
    }
    
    temp = errno;
    if (close(fromChild[0]) == -1)
    {
        messageFunc("Child process FAILED",temp);
    }
    
    temp = errno;
    if (dup2(toChild[0], 0) == -1)
    {
        messageFunc("Child process FAILED",temp);
    }
    
    temp = errno;
    if (dup2(fromChild[1], 1) == -1)
    {
        messageFunc("Child process FAILED",temp);
    }
    
    temp = errno;
    if (dup2(fromChild[1], 2) == -1)
    {
        messageFunc("Child process FAILED",temp);
    }
    
    temp = errno;
    if (close(toChild[0]) == -1)
    {
        messageFunc("Child process FAILED",temp);
    }
    
    temp = errno;
    if (close(fromChild[1]) == -1)
    {
        messageFunc("Child process FAILED",temp);
    }
    
    char* arg[] = {NULL};
    
    temp = errno;
    if (execvp("/bin/bash", arg) == -1)
    {
        messageFunc("Child process FAILED",temp);
    }
}

//restore back the original terminal 
void restoreTerminal() {
  int status;
  
  tcsetattr(STDIN_FILENO, TCSANOW, &saveOriginalTerminal);
    
  pid_t shell_status;
  shell_status = waitpid(pid, &status, 0);
  int num = errno;
  if(shell_status == -1) {
    messageFunc("Process Failed", num);
  }

  fprintf(stderr, "SHELL exits SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
}

//Saving the original terminal 
void input() {
  tcgetattr(STDIN_FILENO, &saveOriginalTerminal);
    
  currentTerminal = saveOriginalTerminal;
    
  currentTerminal.c_iflag = ISTRIP;
  currentTerminal.c_oflag = 0;
  currentTerminal.c_lflag = 0;
    
  tcsetattr(STDIN_FILENO, TCSANOW, &currentTerminal);
    
  atexit(restoreTerminal);
}


int writeFunc(int FD, char* buffer, int size) {
  int status = write(FD, buffer, size);
  int num = errno;
  if(status == -1) {
    messageFunc("Write buffer FAILED", num);
  }
  return status;
}

void createPipes(int pipefd[2])
{
    int temp = errno;
    if (pipe(pipefd) == -1)
    {
        messageFunc("Pipe created FAILED", temp);
    }
}

void readNWrite(char* buffer, int FD, int numBytes, int proc)
{
    int offset;
    for (offset = 0; offset < numBytes; offset++)
    {
        //When receiving ^C
        if (*(buffer + offset) == 0x03)
        {
            if (proc)
            {
                kill(pid, SIGINT);
            }
        }
        //When receiving ^D
        else if (*(buffer + offset) == 0x04)
        {
            if (proc)
            {
                int status = close(toChild[1]);
                int temp = errno;
                if (status == -1)
                {
                    messageFunc("Close child FAILED", temp);
                }
            }
            else
            {
                exit(0);
            }
        }
        //When receiving <cr><lf>
        else if ((*(buffer + offset) == '\r' || *(buffer + offset) == '\n'))
        {
            if (FD == STDOUT_FILENO)
            {
                char temp[2] = {'\r', '\n'};
                writeFunc(FD, temp, 2);
            }
            else
            {
                char temp[0];
                temp[0] = '\n';
                writeFunc(FD, temp, 1);
            }
        }
        else
        {
            writeFunc(FD, buffer + offset, 1);
        }
    }
}

//When receiving a signal 
void signalHandler(int num) {
  if(num == SIGPIPE) {
    exit(0);
  }
}

int main(int argc, char** argv) {
  
    int x = 0;
    
    struct option longOptions[] = {
        {"shell", no_argument, 0, 's'},
        {0, 0, 0, 0}
    };
    
    while ((x = getopt_long(argc, argv, "s", longOptions, NULL)) != -1) {
        switch (x) {
            case 's':
                signal(SIGPIPE, signalHandler);
                shellFlag = 1;
                break;
            default:
                perror("Incorrect arguments, please pass the correct argument");
                _exit(1);
        }
    }
    
  input();
    
  //When receiving shell argument shellFlag will be 1, otherwise proceed to else statement
  if(shellFlag) {
      
    signal(SIGPIPE, signalHandler);
      
    createPipes(toChild);
    createPipes(fromChild);

    pid = fork();
    int temp = errno;
      
    if(pid == -1) {
      messageFunc("Fork process FAILED", temp);
    }
    else if(pid == 0) {
        exeShell();
    } else {
      temp = errno;
      if(close(toChild[0]) == -1) {
	messageFunc("Close child FAILED", temp);
      }
      
      temp = errno;
      if(close(fromChild[1]) == -1) {
	messageFunc("Close child FAILED", temp);
      }
        
      struct pollfd polls[2];
      
      inputPipe = fromChild[0];
      outputPipe = toChild[1];
        
      polls[0].fd = STDIN_FILENO;
      polls[0].events = POLLIN | POLLHUP | POLLERR;
        
      polls[1].fd = fromChild[0];
      polls[1].events = POLLIN | POLLHUP | POLLERR;
        
      while(1) {
	int status = poll(polls, 2, 0);
	temp = errno;
	if(status == -1) {
	  messageFunc("POLL FAILED", temp);
	} else {
	  if(status == 0) {
	    continue;
	  }
        
	  if(polls[0].revents & POLLIN) {
	    char buf[256];
        int bytes = read(STDIN_FILENO, buf, 256);
	    temp = errno;
	    
	    if(bytes == -1) {
	      messageFunc("STDIN from keyboard FAILED", temp);
	    }
          
	    readNWrite(buf, STDOUT_FILENO, bytes, shell);
          
	    readNWrite(buf, toChild[1], bytes, shell);
	  }
        
	  if(polls[1].revents & POLLIN) {
	    char shell_input_buffer[256];
          
	    int bytes = read(polls[1].fd, shell_input_buffer, 256);
	    temp = errno;
	    if(bytes == -1) {
	      messageFunc("Read shell FAILED", temp);
	    }
          
	    readNWrite(shell_input_buffer, STDOUT_FILENO, bytes, shell);
	  }
        
	  if(polls[0].revents & (POLLHUP | POLLERR)) {
	    temp = errno;
	    if(close(toChild[1]) == -1) {
	      messageFunc("Close child FAILED", temp);
	    }
	  }
        
	  if(polls[1].revents & (POLLHUP | POLLERR)) {
	      temp = errno;
	      if(close(toChild[1]) == -1) {
		messageFunc("Close child FAILED", temp);
	      }
	    break;
	  }
	}
      }
    }
  } 
  //when no shell argument
  else {
      int bytes = read(STDIN_FILENO, bufferSize, 256);
      int temp = errno;
      
      if (bytes == -1)
      {
          messageFunc("STDIN from keyboard FAILED", temp);
      }
      
      while (bytes > 0)
      {
          readNWrite(bufferSize, STDOUT_FILENO, bytes, noShell);
          bytes = read(STDIN_FILENO, bufferSize, 1);
          if (bytes == -1)
          {
              messageFunc("STDIN from keyboard FAILED", temp);
          }
      }
  }
  exit(0);
}


