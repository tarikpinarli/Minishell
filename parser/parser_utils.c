/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:40 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/12 13:54:56 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


int	ft_strcmp(const char *s1, const char *s2)
{
	int	i = 0;

	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}


char	*strip_quotes(const char *token)
{
	size_t	len;
	char	*stripped;

	if (!token)
		return (NULL);

	len = ft_strlen(token);

	// Tırnak yoksa veya çok kısa ise direkt kopyala
	if (len < 2 || (token[0] != '\'' && token[0] != '"') || token[len - 1] != token[0])
		return (ft_strdup(token)); // Libft’ten ft_strdup kullan

	// Aradaki kısmı al (baş ve sondaki tırnaklar hariç)
	stripped = malloc(len - 1); // len - 2 karakter + null
	if (!stripped)
		return (NULL);

	ft_strncpy(stripped, token + 1, len - 2);
	stripped[len - 2] = '\0';

	return (stripped);
}