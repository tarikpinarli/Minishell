/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 16:08:34 by tpinarli          #+#    #+#             */
/*   Updated: 2024/11/16 16:31:28 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int		i;
	char	*p;
	char	ch;

	ch = (char)c;
	i = 0;
	p = (char *)s;
	while (s[i] != '\0')
	{
		if (s[i] == ch)
		{
			return (&p[i]);
		}
		i++;
	}
	if (c == '\0')
		return (&p[i]);
	return (NULL);
}
