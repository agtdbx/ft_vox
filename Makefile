MAKEFLAGS			:= --no-print-directory
.DEFAULT_GOAL		:= all

.DELETE_ON_ERROR:
.SECONDEXPANSION:

#=====================================NAME=====================================#
EXECUTABLE_NAME		:= ft_vox
ARG					:= data/model/42.obj

#==================================DIRECTORIES=================================#
MESON_CONFIG_DIR	:= build
MESON_BUILD_DIR		:= release
S_DIR				:= shaders
S_BUILD				:= shadersbin

#====================================TARGETS===================================#
VS_SRCS	:=	shaders/chunk.vert shaders/chunkBorder.vert shaders/skybox.vert
FS_SRCS	:=	shaders/chunk.frag shaders/chunkFdf.frag shaders/chunkBorder.frag shaders/skybox.frag

VS_OBJS	:= ${VS_SRCS:$(S_DIR)/%.vert=$(S_BUILD)/%_vert.spv}
FS_OBJS	:= ${FS_SRCS:$(S_DIR)/%.frag=$(S_BUILD)/%_frag.spv}
DIRS	:= $(sort $(shell dirname $(S_BUILD) $(VS_OBJS) $(FS_OBJS)))

#====================================COLORS====================================#
NOC			:= \033[0m
BOLD		:= \033[1m
UNDERLINE	:= \033[4m
BLINK		:= \e[5m
BLACK		:= \033[1;30m
RED			:= \e[1m\e[38;5;196m
GREEN		:= \e[1m\e[38;5;76m
YELLOW		:= \e[1m\e[38;5;220m
BLUE		:= \e[1m\e[38;5;33m
PURPLE		:= \033[1;35m
CYAN		:= \033[1;36m
WHITE		:= \033[1;37m

#==================================PRINT UTILS=================================#
VS_COMPIL	:= 0
FS_COMPIL	:= 0

#=====================================RULES====================================#
#----------------------------------UTILS RULES---------------------------------#
$(DIRS):
	@mkdir $@

#----------------------------------BUILD RULES---------------------------------#
$(EXECUTABLE_NAME): build
	@echo "$(BLUE)Compile project$(NOC)"
	@echo -n "$(PURPLE)"
	@ninja -C $(MESON_CONFIG_DIR)
	@echo -n "$(NOC)"
	@ninja install -C $(MESON_CONFIG_DIR) >/dev/null

$(VS_OBJS): $(S_BUILD)/%_vert.spv : $(S_DIR)/%.vert | $$(@D)
	$(if $(filter $(VS_COMPIL),0), @echo "$(BLUE)Compiling vertex shaders$(NOC)")
	$(eval VS_COMPIL=$(shell expr $(VS_COMPIL) + 1))
	@echo "  $(PURPLE)Compiling $< $(NOC)"
	@glslc $< -o $@

$(FS_OBJS): $(S_BUILD)/%_frag.spv : $(S_DIR)/%.frag | $$(@D)
	$(if $(filter $(FS_COMPIL),0), @echo "$(BLUE)Compiling fragment shaders$(NOC)")
	$(eval FS_COMPIL=$(shell expr $(FS_COMPIL) + 1))
	@echo "  $(PURPLE)Compiling $< $(NOC)"
	@glslc $< -o $@

#----------------------------------MESON RULES---------------------------------#
build:
	@echo "$(BLUE)Create meson config dir$(NOC)"
	@meson $(MESON_CONFIG_DIR) --prefix=$$PWD/$(MESON_BUILD_DIR) --bindir="" --libdir="" >/dev/null

#-----------------------------------CMD RULES----------------------------------#
all: $(VS_OBJS) $(FS_OBJS) $(EXECUTABLE_NAME)
	@echo "$(GREEN)Build done !$(NOC)"

clean:
	@echo "$(RED)Remove build dirs$(NOC)"
	@rm -rf $(MESON_BUILD_DIR) $(S_BUILD)

fclean: clean
	@echo "$(RED)Remove meson config dir$(NOC)"
	@rm -rf $(MESON_CONFIG_DIR)

fullclean: fclean
	@echo "$(RED)Remove download lib$(NOC)"
	@for i in $$(find subprojects -maxdepth 1 -type d); do if [ $$i != "subprojects" ]; then rm -rf $$i; fi; done;

re: fclean all

#-----------------------------------RUN RULES----------------------------------#
run: all
	@echo "$(BLUE)Start project$(NOC)"
	@cd $(MESON_BUILD_DIR) && ./$(EXECUTABLE_NAME) $(ARG) || echo "Exit at 1"
	@echo "$(GREEN)Bye !$(NOC)"

runval: all
	@echo "$(BLUE)Start project with $(UNDERLINE)valgrind$(NOC)"
	@cd $(MESON_BUILD_DIR) && valgrind --suppressions=vsupp ./$(EXECUTABLE_NAME) $(ARG) || echo "Exit at 1"
	@echo "$(GREEN)Bye !$(NOC)"

runvalall: all
	@echo "$(BLUE)Start project with $(UNDERLINE)valgrind full option$(NOC)"
	@cd $(MESON_BUILD_DIR) && valgrind --suppressions=vsupp --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE_NAME) $(ARG) || echo "Exit at 1"
	@echo "$(GREEN)Bye !$(NOC)"

genvsupp: all
	@echo "$(BLUE)Start project with $(UNDERLINE)valgrind for generate vsupp$(NOC)"
	@cd $(MESON_BUILD_DIR) && valgrind --leak-check=full --show-leak-kinds=all --gen-suppressions=all --log-file=vsupp ./$(EXECUTABLE_NAME) $(ARG)
	@echo "$(GREEN)Bye !$(NOC)"

#---------------------------------INSTALL RULES--------------------------------#
install:
	@echo "$(BLUE)You need to have sudo permission$(NOC)"
	@echo "$(GREEN)Installing vulkan$(NOC)"
	@sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools
	@echo "$(GREEN)Installing shader compilator and vulkan sdk$(NOC)"
	@wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
	@sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list https://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
	@sudo apt update
	@sudo apt install vulkan-sdk intel-opencl-icd

full_install: install
	@echo "$(GREEN)Installing glfw$(NOC)"
	@sudo apt install libglfw3-dev
	@echo "$(GREEN)Installing glm$(NOC)"
	@sudo apt install libglm-dev

#----------------------------------UPDATE RULE---------------------------------#
update: fullclean all

.PHONY: all clean fclean fullclean re run runval runvalall genvsupp install full_install update
