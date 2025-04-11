/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 13:00:19 by tpinarli          #+#    #+#             */
/*   Updated: 2024/11/14 17:25:50 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*buffer;
	long	length;

	length = nmemb * size;
	if (nmemb != 0 && size != length / nmemb)
		return (NULL);
	if (nmemb < 1 || size < 1)
		length = 1;
	buffer = malloc(length);
	if (buffer != 0)
		ft_bzero(buffer, length);
	else
		return (NULL);
	return (buffer);
}
