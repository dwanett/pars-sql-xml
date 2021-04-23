#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <postgresql/libpq-fe.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

char	*ft_malc(int n, int i)
{
	char	*result;

	result = (char *)malloc(sizeof(char) * (i + 1));
	if (result == NULL)
		return (NULL);
	if (n < 0)
		*result = '-';
	result[i] = '\0';
	return (result);
}

int		ft_chek(int tmp)
{
	int		i;

	i = 0;
	while (1)
	{
		tmp /= 10;
		i++;
		if (tmp == 0)
			break ;
	}
	return (i);
}

char	*ft_itoa(int n)
{
	int		tmp;
	int		i;
	int		minus;
	char	*result;

	tmp = n;
	minus = 1;
	i = ft_chek(tmp);
	if (n < 0)
	{
		minus = -1;
		i++;
	}
	result = ft_malc(n, i);
	if (result == NULL)
		return (NULL);
	i -= 1;
	while (i >= 0 && result[i] != '-')
	{
		result[i--] = 48 + ((n % 10) * minus);
		n /= 10;
	}
	return (result);
}

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

void create_xmldoc(PGresult *res, xmlNodePtr root_node, char *name_table, PGconn *conn)
{
	xmlNodePtr	node;
	xmlNodePtr	node1;
	xmlNodePtr	node2;
	PGresult 	*type;
	Oid			oid_type;
	int size_stolb;
	int size_str;
	int i;
	int j;
	const char* paramValues[1];

	node = NULL;
	node1 = NULL;
	node2 = NULL;
	size_stolb = PQnfields(res);
	size_str = PQntuples(res);
	node = xmlNewChild(root_node, NULL, BAD_CAST name_table, NULL);
	xmlNewProp(node, BAD_CAST "object", BAD_CAST "table");
	for (i = 0; i < size_str ; i++)
	{
		node1 = xmlNewChild(node, NULL, BAD_CAST "row", NULL);
		for (j = 0; j < size_stolb; j++)
		{
			node2 = xmlNewChild(node1, NULL, BAD_CAST PQfname(res, j),
					BAD_CAST PQgetvalue(res, i, j));
			oid_type = PQftype(res, j);
			paramValues[0] = ft_itoa((int)oid_type);
			type = PQexecParams(conn, "SELECT typname FROM \"pg_catalog\".\"pg_type\" WHERE oid = $1::integer",
								1, 0, paramValues, 0, 0, 0);
			free((char *)paramValues[0]);
			xmlNewProp(node2, BAD_CAST "type", BAD_CAST PQgetvalue(type, 0, 0));
			PQclear(type);
		}
	}
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

int main(int argc, char **argv)
{
	PGconn *conn;
	PGresult *res;
	PGresult *table_name;
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL;
	int count_str;
	char *name_table;
	char *zapros;
	char *tmp;

	conn = connect_db("host=127.0.0.1 user=postgres password=3954 dbname=diplom");
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "Data_base");
	xmlDocSetRootElement(doc, root_node);
	table_name = PQexec(conn, "SELECT table_name FROM information_schema.tables  where table_schema='test'");
	check_error(table_name, conn);
	count_str = PQntuples(table_name);
	while (count_str-- > 0)
	{
		name_table = PQgetvalue(table_name, count_str, 0);
		tmp = ft_strjoin("SELECT * FROM \"test\".\"", name_table);
		if (tmp == NULL)
			return (-1);
		zapros = ft_strjoin(tmp, "\"");
		if (zapros == NULL)
			return (-1);
		free(tmp);
		res = PQexec(conn, zapros);
		check_error(res, conn);
		create_xmldoc(res, root_node, name_table, conn);
		PQclear(res);
		free(zapros);
	}
	xmlSaveFormatFileEnc("result.xml", doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	PQclear(table_name);
	PQfinish(conn);
	return 0;
}