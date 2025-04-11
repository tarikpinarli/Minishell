/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 17:39:45 by tpinarli          #+#    #+#             */
/*   Updated: 2024/11/14 12:12:02 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*sp1;
	const unsigned char	*sp2;
	int					i;
	int					nb;

	sp1 = (const unsigned char *)s1;
	sp2 = (const unsigned char *)s2;
	i = 0;
	nb = (int)n;
	while (i != nb)
	{
		if (sp1[i] != sp2[i])
			return (sp1[i] - sp2[i]);
		i++;
	}
	return (0);
}
