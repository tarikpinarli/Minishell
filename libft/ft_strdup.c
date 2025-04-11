/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 12:55:04 by tpinarli          #+#    #+#             */
/*   Updated: 2024/11/14 12:12:39 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *source)
{
	char	*mall;
	int		i;

	mall = malloc(sizeof(char) * (ft_strlen(source) + 1));
	if (!mall)
		return (NULL);
	i = 0;
	while (source[i])
	{
		mall[i] = source[i];
		i++;
	}
	mall[i] = 0;
	return (mall);
}
