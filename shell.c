
#include <stdio.h>
#include <sys/dir.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>



#define true 1
#define MAX 100


char *argv[MAX/2];
char *get_current_dir_name();
char history_Count;
char history[100][20];

void run_from_history(char arg[]);
void history_display(void);
void set_history(char arg[]);



void shellPrompt()
{
	printf("MyShell@Group16> ");//Display by shell.
}



char ** parse(char *line)
{
	char *token; 
        static char *arg[80];
        int i=0;
        if(line==NULL)
	        return NULL;
	token=strtok(line," \t\n");
        while(token)
        {
        	arg[i++]=token;
        	token=strtok(NULL, " \t\n");
        }
        arg[i]=NULL;
        return (char **) arg;
}



void pass_to_token(char *take, char **argv)
{
	while (*take != '\0')
	{
        	while (*take == ' ' || *take == '\t' || *take == '\n')
        	*take++ = '\0';
        	*argv++ = take;
        	while (*take != '\0' && *take != ' ' &&
        	*take != '\t' && *take != '\n')
        	take++;
        }
	*argv = '\0'; 
}



void Process(char **argv)
{
	pid_t pid=fork();
        int status;
        if (pid < 0)
	{ 
               	fprintf(stderr,"command not found\n");
        	exit(1);
	}
        else if (pid == 0)
	{ 
       		if (execvp(*argv, argv) < 0)
		{ 
            		fprintf(stderr,"command not found\n");
              		exit(1);
       		}
  	}
   	else
	{
       		waitpid(-1,&status,0);
       	}
}



void _execute(char **argv)
{
	pid_t  pid;
	int    status;
	if ((pid = fork()) < 0)//when forking a process Failes.
	{     
		 printf("ERROR: forking child process failed\n");
		 exit(1);
	}
	else if (pid == 0)  //When the given command is not found!.
	{          
		  if (execvp(*argv, argv) < 0) 
		  {     
	       	  	printf("ERROR: could not find command\n");
     		  	exit(1);
	          }
        }
	else 
	{                                  
               	  while (wait(&status) != pid)      
                           ;
        }
}



int check_pipe(char **argv)
{
	int i=0;
	while(argv[i] != '\0')
	{
		if(*argv[i] == '|')// checking whether the given command is for piping a proccess or not.
		return 1;
		
		i++;
	}
	return 0;
}



void exec_pipe(char **argv)
{
	pid_t pid;
	int pfds[2];
	char *arg1[10],*arg2[10];
	int i=0;
	//pipe here
	while(*argv[i] != '|')
	{
		arg1[i] = argv[i];
		i++;
	}
	
	arg1[i] = '\0';
	i++;
	int j =0;
	while(argv[i] != NULL)
	{
		arg2[j] = argv[i];
		j++;
		i++;
	}
	arg2[j] = '\0';
	pipe(pfds);
	pid =  fork();
	if (pid == 0) // child process - first command - redirect stdout
	{     		
        	close(1);       // close normal stdout 
		dup(pfds[1]);   // make stdout same as pfds[1] 
		close(pfds[0]); 
		if (execvp(*arg1, arg1) < 0)
		{     
			printf("ERROR: could not find command\n");
	       		exit(1);
	  	}
	    
	}
	else // parent process - second command - redirect stdin
	{ 	             	
		wait(&pid);
		close(0);       // close normal stdin 
		dup(pfds[0]);   // make stdin same as pfds[0] 
		close(pfds[1]); 
		if (execvp(*arg2, arg2) < 0)
		{     
			printf("ERROR: could not find command\n");
	       		exit(1);
	       	}
	}
}



void history_display(void)
{
	int i = 0;
	while(history[i][0] != '\0')
        {
	        printf("%d %s\n", i+1, *(history + i));//displaying the commands stored in history array.
                i++;
        }
}



void set_history(char arg[])
{
	int i = 0;
        while(history[i][0] != '\0')
        {
	        i++;
        }
        strcpy(history[i], arg);//adding commands to history
        history_Count++;
}



void run_from_history(char * command)
{
	int index = 0;
	if(history_Count == 0){
		printf("No commands in history\n");
		return ;
	}
	if(command[1] == '!') // The second character '!' The command takes the index of the last entry
		index = history_Count-1;
	else
	{
		index = atoi(&command[1]) -1 ;	//takes the user to the second character of the history index
		if((index < 0) || (index > history_Count)) //history ' as an index print is no such error(undefined case).
                {
			fprintf(stderr, "No such command in history.\n");
			return;
		}
	}
	printf("The required command is :%s \n",history[index]);
        printf(".................................The command is now being excuted\n");	//Printing the command in history
	pass_to_token(history[index],argv);//Running the Command
        Process(argv);	
}

 
int main(void)
{	
	printf("\n");
	printf("******************Group 16**********************\n");
	printf("\n");
	char command[100];
	while(true)
        {
	        shellPrompt();
	        gets(command);
            	pass_to_token(command, argv);
            	if (!strcmp(argv[0], "history"))
            	{
                	history_display();
                	continue;
               	}
               	if(!strcmp(argv[0],"exit"))
	       	{
			return 0;
	       	}    
            	if (command[0] == '!')
               	{
                	run_from_history(command);
                        continue;
		}
            	set_history(command);
            	if(check_pipe(argv)) //Checking and Excuting the command if it uses piping 
		{
			pid_t pid = fork();
			if(pid < 0)
			{
				printf("\n fork failed");
			}
			if(pid == 0)
			{
				exec_pipe(argv);
			}
			else
			{
				wait(&pid);
			}
			continue;
		}
		
            	else
		{
                   Process(argv);//if none of the above case satisfies then the program runs the procces normally.
                }
       }
    return 0;
}




