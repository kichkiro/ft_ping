# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/24 16:03:40 by kichkiro          #+#    #+#              #
#    Updated: 2025/07/03 21:37:57 by kichkiro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Config ---------------------------------------------------------------------->

NAME		:= ft_ping
SRCS		:= \
				main.c \
				ping.c \
				logger.c \
				parser.c \
				socket.c \
				packets.c \
				verbose.c \
				libft/ft_free.c \
				libft/ft_strf.c \
				libft/ft_int_len.c \
				libft/ft_str_realloc.c
SRC_DIR     := src
SRCS        := $(SRCS:%=$(SRC_DIR)/%)
BUILD_DIR   := .build
OBJS        := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS        := $(OBJS:.o=.d)
INCS        := ./include
CC          := gcc
CFLAGS      := -g
CPPFLAGS    := $(addprefix -I,$(INCS)) -MMD -MP 
LDFLAGS     := -lm
RM          := rm -rf
MAKEFLAGS   := --no-print-directory
DIR_DUP     = mkdir -p $(@D)

.PHONY: all clean fclean re
.SILENT:

# Rules ----------------------------------------------------------------------->

all: banner $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)
	${info ${BLUE}[+] INFO: Creating  -> $(NAME)${NO_COLOR}}

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(DIR_DUP)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
	${info ${BLUE}[+] INFO: Compiling -> $@${NO_COLOR}}

-include $(DEPS)

clean:
	$(RM) $(BUILD_DIR)
	${info ${BLUE}[+] INFO: Cleaning in progress...${NO_COLOR}}

fclean: clean
	$(RM) $(NAME) $(NAME_CLIENT)

re:
	$(MAKE) fclean
	$(MAKE) all

bonus: all

# Style ----------------------------------------------------------------------->

RED			:= ${shell tput setaf 1}
YELLOW		:= ${shell tput setaf 3}
BLUE        := ${shell tput setaf 4}
NO_COLOR	:= ${shell tput sgr0}
BOLD		:= ${shell tput bold}

banner:
	${info ${BLUE} }
	${info  _______  _______          _______  ___   __    _  _______  }
	${info |       ||       |        |       ||   | |  |  | ||       | }
	${info |    ___||_     _|        |    _  ||   | |   |_| ||    ___| }
	${info |   |___   |   |          |   |_| ||   | |       ||   | __  }
	${info |    ___|  |   |          |    ___||   | |  _    ||   ||  | }
	${info |   |      |   |   _____  |   |    |   | | | |   ||   |_| | }
	${info |___|      |___|  |_____| |___|    |___| |_|  |__||_______| }
	${info ${NO_COLOR}}

# ------------------------------------------------------------------------------
