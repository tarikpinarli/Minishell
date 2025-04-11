/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 17:19:39 by tpinarli          #+#    #+#             */
/*   Updated: 2024/11/14 12:17:59 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	char	*destmov;
	char	*srcmov;
	size_t	i;

	if (!src && !dest)
		return (NULL);
	destmov = (char *)dest;
	srcmov = (char *)src;
	i = 0;
	if (destmov > srcmov)
		while (n--)
			destmov[n] = srcmov[n];
	else
	{
		while (n--)
		{
			destmov[i] = srcmov[i];
			i++;
		}
	}
	return (destmov);
}
