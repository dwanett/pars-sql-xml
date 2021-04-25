#include "header.h"

void exit_nicely(PGconn *conn)
{
	PQfinish(conn);
	exit(1);
}

void check_error(PGresult *check, PGconn *conn, ExecStatusType def)
{
	if (PQresultStatus(check) != def)
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
		fprintf(stderr, "Connection to database failed");
		exit_nicely(conn);
	}
	return (conn);
}

char	*ft_strjoin(char *s1, char *s2, int con)
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
	if (con == 0)
		free(s1);
	return (str);
}