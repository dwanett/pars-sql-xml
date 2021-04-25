#ifndef HEADER_H
# define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <postgresql/libpq-fe.h>
#include <libxml2/libxml/parser.h>

typedef struct 			s_list_ins
{
	char				*insert_str;
	char				*values;
	struct s_list_ins	*next;
}						t_list_ins;

typedef struct 			s_list
{
	char 				*table;
	t_list_ins			*insert;
	int 				end_insert;
	struct s_list		*next;
}						t_list;

char					*ft_strjoin(char *s1, char *s2, int con);
PGconn					*connect_db(const char *conninfo);
void					check_error(PGresult *check, PGconn *connm, ExecStatusType def);
void					exit_nicely(PGconn *conn);

#endif