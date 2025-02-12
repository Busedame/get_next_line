/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 14:23:07 by nholbroo          #+#    #+#             */
/*   Updated: 2025/02/12 18:10:40 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*The first three functions are handling the current, next and last lines.
The read_file-function is reading through the file descriptor, returning when
it has encountered a newline somewhere in the last BUFFER_SIZE bytes read.
The get_next_line-function is calling all functions, and keeping track
of when it should stop reading.*/

/*If there was no newline in the buffer, this last_line-function copies the 
contents of the buffer into a new string, returning the whole string to 
be output.*/

static char	*last_line(char *readnow)
{
	char	*lastline;
	int		i;

	i = 0;
	lastline = malloc(sizeof(char) * ft_strlen(readnow) + 1);
	if (!lastline)
		return (NULL);
	while (readnow[i] != '\0')
	{
		lastline[i] = readnow[i];
		i++;
	}
	lastline[i] = '\0';
	return (lastline);
}

/*The next_line-function is returning a string containing the bytes that
was read AFTER the current line, so I can store them until the next time 
the function is called. If there is no new-line, this function will return
NULL - setting the remainder to NULL - meaning we reached the end of the
file.*/

static char	*next_line(char *readnow)
{
	int		bfrsize;
	int		start;
	int		i;
	char	*nextline;

	if (ft_strchr_index(readnow, '\n') < 0)
		return (NULL);
	i = 0;
	start = ft_strchr_index(readnow, '\n') + 1;
	bfrsize = (int)ft_strlen(readnow) - ft_strchr_index(readnow, '\n');
	nextline = malloc(sizeof(char) * bfrsize + 1);
	if (!nextline)
		return (NULL);
	while (i < bfrsize)
	{
		nextline[i] = readnow[start];
		i++;
		start++;
	}
	nextline[i] = '\0';
	return (nextline);
}

/*Returning a string containing the line I want to
output, including the newline-character. Also handling the
last line if there is no newline-character in the string passed to 
the function.*/

static char	*current_line(char *readnow)
{
	int		end;
	int		i;
	char	*currline;

	i = 0;
	if (ft_strchr_index(readnow, '\n') < 0)
		return (last_line(readnow));
	end = ft_strchr_index(readnow, '\n');
	currline = malloc(sizeof(char) * end + 2);
	if (!currline)
		return (NULL);
	while (i <= end)
	{
		currline[i] = readnow[i];
		i++;
	}
	currline[i] = '\0';
	return (currline);
}

/*Reading through the file until it 
contains a newline, if the file has ended or an error occurs.*/

static char	*read_file(char *remainder, int fd)
{
	char	*buffer;
	int		bytesread;
	char	*readnow;

	readnow = remainder;
	buffer = malloc(sizeof(char) * BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	bytesread = 1;
	while (bytesread > 0)
	{
		bytesread = read(fd, buffer, BUFFER_SIZE);
		if (bytesread == -1)
			return (ft_free(buffer));
		buffer[bytesread] = '\0';
		readnow = ft_strjoin(readnow, buffer);
		if (!readnow)
			return (NULL);
		if (ft_strchr_index(readnow, '\n') > -1)
			break ;
	}
	free(buffer);
	return (readnow);
}

char	*get_next_line(int fd)
{
	static char	*remainder = NULL;
	char		*output;
	char		*readnow;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	readnow = read_file(remainder, fd);
	if (!readnow)
	{
		free(remainder);
		remainder = NULL;
		return (NULL);
	}
	if (readnow[0] == '\0')
	{
		free(readnow);
		readnow = NULL;
		remainder = NULL;
		return (NULL);
	}
	output = current_line(readnow);
	remainder = next_line(readnow);
	free(readnow);
	readnow = NULL;
	return (output);
}

// int	main(void)
// {
// 	int		fd;
// 	char	*output;
// 	int		i;

// 	i = 0;
// 	fd = open("test.txt", O_RDONLY);
// 	output = get_next_line(fd);
// 	while (output)
// 	{
// 		printf("%s", output);
// 		free(output);
// 		output = get_next_line(fd);
// 	}
// 	free(output);
// }
