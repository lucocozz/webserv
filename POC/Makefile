# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/21 19:56:36 by lucocozz          #+#    #+#              #
#    Updated: 2022/02/16 22:19:15 by lucocozz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := server

SRC := 	main.cpp

MAKE = make
MAKEFLAGS += --no-print-directory
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -MMD -MD -g3 #-std=c++98
BUILD_DIR := .build
INCLUDES_DIR := includes
SOURCE_DIR := sources
DEP := $(SRC:%.cpp=$(BUILD_DIR)/%.d)
OBJ := $(DEP:.d=.o)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

vpath %.cpp ./ $(shell find $(SOURCE_DIR) -type d)

all: $(NAME)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

test: $(NAME)

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES_DIR:%=-I %) -c $< -o $@

.PHONY: all clean fclean re test
