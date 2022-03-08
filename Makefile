# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/21 19:56:36 by lucocozz          #+#    #+#              #
#    Updated: 2022/03/06 19:12:12 by lucocozz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := webserv

WEBSERV_PATH := $(HOME)/.config/webserv/

SRC := 	main.cpp

MAKE = make
MAKEFLAGS += --no-print-directory
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -MMD -MD -std=c++98 -Wpedantic -g3
BUILD_DIR := .build
INCLUDES_DIR := $(shell find includes -type d)
SOURCE_DIR := sources
DEP := $(SRC:%.cpp=$(BUILD_DIR)/%.d)
OBJ := $(DEP:.d=.o)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

vpath %.cpp ./ $(shell find $(SOURCE_DIR) -type d)

all: $(NAME)
	mkdir -p $(WEBSERV_PATH)
	cp -r sources/config/ $(WEBSERV_PATH)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

test: $(NAME)

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES_DIR:%=-I %) -D WEBSERV_PATH='"$(WEBSERV_PATH)"' -c $< -o $@

.PHONY: all clean fclean re test
