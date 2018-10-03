////////////////////////////////////////////////////////////////////////
// Textual IRC Server - An open source irc server
////////////////////////////////////////////////////////////////////////
// Copyright (C) Unknown author
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "wildcard.h"

int set(char **wildcard, char **test);
int asterisk(char **wildcard, char **test);

int wildcardfit(char *wildcard, char *test)
{
	int fit = 1;
	for (; ('\000' != *wildcard) && (1 == fit) && ('\000' != *test); wildcard++) {
		switch (*wildcard) {
			case '[':
				wildcard++;
				fit = set (&wildcard, &test);
				break;
			case '?':
				test++;
				break;
			case '*':
				fit = asterisk (&wildcard, &test);
				wildcard--;
				break;
			default:
				fit = (int) (*wildcard == *test);
				test++;
		}
	}
	while ((*wildcard == '*') && (1 == fit)) 
		wildcard++;

	return (int) ((1 == fit) && ('\0' == *test) && ('\0' == *wildcard));
}

int set(char **wildcard, char **test)
{
	int fit = 0;
	int negation = 0;
	int at_beginning = 1;
	if ('!' == **wildcard) {
		negation = 1;
		(*wildcard)++;
	}
	while ((']' != **wildcard) || (1 == at_beginning)) {
		if (0 == fit) {
			if (('-' == **wildcard) && ((*(*wildcard - 1)) < (*(*wildcard + 1))) && (']' != *(*wildcard + 1)) && (0 == at_beginning)) {
				if (((**test) >= (*(*wildcard - 1))) && ((**test) <= (*(*wildcard + 1)))) {
					fit = 1;
					(*wildcard)++;
				}
			} else if ((**wildcard) == (**test)) {
				fit = 1;
			}
		}
		(*wildcard)++;
		at_beginning = 0;
	}
	if (1 == negation)
		fit = 1 - fit;

	if (1 == fit) 
		(*test)++;

	return (fit);
}

int asterisk(char **wildcard, char **test)
{
	int fit = 1;
	(*wildcard)++; 
	while (('\000' != (**test))
		&& (('?' == **wildcard) 
		|| ('*' == **wildcard))) {
		if ('?' == **wildcard) {
			(*test)++;
			(*wildcard)++;
		}
	}
	while ('*' == (**wildcard))
		(*wildcard)++;

	if (('\0' == (**test)) && ('\0' != (**wildcard)))
		return (fit = 0);

	if (('\0' == (**test)) && ('\0' == (**wildcard)))
		return (fit = 1); 
	else {
		if (0 == wildcardfit(*wildcard, (*test))) {
			do {
				(*test)++;
				while (((**wildcard) != (**test)) 
					&& ('['  != (**wildcard))
					&& ('\0' != (**test)))
					(*test)++;
			} while ((('\0' != **test))?  (0 == wildcardfit (*wildcard, (*test))) : (0 != (fit = 0)));
		}
		if (('\0' == **test) && ('\0' == **wildcard))
			fit = 1;
		return (fit);
	}
}
