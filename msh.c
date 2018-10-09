/*
  Name : Aayush Karki
  ID   : 1001365863
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"
#define MAX_NUM_ARGUMENTS 10
#define MAX_COMMAND_SIZE 255


pid_t childpid = -1;

//function that handles cd
int cdhandle(char * args[])
{
  if (args[1] == NULL)
  {
    chdir(getenv("HOME"));
    return 1;
  }else{
    if(chdir(args[1]) == -1){
      printf("wrong directory name\n");
      return 1;
    }
  }
  return 0;
}

//thins function kills the ctrl+c and ctrl+z signals
void handle_signal(int signal_value)
{
  if (childpid > 0)
  {
    kill(childpid, signal_value);
  }
  childpid = -1;
}

int main()
{
  //call to the function handle_signal
  signal(SIGINT, handle_signal);
  signal(SIGTSTP, handle_signal);

  char * cmd_str = (char*)malloc(MAX_COMMAND_SIZE);
  char *all_cmds[15]; //this array stores recent total of 15 commands we used in the shell
  char *n_cmd = (char*)malloc(MAX_COMMAND_SIZE);
  int num_arg = 0;
  int pi = 0; int pc = 0;
  int pids[10];
  while(1)
  {
    printf("msh> ");
    while(!fgets(cmd_str,MAX_COMMAND_SIZE, stdin));
    int token_count = 0;
    char * token[MAX_NUM_ARGUMENTS];
    char *arg_str;
    char *working_str = strdup(cmd_str);
    char *working_root = working_str;
    n_cmd= strndup(cmd_str, MAX_COMMAND_SIZE);


    if(working_root != NULL)
    {

      all_cmds[num_arg] = strndup(cmd_str, MAX_COMMAND_SIZE);
      num_arg++;
    }


    //this will parse the user command string and store into token array
    while(((arg_str = strsep(&working_str, WHITESPACE)) != NULL) && (token_count < MAX_NUM_ARGUMENTS) )
    {
      token[token_count] = strndup(arg_str, MAX_COMMAND_SIZE);
      if(strlen(token[token_count]) == 0){
        token[token_count] = NULL;
      }
      token_count++;
    }


    //to run the nth command from the history
    if(n_cmd[0] == '!')
    {
      //converts the string value into integer
      int cmd = atoi(&n_cmd[1]);
      //puts the command from the array to token_count
      //so that the required command will run
      token[0] = strndup(all_cmds[cmd], MAX_COMMAND_SIZE);
    }

    //if the input is null it will just print the other msh> prompt
    if(token[0] == NULL) continue;

    //if the user wants to exit from shell he can type exit or quit
    if((strcmp(token[0],"exit") == 0) || (strcmp(token[0], "quit") == 0))
    {
      for(int i = 0; i < pc; i++)
      {
        kill(pids[i], SIGABRT);
      }
      exit(0);

    }



    //to handle the cd
    if(strcmp(token[0],"cd") == 0)
    {
      //calling the function cdhandle
      cdhandle(token);
      continue;
    }

    //to run the history command
    if((strcmp(token[0], "history")) == 0)
    {
      int i;
      for(i = 0; i < num_arg ; i++)
      {
        printf("%d): %s\n",i,all_cmds[i]);  //lists the history of the commands
      }
    }

    //for the listpid command which list the processing pids
    if((strcmp(token[0],"listpids")) == 0)
    {
        for(int i = 0; i < pc + 1; i++)
        {
          printf("%d)  %d\n",i, pids[i]);  //lists the pids
        }
        continue;
    }

    pid_t pid = fork();
    if (pid != 0)
    {
      //this will wait for the current pid to process
      waitpid(pid, NULL, 0);
    }
    else
    {
      //this will give you the current process id
      childpid = getpid();
      if (pi < 10) {
          pids[pi] = childpid;
          pi++;
          pc++;
      }
      else
      {
          pids[0] = childpid;
          pi = 1;
      }
      //exec happens over here
      if (execvp(token[0], token) == -1)
      {
            printf("%s: Command not found.\n\n", token[0]);
            continue;
      }

    }
      free(working_root);
  }

  return 0;
}
