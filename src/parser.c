/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcaratti <vcaratti@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 12:49:09 by vcaratti          #+#    #+#             */
/*   Updated: 2025/06/11 12:21:40 by vcaratti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

static int	treat_rest(t_cub_data *cub_data, t_node *list)
{
	int	ret;

	while (list)
	{
		ret = process_identify(list->line);
		if (ret == 3)
			list = list->next;
		else if (ret == 0)
			free_node(list_pop(&list));
		else if (ret == 1)
		{
			if (process_texture(list->line, cub_data))
				return (1);
			free_node(list_pop(&list));
		}
		else if (ret == 2)
		{
			if (process_rgb(list->line, cub_data))
				return (1);
			free_node(list_pop(&list));
		}
	}
	return (0);
}

static char	ft_dtoc(int digit)
{
	if (digit < 0)
		digit = -digit;
	return ((char)(digit + 48));
}

static int	map_int_to_char(t_cub_data *cub_data)
{
	int	i;
	int	j;

	cub_data->map_c = malloc(sizeof(char *) * (cub_data->height + 1));
	if (!cub_data->map_c)
		return (1);
	i = 0;
	while (i < cub_data->height)
	{
		cub_data->map_c[i] = malloc(sizeof(char) * (cub_data->width + 1));
		if (!cub_data->map_c[i])
			return (free_until(cub_data->map_c, i), 1);
		j = 0;
		while (j < cub_data->width)
		{
			cub_data->map_c[i][j] = ft_dtoc(cub_data->map_i[i][j]);
			j++;
		}
		cub_data->map_c[i][j] = '\0';
		i++;
	}
	cub_data->map_c[i] = 0;
	return (0);
}

static int	parsing_routine(char *path, t_cub_data *cub_data, t_node *list_head)
{
	init_data(cub_data, &(cub_data->map_head), list_head);
	if (path_to_list(cub_data, path, list_head))
		return (1);
	remove_next_lines(list_head->next);
	if (extract_map(cub_data, list_head->next, &(cub_data->map_head)))
		return (1);
	if (convert_map(cub_data))
		return (1);
	if (extract_p_position(cub_data))
		return (set_error(cub_data, 3), 1);
	if (verify_map(cub_data))
		return (set_error(cub_data, 2), 1);
	if (treat_rest(cub_data, list_head->next))
		return (1);
	if (map_int_to_char(cub_data))
		return (set_error(cub_data, 1), 1);
	free_map_i(cub_data);
	if (list_head->next)
		return (set_error(cub_data, 4), 1);
	if (!cub_data->so_texture || !cub_data->no_texture
		|| !cub_data->ea_texture || !cub_data->we_texture)
		return (set_error(cub_data, 4), 1);
	return (0);
}

int	parsing(t_cub_data *cub_data, char *path)
{
	t_node	list_head;

	if (parsing_routine(path, cub_data, &list_head))
	{
		if (cub_data->error == 1)
			write(2, "Error: Allocation error during parsing\n", 39);
		else if (cub_data->error == 2)
			write(2, "Error: Invalid map\n", 19);
		else if (cub_data->error == 3)
			write(2, "Error: Invalid player position\n", 31);
		else if (cub_data->error == 4)
			write(2, "Error: Invalid input file\n", 27);
		else if (cub_data->error == 5)
			write(2, "Error: No map\n", 14);
		free_all(&list_head, cub_data);
	}
	free_list(&list_head);
	list_head.next = 0;
	return (cub_data->error);
}
