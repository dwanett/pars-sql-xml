#include "header.h"

int is_class_table(xmlNode *cur_node, PGresult *name_cim_class_table, t_info_table **info_class, PGconn *conn)
{
	int count_str;
	char *tmp_name;
	PGresult *name_in_bd_cur_node;
	t_info_table *tmp;
	int n;

	n = 0;
	count_str = PQntuples(name_cim_class_table);
	while (n < count_str)
	{
		tmp_name = PQgetvalue(name_cim_class_table, n, 0);
		if (strcmp(tmp_name + 4, (char *)cur_node->name) == 0)
		{
			tmp = (t_info_table*) malloc(sizeof(t_info_table));
			tmp->class_name = tmp_name;
			tmp->uuid = (char*)xmlGetProp(cur_node, (const xmlChar *)"about");
			tmp->value = NULL;
			tmp->namevalue = 0;
			if (strcmp((char*)cur_node->children->next->name, "Naming.name") == 0)
			{
				tmp->namevalue = 1;
				tmp->value = (t_value*) malloc(sizeof(t_value));
				tmp->value->name_value = (char*)cur_node->children->next->name;
				tmp->value->value = (char *) xmlNodeGetContent(
						cur_node->children->next);
				name_in_bd_cur_node = PQexec(conn, "SELECT name_column FROM \"power_grid\".\"Cim_model_nodes\" WHERE \"attributes\" = 'cim:Naming.name';");
				check_error(name_in_bd_cur_node, conn, PGRES_TUPLES_OK);
				tmp->value->name_value_in_bd = PQgetvalue(name_in_bd_cur_node, 0, 0);
				tmp->value->next = NULL;
				//PQclear(name_in_bd_cur_node);
			}
			tmp->next = *info_class;
			*info_class = tmp;
			return (1);
		}
		n++;
	}
	return (0);
}

void getting_table_name(PGconn *conn, t_info_table **info_class)
{
	PGresult *tabel_name_bd;
	PGresult *name_uuid_in_bd;
	const char *paramValues[2];
	char *tmp;
	char *TP = "Т";
	char *RP = "Р";

	paramValues[0] = (*info_class)->class_name;
	paramValues[1] = NULL;
	if ((*info_class)->value != NULL)
	{
		tmp = (*info_class)->value->value;
		if (tmp[0] == TP[0] && tmp[1] == TP[1])
			paramValues[1] = "Т%";
		if (tmp[0] == RP[0] && tmp[1] == RP[1])
			paramValues[1] = "Р%";
	}
	if (paramValues[1] == NULL)
		tabel_name_bd = PQexecParams(conn, "SELECT * FROM \"power_grid\".\"Сonformity_table\" WHERE \"path_to_object_in_cim\" = $1;",
				1, 0, paramValues, 0, 0, 0);
	else
		tabel_name_bd = PQexecParams(conn, "SELECT * FROM \"power_grid\".\"Сonformity_table\" WHERE \"path_to_object_in_cim\" = $1 AND \"type_name_in_cim\" LIKE $2;",
				2, 0, paramValues, 0, 0, 0);
	check_error(tabel_name_bd, conn, PGRES_TUPLES_OK);
	(*info_class)->table_name = PQgetvalue(tabel_name_bd, 0, 0);
	paramValues[0] = (*info_class)->table_name;
	name_uuid_in_bd = PQexecParams(conn, "SELECT column_name FROM information_schema.constraint_column_usage WHERE \"table_name\" = $1;",
			1, 0, paramValues, 0, 0, 0);
	(*info_class)->name_uuid_in_bd = PQgetvalue(name_uuid_in_bd, 0, 0);
	//PQclear(tabel_name_bd);
}

void help_serch_link_class_and_get_value(xmlNode *root_node, xmlNode *cur_node, char *search, xmlNode **discovered_node, char *uuid, char resurce, char *value)
{
	xmlNode *tmp_node;
	xmlNode *node_for_uuid;
	char *uuid_for_search_1;
	char *uuid_for_search_2;

	for (tmp_node = cur_node; tmp_node && *discovered_node == NULL; tmp_node = tmp_node->next)
	{
		if (tmp_node->type != XML_ELEMENT_NODE && tmp_node->next != NULL)
			cur_node = cur_node->next;
		if (uuid != NULL)
		{
			if ((resurce == '1' || resurce == '4' || resurce == '8' || resurce == '-' || resurce == '9') && tmp_node->type == XML_ELEMENT_NODE &&
				strcmp((char *) tmp_node->name, search) == 0 &&
				strcmp((char *) xmlGetProp(tmp_node->parent,
						(const xmlChar *) "about"), uuid) == 0)
				*discovered_node = tmp_node;
			if (resurce == '0' && tmp_node->type == XML_ELEMENT_NODE &&
				strcmp((char *) tmp_node->name, search) == 0 &&
				strcmp((char *) xmlGetProp(tmp_node,
						(const xmlChar *) "about"), uuid) == 0)
				*discovered_node = tmp_node;
			if ((resurce == '5' || resurce == '3') && tmp_node->type == XML_ELEMENT_NODE &&
				strcmp((char *) tmp_node->name, search) == 0 &&
				strcmp((char *) xmlGetProp(tmp_node,
						(const xmlChar *) "resource"), uuid) == 0)
				*discovered_node = tmp_node;
			if (resurce == '*' && tmp_node->type == XML_ELEMENT_NODE &&
				strcmp((char *) tmp_node->name, search) == 0 && strcmp((char *) tmp_node->parent->name, "FullModel") != 0 &&
				strcmp((char *) xmlGetProp(tmp_node,
						(const xmlChar *) "resource"), uuid) == 0)
				*discovered_node = tmp_node;
			if (value != NULL && resurce != '2')
			{
				if (tmp_node->type == XML_ELEMENT_NODE && strcmp((char *) tmp_node->name, search) == 0 && strcmp((char *)xmlGetProp(tmp_node, (const xmlChar *)"resource"), uuid) == 0)
				{
					uuid_for_search_1 = (char *) xmlGetProp(
							tmp_node->parent,
							(const xmlChar *) "about");
					node_for_uuid = NULL;
					help_serch_link_class_and_get_value(root_node, root_node, "Naming.description", &node_for_uuid, uuid_for_search_1, '2', value);
					if (node_for_uuid != NULL)
						*discovered_node = tmp_node;
					else
						*discovered_node = NULL;

				}
			}
			if ((resurce == '2') && tmp_node->type == XML_ELEMENT_NODE &&
				strcmp((char *) tmp_node->name, search) == 0 &&
				strcmp((char *) xmlGetProp(tmp_node->parent, (const xmlChar *) "about"), uuid) == 0 &&
				strcmp((char *) (char *) xmlNodeGetContent(tmp_node), value) == 0)
				*discovered_node = tmp_node;
		}
		else
			if (tmp_node->type == XML_ELEMENT_NODE && strcmp((char *) tmp_node->name, search) == 0)
				*discovered_node = tmp_node;
		help_serch_link_class_and_get_value(root_node, tmp_node->children, search, discovered_node, uuid, resurce, value);
	}
}

void serch_link_class_and_get_value(xmlNode *cur_node, PGconn *conn, t_info_table **info_class)
{
	PGresult *namespace_cur_table;
	PGresult *info_class_for_table_column;
	t_value *tmp;
	xmlNode *discovered_node;
	int count_namespace_cur_table;
	int count_class_for_table_column;
	char *attributes_for_search;
	char *link_source_for_search;
	char *value_for_search;
	char *uuid_for_search;
	char *resurce;
	char *tmp_resurce;
	char *ferst_class;
	char *cur_class;
	char *cur_attr;
	const char *paramValues[1];
	int i;
	int n;
	int stop;
	int flag;

	i = 0;
	n = 0;
	stop = 0;
	namespace_cur_table = PQexecParams(conn, "SELECT column_name FROM information_schema.columns WHERE information_schema.columns.table_name= $1 AND \"column_name\" NOT LIKE 'uuid%' AND \"column_name\" != 'name'; ",
			1, 0, &((*info_class)->table_name), 0, 0, 0);
	check_error(namespace_cur_table, conn, PGRES_TUPLES_OK);
	count_namespace_cur_table = PQntuples(namespace_cur_table);
	if (count_namespace_cur_table != 0)
	{
		while (n != count_namespace_cur_table)
		{
			tmp = (t_value*) malloc(sizeof(t_value));
			tmp->name_value_in_bd  = PQgetvalue(namespace_cur_table, n, 0);
			tmp->next = (*info_class)->value;
			tmp->coord.x = NULL;
			tmp->coord.y = NULL;
			(*info_class)->value = tmp;
			n++;
		}
		tmp = (*info_class)->value;
		while ((*info_class)->value->next != NULL)
		{
			value_for_search = NULL;
			discovered_node = NULL;
			n = 0;
			i = 0;
			stop = 0;
			flag = 0;
			paramValues[0] = (*info_class)->value->name_value_in_bd;
			info_class_for_table_column = PQexecParams(conn,
					"SELECT attributes, class, resurce_or_value, link_source, \"name_column\", const_value FROM \"power_grid\".\"Cim_model_nodes\" WHERE \"name_column\" = $1 ORDER BY \"class\" DESC , \"resurce_or_value\";",
					1, 0, paramValues, 0, 0, 0);
			check_error(info_class_for_table_column, conn, PGRES_TUPLES_OK);
			count_class_for_table_column = PQntuples(
					info_class_for_table_column);
			resurce = PQgetvalue(info_class_for_table_column, n, 2);
			ferst_class = PQgetvalue(info_class_for_table_column, n, 1);
			while (i != count_class_for_table_column)
			{
				resurce = PQgetvalue(info_class_for_table_column, i, 2);
				if (*resurce == '3')
				{
					n = i;
					break;
				}
				i++;
			}
			resurce = PQgetvalue(info_class_for_table_column, n, 2);
			if (*resurce != '3')
				i = 0;
			if (*resurce == '1' || *resurce == '0' || *resurce == '3')
			{
				attributes_for_search = PQgetvalue(info_class_for_table_column,
						n, 0);
				help_serch_link_class_and_get_value(cur_node, cur_node,
						attributes_for_search + 4, &discovered_node,
						(*info_class)->uuid, '1', value_for_search);
				if (*resurce == '1')
				{
					(*info_class)->value->value = (char *) xmlNodeGetContent(
							discovered_node);
				}
				else
				{
					if (*resurce == '3')
						n = 0;
					while (1)
					{
						if (*resurce == '5' || *resurce == '3')
							uuid_for_search = (*info_class)->uuid;
						else if (*resurce == '*')
							uuid_for_search = (char *) xmlGetProp(
									discovered_node->parent,
									(const xmlChar *) "about");
						else if ((*resurce == '8' && flag == 1) || *resurce == '7')
						{
							uuid_for_search = (char *) xmlGetProp(
									discovered_node,
									(const xmlChar *) "about");
							flag = 0;
						}
						else if (*resurce != '1' && *tmp_resurce != '7')
						{
							uuid_for_search = (char *) xmlGetProp(
									discovered_node,
									(const xmlChar *) "resource");
						}
						even:
						link_source_for_search = PQgetvalue(info_class_for_table_column, i, 3);
						attributes_for_search = PQgetvalue(info_class_for_table_column, i, 0);
						if (strcmp((*info_class)->value->name_value_in_bd, "input") == 0 || strcmp((*info_class)->value->name_value_in_bd, "output") == 0)
							if (strcmp(attributes_for_search, "cim:Terminal.ConductingEquipment") == 0)
								value_for_search = (*info_class)->value->name_value_in_bd;
						discovered_node = NULL;
						if (*tmp_resurce == '7')
							*resurce = '3';
 						if (*link_source_for_search != '\0' && *resurce != '*' && *resurce != '3' && *resurce != '-' && *resurce != '8' && *tmp_resurce != '7')
							help_serch_link_class_and_get_value(cur_node, cur_node, link_source_for_search + 4, &discovered_node, uuid_for_search, *resurce, value_for_search);
						else
							help_serch_link_class_and_get_value(cur_node, cur_node,attributes_for_search + 4, &discovered_node, uuid_for_search, *resurce, value_for_search);
						stop = i;
						if (*resurce == '1' || *resurce == '4' || *resurce == '9')
						{
							if (strcmp(attributes_for_search,
									"cim:PositionPoint.yPosition") == 0)
							{
								(*info_class)->value->coord.y = (char *) xmlNodeGetContent(
										discovered_node);
								stop = i;
							}
							else if (strcmp(attributes_for_search,
									"cim:PositionPoint.xPosition") == 0)
							{
								(*info_class)->value->coord.x = (char *) xmlNodeGetContent(
										discovered_node);
								stop = i;
							}
							else if (*resurce != '9')
							{
								(*info_class)->value->value = (char *) xmlNodeGetContent(
										discovered_node);
								break;
							}
							else
							{
								(*info_class)->value->value = (char *) xmlGetProp(
										discovered_node,
										(const xmlChar *) "resource");
								break;
							}
							if ((*info_class)->value->coord.y != NULL &&
								(*info_class)->value->coord.x != NULL)
								break;
						}
						cur_class = PQgetvalue(info_class_for_table_column, i,1);
						if (*cur_class == '\0')
							i++;
						if (*resurce != '5' && *resurce != '*')
							i = n;
						else
						{
							link_source_for_search = cur_class;
							if (*resurce != '*')
								i = 0;
							else
								i = n;
						}
						if (*link_source_for_search == '\0' || *tmp_resurce == '7')
							link_source_for_search = cur_class;
						while (i != count_class_for_table_column)
						{
							if (stop != 0 && i == stop)
								i++;
							if (*resurce == '5')
								cur_class =  PQgetvalue(info_class_for_table_column, i, 3);
							else
								cur_class = PQgetvalue(info_class_for_table_column, i, 1);
							tmp_resurce = PQgetvalue(info_class_for_table_column, i, 2);
							if (strcmp(link_source_for_search, cur_class) == 0 )
								break ;
							i++;
						}
						if (*tmp_resurce == '7')
						{
							stop = i;
							i = 1;
							while (i != count_class_for_table_column)
							{
								if (i == stop && stop != count_class_for_table_column)
									i++;
								cur_class =  PQgetvalue(info_class_for_table_column, i, 3);
								if (strcmp(link_source_for_search, cur_class) == 0 )
									break ;
								i++;
							}
							cur_class = PQgetvalue(info_class_for_table_column, i, 1);
						}
						if (*resurce == '*' || *resurce == '3')
						{
							if (*resurce == '3')
								*resurce = '-';
							else
								resurce = PQgetvalue(info_class_for_table_column, i, 2);
							uuid_for_search = (char *) xmlGetProp(
									discovered_node->parent,
									(const xmlChar *) "about");
							goto even;
						}
						if (*resurce == '5' &&
							*PQgetvalue(info_class_for_table_column, i, 2) == '0')
							resurce = "*";
						else
							resurce = PQgetvalue(info_class_for_table_column, i, 2);
						n = 1;
					}
				}
			}
			if ((*info_class)->namevalue == 1 && (*info_class)->value->next->next == NULL)
				break ;
			(*info_class)->value = (*info_class)->value->next;
		}
		(*info_class)->value = tmp;
	}
}

void create_list_insert(xmlNode *a_node, t_list **list, PGconn *conn, PGresult *name_cim_class_table, t_info_table **info_class)
{
	xmlNode *cur_node;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		if (cur_node->type != XML_ELEMENT_NODE && cur_node->next != NULL)
			cur_node = cur_node->next;
		if (cur_node->type == XML_ELEMENT_NODE && is_class_table(cur_node, name_cim_class_table, info_class, conn))
		{
			getting_table_name(conn, info_class);
			serch_link_class_and_get_value(cur_node, conn, info_class);
		}
		create_list_insert(cur_node->children, list, conn, name_cim_class_table, info_class);
	}
}

void free_list(t_list **list)
{
	t_list *tmp;
	t_list_ins *tmp_ins;

	while (*list != NULL)
	{
		tmp = (*list)->next;
		free((*list)->table);
		while ((*list)->insert != NULL)
		{
			tmp_ins = (*list)->insert->next;
			free((*list)->insert->insert_str);
			free((*list)->insert->values);
			free((*list)->insert);
			(*list)->insert = tmp_ins;
		}
		free(*list);
		*list = tmp;
	}
}

void send_data(t_info_table *info_class, PGconn *conn)
{
	t_info_table *tmp;
	t_value *tmp_value;
	PGresult *send_insert;
	char *insert;
	char *insert_name_value;
	char *insert_value;

	tmp = info_class;
	tmp_value = info_class->value;
	while(info_class != NULL)
	{
		if (strcmp(info_class->table_name, "ACLineSegment") != 0)
		{
			insert_name_value = ft_strjoin("INSERT INTO power_grid.\"", info_class->table_name, 1);
			insert_name_value = ft_strjoin(insert_name_value, "\" (", 0);
			insert_value = ft_strjoin("VALUES (", "\0", 1);
			while (info_class->value != NULL)
			{
				insert_name_value = ft_strjoin(insert_name_value, info_class->value->name_value_in_bd, 0);
				insert_value = ft_strjoin(insert_value, "'", 0);
				if (strcmp(info_class->value->name_value_in_bd, "coordinates") == 0)
				{
					insert_value = ft_strjoin(insert_value, info_class->value->coord.x, 0);
					insert_value = ft_strjoin(insert_value, ",", 0);
					insert_value = ft_strjoin(insert_value, info_class->value->coord.y, 0);
				}
				else
				{
					insert_value = ft_strjoin(insert_value,
							info_class->value->value, 1);
				}
				insert_value = ft_strjoin(insert_value, "'", 0);
				info_class->value = info_class->value->next;
				if (info_class->value != NULL)
				{
					insert_value = ft_strjoin(insert_value, ",", 0);
					insert_name_value = ft_strjoin(insert_name_value, ",", 0);
				}
				else
				{
					insert_value = ft_strjoin(insert_value, ",", 0);
					insert_value = ft_strjoin(insert_value, "'", 0);
					insert_value = ft_strjoin(insert_value, info_class->uuid, 0);
					insert_value = ft_strjoin(insert_value, "');", 0);
					insert_name_value = ft_strjoin(insert_name_value, ", \"", 0);
					insert_name_value = ft_strjoin(insert_name_value, info_class->name_uuid_in_bd, 0);
					insert_name_value = ft_strjoin(insert_name_value, "\") ", 0);
				}
			}
			insert = ft_strjoin(insert_name_value, insert_value, 0);
			free(insert_value);
			send_insert = PQexec(conn, insert);
			check_error(send_insert, conn, PGRES_COMMAND_OK);
			PQclear(send_insert);
			free(insert);
			info_class->value = tmp_value;
		}
		info_class = info_class->next;
	}
	info_class = tmp;
	while(info_class != NULL)
	{
		if (strcmp(info_class->table_name, "ACLineSegment") == 0)
		{
			insert = ft_strjoin("CALL power_grid.insert_into_ACLineSegment", "(", 1);
			while (info_class->value != NULL)
			{
				insert = ft_strjoin(insert, info_class->value->name_value_in_bd, 0);
				insert = ft_strjoin(insert, " => '", 0);
				insert = ft_strjoin(insert, info_class->value->value, 0);
				insert = ft_strjoin(insert, "'", 0);
				info_class->value = info_class->value->next;
				if (info_class->value != NULL)
					insert = ft_strjoin(insert, ", ", 0);
				else
				{
					insert = ft_strjoin(insert, ", ", 0);
					insert = ft_strjoin(insert, info_class->name_uuid_in_bd, 0);
					insert = ft_strjoin(insert, " => '", 0);
					insert = ft_strjoin(insert, info_class->uuid, 0);
					insert = ft_strjoin(insert, "'", 0);
					insert = ft_strjoin(insert, ");", 0);
				}
			}
			info_class->value = tmp_value;
			send_insert = PQexec(conn, insert);
			check_error(send_insert, conn, PGRES_COMMAND_OK);
			PQclear(send_insert);
			free(insert);
		}
		info_class = info_class->next;
	}
	info_class = tmp;
}

int	main(int argc, char **argv)
{
	PGconn *conn;
	xmlDoc *doc;
	xmlNode *root_element;
	PGresult *name_cim_class_table;
	t_list *list;
	t_info_table *info_class;

	doc = NULL;
	root_element = NULL;
	list = NULL;
	info_class = NULL;
	if (argc != 2)
	{
		fprintf(stderr,"Error: Enter file name\n");
		return (0);
	}
	doc = xmlReadFile(argv[1], "UTF-8", 0);
	if (doc == NULL)
	{
		fprintf(stderr, "Error: could not parse file %s\n", argv[1]);
		return (0);
	}
	conn = connect_db("host=127.0.0.1 user=postgres password=3954 dbname=diplom");
	name_cim_class_table = PQexec(conn, "SELECT path_to_object_in_cim FROM \"power_grid\".\"Сonformity_table\" GROUP BY \"path_to_object_in_cim\";");
	check_error(name_cim_class_table, conn, PGRES_TUPLES_OK);
	root_element = xmlDocGetRootElement(doc);
	create_list_insert(root_element, &list, conn, name_cim_class_table, &info_class);
	send_data(info_class, conn);
	PQfinish(conn);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return (0);
}