/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 14:23:12 by nholbroo          #+#    #+#             */
/*   Updated: 2024/01/08 13:50:04 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	*ft_free(void *ptr)
{
	free(ptr);
	ptr = NULL;
	return (NULL);
}

static char	*ft_strjoin_cat_part(char *s1, char *s2, char *result)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (s1[i] != '\0')
	{
		result[i] = s1[i];
		i++;
	}
	while (s2[j] != '\0')
	{
		result[i] = s2[j];
		j++;
		i++;
	}
	result[i] = '\0';
	ft_free(s1);
	return (result);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*result;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (!s1)
	{
		s1 = malloc(sizeof(char));
		if (!s1)
			return (ft_free(s2));
		s1[0] = 0;
	}
	while (s1[i] != '\0')
		i++;
	while (s2[j] != '\0')
		j++;
	result = (char *)malloc(i + j + 1);
	if (!result)
		return (ft_free(s2));
	result = ft_strjoin_cat_part(s1, s2, result);
	return (result);
}

int	ft_strchr_index(const char *s, int c)
{
	int	i;

	i = 0;
	if (!s || c > 127)
		return (-1);
	while (s[i] != '\0' && s[i] != c)
		i++;
	if (s[i] == '\0' && s[i] != c)
		return (-1);
	return (i);
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i] != '\0')
		i++;
	return (i);
}
