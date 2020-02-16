/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libcorewar_out_cor_file.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afeuerst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/13 09:11:42 by afeuerst          #+#    #+#             */
/*   Updated: 2020/02/16 11:29:39 by afeuerst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcorewar_out_cor_file.h"

static const struct s_libcorewar_opcode_info		g_opcodes_info[256] =
{
	[0x01] = {"live",  1, 10,   0, 0, 0x01, 0, {T_DIR}},
	[0x02] = {"ld",    2, 5,    1, 0, 0x02, 1, {T_DIR | T_IND, T_REG}},
	[0x03] = {"st",    2, 5,    1, 0, 0x03, 0, {T_REG, T_IND | T_REG}},
	[0x04] = {"add",   3, 10,   1, 0, 0x04, 1, {T_REG, T_REG, T_REG}},
	[0x05] = {"sub",   3, 10,   1, 0, 0x05, 1, {T_REG, T_REG, T_REG}},
	[0x06] = {"and",   3, 6,    1, 0, 0x06, 1, {T_REG | T_DIR | T_IND, T_REG | T_IND | T_DIR, T_REG}},
	[0x07] = {"or",    3, 6,    1, 0, 0x07, 1, {T_REG | T_IND | T_DIR, T_REG | T_IND | T_DIR, T_REG}},
	[0x08] = {"xor",   3, 6,    1, 0, 0x08, 1, {T_REG | T_IND | T_DIR, T_REG | T_IND | T_DIR, T_REG}},
	[0x09] = {"zjmp",  1, 20,   0, 1, 0x09, 0, {T_DIR}},
	[0x0a] = {"ldi",   3, 25,   1, 1, 0x0a, 0, {T_REG | T_DIR | T_IND, T_DIR | T_REG, T_REG}},
	[0x0b] = {"sti",   3, 25,   1, 1, 0x0b, 0, {T_REG, T_REG | T_DIR | T_IND, T_DIR | T_REG}},
	[0x0c] = {"fork",  1, 800,  0, 1, 0x0c, 0, {T_DIR}},
	[0x0d] = {"lld",   2, 10,   1, 0, 0x0d, 1, {T_DIR | T_IND, T_REG}},
	[0x0e] = {"lldi",  3, 50,   1, 1, 0x0e, 1, {T_REG | T_DIR | T_IND, T_DIR | T_REG, T_REG}},
	[0x0f] = {"lfork", 1, 1000, 0, 1, 0x0f, 0, {T_DIR}},
	[0x10] = {"aff",   1, 2,    1, 0, 0x10, 0, {T_REG}}
};

static int											out_cor_ins_fill(const uint8_t *src, struct s_out_cor_ins *const ins)
{
	const uint8_t *const							start = src;
	uint8_t											parameters;
	uint8_t											parameter;
	int												index;

	if (!(ins->info = g_opcodes_info + (((int)*src++) & 0xFF))->name)
		return (0);
	if (ins->info->parameters_encoding)
		parameters = *src++;
	else
		parameters = DIR_CODE << 6;
	index = 0;
	while (index < ins->info->parameters)
	{
		parameter = ((parameters << (index << 1)) >> 6) & 0x3;
		*(ins->types + index) = parameter;
		*(ins->labels + index) = 0;
		if (parameter == REG_CODE && ins->info->parameters_type[index] & T_REG)
			ins->params[index] = (int32_t)*src++;
		else if (parameter == IND_CODE && ins->info->parameters_type[index] & T_IND)
		{
			ins->params[index] = (int32_t)__builtin_bswap16(*((int16_t*)src));
			src += IND_SIZE;
		}
		else if (parameter == DIR_CODE && ins->info->parameters_type[index] & T_DIR)
			if (ins->info->parameters_direct_small)
			{
				ins->params[index] = (int32_t)__builtin_bswap16(*((int16_t*)src));
				src += IND_SIZE;
			}
			else
			{
				ins->params[index] = __builtin_bswap32(*((int32_t*)src));
				src += DIR_SIZE;
			}
		else
			return (0);
		++index;
	}
	ins->size = (int64_t)(src - start);
	ins->label = 0;
	return (1);
}

static char											*out_cor_ins_string(const struct s_out_cor_ins *const ins, const char *const label)
{
	int												index;
	static char										buffer[256];
	char											*ptr;

	ptr = buffer + ft_sprintf(buffer, "%6s ", ins->info->name);
	index = 0;
	while (index < ins->info->parameters)
	{
		if (ins->types[index] == REG_CODE)
			ptr += ft_sprintf(ptr, "r%d", ins->params[index]);
		else if (ins->types[index] == IND_CODE)
			if (ins->labels[index])
				ptr += ft_sprintf(ptr, ":%s%d", label, ins->labels[index] - 1);
			else
				if ((int16_t)ins->params[index] < 0)
					ptr += ft_sprintf(ptr, "-%d", -(int16_t)ins->params[index]);
				else
					ptr += ft_sprintf(ptr, "%d", ins->params[index]);
		else
			if (ins->labels[index])
				ptr += ft_sprintf(ptr, "%%:%s%d", label, ins->labels[index] - 1);
			else
			{
				if (ins->info->parameters_direct_small)
				{
					if ((int16_t)ins->params[index] < 0)
						ptr += ft_sprintf(ptr, "-%d", -(int16_t)ins->params[index]);
					else
						ptr += ft_sprintf(ptr, "%d", ins->params[index]);;
				}
				else if (ins->params[index] < 0)
					ptr += ft_sprintf(ptr, "%%-%d", -ins->params[index]);
				else
					ptr += ft_sprintf(ptr, "%%%d", ins->params[index]);
			}
		if (++index < ins->info->parameters)
			ptr += ft_sprintf(ptr, ", ");
	}
	return (buffer);
}

static char											*out_cor_ins_string_ins(const struct s_out_cor_ins *const ins, const uint8_t *src, const int show_label)
{
	static char										buffer[128];

	if (show_label)
		ft_sprintf(buffer, " %-5s %#02hhx %02hh[* ]x", ins->info->name, *src, ins->size - 1, src + 1);
	else
		ft_sprintf(buffer, "%#02hhx %02hh[* ]x", *src, ins->size - 1, src + 1);
	return (buffer);
}

static void											out_cor_resolve_subins(struct s_out_cor_file *const outfile, struct s_out_cor_ins *target)
{
	struct s_out_cor_ins							*ins_end = outfile->ins + outfile->ins_count;
	struct s_out_cor_ins							*ins;
	int												index;
	int32_t											offset;

	index = -1;
	while (++index < target->info->parameters)
	{
		if (target->types[index] == IND_CODE || target->types[index] == DIR_CODE)
		{
			if (target->types[index] == IND_CODE || target->info->parameters_direct_small)
				offset = target->offset + (int16_t)target->params[index];
			else
				offset = target->offset + target->params[index];
			if (offset >= 0 && offset < CHAMP_MAX_SIZE)
			{
				ins = outfile->ins;
				while (ins < ins_end)
				{
					if (ins->offset == offset)
					{
						target->labels[index] = ins->label;
						break ;
					}
					++ins;
				}
			}
		}
	}
}


static void											out_cor_print_subins(struct s_out_cor_file *const outfile, char **lines)
{
	register const struct s_out_cor_ins				*ins;
	const struct s_out_cor_ins *const				ins_end = outfile->ins + outfile->ins_count;
	static char										buffer[1024];
	char											*ptr;
	int32_t											offset;
	struct s_out_cor_ins							ins_buff;

	ins = outfile->ins;
	while (ins < ins_end)
	{
		if (ins->label)
			ft_dprintf(outfile->fd, "\n%s%d:\n",  outfile->label, ins->label - 1);
		offset = 0;
		ptr = buffer;
		*ptr = 0;
		while (++offset < ins->size)
			if (out_cor_ins_fill(outfile->file->instructions + ins->offset + offset, &ins_buff))
			{
				if (outfile->flags & OUT_COR_SHOW_SUBINS_LABEL)
					out_cor_resolve_subins(outfile, &ins_buff);
				ptr += ft_sprintf(ptr, "<[%d] %-*s %-*s> ", offset, outfile->ins_max_length, out_cor_ins_string(&ins_buff, outfile->label),
																	outfile->ins_max_size, out_cor_ins_string_ins(&ins_buff, outfile->file->instructions + ins->offset + offset, false));
			}
			if (ptr == buffer)
			ft_dprintf(outfile->fd, "%-*s\n", outfile->ins_max_length, *lines);
		else
			ft_dprintf(outfile->fd, "%-*s # %s\n", outfile->ins_max_length, *lines, buffer);
		free(*lines++);
		++ins;
	}
}

static void											out_cor_print_ins_subins(struct s_out_cor_file *const outfile, char **lines)
{
	register const struct s_out_cor_ins				*ins;
	const struct s_out_cor_ins *const				ins_end = outfile->ins + outfile->ins_count;
	static char										buffer[1024];
	char											*ptr;
	int32_t											offset;
	struct s_out_cor_ins							ins_buff;

	ins = outfile->ins;
	while (ins < ins_end)
	{
		if (ins->label)
			ft_dprintf(outfile->fd, "\n%s%d:\n",  outfile->label, ins->label - 1);
		offset = 0;
		ptr = buffer;
		*ptr = 0;
		while (++offset < ins->size)
			if (out_cor_ins_fill(outfile->file->instructions + ins->offset + offset, &ins_buff))
			{
				if (outfile->flags & OUT_COR_SHOW_SUBINS_LABEL)
					out_cor_resolve_subins(outfile, &ins_buff);
				ptr += ft_sprintf(ptr, "<[%d] %-*s %-*s> ", offset, outfile->ins_max_length, out_cor_ins_string(&ins_buff, outfile->label),
																	outfile->ins_max_length, out_cor_ins_string_ins(&ins_buff, outfile->file->instructions + ins->offset + offset, false));
			}
		ft_dprintf(outfile->fd, "%-*s #%-*s %s\n", outfile->ins_max_length, *lines, outfile->ins_max_size, out_cor_ins_string_ins(ins, outfile->file->instructions + ins->offset, true), buffer);
		free(*lines++);
		++ins;
	}
}

static void											out_cor_print_ins(const struct s_out_cor_file *const outfile, char **lines)
{
	register const struct s_out_cor_ins				*ins;
	const struct s_out_cor_ins *const				ins_end = outfile->ins + outfile->ins_count;

	ins = outfile->ins;
	while (ins < ins_end)
	{
		if (ins->label)
			ft_dprintf(outfile->fd, "\n%s%d:\n",  outfile->label, ins->label - 1);
		ft_dprintf(outfile->fd, "%-*s # %-5s %#02hhx %02hh[* ]x\n", outfile->ins_max_length, *lines, ins->info->name, ins->info->opvalue, ins->size - 1, outfile->file->instructions + ins->offset + 1);
		free(*lines++);
		++ins;
	}
}

static void											out_cor_print(const struct s_out_cor_file *const outfile, char **lines)
{
	register const struct s_out_cor_ins				*ins;
	const struct s_out_cor_ins *const				ins_end = outfile->ins + outfile->ins_count;

	ins = outfile->ins;
	while (ins < ins_end)
	{
		if (ins->label)
			ft_dprintf(outfile->fd, "\n%s%d:\n",  outfile->label, ins->label - 1);
		ft_dprintf(outfile->fd, "%s\n", *lines);
		free(*lines++);
		++ins;
	}
}

static void											out_cor_file(struct s_out_cor_file *const outfile, int32_t count)
{
	register const struct s_out_cor_ins				*ins;
	char											*lines[512];
	int32_t											lines_count;
	int												tmp;
	const char										*string;

	lines_count = 0;
	outfile->ins_max_size = 0;
	ins = outfile->ins;
	while (count--)
	{
		string = out_cor_ins_string(ins, outfile->label);
		tmp = (int)ft_strlen(string) + 4;
		if (tmp > outfile->ins_max_length)
			outfile->ins_max_length = tmp;
		if (ins->size > outfile->ins_max_size)
			outfile->ins_max_size = ins->size;
		ft_asprintf(lines + lines_count++, "    %-s", string);
		++ins;
	}
	outfile->ins_max_size = outfile->ins_max_size * 3 + 7;
	if ((outfile->flags & (OUT_COR_SHOW_INS | OUT_COR_SHOW_SUBINS)) == (OUT_COR_SHOW_INS | OUT_COR_SHOW_SUBINS))
		out_cor_print_ins_subins(outfile, lines);
	else if (outfile->flags & OUT_COR_SHOW_SUBINS)
		out_cor_print_subins(outfile, lines);
	else if (outfile->flags & OUT_COR_SHOW_INS)
		out_cor_print_ins(outfile, lines);
	else
		out_cor_print(outfile, lines);
}

static void											out_cor_resolve(struct s_out_cor_file *const outfile, struct s_out_cor_ins *ins)
{
	struct s_out_cor_ins							*ins_end = ins + outfile->ins_count;
	struct s_out_cor_ins							*ptr;
	int												index;
	int32_t											offset;
	int32_t											label;

	label = 0;
	while (ins < ins_end)
	{
		ptr = outfile->ins;
		while (ptr < ins_end)
		{
			index = -1;
			while (++index < ptr->info->parameters)
			{
				if (ptr->types[index] == IND_CODE || ptr->types[index] == DIR_CODE)
				{
					if (ptr->types[index] == IND_CODE || ptr->info->parameters_direct_small)
						offset = ptr->offset + (int16_t)ptr->params[index];
					else
						offset = ptr->offset + ptr->params[index];
					if (offset >= 0 && offset < CHAMP_MAX_SIZE && offset == ins->offset && ptr != ins)
					{
						if (!ins->label)
							ins->label = ++label;
						ptr->labels[index] = ins->label;
					}
				}
			}
			++ptr;
		}
		++ins;
	}
}

void												libcorewar_out_cor_file(const int fd, const struct s_libcorewar_cor_file *const file, const char *const label, const int flags)
{
	struct s_out_cor_file							outfile;
	int32_t											offset;

	ft_dprintf(fd, "#\n# generated by libcorewar\n#\n\n");
	ft_dprintf(fd, ".name \"%s\"\n.comment \"%s\"\n\n", file->header->prog_name, file->header->comment);

	outfile.file = file;
	outfile.fd = fd;
	outfile.flags = flags;
	if (label)
		outfile.label = label;
	else
		outfile.label = "offset_";
	outfile.ins_count = 0;

	offset = 0;
	while (offset < file->length)
	{
		if (out_cor_ins_fill(file->instructions + offset, outfile.ins + outfile.ins_count))
		{
			(outfile.ins + outfile.ins_count)->offset = offset;
			offset += (outfile.ins + outfile.ins_count++)->size;
		}
		else
			break ;
	}
	if (flags & OUT_COR_SHOW_LABEL)
		out_cor_resolve(&outfile, outfile.ins);
	out_cor_file(&outfile, outfile.ins_count);
}





















