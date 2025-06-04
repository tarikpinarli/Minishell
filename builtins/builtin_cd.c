/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:22 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 15:00:43 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// builin_cd_2.c
char	*get_env_value(char **env, char *key)
{
	int	key_len;
	int	i;

	i = 0;
	key_len = ft_strlen(key);
	while (env[i])
	{
		if (ft_strncmp(env[i], key, key_len) == 0 && env[i][key_len] == '=')
			return (env[i] + key_len + 1);
		i++;
	}
	return (NULL);
}

char	*make_env_entry(const char *key, const char *value)
{
	size_t	key_len;
	size_t	total_len;
	char	*entry;

	key_len = ft_strlen(key);
	total_len = key_len + 1 + ft_strlen(value) + 1;
	entry = malloc(total_len); // protected!
	if (!entry)
		return (NULL);
	ft_strlcpy(entry, key, key_len + 1);
	ft_strlcat(entry, "=", key_len + 2);
	ft_strlcat(entry, value, total_len);
	return (entry);
}

int	append_env(char ***env, char *new_entry)
{
	int		i;
	char	**new_env;

	i = 0;
	while ((*env)[i])
		i++;
	new_env = malloc(sizeof(char *) * (i + 2));
	if (!new_env)
	{
		free(new_entry);
		return (0);
	}
	i = 0;
	while ((*env)[i])
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[i] = new_entry;
	new_env[i + 1] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}

int	update_env_var(char ***env, const char *key, const char *value)
{
	int		i;
	size_t	key_len;
	char	*new_entry;

	new_entry = make_env_entry(key, value);
	if (!new_entry)
		return(0);
	key_len = ft_strlen(key);
	i = 0;
	while ((*env)[i])
	{
		if (!ft_strncmp((*env)[i], key, key_len) &&
			(*env)[i][key_len] == '=')
		{
			free((*env)[i]);
			(*env)[i] = new_entry;
			return (1);
		}
		i++;
	}
	if (!append_env(env, new_entry))
		return (0);
	return (1);
}

// builtin_cd.c
static char	*cd_get_target(char **argv, char **env)
{
	char	*home;
	char	*oldpwd;

	if (!argv[1] || !ft_strcmp(argv[1], "~"))
	{
		home = get_env_value(env, "HOME");
		if (!home)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (NULL);
		}
		return (home);
	}
	if (!ft_strcmp(argv[1], "-"))
	{
		oldpwd = get_env_value(env, "OLDPWD");
		if (!oldpwd)
		{
			ft_putendl_fd("minishell: cd: OLDPWD not set", 2);
			return (NULL);
		}
		ft_putendl_fd(oldpwd, 1);
		return (oldpwd);
	}
	return (argv[1]);
}

int	update_pwd_vars(char ***env, const char *oldpwd, const char *newpwd)
{
	if (!update_env_var(env, "OLDPWD", oldpwd))
		return (0);
	if (!update_env_var(env, "PWD", newpwd))
		return (0);
	return (1);
}

static int	cd_change_directory(char *target, char ***env)
{
	char	oldpwd[PATH_MAX];
	char	newpwd[PATH_MAX];

	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return (1);
	if (chdir(target) != 0)
	{
		perror("cd");
		return (1);
	}
	if (getcwd(newpwd, sizeof(newpwd)))
	{
		if (!update_pwd_vars(env, oldpwd, newpwd))
			return (-1);
	}
	return (0);
}

int	too_mant_argument_err(char *arg)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd(": too many arguments", 2);
	return (1);
}

int	builtin_cd(char **argv, char ***env)
{
	int		arg_count;
	char	*target;

	arg_count = 0;
	if (!argv || !argv[0])
		return (0);
	while (argv[arg_count])
		arg_count++;
	if (arg_count > 2)
		return (too_mant_argument_err(argv[0]));
	target = cd_get_target(argv, *env);
	if (!target)
		return (-1);
	if (cd_change_directory(target, env) == -1)
		return (-1);
	else
		return (0);
}
