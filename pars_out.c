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
	PGresult 	*cim_model_table;
	PGresult 	*cim_model_column;
	PGresult 	*cim_model_node;
	char *name_table_in_cim;
	char *name_column_in_cim;
	char *name_node_in_cim;
	char *summ_str;
	const char* paramValues[2];
	int size_stolb;
	int size_str;
	int i;
	int j;
	int n;
	int m;
	uuid_t uid;
	char *uuid_str;

	node = NULL;
	node1 = NULL;
	m = 0;
	size_stolb = PQnfields(res);
	size_str = PQntuples(res);
	paramValues[0] = name_table;
	cim_model_table = PQexecParams(conn, "SELECT path_to_object_in_cim FROM \"power_grid\".\"Сonformity_table\" WHERE \"name_table_in_bd\" = $1",
			1, 0, paramValues, 0, 0, 0);
	check_error(cim_model_table, conn, PGRES_TUPLES_OK);
	if (PQntuples(cim_model_table) != 0)
	{
		name_table_in_cim = PQgetvalue(cim_model_table, 0, 0);
		for (i = 0; i < size_str; i++)
		{
			summ_str = ft_strjoin("cim:", name_table_in_cim, 1);
			node = xmlNewChild(root_node, NULL, BAD_CAST summ_str, NULL);
			free(summ_str);
			xmlNewProp(node, BAD_CAST "rdf:about",
					BAD_CAST PQgetvalue(res, i, 0));
			for (j = 1; j < size_stolb; j++)
			{
				paramValues[1] = PQfname(res, j);
				cim_model_column = PQexecParams(conn, "SELECT name_in_cim_model FROM \"power_grid\".\"Сonformity_column\" WHERE \"name_column\" = $2 AND \"name_table\" = $1",
						2, 0, paramValues, 0, 0, 0);
				check_error(cim_model_column, conn, PGRES_TUPLES_OK);
				if (PQntuples(cim_model_column) != 0)
				{
					name_column_in_cim = PQgetvalue(cim_model_column, 0, 0);
					cim_model_node = PQexecParams(conn, "SELECT attributes, class FROM \"Cim_model_nodes\" WHERE \"name_in_cim\" = $1",
							2, 0, (const char**)name_column_in_cim, 0, 0, 0);
					check_error(cim_model_node, conn, PGRES_TUPLES_OK);
					if (PQntuples(cim_model_node) != 0)
					{
						n = PQnfields(cim_model_node);
						while (m != n)
						{
							name_node_in_cim = PQgetvalue(cim_model_node, m, 0);

							m++;
						}
					}
					else
					{
						summ_str = ft_strjoin("cim:", name_column_in_cim, 1);
						node1 = xmlNewChild(node, NULL, BAD_CAST summ_str,
								BAD_CAST PQgetvalue(res, i, j));
						free(summ_str);
					}
				}
				else
				{
					//--------------------Временно---------------------------
					node1 = xmlNewChild(node, NULL, BAD_CAST paramValues[1],
							BAD_CAST PQgetvalue(res, i, j));
					xmlNewProp(node1, BAD_CAST "ВНИМАНИЕ",
							BAD_CAST "нет в соответствия");
					//--------------------Временно---------------------------
				}
				PQclear(cim_model_column);
			}
		}
	}
	PQclear(cim_model_table);
}

void help_from_header_cim_model(xmlNodePtr root_node, char *name, char *content, char *prop, char *conten_prop)
{
	xmlNodePtr tmp = NULL;
	tmp = xmlNewChild(root_node, NULL, BAD_CAST name, (xmlChar*)content);
	if (prop != NULL && conten_prop != NULL)
		xmlNewProp(tmp, BAD_CAST prop, BAD_CAST conten_prop);
}

void form_header_cim_model(xmlDocPtr *doc, xmlNodePtr *root_node)
{
	xmlNodePtr node = NULL;
	char date[12];
	time_t timer;

	timer = time(NULL);
	strftime(date, 12, "%Y-%m-%d", localtime(&timer));
	*doc = xmlNewDoc(BAD_CAST "1.0");
	node = xmlNewDocPI(*doc, (xmlChar*)"iec61970-552", (xmlChar*)"version=\"2.0\"");
	xmlDocSetRootElement(*doc, node);
	*root_node = xmlNewNode(NULL, BAD_CAST "rdf:RDF");
	xmlDocSetRootElement(*doc, *root_node);
	xmlNewProp(*root_node, BAD_CAST "xmlns:rdf", BAD_CAST "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
	xmlNewProp(*root_node, BAD_CAST "xmlns:cim", BAD_CAST "http://www.iec.ch/TC57/2004/CIM-schema-cim10#");
	xmlNewProp(*root_node, BAD_CAST "xmlns:md", BAD_CAST "http://www.iec.ch/TC57/61970-552/ModelDescription/1#");
	xmlNewProp(*root_node, BAD_CAST "xml:base", BAD_CAST "urn:uuid:");
	*root_node = xmlNewChild(*root_node, NULL, BAD_CAST "md:FullModel", NULL); //может быть изменено
	xmlNewProp(*root_node, BAD_CAST "rdf:about", BAD_CAST "model-uri"); //дописать
	help_from_header_cim_model(*root_node, "md:Model.created", "2008-12-24", NULL, NULL);
	help_from_header_cim_model(*root_node, "md:Model.scenarioTime", date, NULL, NULL);
	help_from_header_cim_model(*root_node, "md:Model.Supersedes", NULL, "rdf:resource", "urn:uuid:*-*-*-*-*");
	help_from_header_cim_model(*root_node, "md:Model.DependentOn", NULL, "rdf:resource", "urn:uuid:*-*-*-*-*");
	help_from_header_cim_model(*root_node, "md:Model.version", "32", NULL, NULL);
	help_from_header_cim_model(*root_node, "md:Model.modelingAuthoritySet", "http://", NULL, NULL);
	help_from_header_cim_model(*root_node, "md:Model.description", "description", NULL, NULL);
	help_from_header_cim_model(*root_node, "md:Model.profile", "http://", NULL, NULL); //дописать, может быть не один
}

int main(int argc, char **argv)
{
	PGconn *conn;
	PGresult *res;
	PGresult *table_name;
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL;
	int count_str;
	int str;
	char *name_table;
	char *zapros;

	str = 0;
	conn = connect_db("host=127.0.0.1 user=postgres password=3954 dbname=diplom");
	form_header_cim_model(&doc, &root_node);
	table_name = PQexec(conn, "SELECT table_name FROM information_schema.tables where table_schema='power_grid'");
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