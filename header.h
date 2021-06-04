#ifndef HEADER_H
# define HEADER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <uuid/uuid.h>

# ifdef __APPLE__
# include </usr/local/opt/libpq/include/libpq-fe.h>
# include </usr/local/include/libxml2/libxml/parser.h>
# else
# include <postgresql/libpq-fe.h>
# include <libxml2/libxml/parser.h>
# endif
typedef struct			s_list_ins
{
	char				*insert_str;
	char				*values;
	struct s_list_ins	*next;
}						t_list_ins;

typedef struct			s_cim_model
{
	char				*attributes;
	char				*class;
	char				*value;
	char				*uuid;
	char 				*resource;
	xmlNodePtr			node_uuid;
	xmlNodePtr			root_node;
	struct s_cim_model	*next;
}						t_cim_model;

typedef struct 			s_list
{
	char 				*table;
	t_list_ins			*insert;
	int 				end_insert;
	int 				have_FK;
	struct s_list		*next;
}						t_list;

char					*ft_strjoin(char *s1, char *s2, int con);
PGconn					*connect_db(const char *conninfo);
void					check_error(PGresult *check, PGconn *connm, ExecStatusType def);
void					exit_nicely(PGconn *conn);

#endif