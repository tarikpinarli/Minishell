/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:22 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 12:42:09 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*cd_get_target(char **argv, char **env)
{
	if (!argv[1] || !ft_strcmp(argv[1], "~"))
	{
		char *home = get_env_value(env, "HOME");
		if (!home)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (NULL);
		}
		return (home);
	}
	if (!ft_strcmp(argv[1], "-"))
	{
		char *oldpwd = get_env_value(env, "OLDPWD");
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

void	update_pwd_vars(char ***env, const char *oldpwd, const char *newpwd)
{
	update_env_var(env, "OLDPWD", oldpwd);
	update_env_var(env, "PWD", newpwd);
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
		update_pwd_vars(env, oldpwd, newpwd);
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
		return (1);
	return (cd_change_directory(target, env));
}

