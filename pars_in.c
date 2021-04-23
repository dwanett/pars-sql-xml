#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <postgresql/libpq-fe.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

void exit_nicely(PGconn *conn)
{
	PQfinish(conn);
	exit(1);
}

void check_error(PGresult *check, PGconn *conn)
{
	if (PQresultStatus(check) != PGRES_TUPLES_OK)
	{
		fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
		PQclear(check);
		exit_nicely(conn);
	}
}

PGconn *connect_db(const char *conninfo)
{
	PGconn *conn;

	conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr, "Connection to database failed: %s",
				PQerrorMessage(conn));
		exit_nicely(conn);
	}
	return (conn);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*str;
	int		i;
	int		j;
	int		k;

	i = 0;
	j = 0;
	k = 0;
	str = (char*)malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
	if (str == NULL)
		return (NULL);
	while (s2[j] != '\0' || s1[k] != '\0')
	{
		if (s1[k] != '\0')
			str[i] = s1[k++];
		else
			str[i] = s2[j++];
		i++;
	}
	str[i] = '\0';
	return (str);
}

void	print_element_names(xmlNode *a_node, char **head, PGconn *conn)
{
	xmlNode *cur_node;
	xmlChar *tmp;
	xmlChar *flag;
	char *content;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		flag = xmlGetProp(cur_node, (const xmlChar *)"object");
		if (flag != NULL)
		{
			if (*head)
				free(*head);
			*head = ft_strjoin((char *) cur_node->name, "");
		}
		xmlFree(flag);
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			tmp = xmlGetProp(cur_node, (const xmlChar *)"type");
			content = (char *)xmlNodeGetContent(cur_node);
			xmlFree(tmp);
			free(content);
		}
		print_element_names(cur_node->children, head, conn);
	}
}


int	main(int argc, char **argv)
{
	PGconn *conn;
	xmlDoc *doc;
	xmlNode *root_element;
	char *head;

	doc = NULL;
	head = NULL;
	root_element = NULL;
	if (argc != 2)
		return(1);
	doc = xmlReadFile(argv[1], NULL, 0);
	if (doc == NULL)
		printf("error: could not parse file %s\n", argv[1]);
	conn = connect_db("host=127.0.0.1 user=postgres password=3954 dbname=diplom");
	root_element = xmlDocGetRootElement(doc);
	print_element_names(root_element, &head, conn);
	free(head);
	PQfinish(conn);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 0;
}