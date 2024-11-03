
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%union	{
		char   *string_val;
	}

%token	<string_val> WORD

%token 	NOTOKEN GREAT NEWLINE OR GG SMALL BACKGROUND GGER GER EXIT


%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include <unistd.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command
	;

command: 
	simple_command
	|EXIT NEWLINE{
		printf("\n\tGood Bye!\n\n");
		exit(0);
	}
  ;

simple_command:	
	command_and_args pipes iomodifier_opt_list later NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list{
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->insertArgument( $1 );\
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

pipes:
	pipes pipe
	| /* can be empty */ 
	;

pipe:
	pipe_word command_and_args
	;

pipe_word:
	OR{
		printf("   Yacc: insert pipe to command\n");
	}
	;


iomodifier_opt_list:
	iomodifier_opt_list iomodifier_opt
	| /* can be empty */ 
	;

iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	|GG WORD {
		printf("   Yacc: append output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._appendOut = 1;
	}
	|SMALL WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand. _inputFile = $2;
	}
	|GER WORD {
		printf("   Yacc: insert output and error \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
	}
	|GGER WORD {
		printf("   Yacc: append output and error \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._appendOut = 1;
	}
	;

	later:
	BACKGROUND{
		printf("   Yacc: insert background operating\n");
		Command::_currentCommand._background = 1;
	}
	| /* can be empty */ 
	;
%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
