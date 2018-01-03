#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

//This variable is for segfault option
int var = 0;

//Handle catch option
void signal_handler()
{
  perror("Catch a segmentation fault");
  _exit(4);
}

//Read 1 byte at a time from input file descriptor to output file descriptor
void readWriteFD(int inputFD, int outputFD)
{
  char *buffer;
  buffer = (char*) malloc(sizeof(char));
  ssize_t temp = read(inputFD, buffer, 1);
    
  while (temp > 0)
    {
      write(outputFD, buffer, 1);
      temp = read(inputFD, buffer, 1);
    }
    
  free(buffer);
}

//This function is for segfault option
void segFaultFunc() {
  int *x = NULL;
  var = *x;
}

int main(int argc, char **argv)
{
  int temp = 0;
  char *input = NULL;
  char *output = NULL;
  int segmentationFlag = 0; //If there is a segmentation fault, this flag will be 1
  int inputFD = 0, outputFD = 1;
    
    struct option opts[] =
      {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"segfault", no_argument, 0, 's'},
        {"catch", no_argument, 0, 'c'},
        {0, 0, 0, 0}
      };
    
    //whatever comes with column after it will require argument. In this case, i and o
    while ((temp = getopt_long(argc, argv, "i:o:sc", opts, NULL)) != -1)
      {
        switch (temp) {
	case 'i':
	  input = optarg;
	  break;
	case 'o':
	  output = optarg;
	  break;
	case 's':
	  segmentationFlag = 1;
	  break;
	case 'c':
	  signal(SIGSEGV, signal_handler);
	  break;
	default:
	  //strerror("Incorrect arguments, please pass the correct argument"); 
	  fprintf(stderr, "Incorrect arguments, please pass the correct argument");
	  _exit(1);
        }
      }
    
    if (input)
      {
        inputFD = open(input, O_RDONLY);
        
        if (inputFD >= 0)
	  {
            //Successful open a file
            close(0);
            dup(inputFD);
            close(inputFD);
	  }
        else
	  {
	    //strerror("Unsuccessful open a file");
            fprintf(stderr, "Unsuccessful open a file (error %d)", errno);
            perror(NULL);
            _exit(2);
	  }
      }
    
    if (output)
      {
        outputFD = creat(output, S_IRUSR | S_IWUSR);
        
        if (outputFD >= 0)
	  {
            //Successful create a file
            close(1);
            dup(outputFD);
            close(outputFD);
	  }
        else
	  {
	    //strerror("Unsuccessful create a file");
            fprintf(stderr, "Unsuccessful create a file (error %d)", errno);
            perror(NULL);
            _exit(3);
	  }
      }
    
    //Force a segmentation fault
    if (segmentationFlag)
      {
	segFaultFunc();
      }
    
    readWriteFD(0, 1);
    _exit(0);
}

