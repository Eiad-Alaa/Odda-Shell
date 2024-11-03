
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h> 

#include "command.h"

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_appendOut = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
		if(_outFile == _errFile)
			_errFile = NULL;
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_appendOut = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

void
Command::chd()
{
if(_numberOfSimpleCommands > 1){
			perror("invalid command");
		}
		else{
			int args = _simpleCommands[0]->_numberOfArguments;
			if(args == 1)  //ch dir to home
			{
				char *homedir = getenv("HOME");
				if(chdir(homedir) != 0)
					perror("cd error: No home directory found");
			} else if(args== 2) {
				if(chdir(_simpleCommands[0]->_arguments[1]) != 0)
					printf("cd error '%s' : No such file or directory\n",_simpleCommands[0]->_arguments[1]);
			} else {
				int dir_size = 0;
				for (int i = 1; i < args; i++) {
    		dir_size += strlen(_simpleCommands[0]->_arguments[i]) + 1;
				}
				char *dir = (char *)malloc(dir_size);
				char *first = (char *)malloc(strlen(_simpleCommands[0]->_arguments[1])+1);
				char *last = (char *)malloc(strlen(_simpleCommands[0]->_arguments[args-1])+1);
				char *f = _simpleCommands[0]->_arguments[1];
				char *l = _simpleCommands[0]->_arguments[args-1];
				bool q = ((f[0]=='\"'&&l[strlen(l)-1]=='\"')||(f[0]=='\''&&l[strlen(l)-1]=='\''));
				dir[0] = '\0';
				for(int i = 1;i<args;i++)
				{
					char *arg = _simpleCommands[0]->_arguments[i];
					if(i == 1 && q){
				    strcpy(first, &(arg[1]));
						strcat(dir,first);
					} else if(i == args-1 && q){
							strncpy(last, arg, strlen(arg) - 1);
							last[strlen(arg) - 1] = '\0';
							strcat(dir, " ");
							strcat(dir,last);
					}else{
						if(i!=1)
						strcat(dir, " ");
						strcat(dir,arg);
					}
				}

					if(chdir(dir) != 0)
						printf("cd error '%s' : No such file or directory\n",dir);
				free(first);
				if(last != NULL)
				free(last);
				free(dir);
			}
		}
}

void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	if(strcmp(_simpleCommands[0]->_arguments[0],"cd") == 0)
	{
		chd();
		clear();
		prompt();
		return;
	}


	// Print contents of Command data structure
	print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec


	int defIn = dup(0);
	int defOut = dup(1);
	int defError = dup(2);

    if (_errFile) {
				int fdErr;
				if(_appendOut == 0)
        	fdErr = open(_errFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				else
        	fdErr = open(_errFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (fdErr < 0) {
            perror("error file open failed");
            return;
        }
        dup2(fdErr, 2);
        close(fdErr);
    }

	int fdIn = 0;
	int pids[_numberOfSimpleCommands];

	for(int i = 0; i < _numberOfSimpleCommands;i++)
	{
		if(i == 0 && _inputFile != 0)
		{
			fdIn = open(_inputFile, O_RDONLY);
			if(fdIn < 0){
				perror("open input file failed");
				return;
			}
		}
		if(fdIn != 0){
			dup2(fdIn, 0);
			close(fdIn);
		}


		if(i == _numberOfSimpleCommands - 1)
		{
			if(_outFile != 0){
				int fdOut;
				if(_appendOut == 0)
					fdOut = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				else
					fdOut = open(_outFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
				if(fdOut < 0){
					perror("open output file failed");
					return;
				}
				dup2(fdOut, 1);
				close(fdOut);
			}else{
				dup2(defOut, 1);
				close(defOut);
			}	
		}

		int fdpipe[2];
		if(i < _numberOfSimpleCommands - 1){
			if(pipe(fdpipe) == -1){
				perror("pipe failed");
				exit(2);
			}
			dup2(fdpipe[1],1);
			close(fdpipe[1]);
		}

		int pid = fork();
		if(pid == -1){
			perror("fork failed");
			return;
		}

		if(pid == 0){
				execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
				perror("execvp failed");
				_exit(1);
		}

		pids[i]= pid;
		fdIn = fdpipe[0];
	}
		dup2(defIn, 0);
		dup2(defOut, 1);
		dup2(defError, 2);

		if(_background == 0)
			for(int i = 0; i < _numberOfSimpleCommands;i++)
			waitpid(pids[i], NULL, 0);
		else
			printf("Command running in background\n");
		
		close(defIn);
		close(defOut);
		close(defError);

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation
void nothing(int sig){
	write(STDOUT_FILENO, "\b\b  \b\b", 6);
}

void
Command::prompt()
{
	char cwd[1024];
  getcwd(cwd, sizeof(cwd));
	printf("Odda_shell> %s $", cwd);
	signal(SIGINT, nothing);

	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);
int 
main()
{
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

