DATE = $(shell date +"%Y%m%0e%H%Mull")

CC	= gcc -pipe $(CFLAGS) $(INCFLAGS) -Werror=return-type
CXX	= g++ -pipe $(CFLAGS) $(INCFLAGS) -Werror=return-type
ASM = as -Qy
LINKER = g++

TARGET_NAME = pvconverter

ifeq ($(strip $(SOURCE_ROOT)), )
SOURCE_ROOT = .
endif

INCFLAGS += -I/home/ashamsi/Downloads/boost_1_88_0
INCFLAGS += -I$(SOURCE_ROOT)/../share/include
INCFLAGS += -I$(SOURCE_ROOT)/public/manager
INCFLAGS += -I$(SOURCE_ROOT)
INCFLAGS += -I/home/ashamsi/include

# Add Tk libraries and ensure correct linking order
LIBFLAGS += -L$(SOURCE_ROOT)/../share -ltk8.5 -ltcl8.5 -lX11 -lXft -ldl -lpthread -lm -lz -lfontconfig

_SOURCES  = $(shell find $(SOURCE_ROOT) -type f \
							"(" -name '*.cpp' -or -name '*.c' -or -name '*.cc' -or -name '*.cxx' ")" )
							
SOURCES   = $(_SOURCES)

TARGET_PATH = ../platform/$(PLATFORM)/bin/

OBJECTS     = $(sort $(addprefix $(TARGET_PATH), $(addsuffix .o, $(basename $(SOURCES)))))

DEPS        = $(OBJECTS:.o=.d)

-include    $(DEPS)

$(TARGET_PATH)%.o: %.c
	@mkdir -p "$(TARGET_PATH)$(dir $*)"
	@echo "Compiling $<  ... "
	@$(CC) -o "$@" -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<"
	@echo "done"

$(TARGET_PATH)%.o: %.cc
	@mkdir -p "$(TARGET_PATH)$(dir $*)"
	@echo "Compiling $< ... "
	@$(CXX) -o "$@" -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<"
	@echo "done"

$(TARGET_PATH)%.o: %.cpp
	@mkdir -p "$(dir $@)"
	@echo "Compiling $< ... "
	@$(CXX) -o "$@" -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<"
	@echo "done ($<)"

$(TARGET_PATH)%.o: %.cxx
	@mkdir -p "$(dir $@)"
	@echo "Compiling $< ... "
	@$(CXX) -o "$@" -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<"
	@echo "done"

all: $(TARGET_PATH)$(TARGET_NAME)
		
$(TARGET_PATH)$(TARGET_NAME): $(OBJECTS)
	@echo "Linking $(TARGET_PATH)$(TARGET_NAME) from $(shell echo $(OBJECTS) | wc -w) files ..."
ifeq ($(suffix $(TARGET_NAME)), .a)
	@ar -r "$(TARGET_PATH)$(TARGET_NAME)" $(OBJECTS)
else
ifeq ($(TYPE), publish)
	@$(LINKER) -o "$(TARGET_PATH)$(TARGET_NAME)_tmp" $(OBJECTS) $(LIBFLAGS)
	@objcopy --only-keep-debug "$(TARGET_PATH)$(TARGET_NAME)_tmp" "$(TARGET_PATH)$(TARGET_NAME).symbol"
	@strip -s "$(TARGET_PATH)$(TARGET_NAME)_tmp"
	@objcopy "$(TARGET_PATH)$(TARGET_NAME)_tmp" --add-gnu-debuglink="$(TARGET_PATH)$(TARGET_NAME).symbol"
else
	@$(LINKER) -rdynamic -o "$(TARGET_PATH)$(TARGET_NAME)_tmp" $(OBJECTS) $(LIBFLAGS)
endif
	@mv "$(TARGET_PATH)$(TARGET_NAME)_tmp" "$(TARGET_PATH)$(TARGET_NAME)"
endif
	@echo done	

clean:
	@echo "rm *.o *.d $(TARGET_NAME)"
	@rm -f $(OBJECTS) $(DEPS)
	@rm -f "$(TARGET_PATH)$(TARGET_NAME)"
	
.PHONY: all clean	
