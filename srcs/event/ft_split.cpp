#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	ft_splitstrcpy(char **split, char const *s, int slen, int i)
{
	int	di;

	di = 0;
	while (di < slen)
	{
		(*split)[di] = s[i];
		di++;
		i++;
	}
	(*split)[di] = '\0';
	return (i);
}

static int	ft_countlen(char const *s, char c)
{
	int	i;
	int	size;	

	i = 0;
	size = 0;
	while (s[i] != c && s[i] != '\0')
	{
		size++;
		i++;
	}
	return (size);
}

static int	ft_countlines(char const *s, char c)
{
	size_t	i;
	size_t	lines;

	i = 0;
	lines = 0;
	while (i < strlen(s))
	{
		while (s[i] == c)
			i++;
		if (s[i] != '\0')
			lines++;
		while (s[i] != c && s[i] != '\0')
			i++;
	}
	return (lines);
}

static void	ft_createsplit(char const *s, char c, char **split)
{
	size_t	i;
	int		l;
	int		lines;

	i = 0;
	l = 0;
	lines = ft_countlines(s, c);
	while (i <= strlen(s) && l <= lines)
	{
		while (s[i] == c)
			i++;
		if (ft_countlen(&s[i], c) == 0)
			break ;
		split[l] = (char*)malloc(ft_countlen(&s[i], c) + 1);
		if (split[l] == NULL)
		{
			l++;
			while (l--)
				free(split[l]);
			return ;
		}
		i = ft_splitstrcpy(&split[l], s, ft_countlen(&s[i], c), i);
		i++;
		l++;
	}
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	int		lines;

	if (strlen(s) == 0 || (!s && c == 0))
	{
		split = (char **)malloc(sizeof(char *));
		split[0] = NULL ;
		return (split);
	}
	else
		lines = ft_countlines(s, c);
	split = (char**)malloc((sizeof(char *)) * (lines + 1));
	if (split == NULL)
		return (NULL);
	ft_createsplit(s, c, split);
	if (!split)
		return (NULL);
	split[lines] = NULL;
	return (split);
}