/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   merge_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 12:07:44 by ykadosh           #+#    #+#             */
/*   Updated: 2025/04/29 17:10:43 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// NOTE: *ptr is called as such because it is not the same as *tokens;
// rather, it is a pointer to the token struct containing the first string
// we are about to join with other ones.
static char	*join_multiple_strings(t_token *ptr, uint32_t n_strs_to_join, size_t length)
{
	char	*result;
	size_t	i;
	size_t	temp_len;

	result = NULL;
	result = (char *)ft_calloc(1, length + 1);
	if (!result)
		return (NULL);
	length = ft_strlen(ptr->str); // I think the syntax (ptr[i]str) could work here as well!
	(void)ft_memmove(result, ptr->str, length);
	free(ptr->str); // I think the syntax (ptr[i].str) could work here as well!
	ptr->str = NULL;
	temp_len = 0;
	i = 1;
	while (i <= n_strs_to_join)
	{
		temp_len = ft_strlen(ptr[i].str);
		(void)ft_memmove(&result[length], ptr[i].str, temp_len);
		length += temp_len;
		free(ptr[i].str);
		ptr[i].str = NULL;
		i++;
	}

	// printf debugging starts here
	printf("result in join_multiple_strings():	<%s>\n\n", result);
	return (result);
}

// this function returns 0 upon failure.
// 1 upton success.
t_token	*merge_tokens(t_token *tokens)
{
	uint32_t	i;
	uint32_t	j;
	uint32_t	k;
	size_t		length;
	
	if (!tokens)
		return (NULL);


	length = 0;
	i = 0;
	k = 0;
	while (tokens[i].str)
	{
		j = i;
		printf("at the beginning of the while loop, j == <%u>	i == <%u>, k == <%u>\n", j, i, k);
		if (tokens[i + 1].str && tokens[i].line_id == tokens[i + 1].line_id)
		{
			while (tokens[j + 1].str && tokens[j].line_id == tokens[j + 1].line_id)
			{
				length += ft_strlen(tokens[j].str);
				j++;
			}
			tokens[k].str = join_multiple_strings(&tokens[i], j - i, length);
			printf("did we get here?\n\n\n");
			if (!tokens[k].str)
			{
				while (tokens[i].str)
					i++;
				while (i > 0)
				{
					i--;
					free(tokens[i].str);
					tokens[i].str = NULL;
				}
				free(tokens);
				return (NULL);
			}
			/*
			else
			{
				str_size = ft_strlen(expanded_tokens[i]) + 1;
				concatenated_string = (char *)ft_calloc(1, str_size);
				(void)ft_memmove(concatenated_string[k], expanded_tokens[i], str_size);
			}
			*/
		}
		else if (k != i) // have to make sure that this pointer is always NULL already.
		{
			printf("after the k!= i boolean:	 i == <%u>, k == <%u>\n", i, k);

			if (tokens[k].str)
			{
				free(tokens[k].str);
				tokens[k].str = NULL;
				printf("WARNING!!!	tokens[k].str is not NULL!\n\n");
			}
			tokens[k].str = ft_strdup(tokens[i].str);
//			tokens[k].str = (char *)ft_calloc(1, (ft_strlen(tokens[i].str) + 1));
			if (!tokens[k].str) // this is the exact same freeing block as you have a little bit earlier.
			{
				while (tokens[i].str)
					i++;
				while (i > 0)
				{
					i--;
					free(tokens[i].str);
					tokens[i].str = NULL;
				}
				free(tokens);
				return (NULL);
			}
			free(tokens[i].str);
			tokens[i].str = NULL;
		}

		printf("value of k is now:		<%u>\n", k);
		printf("just after receiving the result, tokens[k].str is now:	<%s>\n", tokens[k].str);

		i = j + 1;
		k++;
		/*
		printf("value of k is now:		<%u>\n", k);
		printf("at the end of the loop tokens[k].str is now:	<%s>\n\n", tokens[k].str);
		*/

	}
	// TODO:
	//	free all strings after the one we got to, and reassign them to NULL.
	//	This is very important! If you do this properly, we can then forget about
	//	freeing those later, perhaps?
	//	FIXME: I have to ask someone how does the freeing() work, if you have
	//	malloc'ed at first for an ARRAY of STRUCTS (containing different members).
	//	Do you have to free() the whole array, or only the strings you have allocated
	//	for within those structs?
	//	I bet we have to free everything, since the allocation was done on the heap,
	//	and all heap allocation has to be freed...
	//	Which means, we have to pass the "count" variable to the free_all() function!
	//
	//	But you can already here free all of the structs' string members.
	//	something like:
	printf("value of k just before freeing the extra strings:	<%d>\n", k);
	while (tokens[k].str)
	{
		free(tokens[k].str);
		tokens[k].str = NULL;
		k++;
	}

	return (tokens);
}

/*
 * example:
input is:
<                                   echo   "hello"there'how'are'    you today'"?" is there something" I can help with?"'             '>
token[0] =	<echo>			---- quote type:	<0>	---- line_id =	<0>

token[1] =	<hello>			---- quote type:	<2>	---- line_id =	<1>
token[2] =	<there>			---- quote type:	<0>	---- line_id =	<1>
token[3] =	<how>			---- quote type:	<1>	---- line_id =	<1>
token[4] =	<are>			---- quote type:	<0>	---- line_id =	<1>
token[5] =	<    you today>	---- quote type:	<1>	---- line_id =	<1>
token[6] =	<?>				---- quote type:	<2>	---- line_id =	<1>

token[7] =	<is>			---- quote type:	<0>	---- line_id =	<2>

token[8] =	<there>			---- quote type:	<0>	---- line_id =	<3>

token[9] =	<something>		---- quote type:	<0>	---- line_id =	<4>
token[10] =	< I can help with?>	---- quote type:	<2>	---- line_id =	<4>
token[11] =	<             >	---- quote type:	<1>	---- line_id =	<4>
*/



/*
 * older version:
// NOTE: *ptr is called as such because it is not the same as *tokens;
// rather, it is a pointer to the token struct containing the first string
// we are about to join with other ones.
static char	*join_multiple_strings(t_token **ptr, uint32_t n_strs_to_join, size_t length)
{
	char	*temp_string;
	char	*result;
	size_t	i;
	size_t	temp_len;

	temp_string = NULL;
	result = NULL;
	result = (char *)ft_calloc(1, length + 1);
	if (!result)
		return (NULL);
	i = 0;
	length = ft_strlen(ptr[i].str); // I think the syntax (ptr->str) could work here as well!
	temp_string = (char *)ft_calloc(1, length + 1);
	if (!temp_string)
		return (NULL);
	(void)ft_memmove(temp_string, ptr[i].str, length + 1);
	free(ptr[i].str); // I think the syntax (ptr->str) could work here as well!
	ptr[i].str = NULL;



	free (temp_string);
	temp_string = NULL;
	i++;
	while (i <= n_strs_to_join)
	{
		temp_len = ft_strlen(ptr[i].str);
		(void)ft_memmove(&result[length], ptr[i].str, temp_len);
		length += ft_strlen(ptr[i].str);
		free(ptr[i].str);
		ptr[i].str = NULL;
		i++;
	}
	return (result);
}
*/
/*
char	**cat_adjacent_strings(char **expanded_tokens, t_token *tokens,
			int line_id)
{
	char		**concatenated_string;
	uint32_t	i;
	uint32_t	j;
	uint32_t	k;
	size_t		str_size;
	
	concatenated_string = NULL;
	if (!tokens || !*expanded_tokens)
		return (NULL);
	// since line_id is 0 indexed, and we have to have a null pointer at the
	// tail end of our array of strings, we add 2 in the allocation call.
	//	*conctatenated_string = (char *)ft_calloc(line_id + 2, sizeof(char *));
	i = 0;
	k = 0;
	while (expanded_tokens[i])
	{
		j = i;
		// count how many strings are to be joined to the current one
		if (tokens[i + 1]->line_id && tokens[i]->line_id == tokens[i + 1]->line_id)
		{
			while (tokens[j + 1]->line_id && tokens[j]->line_id == tokens[j + 1]->line_id)
				j++;
			// start joining the strings together
			if (j > 0)
				concatenated_string[k] = join_multiple_strings(expanded_tokens[i], j - i);
			else
			{
				str_size = ft_strlen(expanded_tokens[i]) + 1;
				concatenated_string = (char *)ft_calloc(1, str_size);
				(void)ft_memmove(concatenated_string[k], expanded_tokens[i], str_size);
			}
		i += j + 1;
		k++;
	}

*/



