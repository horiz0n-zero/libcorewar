/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libcorewar_out_cor_file.h                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afeuerst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/13 09:19:44 by afeuerst          #+#    #+#             */
/*   Updated: 2020/02/16 10:31:56 by afeuerst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCOREWAR_OUT_COR_FILE_H
# define LIBCOREWAR_OUT_COR_FILE_H

# include "libcorewar.h"

struct										s_out_cor_ins
{
	const struct s_libcorewar_opcode_info	*info;
	int32_t									offset;
	int32_t									size;

	int32_t									params[MAX_ARGS_NUMBER];
	int8_t									types[MAX_ARGS_NUMBER];

	int32_t									label;
	int16_t									labels[MAX_ARGS_NUMBER];

};

struct										s_out_cor_file
{
	const struct s_libcorewar_cor_file		*file;
	int										fd;
	int										flags;
	const char								*label;
	int										ins_max_length;
	int										ins_max_size;
	struct s_out_cor_ins					ins[4096];
	int32_t									ins_count;
};

#ifndef true
# define true 1
# define false 0
#endif

# define MAX_OPVALUE_SIZE (1 + 1 + 4 + 4 + 4)

#endif
