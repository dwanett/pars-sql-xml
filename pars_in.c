#include "header.h"

void	save_table_name(t_list **list, xmlNode *cur_node)
{
	xmlChar *flag;
	t_list *new_table;
	t_list *head;

	flag = xmlGetProp(cur_node, (const xmlChar *)"object");
	if (flag != NULL)
	{
		new_table = (t_list*)malloc(sizeof(t_list));
		new_table->table = ft_strjoin((char *) cur_node->name, "", 1);
		new_table->insert = NULL;
		new_table->next = NULL;
		if (*list == NULL)
			*list = new_table;
		else
		{
			head = *list;
			while ((*list)->next != NULL)
				*list = (*list)->next;
			(*list)->next = new_table;
			*list = head;
		}
	}
	xmlFree(flag);
}

void save_insert(t_list **list, xmlNode *cur_node, char *content, xmlChar *type)
{
	t_list_ins *new_insert;
	t_list_ins *head_insert;

	if ((*list)->insert == NULL || (*list)->end_insert == 1)
	{
		new_insert = (t_list_ins*)malloc(sizeof(t_list_ins));
		new_insert->insert_str = ft_strjoin("INSERT INTO \"power_grid\".\"", (*list)->table, 1);
		new_insert->insert_str = ft_strjoin(new_insert->insert_str, "\" (\"", 0);
		new_insert->insert_str = ft_strjoin(new_insert->insert_str, (char *)cur_node->name, 0);
		new_insert->insert_str = ft_strjoin(new_insert->insert_str, "\" ", 0);
		new_insert->values = ft_strjoin("VALUES (", content, 1);
		new_insert->next = NULL;
		if ((*list)->insert != NULL)
		{
			head_insert = (*list)->insert;
			while ((*list)->insert->next != NULL)
				(*list)->insert = (*list)->insert->next;
			(*list)->insert->next = new_insert;
			(*list)->insert = head_insert;
		}
		else
			(*list)->insert = new_insert;
		(*list)->end_insert = 0;
	}
	else
	{
		head_insert = (*list)->insert;
		while ((*list)->insert->next != NULL)
			(*list)->insert = (*list)->insert->next;
		(*list)->insert->insert_str = ft_strjoin((*list)->insert->insert_str, ", \"", 0);
		(*list)->insert->insert_str = ft_strjoin((*list)->insert->insert_str, (char *)cur_node->name, 0);
		(*list)->insert->insert_str = ft_strjoin((*list)->insert->insert_str, "\" ", 0);
		(*list)->insert->values = ft_strjoin((*list)->insert->values , ", ", 0);
		if (*content == '\0')
			(*list)->insert->values = ft_strjoin((*list)->insert->values , "0", 0);
		else
		{
			if (!strcmp((char *) type, "point"))
			{
				(*list)->insert->values = ft_strjoin(
						(*list)->insert->values, "'", 0);
				(*list)->insert->values = ft_strjoin(
						(*list)->insert->values, content, 0);
				(*list)->insert->values = ft_strjoin(
						(*list)->insert->values, "'", 0);
			}
			else
				(*list)->insert->values = ft_strjoin(
						(*list)->insert->values, content, 0);
		}
		(*list)->insert = head_insert;
	}
	if (cur_node->next->next == NULL)
	{
		head_insert = (*list)->insert;
		while ((*list)->insert->next != NULL)
			(*list)->insert = (*list)->insert->next;
		(*list)->insert->insert_str = ft_strjoin((*list)->insert->insert_str, ")", 0);
		(*list)->insert->values = ft_strjoin((*list)->insert->values , ");", 0);
		(*list)->end_insert = 1;
		(*list)->insert = head_insert;
	}
}

void	create_list_insert(xmlNode *a_node, t_list **list)
{
	xmlNode *cur_node;
	xmlChar *type;
	t_list *head;
	char *content;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		save_table_name(list, cur_node);
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			type = xmlGetProp(cur_node, (const xmlChar *)"type");
			content = (char *)xmlNodeGetContent(cur_node);
			if (content != NULL && *content != '\n')
			{
				head = *list;
				while ((*list)->next != NULL)
					*list = (*list)->next;
				save_insert(list, cur_node, content, type);
				*list = head;
			}
			xmlFree(type);
			free(content);
		}
		create_list_insert(cur_node->children, list);
	}
}

void send_insert(t_list **list, PGconn *conn)
{
	t_list *tmp;
	t_list_ins *tmp_ins;
	char *full_insert;
	PGresult *res;

	tmp = (*list);
	while ((*list)->next != NULL)
	{
		tmp_ins = (*list)->insert;
		while ((*list)->insert->next != NULL)
		{
			full_insert = ft_strjoin((*list)->insert->insert_str, (*list)->insert->values, 1);
			res = PQexec(conn, full_insert);
			check_error(res, conn);
			free(full_insert);
			(*list)->insert = (*list)->insert->next;
		}
		(*list)->insert = tmp_ins;
		*list = (*list)->next;
	}
	*list = tmp;
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

int	main(int argc, char **argv)
{
	PGconn *conn;
	xmlDoc *doc;
	xmlNode *root_element;
	t_list *list;

	doc = NULL;
	root_element = NULL;
	list = NULL;
	if (argc != 2)
	{
		fprintf(stderr,"Error: Enter file name\n");
		return (0);
	}
	doc = xmlReadFile(argv[1], NULL, 0);
	if (doc == NULL)
	{
		fprintf(stderr, "Error: could not parse file %s\n", argv[1]);
		return (0);
	}
	conn = connect_db("host=127.0.0.1 user=postgres password=3954 dbname=diplom");
	root_element = xmlDocGetRootElement(doc);
	create_list_insert(root_element, &list);
	send_insert(&list, conn);
	free_list(&list);
	PQfinish(conn);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return (0);
}