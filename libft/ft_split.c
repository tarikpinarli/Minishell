/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 15:36:22 by tpinarli          #+#    #+#             */
/*   Updated: 2024/11/14 12:49:21 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int		split_count(char const *s, char c);
static char		**sub_array_size(char const *s, char c, char **ar, int ct);
static char		**ch_alloc(char const *s, char c, char **array);
static void		free_array(char **array, int count);

static void	free_array(char **array, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (array[i])
			free(array[i]);
		i++;
	}
	free(array);
}

char	**ft_split(char const *s, char c)
{
	int		count;
	char	**array;

	if (!s)
		return (NULL);
	count = split_count(s, c);
	array = (char **)malloc((count + 1) * sizeof(char *));
	if (!array)
		return (NULL);
	array = sub_array_size(s, c, array, count);
	array = ch_alloc(s, c, array);
	return (array);
}

static char	**sub_array_size(char const *s, char c, char **ar, int ct)
{
	int	i;
	int	len;

	i = 0;
	while (i < ct)
	{
		while (*s == c && *s != '\0')
			s++;
		len = 0;
		while (*s != c && *s != '\0')
		{
			len++;
			s++;
		}
		ar[i] = (char *)malloc((len + 1) * sizeof(char));
		if (!ar[i])
		{
			free_array(ar, i);
			return (NULL);
		}
		i++;
	}
	ar[ct] = NULL;
	return (ar);
}

static char	**ch_alloc(char const *s, char c, char **array)
{
	int	i;
	int	k;
	int	j;

	i = 0;
	k = 0;
	j = 0;
	while (array[i] != NULL)
	{
		j = 0;
		while (s[k] == c && s[k] != '\0')
			k++;
		while (s[k] != '\0' && s[k] != c)
		{
			array[i][j] = s[k];
			j++;
			k++;
		}
		array[i][j] = '\0';
		i++;
	}
	return (array);
}

static int	split_count(char const *s, char c)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	if (s[0] == '\0')
		return (count);
	while (s[i] != '\0')
	{
		while (s[i] == c && s[i] != '\0')
			i++;
		if (s[i] != c && s[i] != '\0')
			count++;
		while (s[i] != c && s[i] != '\0')
			i++;
	}
	return (count);
}
