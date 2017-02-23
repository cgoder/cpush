

# **********************************************

LIBPATH   = ./lib
LIBNAME  = libcpush

CROSS_TOOL_PREFIX = 
CXX = $(CROSS_TOOL_PREFIX)gcc
CC  = $(CROSS_TOOL_PREFIX)gcc
AR  = $(CROSS_TOOL_PREFIX)ar
LD  = $(CROSS_TOOL_PREFIX)ld
RANLIB  = $(CROSS_TOOL_PREFIX)ranlib

#ifeq ($(TARGET_CPU), mips)
#   CC_OPTS += -DTARGET_CPU=0
#else
#ifeq ($(TARGET_CPU), arm)
#CC_OPTS += -DTARGET_CPU=2
#endif
#endif

CC_OPTS += -g -Os
# **********************************************

SRCDIR += ./client/
SRCDIR += ./packet/

INCDIR += $(SRCDIR)

SRC_C += $(foreach dir,$(SRCDIR), $(wildcard $(dir)*.c))
SRC_CPP += $(foreach dir, $(SRCDIR), $(wildcard $(dir)*.cpp))


INCLUDE = $(foreach dir,$(INCDIR), -I$(dir))

STATIC_OBJ_C0  = $(patsubst %.c, %.o, $(SRC_C))
STATIC_OBJ_C1  = $(patsubst %.cpp, %.o, $(SRC_CPP))

STATIC_OBJ_C  = $(foreach file, $(STATIC_OBJ_C0), $(file) )
STATIC_OBJ_CPP = $(foreach file, $(STATIC_OBJ_C1), $(file))

# **********************************************

compile: setup $(STATIC_OBJ_C) $(STATIC_OBJ_CPP) 

static_lib : compile makeStatic

dynamic_lib : compile makeDynamic

libs: static_lib dynamic_lib
all: static_lib dynamic_lib


$(STATIC_OBJ_C) : %.o : %.c 
#	@echo "begin compiling for c file   ........." 
	@$(CXX) $(INCLUDE) $(CC_OPTS) -c $< -o $@ 
#	@echo "make c file obj ok....."

$(STATIC_OBJ_CPP) : %.o : %.cpp
#	@echo "begin compiling for cpp file....."
	@$(CXX) $(INCLUDE) $(CC_OPTS) -c $< -o $@
#	@echo "make cpp file objs ok...."

	 
makeStatic:$(STATIC_OBJ_C) $(STATIC_OBJ_CPP)
	@$(AR) cru $(LIBPATH)/$(LIBNAME).a $(STATIC_OBJ_C) $(STATIC_OBJ_CPP)
	@echo "make static lib ok ........."
makeDynamic:$(STATIC_OBJ_C) $(STATIC_OBJ_CPP)
	@$(CXX) $(STATIC_OBJ_C) $(STATIC_OBJ_CPP) -fPIC -shared -o $(LIBPATH)/$(LIBNAME).so
	@echo "make dynamic lib ok ......."
	
setup :
	@mkdir -p $(LIBPATH)
	@find . -regex '.*\.c\|.*\.cpp\|.*\.h'
#	@echo $(STATIC_OBJ_C)
#	@echo $(STATIC_OBJ_CPP)
#	@echo "Build staitc lib...."

	
clean :
	@rm -f $(STATIC_OBJ_C)
	@rm -f $(STATIC_OBJ_CPP)
	@rm -f $(LIBPATH)/$(LIBNAME).a
	@rm -f $(LIBPATH)/$(LIBNAME).so
	@rm -rf $(LIBPATH)

test: $(LIBPATH)/$(LIBNAME).a
	@$(CXX) -g $(INCLUDE) -o sample/test ./sample/test.c $(LIBPATH)/$(LIBNAME).a