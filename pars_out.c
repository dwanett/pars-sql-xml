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

size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	size_t	len;
	size_t	n;
	int		i;
	int		b;

	len = 0;
	i = 0;
	b = 0;
	if (!dst || !src)
		return (0);
	n = (size_t)strlen(src);
	if (dst == NULL || dstsize == 0)
		return (n);
	if (dstsize != 0)
	{
		while (len < dstsize - 1)
		{
			if (src[b] == '\0')
				break ;
			dst[i++] = src[b++];
			len++;
		}
		dst[i] = '\0';
	}
	return (n);
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

void free_nodes(t_cim_model **nodes)
{
	t_cim_model *tmp;

	while (*nodes != NULL)
	{
		tmp = *nodes;
		*nodes = (*nodes)->next;
		if (tmp->Position == 1)
			free(tmp->value);
		if (tmp->uuid != NULL && *tmp->uuid != '\0')
		{
			free(tmp->uuid);
			tmp->uuid = NULL;
		}
		free(tmp);
		tmp = NULL;
	}
}

char *split_coordinates(char *full_coordinates, char type)
{
	int b;
	char *coordinates;

	b = 1;
	while (full_coordinates[b] != ',')
		b++;
	if (type == 'x')
	{
		coordinates = (char*)malloc(sizeof(char) * b);
		ft_strlcpy(coordinates, full_coordinates + 1, b);
	}
	if (type == 'y')
	{
		full_coordinates += b + 1;
		b = 0;
		while (full_coordinates[b + 1] != '\0')
			b++;
		coordinates = (char*)malloc(sizeof(char) * b + 1);
		ft_strlcpy(coordinates, full_coordinates, b + 1);
	}
	return (coordinates);
}

void create_xmldoc(PGresult *res, xmlNodePtr root_node, char *name_table, PGconn *conn)
{
	xmlNodePtr	node;
	xmlNodePtr	node_table;
	PGresult 	*cim_model_table;
	PGresult 	*cim_model_nodes;
	char *name_table_in_cim;
	char *name_column_in_cim;
	t_cim_model *nodes;
	t_cim_model *tmp;
	t_cim_model *tmp2;
	char *tmp_class;
	int size_stolb;
	int size_str;
	int i;
	int j;
	int n;
	int m;
	uuid_t uid;
	char *root_uuid;
	char *save_uuid;

	node = NULL;
	node_table = NULL;
	nodes = NULL;
	save_uuid = NULL;
	size_stolb = PQnfields(res);
	size_str = PQntuples(res);
	cim_model_table = PQexecParams(conn, "SELECT path_to_object_in_cim FROM \"power_grid\".\"Сonformity_table\" WHERE \"name_table_in_bd\" = $1",
			1, 0, &name_table, 0, 0, 0);
	check_error(cim_model_table, conn, PGRES_TUPLES_OK);
	if (PQntuples(cim_model_table) != 0)
	{
		name_table_in_cim = PQgetvalue(cim_model_table, 0, 0);
		for (i = 0; i < size_str; i++)
		{
			root_uuid = PQgetvalue(res, i, 0);
			node_table = xmlNewChild(root_node, NULL, BAD_CAST name_table_in_cim, NULL);
			xmlNewProp(node_table, BAD_CAST "rdf:about",
					BAD_CAST root_uuid);
			for (j = 1; j < size_stolb; j++)
			{
				m = 0;
				tmp_class = NULL;
				tmp = NULL;
				name_column_in_cim = PQfname(res, j);
				cim_model_nodes = PQexecParams(conn, "SELECT attributes, class, resurce_or_value, link_source, const_value FROM \"power_grid\".\"Cim_model_nodes\" WHERE \"name_column\" = $1 ORDER BY \"class\";",
						1, 0, &name_column_in_cim, 0, 0, 0);
				check_error(cim_model_nodes, conn, PGRES_TUPLES_OK);
				if ((n = PQntuples(cim_model_nodes)) != 0)
				{
					while (m < n)
					{
						tmp = (t_cim_model*)malloc(sizeof(t_cim_model));
						tmp->class = PQgetvalue(cim_model_nodes, m, 1);
						tmp->attributes = PQgetvalue(cim_model_nodes, m, 0);
						tmp->resource = PQgetvalue(cim_model_nodes, m, 2);
						tmp->next = nodes;
						tmp->value = NULL;
						tmp->resource_class = NULL;
						tmp->Position = 0;
						if (*tmp->resource == '4' || *tmp->resource == '1')
							tmp->value = PQgetvalue(res, i, j);
						if (*tmp->resource == '2')
							tmp->value = PQgetvalue(cim_model_nodes, m, 4);
						if (strcmp(tmp->attributes, "cim:PositionPoint.xPosition") == 0)
						{
							tmp->value = split_coordinates(tmp->value, 'x');
							tmp->Position = 1;
						}
						if (strcmp(tmp->attributes, "cim:PositionPoint.yPosition") == 0)
						{
							tmp->value = split_coordinates(tmp->value, 'y');
							tmp->Position = 1;
						}
						if (*tmp->resource == '6')
							tmp->resource_class = PQgetvalue(cim_model_nodes, m, 4);
						if (*tmp->class == '\0')
							tmp->node_class = node_table;
						else if (tmp_class != NULL && strcmp(tmp_class, tmp->class) == 0)
							tmp->node_class = node;
						else
						{
							uuid_generate(uid);
							tmp->uuid = (char*)malloc(sizeof(char) * 37);
							uuid_unparse(uid, tmp->uuid);
							node = xmlNewChild(root_node, NULL, BAD_CAST tmp->class, NULL);
							xmlNewProp(node, BAD_CAST "rdf:about",
									BAD_CAST tmp->uuid);
							tmp->node_class = node;
							tmp_class = tmp->class;
						}
						if (*tmp->resource == '0' || *tmp->resource == '8')
							tmp->resource_class = PQgetvalue(cim_model_nodes, m, 3);
						nodes = tmp;
						m++;
					}
					tmp = nodes;
					while (nodes != NULL)
					{
						if (*nodes->resource == '4' || *nodes->resource == '7' || *nodes->resource == '5' || *nodes->resource == '8')
						{
							tmp2 = nodes;
							tmp_class = nodes->class;
							save_uuid = nodes->uuid;
							nodes = tmp;
							while (nodes != NULL)
							{
								if (nodes->resource_class != NULL && strcmp(tmp_class, nodes->resource_class) == 0)
									nodes->resource_class = save_uuid;
								nodes = nodes->next;
							}
							nodes = tmp2;
						}
						if (*nodes->resource == '3' || *nodes->resource == '5')
							nodes->resource_class = root_uuid;
						nodes = nodes->next;
					}
					nodes = tmp;
					tmp = nodes;
					while (nodes != NULL)
					{
						if (*nodes->resource != '7')
							node = xmlNewChild(nodes->node_class, NULL, BAD_CAST nodes->attributes,  nodes->value);
						if (*nodes->resource == '0' || *nodes->resource == '3' || *nodes->resource == '6' || *nodes->resource == '5' || *nodes->resource == '8')
							xmlNewProp(node, BAD_CAST "rdf:resource",
								BAD_CAST nodes->resource_class);
						nodes = nodes->next;
					}
					nodes = tmp;
					free_nodes(&nodes);
				}
				PQclear(cim_model_nodes);
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