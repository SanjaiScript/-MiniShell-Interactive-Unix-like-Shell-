#include "header.h"
char *ext_cmd[154];		// Increased size to 154
extern char prompt[25]; //
extern char input_string[100];
extern struct stop arr[50];
extern int stop_count;
int k = 0;
int pid; // process id
int status;
void signal_handler(int sig_num) // Signal handler function
{
	if (sig_num == SIGINT) // Ctrl+C
	{
		if (pid == 0)
		{
			printf("\n%s", prompt); // Print prompt again
			fflush(stdout);
		}
	}
	else if (sig_num == SIGTSTP) // Ctrl+Z
	{
		if (pid == 0)
		{
			printf("\n%s", prompt); // Print prompt again
			fflush(stdout);
		}
		else
		{
			arr[stop_count].spid = pid;					// Store process id
			strcpy(arr[stop_count].name, input_string); // Store process name
			kill(pid, SIGTSTP);							// Send stop signal to process
			stop_count++;								// Increment stop count
		}
	}
	else if ((sig_num) == SIGCHLD) // Child process status change
	{
		waitpid(-1, &status, WNOHANG); // Non-blocking wait for child process
	}

	return;
}
void scan_input(char *prompt, char *input_string) // Function to scan user input
{
	signal(SIGINT, signal_handler);		// Set signal handler for Ctrl+C
	signal(SIGTSTP, signal_handler);	// Set signal handler for Ctrl+Z
	extract_external_commands(ext_cmd); // Load external commands

	while (1) // Infinite loop to continuously scan input

	{
		printf("%s", prompt); // Print prompt
		scanf("%[^\n]", input_string);
		getchar();								   // Read input string
		if (strncmp(input_string, "PS1=", 4) == 0) // Change prompt command
		{
			if (input_string[4] != ' ' && input_string[4] != '\0')
			{
				strcpy(prompt, input_string + 4); // Update prompt
				continue;
			}
			else
			{
				printf("Command not found\n"); // Invalid prompt command
				continue;
			}
		}

		char *cmd = get_command(input_string); // Extract command from input string
		int type = check_command_type(cmd);	   // Check command type
		if (type == BUILTIN)
		{
			execute_internal_commands(input_string); // Execute built-in command
		}
		else if (type == EXTERNAL) // External command
		{
			pid = fork(); // Create a new process
			if (pid == 0)
			{
				signal(SIGINT, SIG_DFL);				 // Default signal handling for child process
				signal(SIGTSTP, SIG_DFL);				 // Default signal handling for child process
				execute_external_commands(input_string); // Execute external command
				exit(0);								 // Exit child process
			}
			else
			{

				waitpid(pid, &status, WUNTRACED); // Wait for child process to finish
				pid = 0;						  // Reset process id
			}
		}
		else
		{
			printf("Command not found\n"); // Invalid command
		}
	}
}
