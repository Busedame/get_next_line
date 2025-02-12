/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 14:23:07 by nholbroo          #+#    #+#             */
/*   Updated: 2025/02/12 20:10:11 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*The first three functions are handling the current, next and last lines.
The read_file-function is reading through the file descriptor, returning when
it has encountered a newline somewhere in the last BUFFER_SIZE bytes read.
The get_next_line-function is calling all functions, and keeping track
of when it should stop reading.*/

/*
If there was no newline in the buffer, this last_line-function copies the 
contents of the buffer into a new string, returning the whole string to 
be output.
*/
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

/*
Returns a string containing the bytes that
was read AFTER the current line, so they can get stored until the next time 
the function is called (remainder). 
If there is no newline found in readnow, this function will return
NULL - setting the remainder to NULL - meaning end of file.
@param start The position after '\n', so where the remainder starts.
@param bfrsize Used to calculate the size of the remainder.
@param nextline The remainder.
*/
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

/*
Returns a string containing the current line (the line to be returned from
get_next_line()), including the newline-character. Also handling the
last line if there is no newline-character in the string passed to 
the function.
Returns NULL upon error.
@param readnow The entire line that was read now, potentially including
both current line and parts of next line.
@param end The index of where the first newline character is found.
*/
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

/*
Reads through the file until it 
contains a newline, if the file has ended or an error occurs.
@param buffer A buffer that stores what was read by the read()-function.
@param readnow Initially stores the remainder from last get_next_line() call, 
and dynamically appends what is getting read in each read()-function call.
@param bytesread How many bytes were read in the read()-function.
*/
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

/*
	Reads through a file (fd), and returns one line at a time.
	Returns NULL upon error or EOF.
	@param output The current line (which should be returned).
	@param remainder The next line (which should be saved for next call).
	@param readnow The entire line that was read now, potentially including
	both current line and parts of next line.
	@param BUFFER_SIZE Defined in get_next_line.h
	@param fd File descriptor
*/
char	*get_next_line(int fd)
{
	static char	*remainder = NULL;
	char		*output;
	char		*readnow;

	if (fd < 0 || BUFFER_SIZE <= 0)
	{
		if (remainder)
			return (ft_free(remainder));
		return (NULL);
	}
	readnow = read_file(remainder, fd);
	if (!readnow)
		return (ft_free(remainder));
	if (readnow[0] == '\0')
	{
		remainder = NULL;
		return (ft_free(readnow));
	}
	output = current_line(readnow);
	remainder = next_line(readnow);
	ft_free(readnow);
	return (output);
}

/*
int	main(void)
{
	int		fd;
	char	*output;

	fd = open("test.txt", O_RDONLY);
	output = get_next_line(fd);
	while (output)
	{
		printf("%s", output);
		free(output);
		output = get_next_line(fd);
	}
	free(output);
}
*/