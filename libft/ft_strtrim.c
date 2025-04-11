/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/03 13:53:56 by tpinarli          #+#    #+#             */
/*   Updated: 2024/11/14 12:14:19 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	match(char c, char const *set)
{
	size_t	i;

	i = 0;
	while (i < ft_strlen(set))
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	int		i;
	int		x;
	int		max;
	char	*res;
	int		total;

	if (!s1)
		return (NULL);
	i = 0;
	x = 0;
	max = ft_strlen(s1);
	while (match(s1[i], set) == 1)
		i++;
	while (match(s1[max - 1], set) == 1)
		max--;
	total = max - i + 1;
	if (total <= 0)
		total = 1;
	res = (char *)malloc((total) * sizeof(char));
	if (!res)
		return (NULL);
	while (i < max)
		res[x++] = s1[i++];
	res[x] = '\0';
	return (res);
}
