#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
//=============================================================================================
/* Function Declarations */
int getUserInput(char *input, char directoryPath[]);
void parseCommands(char *commandArray[], int argLength);
void tokenizeInput(char *input);
int runProgram(char *argv[], int argc);
int gofolder(char *argv[], int argc);
int push(char *argv[], int argc);
int pop(int argc);
int dirs(int argc);
//=============================================================================================
/* Directory Stack (Linked-List) */
struct directoryNode
{
  char *directoryName;
  struct directoryNode *next;
};
//=============================================================================================
/* Global Variables */
struct directoryNode *stackTop = NULL;
char cwd[PATH_MAX];    //current working directory, updated to reflect cwd in shell prompt
int currentStatus = 0; //used for command chaining, deals with exit status of child process
//=============================================================================================
/* push [directory path]
1. push current directory into the stack
2. change to the directory specified by directory path
3. print the contents of the stack
*/
int push(char *argv[], int argc)
{
  if (argc < 2 || argc > 2)
  {
    printf("push: wrong number of arguments\n");
    return -1;
  }
  char *cwdCopy = malloc(strlen(cwd) + 1);
  strcpy(cwdCopy, cwd);
  if (gofolder(argv, 2) == -1)
  {
    return -1;
  }

  char *newDirectoryName = argv[1];
  struct directoryNode *newNode;
  newNode = (struct directoryNode *)malloc(sizeof(struct directoryNode)); //allocate memory for new node
  newNode->directoryName = calloc(strlen(cwdCopy), sizeof(char));         //allocate memory for directory
  strcpy(newNode->directoryName, cwdCopy);                                //set directory name
  newNode->next = stackTop;                                               //append to top of stack
  stackTop = newNode;                                                     //set to new top of stack
  //printf("%s pushed to stack\n", stackTop->directoryName);
  dirs(1);
  return 0;
}
//=============================================================================================
/* pop
1. pop an item from the stack and alert if stack is empty
2. change to the directory specified by directory path
3. print the contents of the stack
*/
int pop(int argc)
{
  if (argc != 1)
  {
    printf("pop: wrong number of arguments\n");
    return -1;
  }

  struct directoryNode *poppedNode = stackTop; //save pointer to top of stack
  if (poppedNode == NULL)
  {
    printf("pop: directory stack empty\n");
    return 0;
  }
  char *argv[2];
  argv[1] = poppedNode->directoryName;
  stackTop = stackTop->next; //set new top of stack to second node from top
  free(poppedNode);
  //printf("%s popped from stack\n", poppedNode->directoryName);
  gofolder(argv, 2);
  /*  if (stackTop == NULL)
    printf("pop: reached end of stack\n"); */
  return 0;
}
//=============================================================================================
/* dirs
1. print the contents of the stack [item number] [path]
most recent is item number 0, oldest is largest
*/
int dirs(int argc)
{
  if (argc != 1)
  {
    printf("dirs: wrong number of arguments\n");
    return -1;
  }

  struct directoryNode *temp = stackTop;
  if (temp == NULL)
  {
    //stack is empty
    return 0;
  }

  int counter = 0;
  while (temp != NULL)
  {
    printf("%d %s\n", counter, temp->directoryName);
    temp = temp->next;
    counter++;
  }
  return 0;
}
//=============================================================================================
int gofolder(char *argv[], int argc)
{
  if (argc < 2 || argc > 2)
  {
    printf("gofolder: wrong number of arguments\n");
    return -1;
  }
  char *folderName = argv[1];
  if (strcmp(folderName, "..") == 0) //cd ..
  {
    chdir(folderName);
    getcwd(cwd, PATH_MAX);
    return 0;
  }
  if (chdir(folderName) == 0) //if absolute path
  {
    getcwd(cwd, PATH_MAX);
    return 0;
  }
  if (folderName[0] != '/') //if need slash to search within directory
  {
    // printf("NEED TO ADD SLASH\n");
  }

  char *cwdCopy = malloc(strlen(cwd) + 1);
  strcpy(cwdCopy, cwd);
  char *dirPath = strcat(cwdCopy, folderName); //search within directory
  //printf("%s\n", dirPath);
  if (chdir(dirPath) != 0)
  {
    printf("{%s}: cannot change directory\n", argv[1]);
    return -1;
  }
  getcwd(cwd, PATH_MAX); //updated cwd in shell prompt
  return 0;
}
//=============================================================================================
int bye(int argc)
{
  if (argc != 1)
  {
    printf("bye: wrong number of arguments\n");
    return -1;
  }
  exit(0);
}
//=============================================================================================
/* Functions */
int main(int argc, char *argv[])
{
  //ignore signals
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  int isExit = 0;
  getcwd(cwd, PATH_MAX); //get current working directory
  do
  {
    char rawInput[255];
    isExit = getUserInput(rawInput, cwd);
    if (isExit)
      break;

    tokenizeInput(rawInput);
  } while (isExit = 1);

  return 0;
}
//=============================================================================================
/*
  GetUserInput()
  - To parse User Input and remove new line character at the end.
  - Copy the cleansed input to parameter.
  - Return 1 if encountered EOF (Ctrl-D), 0 otherwise.
*/
int getUserInput(char *input, char directoryPath[])
{
  char buf[255];
  char *s = buf;
  printf("[3150 Shell:%s]=> ", directoryPath);
  if (fgets(buf, 255, stdin) == NULL)
  {
    putchar('\n');
    return 1;
  }
  // Remove \n
  for (; *s != '\n'; s++)
    ;
  *s = '\0';

  strcpy(input, buf);
  return 0;
}
//=============================================================================================
int runProgram(char *argv[], int argc)
{
  /*  printf("RUNNING...! ARGC: %d\n", argc);
  for (int i = 0; i <= argc; i++)
  {
    printf("ARGV[%d]: %s\n", i, argv[i]);
  }
  printf("Comparing with bye %d\n", strcmp(argv[0], "bye"));
 */
  if (strcmp(argv[0], "bye") == 0)
    return bye(argc);

  if (strcmp(argv[0], "ifconfig") == 0)
  {
    printf("{%s}: command not found\n", argv[0]);
    return -1;
  }
  if (strcmp(argv[0], "hello") == 0)
  {
    char *commandArray[2];
    commandArray[0] = "./hello";
    commandArray[1] = NULL;
    return runProgram(commandArray, 1);
  }

  if (strcmp(argv[0], "gofolder") == 0)
    return gofolder(argv, argc);

  if (strcmp(argv[0], "push") == 0)
    return push(argv, argc);

  if (strcmp(argv[0], "dirs") == 0)
    return dirs(argc);

  if (strcmp(argv[0], "pop") == 0)
    return pop(argc);

  int stat;
  if (fork() == 0) //if child process
  {
    if (argv[0][0] == '/' || argv[0][0] == '.') //for args with path name
      execv(argv[0], argv);
    else
      execvp(argv[0], argv); //for args without path name
    exit(-1);
  }
  else
    wait(&stat); //parent process waits

  if (WEXITSTATUS(stat) == 0) //WEXITSTATUS to check if ran successfully

  {
    // printf("STATUS ALL GOOD\n");
    return 0;
  }
  else
  {
    printf("{%s}: command not found\n", argv[0]);
    return -1;
  }
}
//=============================================================================================
/*
Takes in array of command tokens
Parses all commands before &&, ||, or NULL (end of chain)
Runs commands and deals with command chaining using currentStatus
*/
void parseCommands(char *commandArray[], int argLength)
{
  int startIndex = 0; //index of &&, ||, or NULL, type of offset for parsing the tokens
  for (int i = 0; i <= argLength; i++)
  {
    //printf("I == LENGH IS: %d \t ARGV[I] TRUTH is %d\t", i == argLength, commandArray[i] == NULL);
    //printf("CURRENT STATUS: %d\t START INDEX: %d\n", currentStatus, startIndex);
    if ((commandArray[i] == NULL) && (currentStatus != 0))
    {
      //printf("REACHED BREAK\n");
      break;
    }
    if ((commandArray[i] == NULL) && (currentStatus == 0))
    {
      if (startIndex == 0)
      {
        char *argv[i];

        for (int j = startIndex, k = 0; j < i; j++, k++)
        {
          argv[k] = commandArray[j];
          //printf("NULL %d: %s\n", k, argv[k]);
        }
        argv[i] = NULL;
        //printf("NULL %d: %s\n", i, argv[i]);
        runProgram(argv, i);
        startIndex = i;
        if (i == argLength)
          break;
      }
      else
      {
        char *argv[i - startIndex];

        for (int j = startIndex + 1, k = 0; j < i; j++, k++)
        {
          argv[k] = commandArray[j];
          //printf("NULL %d: %s\n", k, argv[k]);
        }
        argv[i - startIndex - 1] = NULL;
        //printf("NULL %d: %s\n", i - startIndex, argv[i - startIndex]);
        for (int j = 0; j <= i - startIndex - 1; j++)
        {
          //printf("TESTER: %d: %s\n", j, argv[j]);
        }
        runProgram(argv, i - startIndex - 1);
        startIndex = i;
        if (i == argLength)
          break;
      }
    }

    if ((strcmp(commandArray[i], "&&") == 0) && (currentStatus == 0))
    {
      if (startIndex == 0)
      {
        char *argv[i];

        for (int j = startIndex, k = 0; j < i; j++, k++)
        {
          argv[k] = commandArray[j];
          //printf("&& %d: %s\n", k, argv[k]);
        }
        argv[i] = NULL;
        //printf("&& %d: %s\n", i, argv[i]);
        currentStatus = runProgram(argv, i); //update current status if && is found
        startIndex = i;
        if (i == argLength)
          break;
      }
      else
      {
        char *argv[i - startIndex];

        for (int j = startIndex + 1, k = 0; j < i; j++, k++)
        {
          argv[k] = commandArray[j];
          //printf("&& %d: %s\n", k, argv[k]);
        }
        argv[i - startIndex - 1] = NULL;
        //printf("&& %d: %s\n", i, argv[i - startIndex]);
        currentStatus = runProgram(argv, i - startIndex - 1); //update current status if && is found
        startIndex = i;
        if (i == argLength)
          break;
      }
    }

    if ((strcmp(commandArray[i], "||") == 0) && (currentStatus == 0))
    {
      if (startIndex == 0)
      {
        char *argv[i];

        for (int j = startIndex, k = 0; j < i; j++, k++)
        {
          argv[k] = commandArray[j];
          //printf("|| %d: %s\n", k, argv[k]);
        }
        argv[i] = NULL;
        //printf("|| %d: %s\n", i, argv[i]);
        if (runProgram(argv, i) == 0) //update currentStatus depending on success
          currentStatus = -1;
        else
          currentStatus = 0;
        startIndex = i;
        if (i == argLength)
          break;
      }
      else
      {
        char *argv[i - startIndex];

        for (int j = startIndex + 1, k = 0; j < i; j++, k++)
        {
          argv[k] = commandArray[j];
          //printf("|| %d: %s\n", k, argv[k]);
        }
        argv[i - startIndex - 1] = NULL;
        //printf("&& %d: %s\n", i, argv[i - startIndex]);
        if (runProgram(argv, i - startIndex - 1) == 0) //update currentStatus depending on success
          currentStatus = -1;
        else
          currentStatus = 0;

        startIndex = i;
        if (i == argLength)
          break;
      }
    }
    /* if ((strcmp(commandArray[i], "||") == 0) && (currentStatus == -1))
      currentStatus = 0; */
  }
}

//=============================================================================================
/*
  tokenizeInput()
  - Tokenize the string stored in parameter, delimiter is space
  - Populate array with tokenized input
  - Send to handleCommands
*/
void tokenizeInput(char *input)
{
  char buf[255];
  strcpy(buf, input);
  char *commandArray[50];
  char *token = strtok(buf, " ");
  int argLength = 0;

  for (int i = 0; token != NULL; i++)
  {
    commandArray[i] = token;
    token = strtok(NULL, " ");
    argLength++;
  }

  commandArray[argLength] = NULL;
  currentStatus = 0;
  parseCommands(commandArray, argLength);
  return;
}
