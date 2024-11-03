
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

"exit"	{
		return EXIT;
}
[^ <>&|\t\n]+  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}
[ \t]+	{
		/* Discard spaces and tabs */
     	}	

">" 	{
		return GREAT;
	}

">>" 	{
		return GG;
	}

"<" 	{
		return SMALL;
	}

"&>>" 	{
		return GGER;
	}

"&>" 	{
		return GER;
	}

"|" 	{
		return OR;
	}

[ \t]+"&"	{
		return BACKGROUND;
	}


.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
