#include "header.h"
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
					"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source", "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "jobs", "fg", "bg", NULL};
char command[20];		   // To store the command
extern char *ext_cmd[154]; // Increased size to 154
extern int status;		   // To store the status of last executed command
struct stop arr[50];	   // To store stopped processes

int stop_count = 0;										 // Count of stopped processes
void extract_external_commands(char **external_commands) // Function to extract external commands from file
{
	int fd = open("external_commands.txt", O_RDONLY, 0666); // Open the file
	if (fd < 0)										   // Check for errors
	{
		perror("Failed to open external_commands.txt"); // Print error message
		exit(1);								   // Exit the program
	}
	char arr[50]; // To store each command
	char ch;	  // To read each character
	int i = 0;
	int idx = 0;
	while ((read(fd, &ch, 1)) > 0) // Read each character
	{
		if (ch == '\n')
		{
			arr[i] = '\0';
			external_commands[idx] = malloc(strlen(arr) + 1); // Allocate memory for command

			strcpy(external_commands[idx], arr); // Copy command to array
			// printf("%s\n",external_commands[idx]);
			idx++;

			i = 0;
		}
		else
		{
			arr[i++] = ch; // Store character in array
		}
	}
	external_commands[idx] = NULL; //	 Null terminate the array
	close(fd);					   // Close the file
}

char *get_command(char *input_string) // Function to get the command from input string
{

	int i = 0;												  // Index for input string
	while (input_string[i] != ' ' && input_string[i] != '\0') // Loop until space or end of string
	{
		command[i] = input_string[i]; //	 Copy character to command
		i++;						  // Increment index
	}
	command[i] = '\0'; // Null terminate the command
	return command;	   // Return the command
}
int check_command_type(char *command) // Function to check the type of command
{
	for (int i = 0; builtins[i]; i++) // Loop through built-in commands
	{
		if (strcmp(command, builtins[i]) == 0) // Compare command with built-in command
		{
			return BUILTIN; // Return BUILTIN if match found
		}
	}
	for (int i = 0; ext_cmd[i]; i++) // Loop through external commands
	{
		if (strcmp(command, ext_cmd[i]) == 0) // Compare command with external command
		{
			return EXTERNAL; //	Return EXTERNAL if match found
		}
	}
	return NO_COMMAND;
}
void execute_internal_commands(char *input_string) // Function to execute built-in commands
{
	if (strncmp(input_string, "exit", 4) == 0) // Exit command
	{
		exit(0); // Exit the program
	}
	else if (strncmp(input_string, "pwd", 3) == 0)
	{
		char buff[50]; // Buffer to store current working directory
		getcwd(buff, 50);
		printf("%s\n", buff);
	}
	else if (strncmp(input_string, "cd", 2) == 0) // Change directory command
	{
		char *token = strtok(input_string, " ");
		token = strtok(NULL, " ");
		chdir(token); // Change directory
	}
	else if (strncmp(input_string, "echo $$", 7) == 0) // Print process ID
	{
		printf("%d\n", getpid());
	}
	else if (strncmp(input_string, "echo $?", 7) == 0) // Print status of last executed command
	{
		if (WIFEXITED(status))
		{
			printf("%d\n", WEXITSTATUS(status)); // Print exit status
		}
		else
		{
			printf("%d\n", 555);
		}
	}
	else if (strncmp(input_string, "echo $SHELL", 11) == 0) // Print shell path
	{
		printf("%s\n", getenv("SHELL"));
	}
	else if (strncmp(input_string, "jobs", 4) == 0) // Print stopped jobs
	{
		for (int i = 0; i < stop_count; i++)
		{
			printf("[%d] Stopped\t %s [%d]\n", i + 1, arr[i].name, arr[i].spid);
		}
	}
	else if (strncmp(input_string, "fg", 2) == 0) //	 Bring stopped job to foreground
	{
		if (stop_count > 0)
		{
			kill(arr[stop_count - 1].spid, SIGCONT);
			waitpid(arr[stop_count - 1].spid, &status, WUNTRACED);
			stop_count--;
		}
	}
	else if (strncmp(input_string, "bg", 2) == 0) // Resume stopped job in background
	{
		if (stop_count > 0)
		{
			signal(SIGCHLD, signal_handler); // Set signal handler for child process status change
			kill(arr[stop_count - 1].spid, SIGCONT);
			stop_count--;
		}
	}
}
void execute_external_commands(char *input_string) // Function to execute external commands
{
	for (int i = strlen(input_string) - 1; i >= 0; i--)
	{
		if (input_string[i] == ' ')
		{
			input_string[i] = '\0';
		}
		else
		{
			break;
		}
	}
	int c = 0;
	for (int i = 0; input_string[i] != '\0'; i++) // Count number of arguments
	{
		if (input_string[i] == ' ' || input_string[i + 1] == '\0')
		{
			c++;
		}
	}
	char **argv = malloc((c + 1) * sizeof(char *));
	int v = 0;
	int p = 0;
	char a[50];
	for (int i = 0, j = 0;; i++)
	{
		if (input_string[i] == ' ' || input_string[i] == '\0') // If space or end of string
		{
			if (j > 0)
			{
				a[j] = '\0';
				argv[v] = malloc(strlen(a) + 1);
				strcpy(argv[v], a);
				v++;
				j = 0;
				if (input_string[i] == '\0')
					break;
			}
		}
		else
		{
			a[j++] = input_string[i]; // Store character in argument
		}
	}
	argv[v] = NULL;
	for (int i = 0; i < v; i++) // Check for pipe symbol
	{
		if (strcmp(argv[i], "|") == 0) // If pipe symbol found
		{
			p = 1;
			break;
		}
	}

	if (p == 0)
	{
		if (execvp(argv[0], argv) < 0) // Execute command
		{
			printf("Command not found\n");
		}
	}

	else // If pipe symbol found
	{
		int arr[v];
		int pipe_count = 0;
		int idx = 0;

		arr[idx++] = 0;

		for (int i = 0; i < v; i++) // Store indices of pipe symbols
		{
			if (strcmp(argv[i], "|") == 0)
			{
				argv[i] = NULL;
				arr[idx++] = i + 1;
				pipe_count++;
			}
		}

		int prev_fd = -1;
		int fd[2];

		for (int i = 0; i <= pipe_count; i++) // Loop through each command separated by pipe
		{
			if (i != pipe_count)
			{
				if (pipe(fd) == -1)
				{
					perror("pipe");
					exit(1);
				}
			}

			pid_t pid = fork(); // Create a new process

			if (pid == 0) // Child
			{
				if (prev_fd != -1)
				{
					dup2(prev_fd, 0);
					close(prev_fd);
				}

				if (i != pipe_count) // Not the last command
				{
					dup2(fd[1], 1);
					close(fd[0]);
					close(fd[1]);
				}

				execvp(argv[arr[i]], argv + arr[i]); // Execute command
				perror("execvp");
				exit(1);
			}

			// Parent
			if (prev_fd != -1)
				close(prev_fd);

			if (i != pipe_count) // Not the last command
			{
				close(fd[1]);
				prev_fd = fd[0];
			}
		}

		// Wait for all children
		for (int i = 0; i <= pipe_count; i++)
			wait(NULL);
	}
}
