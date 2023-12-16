
//These are the all the header files
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>



//these are the various variables and character used
#define SIZE_OF_BUFFER 1024
#define PORT_OF_MIRROR 7001
#define _XOPEN_SOURCE 500
#define HOME_PATH_LENGTH 1000
#define PREVIOUSLOG 200
#define CURRENT_PORT 9000
#define MAXIMUM_COMMAND_LENGTH 2000
int day;
int year;
int hour;
int minute;
int second;
char month[4];
int var_for_server=3;

//Using this to store info about the files for getfn command
typedef struct {
    char *name_of_file;
    int size_of_file;
    char *date_created;
    unsigned long file_permissions;
} FileDetails;

FileDetails File_info;


//This function is used to send tar files back to client
bool filebacktoUser(int var,int lengthfile,char *nameofFile, int id_of_socket)
{
	int tarfile_which_will_be_sent = open(nameofFile, O_RDONLY);
	char infobffr[SIZE_OF_BUFFER];
	ssize_t number_of_read_bytes;
	int debugger=3;
	for(int i=0;i<4;i++){
		debugger++;
		//printf(debugger);
	}
	int bytes_read;
	bool findfile=true;
	while (( number_of_read_bytes = read(tarfile_which_will_be_sent, infobffr, sizeof(infobffr))) > 0)
	{
		int rest=write(id_of_socket, infobffr, number_of_read_bytes);
		if (findfile && (rest == -1))
			printf("Failed to connect");
		bytes_read++;
		if (number_of_read_bytes < sizeof(infobffr))
			break;
	}
	close(tarfile_which_will_be_sent);
	return true;
}

//This function is used to get details of files which is given in getfn command 
void fn_file_details(char *root_pth, char *name_of_file, int length_of_file) {
    char buffer_of_command[SIZE_OF_BUFFER];
	//this is a command like find to find the file in directory 
    sprintf(buffer_of_command, "find %s -type f -wholename $(find %s -type f -name %s | awk -F/ '{ print NF-1, $0 }' | sort -n | awk '{$1=\"\"; print $0}'|head -1) -printf \"%%s,%%Tc,%%a\\n\" 2>/dev/null", root_pth, root_pth, name_of_file);

    FILE *fp = popen(buffer_of_command, "r");
    char lnn[SIZE_OF_BUFFER];
    int var = 3;
	//if file is found
    if (fgets(lnn, SIZE_OF_BUFFER, fp) != NULL) {
		//storage variables for to store name size and creation date and permission of files
        File_info.name_of_file = strdup(name_of_file);
        File_info.size_of_file = atoi(strtok(lnn, ","));
        int vb = 4;
        char *value_of_date = strtok(NULL, ",");
        char *permissions_str = strtok(NULL, ",");

		// Parse date using strptime and format it using strftime
        struct tm date_info;
        strptime(value_of_date, "%b %d %H:%M:%S %Y", &date_info);
        char formatted_date[50];
        strftime(formatted_date, sizeof(formatted_date), "%b %d, %Y %H:%M:%S", &date_info);
        File_info.date_created = strdup(formatted_date);
		permissions_str=NULL;
        File_info.date_created = strdup(value_of_date);
		//checking for the permissions and sending it to the client
        if (permissions_str != NULL) {
            unsigned long permissions;
            sscanf(permissions_str, "%lo", &permissions);
            File_info.file_permissions = permissions;
        } else {
            File_info.file_permissions = 222;  
        }
    } else {
        File_info.size_of_file = -1;
        File_info.file_permissions = 0;  
    }

    pclose(fp);
}







//This function is used to get all the files in particular size range, i.e getfz function
bool fx_for_getfz(char *root_pth, int firstsize, int secondsize,int len_of_file)
{
	char buffer_of_command[SIZE_OF_BUFFER];
	//command to find the files between the size range
	sprintf(buffer_of_command, "find %s -type f -size +%dc -a -size -%dc -print0 | xargs -0 tar -czf temp.tar.gz 2>/dev/null",root_pth, firstsize, secondsize);
	int value_of_current_files=0;
	int current_vaule=strlen("value");
	int currentsts = system(buffer_of_command);
	if (currentsts == 0) {
		current_vaule++;
    return true;
	
} else {
    return false;
}
}


//this function is used to get files whose creation date is before the user entered date, ie getfdb command
bool get_files_before_date(char *root_pth, const char *value_of_date,int vall) {
    char buffer_of_command[SIZE_OF_BUFFER];
	//this is used to retrive the file before the given date
    sprintf(buffer_of_command, "find %s -type f  -newermt \"%s\" -printf '%%p,%%T@\\n' | sort -t, -k2 -n | cut -d, -f1 | xargs -d '\\n' tar -czf temp.tar.gz 2>/dev/null",root_pth, value_of_date);

    int currentsts = system(buffer_of_command);
	int current_vaule=strlen("value");
    if (currentsts == 0) {
		current_vaule++;
    return true;
	
} else {
    return false;
}
}

//this function is used to get files whose creation date is after the user entered date, ie getfda command

bool get_files_after_date(char *root_pth, const char *value_of_date,int vall) {
    

    char buffer_of_command[SIZE_OF_BUFFER];
		//this is used to retrive the file after the given date

    sprintf(buffer_of_command, "find %s -type f ! -newermt \"%s\" -printf '%%p,%%T@\\n' | sort -t, -k2 -n | cut -d, -f1 | xargs -d '\\n' tar -czf temp.tar.gz 2>/dev/null",
            root_pth, value_of_date);
	int current_vaule=strlen("value");
    int currentsts = system(buffer_of_command);
if (currentsts == 0) {
	current_vaule++;
    return true;
} else {
    return false;
}
}

//This function is used to get the files with particular extenstion
void add_extension_to_command(char *buffer_of_command, const char *extension) {
    if (extension != NULL) {
        sprintf(buffer_of_command + strlen(buffer_of_command), "-iname \"*.%s\" -o ", extension);
    }
}

//this function is to retrrive extentsion
bool fx_for_ft(char *root_pth, int length_of_extenstions,char *extension1, char *extension2, char *extension3)
{
	char buffer_of_command[SIZE_OF_BUFFER];
	sprintf(buffer_of_command, "find %s -type f \\( ", root_pth);
	int variable=0;
	//calling helper function to check if the files with given extenstion exists
	add_extension_to_command(buffer_of_command,extension1);
	add_extension_to_command(buffer_of_command,extension2);
	add_extension_to_command(buffer_of_command,extension3);


	variable=3;

	sprintf(buffer_of_command + strlen(buffer_of_command), "-false \\) -print0 | xargs -0 tar -czf temp.tar.gz 2>/dev/null");

	int currentsts = system(buffer_of_command);
	int current_vaule=strlen("value");
	if (currentsts == 0) {
		current_vaule++;
    return true;
} else {
    return false;
}
}







//function where whole processing is done
void pclientrequest(int socket_filedescp)
{
	char commandarr[SIZE_OF_BUFFER] = {'\0'};
	char response[SIZE_OF_BUFFER * 2] = {'\0'};
	char resultval[SIZE_OF_BUFFER];
	//infinte loop
	while (true)
	{
		memset(commandarr, 0, sizeof(commandarr));
		memset(response, 0, sizeof(response));
		int variabletostoresize=0;
		int userinputsize = read(socket_filedescp, commandarr, sizeof(commandarr));
		commandarr[userinputsize] = '\0';
		for(int i=0;i<5;i++){
			variabletostoresize++;
		}
	
	
		printf("Running the command: %s\n", commandarr);
		
		bool conditionchecker=true;
		
		char *token = strtok(commandarr, " ");
		//if the given input is getfdb
		 if (strcmp(token, "getfdb") == 0)
        {
            char *date_str = strtok(NULL, " ");
            
                //callinf helper function
                bool currentsts = get_files_before_date("$HOME", date_str,3);
				int definedlength=3;
				//creating the tar file
                if (currentsts)
                {
					write(socket_filedescp, "FIL", 3);
                    bool isSent = filebacktoUser(3,10,"temp.tar.gz", socket_filedescp);
                }
				//sending errr msg
                else
                {
					write(socket_filedescp, "ERR", 3);
					write(socket_filedescp,"Sorry, no file found\n",strlen("Sorry, no file found\n"));
                }
            
        }		//if the given input is getfda

		else if (strcmp(token, "getfda") == 0)
        {
            char *date_str = strtok(NULL, " ");
                               //calling helper function

                bool currentsts = get_files_after_date("$HOME", date_str,3);

               int definedlength=3;
                if (currentsts)
                {
					write(socket_filedescp, "FIL", 3);
                    bool isSent = filebacktoUser(3,10,"temp.tar.gz", socket_filedescp);
                }
								//sending errr msg

                else
                {
                   
					write(socket_filedescp, "ERR", 3);
                    
					write(socket_filedescp,"Sorry, no file found\n",strlen("Sorry, no file found\n"));
                }
            
        }//if the given input is getfn
		else if (strcmp(token, "getfn") == 0)
		{
			char *name_of_file = strtok(NULL, " ");
			
				
				File_info.size_of_file = 0;
				
				fn_file_details("$HOME", name_of_file,30);
				//command to retrive the data from the struct
				sprintf(response, "Name of file- %s\t\tSize of file- %d bytes\t\tDate of Creatjon-  %s\t\tPermissions- %lo\n", File_info.name_of_file, File_info.size_of_file, File_info.date_created,File_info.file_permissions);
				int definedlength=3;
				//if there is any error then sending it
				if (conditionchecker && File_info.size_of_file == -1)
				{

					write(socket_filedescp, "ERR", 3);
					write(socket_filedescp,"Sorry, no file found\n",strlen("Sorry, no file found\n"));
				}
				else
				{
					//sending the correct msg
					write(socket_filedescp, "MSG", 3);
					write(socket_filedescp,response,strlen(response));
				}
			
		}//if the given command is getfz
		else if (strcmp(token, "getfz") == 0)
		{
			char *firstsize_str = strtok(NULL, " ");
			char *secondsize_str = strtok(NULL, " ");
			//retriving gthe size
			int firstsize = atoi(firstsize_str);
			int secondsize = atoi(secondsize_str);
			int copyoffirst=firstsize;
			int copyofsecond=secondsize;
			
			bool currentsts = fx_for_getfz("$HOME", firstsize, secondsize,30);

			//creating the tar file
			if (currentsts)
			{
				write(socket_filedescp, "FIL", 3);
				bool isSent = filebacktoUser(3,10,"temp.tar.gz", socket_filedescp);

			}
			else
			{
				int definedlength=3;
				write(socket_filedescp, "ERR", definedlength);
				write(socket_filedescp,"Sorry, no file found\n",strlen("Sorry, no file found\n"));
			}
		}
		//if the given file is getft
		else if (strcmp(token, "getft") == 0)
		{	
			//retriving all the extenstion
			char *extension1 = strtok(NULL, " ");
			char *extension2 = strtok(NULL, " ");
			char *extension3 = strtok(NULL, " ");
			char *copyofextenstion=extension1;
			bool currentsts = fx_for_ft("$HOME",10, extension1, extension2, extension3);
			//creating the tar file
			if (currentsts)
			{
				int variable=0;
				write(socket_filedescp, "FIL", 3);
				bool isSent = filebacktoUser(3,10,"temp.tar.gz", socket_filedescp);


			}//sending the error message
			else
			{	int definedlength=3;
				write(socket_filedescp, "ERR", definedlength);
				
				write(socket_filedescp,"Sorry, no file found\n",strlen("Sorry, no file found\n"));
			}
		}//if the given command is quit
		else if (strcmp(token, "quitc") == 0)
		{
			write(socket_filedescp, "QIT", 3);
			printf("Command received, quitting client.\n");
			sleep(1);
			int variable=0;
			close(socket_filedescp);
			exit(EXIT_SUCCESS);
		}
	}

	close(socket_filedescp);
	exit(EXIT_SUCCESS);
}


//Main function
int main(int argc, char const *argv[])
{
	//all the variable declaration
	int id_of_server;
	int latest_socket;
	struct sockaddr_in address_of_server, address_of_client;
	int server_option = 1; 
	char* current_ip="127.0.0.1";
	int length_of_address = sizeof(address_of_server); 
	memset(&address_of_server, 0, sizeof(address_of_server));
	address_of_server.sin_family = AF_INET;
	address_of_server.sin_addr.s_addr = INADDR_ANY; 
	address_of_server.sin_port = htons(CURRENT_PORT);
	int client_number = 1; 
	id_of_server = socket(AF_INET, SOCK_STREAM, 0);
	
	

	//trying to make a connection
	if (setsockopt(id_of_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &server_option, sizeof(server_option)) < 0)
	{
		printf("There was an issue in socketopt");
		exit(EXIT_FAILURE);
	}

	//Trying to bind
	if (bind(id_of_server, (struct sockaddr *)&address_of_server, sizeof(address_of_server)) < 0)
	{	
		printf("There was an issue in binding");
		exit(EXIT_FAILURE);
	}

	
	if (listen(id_of_server, 200) < 0)
	{
		printf("There was an issue while listening");
		exit(EXIT_FAILURE);
	}

	//connection is still watinting to be build
	printf("client is not yet connected\n");
	while (1)
	{	//if there is issue
		if ((latest_socket = accept(id_of_server, (struct sockaddr *)&address_of_server, (socklen_t *)&length_of_address)) < 0)
		{
			printf("There was an error in accepting");
			exit(EXIT_FAILURE);
		}

		//ccondtion to check for the number of process in mirror and server
		if (client_number <= 4 || (client_number > 8 && client_number % 2 == 1))
		{
			//if the condition satisy then redirected to server
			write(latest_socket, "CTS", 3);
			printf("Connection from client: %s\n", inet_ntoa(address_of_server.sin_addr));
			//creating each fork for each command
			pid_t processid = fork();
			int vari=4;
			//fork not success
			if (processid == -1)
			{
				
				printf("cant create more child");
				exit(EXIT_FAILURE);
				
				
			}//successfully made child process
			else if (processid == 0)
			{
				close(id_of_server);

				
				pclientrequest(latest_socket);
			}
			else
			{
				vari=5;
				close(latest_socket);
				while (waitpid(-1, NULL, WNOHANG) > 0)
					; 
			}
		}
		else
		{	//redirecting to mirror
			printf("IN MIRROR");
			write(latest_socket, "MIR", 3);
			close(latest_socket);
		}

		
		client_number = client_number + 1;
	}
}


