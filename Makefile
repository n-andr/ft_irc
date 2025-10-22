# Compiler
CXX = c++

# Compiler flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Source files
SRC_DIR = ./src
SRCS = $(addprefix $(SRC_DIR)/,\
	main.cpp Server_eventloop.cpp \
	Client.cpp Client_get_set.cpp Client_parsing.cpp \
	Server.cpp Server_ocf.cpp Server_setup.cpp Server_socket.cpp Server_send_data.cpp \
	Channel.cpp \
	Registration.cpp Privmsg.cpp Invite_join.cpp Mode.cpp Mode_execution.cpp\
	debug.cpp )

OBJ_DIR = ./obj
OBJS = $(SRCS:./src/%.cpp=$(OBJ_DIR)/%.o)

INC_DIR = ./inc
INC = $(addprefix $(INC_DIR)/,\
	Client.hpp Server.hpp Color.hpp Channel.hpp Macros.hpp)
#INC_DIR (in case we want to use this later)


# Executable name
NAME = ircserv

all: $(NAME) 

$(NAME): $(OBJS) $(INC)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC)
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

test:
	@bash tests/run_tests.sh

# verbose run (show live logs)
testv:
	@VERBOSE=1 bash tests/run_tests.sh
	
test-clean:
	@rm -rf tests/logs

re: fclean all

.PHONY: all clean fclean re
