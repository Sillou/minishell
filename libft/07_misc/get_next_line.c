/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 13:01:07 by dcresce           #+#    #+#             */
/*   Updated: 2026/02/10 13:07:58 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static char	*fill_line(int fd, char *buf, char *rest);
static char	*set_line(char *buf);

char	*gnl(int fd)
{
	char		*buf;
	char		*line;
	static char	*rest[1024];

	buf = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buf)
		return (NULL);
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (free(rest[fd]), free(buf), rest[fd] = NULL, buf = NULL, NULL);
	line = fill_line(fd, buf, rest[fd]);
	free(buf);
	if (!line || !*line)
		return (free(rest[fd]), rest[fd] = NULL, NULL);
	rest[fd] = set_line(line);
	return (line);
}

static char	*fill_line(int fd, char *buf, char *rest)
{
	ssize_t	read_res;
	char	*temp;

	read_res = 1;
	while (read_res > 0)
	{
		read_res = read(fd, (void *)buf, BUFFER_SIZE);
		if (read_res < 0)
			return (NULL);
		buf[read_res] = '\0';
		if (!read_res)
			break ;
		if (!rest)
			rest = ft_strdup("");
		temp = rest;
		rest = ft_strjoin(temp, buf);
		free(temp);
		temp = NULL;
		if (ft_strchr(buf, '\n'))
			break ;
	}
	return (rest);
}

static char	*set_line(char *line)
{
	int		i;
	char	*rest;

	i = 0;
	while (line[i] && (line[i] != '\n'))
		i++;
	if (line[i] == '\n')
	{
		i++;
		rest = NULL;
		rest = ft_substr(line, i, (ft_strlen(line) - i));
	}
	else
		rest = NULL;
	line[i] = '\0';
	return (rest);
}
