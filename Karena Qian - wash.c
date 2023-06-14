/* Karena Qian
 * Wash Extra Credit
 * CSC 3350
 * May 31, 2023*/

#include <stdio.h>
#include <string.h> //for string operations
#include <ctype.h> //for isspace: https://www.tutorialspoint.com/c_standard_library/c_function_isspace.htm
#include <unistd.h> //for getcwd: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
#include <limits.h> //for PATH_MAX: see above
#include <stdlib.h> //for malloc + free
/*A shell interpreter, often called a shell or command line interpreter, is a program that continually loops, accepting user input. The shell interprets the user’s input to execute programs, run built-in functions, etc. Here is a high-level pseudocode sketch of the behavior of a shell:
	while true
		read user_input					// The Parser
		if user_input is a built-in command	// The Executor
			handle it
		else if user_input is a program in path
		 	fork child process to run the program
		 	wait for the child process to complete
		else
			print error
*/

/*Invoking  wash, usage, and errors (8 points)
	Usage: wash [-h]
	The optional -h flag prints the help message (see below) and immediately exits.
	After invoking wash, it runs until the user types the exit command.
	Errors should not end a wash session. Instead, print an appropriate error message – usually, this will be things like “command not found.”
*/

/*Redirection (15 points)
	Shells typically allow for redirection between programs. For example, in bash, try the following: echo blah > tmp_file_lab3.txt
	In this example, nothing is printed to the screen, and the text “blah” is instead directed to the file tmp_file_lab3.txt. Note: this file is overwritten if it exists!
	You will implement simplified redirection in wash: when the user invokes 
command > filename, redirect command’s standard output to <filename>.out and standard error to <filename>.err.  A missing filename argument or multiple arguments should not be allowed: print a useful error message and do not run the command.
*/

/*Other Commands (24 points)
	Show that wash handles five external commands, including 1 program you write, viz. “new_head” (see below). Note: These five Other Commands are in addition to the built-ins listed above.
		Show examples of wash handling 4 non-built-in external commands. 
			The command should only be executed if it is in one of the directories specified in the user’s path. Search the paths in the order they appeared in the setpath built-in. Fork a child process, pass the appropriate arguments entered by the user to exec, and wait for the child to return before continuing to accept more user shell commands.
			You might choose from the following list of Linux commands (or other favorites): cat, date, diff, df, find, grep, man, ls, sum, tail, wc.
			Be sure to use the appropriate arguments for the 4 commands you choose.
		Write the code for a new 5th command that can be invoked in wash: new_head and show the sample output of wash running it.
			Usage: ./new_head [-h] [-n N] [file.txt]
			If [file.txt] is specified, print the first N lines of that file (default is 5 lines if -n is not used). Otherwise, read from stdin until the user hits CTRL+D, and print the first N lines typed by the user.
			If the [-n N] flag is used, print the first N lines.
			The [-h] flag should print an argument usage summary (help message).
			The [file.txt] and [-n N] argument order should not matter – both orderings should be allowed.
			Make sure to compare your output to the real head command. Note this is not exactly like head: only one file argument should be accepted, and we are not implementing the various command-line flags (except for -n and -h). In addition, head prints the first 10 lines by default, whereas new_head prints the first 5.
			Make sure to fail gracefully if the file does not exist, if too many (or too few) arguments are passed, or if the argument order is wrong (e.g., ./new_head -n file.txt 10).
			Hint: The code you developed for the “Threads Assignment” might be of some use here.
*/

/*removeWhitespaces
 *removes whitespaces before and after given cstring + returns the result
 *reference: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 *Params: begin (pointer to beginning of given cstring)
 *Returns: cstring without trailing + leading whitespaces*/
char* removeWhitespaces(char* begin){
	char* end; //end of string
	
	//remove leading spaces
	while(isspace((unsigned char) *begin)){
		begin++;
	}
	
	//what if all spaces?
	if(*begin == 0){
		return begin;
	}
	
	//remove trailing spaces
	end = begin + strlen(begin) - 1;
	while(end > begin && isspace((unsigned char) *end)){
		end--;
	}
	
	//replace old null terminator with new one
	end[1] = '\0';
	
	return begin;
}

/*readLine
 *reads and returns a single line of input; all leading/trailing white spaces are ignored
 *references:
 *	https://stackoverflow.com/questions/21679063/return-value-of-fgets
 *	https://www.w3schools.com/c/c_user_input.php
 *	https://stackoverflow.com/questions/64970163/read-a-line-from-console-in-c
 *Params: max (maximum size of input)
 *Returns: cstring input without whitespaces*/
char* readLine(int max){
	//get line input
	char input[max];
	char* valid = fgets(input, sizeof(input), stdin);
	if(valid == NULL){
                printf("ERROR: input failed\n");
                return NULL;
        }
	//get rid of leading + trailing whitespace
	return removeWhitespaces(input);
}

/*
 *
 *references: addSpaces() from 'Karena Qian - Process Creation.c' in Process Creation Assignment
 *
 **/

void concatChar(char* str, char addChar){
	int len = strlen(str);
	memset(str + len, addChar, 1);
	str[len + 1] = '\0';
}

/*
 *cd 'Karena Qian.c'
 *references: 
 *	https://stackoverflow.com/questions/4085372/how-to-return-a-string-array-from-a-function
 *	https://www.geeksforgeeks.org/strings-in-c/
 *	*/
char** getWords(char* str){
	char* start = str; //ptr to start of str
	char** words = malloc(100 * sizeof(char*)); //ret value
	int size = 0; //num of words in ret value
	char tempWord[100] = ""; //temp for each word
	int numSingleQuotes = 0;
	while(*start != 0){
		unsigned char curr = (unsigned char) *start;
		if(curr == '\''){
			numSingleQuotes++;
		}
		else if(isspace(curr) && numSingleQuotes != 1){
			if(numSingleQuotes == 2){
				numSingleQuotes = 0;
			}
			words[size] = malloc(sizeof(char) * strlen(tempWord));
			strncpy(words[size], "", 1);
			strncpy(words[size++], tempWord, strlen(tempWord)+1);
			strncpy(tempWord, "", 1);
		}
		else{
			concatChar(tempWord, curr);	
		}
		start++;
	}
	words[size] = malloc(sizeof(char) * strlen(tempWord));
	strncpy(words[size], "", 1);
        strncpy(words[size++], tempWord, strlen(tempWord)+1);
	words[size++] = NULL;
	return words;
}

void printHelp(){
	printf("\nWASH shell built-in commands:\n");
        printf("--------------------------------------------------------------\n");
        printf("pwd -> prints out the current working directory\nExample: $path$ pwd\n\n");
        printf("cd -> change the current directory to a different one\n");
        printf("\t*if no arguments are passed, directory changes to the user's home directory\n");
        printf("\t*if 1 argument is passed, directory changes to the specified directory\n");
        printf("\t*more than 1 argument is prohibited\nExample: $path$ cd [optional dir]\n\n");
        printf("setpath -> resets the PATH list to contain at least one new directory\n");
        printf("\t*PATH contains places where the shell will look for executables to run\n");
        printf("Example: $path$ setpath <one mandatory dir> [optional dir] ... [optional dir]\n\n");
        printf("help -> view all built-in commands with short descriptions\nExample: $path$ help\n\n");
        printf("exit -> exits the wash shell\nExample: $path$ exit\n\n");
}

/*buildInCmds
 *handles built-in shell commands and returns appropriate status values
 *status values:
 *0 -> (for exit) command requests executive action on shell
 *1 -> command successively executed
 *-1 -> command not recognized
 *-2 -> command ran into an error (too many arguments, failed, etc)
 *Params: cmd (a given command)
 *Returns: int status value*/
int builtInCmds(char** cmd, char** paths, int* numPaths){
	
	if(strcmp(cmd[0], "exit") == 0){
		if(cmd[1] != NULL){
                 	perror("ERROR: too many arguments");
			return -2;
                }
		printf("Exiting wash....\n");
		return 0;
	}
	else if(strcmp(cmd[0], "pwd") == 0){
		//reference: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
		char currDir[PATH_MAX+1];
		if(cmd[1] != NULL){
			perror("ERROR: too many arguments");
                        return -2;
                }
		else if(getcwd(currDir, sizeof(currDir)) != NULL){
			printf("Your Current Directory:\n%s\n", currDir);
			return 1;
		}
		else{
			perror("ERROR: command pwd error");
		}
	}
	else if(strcmp(cmd[0], "cd") == 0){
		//reference: https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
		int status;
		if(cmd[2] != NULL){
                        perror("ERROR: too many arguments");
                        return -2;
                }
		else if(cmd[1] == NULL){
			status = chdir(getenv("HOME"));
			printf("Changing directory to %s....\n", getenv("HOME"));
		}
		else{
			status = chdir(cmd[1]);
			printf("Changing directory to %s....\n", cmd[1]);
		}
		if(status != 0){
			perror("ERROR: command cd failed: directory not found");
			return -2;
		}
		printf("Directory change successful!\n");
		return 1;
	}
	else if(strcmp(cmd[0], "setpath") == 0){
		if(cmd[1] == NULL){
			perror("ERROR: expected at least 1 argument");
			return -2;
		}
		for(int i = 0; i < *numPaths; i++){
			free(paths[i]);
		}
		*numPaths = 0;
		while(cmd[*numPaths + 1] != NULL && strcmp(cmd[*numPaths + 1], ">") != 0){
			paths[*numPaths] = malloc(sizeof(char) *strlen(cmd[*numPaths + 1]));
			strncpy(paths[*numPaths], cmd[*numPaths + 1], strlen(cmd[*numPaths + 1]) + 1);
			(*numPaths)++;
		}
		printf("Current Paths: {%s", paths[0]);
		for(int i = 1; i < *numPaths; i++){
			printf(", %s", paths[i]);
		}
		printf("}\n");
		return 1;
	}
	else if(strcmp(cmd[0], "help") == 0){
		if(cmd[1] != NULL){
                	perror("ERROR: too many arguments");
                        return -2;
		}
		else{
			printHelp();
			return 1;
		}
	}
	else{
		return -1;
	}
}

void deleteWordList(char** list){
	int index = 0;
        while(list[index] != NULL){
        	free(list[index++]);
       	}
        free(list);
}

void wash(int openHelp){
	printf("Wash started\n");
	char** paths = malloc(100 * sizeof(char*));
	char* bin = "/bin";
	paths[0] = malloc(sizeof(char) * (strlen(bin) + 1));
        strncpy(paths[0], bin, strlen(bin)+1);
	int numPaths = 1;
	
	if(openHelp == 1){
		printHelp();
	}
	
	while(1){
		printf("\nPlease input a command: ");
		char* cmd = readLine(1000);
		if(cmd == NULL){
			return; //input failed
		}
		char** cmdList = getWords(cmd);
		int isBuiltIn = builtInCmds(cmdList, paths, &numPaths);
		if(isBuiltIn > -1){
			if(isBuiltIn == 0){
				deleteWordList(cmdList);
				for(int i = 0; i < numPaths; i++){
					free(paths[i]);
				}
				free(paths);
				return;
			}
		}
		else{
			printf("ERROR: command not recognized\n\n");
		}
		//deallocate list
		deleteWordList(cmdList);
	}
	printf("\nWash exited\n\n");
}

int main(){
	printf("Welcome! What do you want to do?\n");
	printf("\t- open wash [just type 'wash']\n\t\t# if you want to see help, add -h to the end\n");
	printf("\t- exit program [just type 'exit']\n\n>");
	int valid = 0; //-1 == just exit, 0 == not valid, 1 == valid
	int openHelp;
	while(valid == 0){
		char* input = readLine(1000);
		if(input == NULL){
			return -1; //input failed
		}
		char** inputList = getWords(input);
		if(inputList[1] != NULL){ //test for wash -h
			if(strcmp(inputList[1], "-h") == 0){
				openHelp = 1;
				valid = 1;
			}
			else{
				printf("Command invalid, please try again\nPlease type 'wash', 'wash -h', or 'exit'\n\n>");
				deleteWordList(inputList);
			}
		}
		else{
			if(strcmp(inputList[0], "exit") == 0){
				valid = -1;
			}
			else if(strcmp(inputList[0], "wash") == 0){
				openHelp = 0;
				valid = 1;
			}
			else{
				printf("Command invalid, please try again\nPlease type 'wash', 'wash -h', or 'exit'\n\n>");
                                deleteWordList(inputList);
			}
		}
	}
	if(valid == 1){
		wash(openHelp);
	}
	printf("Thank you for coming!\n");
	printf("\nWe hope to see you very soon!\n\n");
	return 0;
}
