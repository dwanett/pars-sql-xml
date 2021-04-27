#include "header.h"

char	*ft_malc(int n, int i)
{
	char	*result;

	result = (char *)malloc(sizeof(char) * (i + 1));
	if (result == NULL)
		return (NULL);
	if (n < 0)
		*result = '-';
	else
		*result = '+';
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
	while (i >= 0)
	{
		result[i--] = 48 + ((n % 10) * minus);
		n /= 10;
		if (result[i] == '-')
			break ;
	}
	return (result);
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

int main(int argc, char **argv)
{
	PGconn *conn;
	PGresult *res;
	PGresult *table_name;
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	int count_str;
	int str;
	char *name_table;
	char *zapros;

	str = 0;
	conn = connect_db("host=127.0.0.1 user=postgres password=3954 dbname=diplom");
	doc = xmlNewDoc(BAD_CAST "1.0");
	node = xmlNewDocPI(doc, (xmlChar*)"iec61970-552", (xmlChar*)"version=\"2.0\"");
	xmlDocSetRootElement(doc, node);
	root_node = xmlNewNode(NULL, BAD_CAST "rdf:RDF");
	xmlDocSetRootElement(doc, root_node);
	xmlNewProp(root_node, BAD_CAST "xmlns:rdf", BAD_CAST "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
	xmlNewProp(root_node, BAD_CAST "xmlns:cim", BAD_CAST "http://www.iec.ch/TC57/2004/CIM-schema-cim10#");
	root_node = xmlNewChild(root_node, NULL, BAD_CAST "Data_base", NULL);
	table_name = PQexec(conn, "SELECT table_name FROM information_schema.tables  where table_schema='power_grid'");
	check_error(table_name, conn, PGRES_TUPLES_OK);
	count_str = PQntuples(table_name);
	while (str < count_str)
	{
		name_table = PQgetvalue(table_name, str, 0);
		zapros = ft_strjoin("SELECT * FROM \"power_grid\".\"", name_table, 1);
		if (zapros == NULL)
			return (-1);
		zapros = ft_strjoin(zapros, "\"", 0);
		if (zapros == NULL)
			return (-1);
		res = PQexec(conn, zapros);
		check_error(res, conn, PGRES_TUPLES_OK);
		create_xmldoc(res, root_node, name_table, conn);
		PQclear(res);
		free(zapros);
		str++;
	}
	if (argc == 2)
		xmlSaveFormatFileEnc(argv[1], doc, "UTF-8", 1);
	else
		xmlSaveFormatFileEnc("result.xml", doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	PQclear(table_name);
	PQfinish(conn);
	return 0;
}