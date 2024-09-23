NAME     = webserv
GCC      = c++
# For linux uncomment below
#LINUX = -lpthread
CPPFLAGS   = -Wall -Wextra -Werror -std=c++98 -g $(LINUX) 
RM       = rm -rf
LIBS     = -I./includes/ 

RESET := \033[0m
RED := \033[31m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m

# Compiled directories
SRC = srcs
OBJ = objects
SUBDIRS = server main event manager parsing

# Directory paths
SRC_DIR  = $(addprefix $(SRC)/,$(SUBDIRS))
OBJ_DIR  = $(addprefix $(OBJ)/,$(SUBDIRS))

# Source and object files
SRCS     = $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.cpp))
OBJS     = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))

all: pre-build $(NAME)

pre-build:
	@printf "$(YELLOW) Creating your webserver... $(RESET)\n"

$(NAME): Makefile $(OBJS)
	@printf "$(YELLOW) Creating your webserver... $(RESET)\n"
	@$(GCC) -o $(NAME) $(OBJS) -g $(CPPFLAGS) $(LIBS)  && \
	printf "$(GREEN) Compiled successfully...\n Launch with $(BLUE)./webserv [Path To Config File] $(RESET)\n" || \
	printf "$(RED) Compilation failed... $(RESET)\n"


$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(OBJ) $(OBJ_DIR)
	@$(GCC) $(CPPFLAGS) $(LIBS) -c $< -o $@

clean:
	@printf "$(RED) Cleaning up... $(RESET)\n"
	@$(RM) $(OBJ_DIR)
	@$(RM) $(OBJ)
	
fclean: clean
	@printf "$(RED) Removing all compilation files and executable... $(RESET)\n"
	@$(RM) $(NAME)
	@printf "$(GREEN) All done... $(RESET)\n"

re: fclean all

.PHONY: all clean fclean re