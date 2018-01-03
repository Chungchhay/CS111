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
void printTemperature(void);

int main(int argc, char* argv[]) {
  struct pollfd fds[1];
  int timeout = 1000;
  int pret;
  int fd = 0;
  //int i = 0;
  char buff[buffSize];
  int longOpt = 0;
  stopFlag = 0;

  static struct option longOptions[] = {
    {"log", required_argument, 0, 'l'},
      {"period", required_argument, 0, 'p'},
      {"scale", required_argument, 0, 's'},
    {0,0,0,0}
  };
 
  while (1)
    {
        longOpt = getopt_long(argc, argv, "l:p:s:", longOptions, NULL);
      if (longOpt == -1)
        {
	  break;
        }
        
      if (longOpt == 'l')
	{
	  logFlag = 1;
	  logFile = fopen(optarg, "w");
	}
        else if (longOpt == 'p')
        {
            pret = poll(fds, 1, timeout);
            
            int temp = atoi(optarg);
            int sleepTime = temp;
            timeout = sleepTime * 1000;
            
            if (logFlag)
            {
                fprintf(logFile, "PERIOD=%d\n", sleepTime);
            }
        }
        else if (longOpt == 's')
        {
            if (strcmp(optarg, "F") == 0)
            {
                tempType = 'F';
                if (logFlag)
                {
                    fprintf(logFile, "SCALE=F\n");
                }
            }
            else if (strcmp(optarg, "C") == 0)
            {
                tempType = 'C';
                if (logFlag)
                {
                    fprintf(logFile, "SCALE=C\n");
                }
            }
        }
      else 
	{
	  perror("Argument Error");
	  exit(1);
	}
    }

  int readButton = 0;
  mraa_gpio_context button;
  button = mraa_gpio_init(73);
  mraa_gpio_dir(button, MRAA_GPIO_IN);
  int temp = 0;
  while(1)
    {
      fds[0].fd = fd;
      fds[0].events = 0;
      fds[0].events |= POLLIN;
      pret = poll(fds, 1, timeout);
 
      readButton = mraa_gpio_read(button); //not 0 ->button pushed                           

      if (readButton != 0)
	{
	  printf("SHUT DOWN\n");
	  if (logFile)
	    {
	      fprintf(logFile, "SHUTDOWN\n");
	    }
	  mraa_gpio_close(button);
	  exit(0);
	}

      if (pret && !flag)
        { 
	  char t;
      read(fd, &t, 1);
	  
	  if (t == '\n')
	    {
	      buff[temp] = '\0';
	      temp = 0;
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
	  else 
	    {
	      buff[temp] = t;
              temp++;
	    }
        }
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
      printf("%s %0.1f\n",tempBuff, temperature);
      if (logFlag)
	{
	  fprintf(logFile, "%s %0.1f\n",tempBuff, temperature);
	}
    }
  else
    {
      printf("%s SHUT DOWN\n", tempBuff);
      if (logFlag)
	{
	  fprintf(logFile, "%s SHUTDOWN", tempBuff);
	}
      mraa_aio_close(temp);
      exit(0);
    }
  
}
