/*
 * Copyright (C)2007 Giulio Bai
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clip.h"

/* Include the right file with your functions! */
#include "../funcs.c"

/* The maximum number of option you want in your
 * program to be enabled.
 * NOTE this number has to correspond to the size
 * of the 'options' struct.
 */
#define NUM_OPTS 4

/* The max number of arguments the user may specify
 * after an option.
 * EXAMPLE:
 * assume the option '-o' wants 4 args and MAX_ARGS
 * is set to 3;
 * whenever the user calls ./your_prog -o a1 a2 a3 a4
 * an error message is displayed and the execution stops.
 * 
 * Set it to the_num_of_the_highest_args_an_option_accepts + 1
 * 
 * This variable is needed only to set the 'args' array
 * size and avoid boring errors such as 'segmentation fault'
 */
#define MAX_ARGS 3

/* Set to 1 in case of debug to test new features */
/* [default: 0] */
#define DEBUG    0

/* If the debug mode is on and you want to get some
 * written informations, set it to 1
 */
/* [default: 0] */
#define VERBOSE  0

/* Prefix for short option calls -- max 1 char*/
/* [default: "-"] */
char *opt_prefix      = "-";

/* Prefix for long option calls -- max 2 chars*/
/* [default: "--"] */
char *opt_long_prefix = "--";

/* The types an option may have
 * (obviously only SHORT and LONG :D)
 */
enum types { SHORT = 1, LONG };

typedef struct {
	/* The user has to define these only */
	char *name; /* Name of the option */
	int numArgs; /* Number of arguments the option takes */
	enum types format; /* Type of the option's call */
	
	/* The function you'll specify as target must exist and declared
	 * BEFORE this point.
	 */
	int (*target)(char **); /* Pointer to the function to initialize */
	
	/* You may not touch these! -- Unless you know what you're doing */
	int done; /* The number of args processed */
	int called; /* Has the option been called? */
	
	/* Size +1 to avoid overflows */
	char *args [MAX_ARGS+1]; /* array of args (starting from 0!)*/
} opt;


/* --------------------------------------------------------------
 * EDIT REQUIRED HERE
 * --------------------------------------------------------------
 * Here is the right place to specify the available options;
 * This is just an sample configuration and, following this
 * example's structure, you shouldn't commit mistakes
 */
opt options [NUM_OPTS] = {
	{ "smooth", 3, SHORT, Smooth },
	{ "negative", 2, SHORT, Negative },
	{ "togray", 2, SHORT, ToGray },
	{ "lighten", 3, SHORT, Lighten },
	{ "to24bit", 2, SHORT, To24Bit },
	{ "help", 0, LONG, PrintInfo },
	{ "h", 0, SHORT, PrintInfo },
};


/* Help option -- to include in the options struct too! */
char *help = "--help";


CLiP::CLiP (int argc, char *argv [])
{
	/* Wow, simple and smart... */ 
	Parse (argc, argv);
}

CLiP::~CLiP (void)
{
}

int FindParent (int i, char *argv [])
{
	/* Go back until an option's found */
	do {
		i --;
	} while ((argv [i][0] != opt_prefix [0] ||
	         (argv [i][0] != opt_long_prefix [0]   &&
	          argv [i][1] != opt_long_prefix [1])) && i > 0);
	
	return i;
}

/* Which type of char has been passed? (long option / short option / arg) */
void CLiP::Parse (int argc, char *argv [])  /* Func1 */
{
	int i;
	
	for (i=1; i<argc; i++) {
		/* First 2 char of long option prefix match: long command */
		if (argv [i][0] == opt_long_prefix [0] &&
		    argv [i][1] == opt_long_prefix [1]) {
		
			/* No option named as 'input' found - error! */
			if (Find (argv [i], LONG) == 0) {
				/* Error0 */
				fprintf (stderr, "ERROR: no existing option with name '%s'. Try '%s' for info.\n", argv [i], help);
				exit (10);
			}
			
			/* debug info */
			if (DEBUG && VERBOSE)
				printf ("option '%s' invoked in long form\n", argv [i]);
		}
		/* First char of short option prefix matches: short form command */
		else if (argv [i][0] == opt_prefix [0]) {
		/* Error1 */
			/* No option named as 'input' found - error! */
			if (Find (argv[i], SHORT) == 0) {
				fprintf (stderr, "ERROR: no existing option with name '%s'. Try '%s' for info.\n", argv [i], help);
				exit (11);
			}
			
			if (DEBUG && VERBOSE)
				printf ("option '%s' invoked in SHORT form\n", argv [i]);
		}
		/* No match: it's an argument! */
		else {
			int x = FindParent (i, argv);
			
			/* If no option's been found something wrong happened */
			if (x == 0) {
			/* Error2 */
				fprintf (stderr, "ERROR: mis-typed word '%s': should be an argument but no option were found\n", argv [i]);
				exit (12);
			}
			/* If everything is ok, fill in the args arrays */
			else {
				FillArgs (i, x, argv);
			}
		}
	}
	
	
	/* Scans the array of options... */
	for (i=0; i<NUM_OPTS; i++)
		/* ...to find a called one! */
		if (options[i].called) {
			/* All args have been specified: ok, we continue */
			if (options[i].done    == options[i].numArgs ||
			   (options[i].numArgs == 0 && options[i].done == 0)) {
				options[i].target (options[i].args);
			}
			/* Got 12 out of 10? wow sounds incredible... */
			else if (options[i].done > options[i].numArgs) {
			/* Error0 */
				fprintf (stderr, "ERROR: too many arguments for option '%s'! Try '%s' for info.\n", options[i].name, help);
				exit (30);
			}
			/* Did you forget something? (wake up!) */
			else if (options[i].done < options[i].numArgs) {
			 /* Error1 */
				fprintf (stderr, "ERROR: missing operand(s). Try '%s' for info\n", help);
				exit (31);
			}
		}
}

char *allocName (int i)
{
	char *name;
	
	if (options[i].format == SHORT) {
		name = (char*)malloc (strlen (options[i].name) + strlen (opt_prefix) + 1);
		strcpy (name, opt_prefix);
	}
	else if (options[i].format == LONG) {
		name = (char*)malloc (strlen (options[i].name) + strlen (opt_long_prefix) + 1);
		strcpy (name, opt_long_prefix);
	}
	
	strcat (name, options[i].name);
	
	return name;
}

int CLiP::Find (char *input, int type)
{
	int i;
	char *name;
	
	for (i=0; i<NUM_OPTS; i++) {
		name = allocName (i);
		
		/* Scan options to check the existence of the one passed */
		if (!strcmp (name, input) && options[i].format == type) {
			/* called flag to 1: the option has been called! */
			options[i].called = 1;
			
			free (name);
			
			return i+1;
		}
		
		free (name);
	}
	
	return 0;
}

/* Put args in their owner's array */
void CLiP::FillArgs (int i, int x, char *argv []) /* Func2 */
{
	int k;
	char *name;
	
	if (DEBUG && VERBOSE)
		printf ("'%s' is an argument of the option '%s'\n", argv[i], argv[x]);
	
	for (k=0; k<NUM_OPTS; k++) {
		name = allocName (k);
		
		/* Parent option found */
		if (!strcmp(name, argv [x])) {
			if (options[k].done < MAX_ARGS) {
				/* Add arg in array */
				options[k].args[options[k].done] = argv[i];
				
				/* Wow, we've done another argument matching! */
				options[k].done ++;
			}
			/* Hey! Too many arguments... Argh... */
			else {
			/* Error0 */
				fprintf (stderr, "ERROR: too many arguments for option '%s'! Try '%s' for info.\n", options[k].name, help);
				
				free (name);
				
				exit (20);
			}
		}
		
		free (name);
	}
}
