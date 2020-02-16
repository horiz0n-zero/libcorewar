# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: afeuerst <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/02/13 08:52:51 by afeuerst          #+#    #+#              #
#    Updated: 2020/02/13 14:58:52 by afeuerst         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libcorewar.a

SRC_DIR = ./sources
INC_DIR = ./includes
INC_PRINTF_DIR = ./sources/libft_printf/includes

SRC = $(shell find $(SRC_DIR) -type f -name "*.c")
OBJ = $(SRC:.c=.o)
INC = $(shell find $(INC_DIR) $(INC_PRINTF_DIR) -type f -name "*.h")

FLAGS = -I $(INC_DIR) -I $(INC_PRINTF_DIR) -Wno-missing-field-initializers -Wno-initializer-overrides -Wno-unused-parameter -Wno-unused-variable
FLAGS += -flto -g -O0

MAIN = main

all: $(NAME)

$(NAME): Makefile $(INC) $(OBJ)
	ar -rc $(NAME) $(OBJ)

%.o: %.c
	clang -c $(FLAGS) $< -o $@

$(MAIN): $(NAME) main.c
	clang $(NAME) $(FLAGS) main.c -o $(MAIN)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
