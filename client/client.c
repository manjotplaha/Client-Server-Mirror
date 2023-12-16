//These are all the header files used

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>


//These are the various constraints
#define MY_MIRROR_PRT_NUMBER 7001
#define MY_PORT_NUMBER 9000
#define PATH_CHECK 1024
#define SIZE_OF_BUFFER 1024
#define HOME_PATH_LENGTH 1024
#define MAXIMUM_COMMAND_LENGTH 10000
#define CURRENT_IP "127.0.0.1"

bool Create_zip = false;



char argumentArray[10][PATH_MAX];

// Function to tokenize the command
void tokenizeCommand(char *command, char argumentArray[12][PATH_MAX], int *size) {
    // used for tokenizing
    char *pointer;
    // tokkenize on space using strtok_r
    char *temp_tkn = strtok_r(command, " ", &pointer);
    char *variable;
    // loop through all the tokens
    *size = 0;
    while (temp_tkn != NULL && *size < 10) {
        // copy string to array
        strcpy(argumentArray[(*size)++], temp_tkn);
        
        // tokenize
        temp_tkn = strtok_r(NULL, " ", &pointer);
    }
}

//this function is used for both fdb and fda command as a validator
bool validator_for_fdb_and_fda(int size,char argumentArray[][PATH_MAX]){
  if (size < 2 || size >2)
    { //print error message accroding to command
      if((strcmp(argumentArray[0], "getfdb") == 0)){
        printf("You have provided wrong format of the fdb command \n");
      }//if the command is getfda then output that
      else if((strcmp(argumentArray[0], "getfda") == 0)){
        printf("You have provided wrong format of the fda command \n");

      }
        
        return false;
    }
    Create_zip = true;
    return true;
  
}


//this function is used to validate for the command for size of file
bool validator_for_getfz(char argumentArray[][PATH_MAX],int size){
  if (size < 3 || size > 4){
      printf("You have given wrong number of arguments for getfz \n");
      return false;
    }
    //retriving the input sizes from the input command
    int firstsize = atoi(argumentArray[1]); 
    int secondsize = atoi(argumentArray[2]);
   
    if(firstsize>secondsize){
      printf("size 1 shpuld be smaller than size 2");
      return false;
    }
   
    Create_zip = true;
    return true;

}

//this validator is for getfn command
bool validator_for_getfn(int size){
  if (size < 2 || size >2)
    {
      printf("you have given wrong number of arguments  \n");
      return false;
    }

    return true;
}

//this validator is for getft command
bool  validator_for_getft(int size){
  if (size < 2 || size >4)
    {
      printf("you have given wrong number of arguments  \n");
      return false;
    }
    Create_zip = true;
    return true;

}



// validate the syntax of commands
bool helper_for_validation(char *command)
{

 char argumentArray[12][PATH_MAX];
    int size;

    // Tokenize the command
    tokenizeCommand(command, argumentArray, &size);

  //these are the various condition according to which each suitable command's validator is called
  //if input is getfdb
  if (strcmp(argumentArray[0], "getfdb") == 0)
{
    return validator_for_fdb_and_fda(size,argumentArray);
}//if the input is getfda
else if (strcmp(argumentArray[0], "getfda") == 0)
{
    return validator_for_fdb_and_fda(size,argumentArray);
}//if the input is getfz
  else if (strcmp(argumentArray[0], "getfz") == 0)
  {

    return validator_for_getfz(argumentArray,size);
   
  }//if the input is getfn
  else if (strcmp(argumentArray[0], "getfn") == 0)
  {
    return validator_for_getfn(size);
  }//if the input is getft
  else if (strcmp(argumentArray[0], "getft") == 0)
  { 
    return validator_for_getft(size);
  }
  //if the input is quitc command
  else if (strcmp(argumentArray[0], "quitc") == 0)
  {
    return true;
  }
  else
  {
    printf("You have given wrong input.\n");
    return false;
  }

  return false;
}



//this is main function
int main(int argc, char const *argv[])
{
  //these are all the variables
  int socket_id;
  struct sockaddr_in id_of_server, id_of_mirror;
  char buff[SIZE_OF_BUFFER] = {'\0'};
  char* current_ip="127.0.0.1";
  char command[SIZE_OF_BUFFER] = {'\0'};
  char datastore[SIZE_OF_BUFFER] = {0};
  char serverresponse[SIZE_OF_BUFFER] = {0};
  bool multipleserver=true;
  int adr;
  socket_id = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_id  < 0)
  {
    printf("\n There was issue in creating socket \n");
    return -1;
  }

  memset(&id_of_server, '\0', sizeof(id_of_server));
  int var;
  id_of_server.sin_family = AF_INET;
  id_of_server.sin_port = htons(MY_PORT_NUMBER);
  
  int connectid=connect(socket_id, (struct sockaddr *)&id_of_server, sizeof(id_of_server));
  if (connectid < 0)
  {
    printf("\n Client not able to connect to server \n");
    return -1;
  }

  int res;
  
  ssize_t datafromserver = read(socket_id, datastore, 3);

  //if the input recived from server defined that client should now run mirror then current connection is broken and new connection is made with server
  if (multipleserver && strcmp(datastore, "MIR") == 0)
  {
    
    close(socket_id);
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_id==-1){
      printf("socket issue in mirror");
    }
  
    int mirrorfnd=3;
    memset(&id_of_mirror, '\0', sizeof(id_of_mirror));
    id_of_mirror.sin_family = AF_INET;
    id_of_mirror.sin_port = htons(MY_MIRROR_PRT_NUMBER);
    mirrorfnd=4;
    id_of_mirror.sin_addr.s_addr = inet_addr(CURRENT_IP);
    int connectidd=connect(socket_id, (struct sockaddr *)&id_of_mirror, sizeof(id_of_mirror));
     if (connectidd == -1)
    {
      printf("There was an issue in connecting");
      exit(EXIT_FAILURE);
    }
    ssize_t datafromserver = read(socket_id, datastore, 3);

  }

 //command to check whether it client is connected to server or mirror
  printf("You are connected to %s.\n", (strcmp(datastore, "CTS") == 0) ? "server" : ((strcmp(datastore, "CTM") == 0) ? "mirror" : "unknown"));


  char usercommand[MAXIMUM_COMMAND_LENGTH];
  char copyofusercommand[MAXIMUM_COMMAND_LENGTH];

  while (1)
  {

    printf("\nEnter a command:\n");
  //output of commands which are shown to users to pick from
    const char *options[] = {
        "getfn <filename>",
        "getfz <size1> <size2>",
        "getfdb <date>",
        "getft <ext1> <ext2>",
        "getfda <date>",
        "quitc"
    };

    for (int i = 0; i < sizeof(options) / sizeof(options[0]); ++i) {
        printf("%d. %s\n", i + 1, options[i]);
    }

    
    
    Create_zip = false;
    fgets(usercommand, MAXIMUM_COMMAND_LENGTH, stdin);
    usercommand[strcspn(usercommand, "\n")] = '\0';
    int retrive=0;
    strncpy(copyofusercommand, usercommand, MAXIMUM_COMMAND_LENGTH);
    //validation is called here
    bool result_from_validation = helper_for_validation(usercommand);

    
    if (!result_from_validation)
      continue;
    bool rspfmsrvr=true;
    memset(serverresponse, 0, sizeof(serverresponse));
    memset(datastore, 0, sizeof(datastore));
    write(socket_id, copyofusercommand, strlen(copyofusercommand));

    fflush(stdout);
   
    
    

    ssize_t datafromserver = read(socket_id, datastore, 3);

    int file=strcmp(datastore,"FIL");
    int errorresponse=strcmp(datastore,"ERR");
    int messageresponse=strcmp(datastore,"MSG");
    int new_temp_fd;
char tempFilePath[PATH_MAX];
    //if the output response is file then create a new file
    if (rspfmsrvr&& file == 0)
    {
      int new_temp_fd;
      if (Create_zip)
      {
        // Check if the directory exists
    struct stat st = {0};
    if (stat("f23project", &st) == -1)
    {
        // If the directory doesn't exist, create it
        if (mkdir("f23project", 0777) != 0)
        {
            perror("Error creating directory");
            exit(EXIT_FAILURE);
        }
    }

   // Construct the path to the file in the "f23project" directory
    snprintf(tempFilePath, PATH_MAX, "f23project/temp.tar.gz");

    // Open the file in the "f23project" directory
    new_temp_fd = open(tempFilePath, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    printf("Creating temp file in f23project directory, please wait\n");


  char datastore[SIZE_OF_BUFFER];
  int datafromserver;

  //creating the file 
  while ((datafromserver = read(socket_id, datastore, SIZE_OF_BUFFER)) > 0)
  {
  
    write(new_temp_fd, datastore, datafromserver);

    if (datafromserver < SIZE_OF_BUFFER)
      break;
  }

        printf("File is recived from server\n");
        close(new_temp_fd);

        
      }
      else
      {
        
        printf("There was an issue\n");
      }
    }
   //if the message recieved from the server is error message then this is printed
    else if (rspfmsrvr&& errorresponse == 0)
    {
      
      ssize_t datafromserver = read(socket_id, serverresponse, SIZE_OF_BUFFER);
  if (datafromserver > 0)
  {
    printf(" Message from server %s\n", serverresponse);
  }
    }

  //if the message is a simple message like output of getfn
   
    else if (rspfmsrvr&& messageresponse == 0)
    {
      
      ssize_t datafromserver = read(socket_id, serverresponse, SIZE_OF_BUFFER);
  if (rspfmsrvr&& datafromserver > 0)
  {
    printf("Message from Server: %s\n", serverresponse);
  }

    }
  //if the response is quit then quit
    
    else if (rspfmsrvr&& strcmp(datastore, "QIT") == 0)
    {
      
      close(socket_id);
      printf("Executing the quit command\n");
     
      exit(EXIT_SUCCESS);
    }
  }

  
  close(socket_id);
  printf("closed connection\n");

  exit(EXIT_SUCCESS);
}
