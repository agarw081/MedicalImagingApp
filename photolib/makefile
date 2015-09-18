UNAME = $(shell uname)

CXX = g++

SOURCE_DIR = src

ifeq ($(UNAME), Darwin) # Mac OSX 
	LINK_LIBS += -framework glut -framework opengl
	EXECUTABLE = PhotoShop
else # LINUX
	LINK_LIBS +=  -lglut -lGL -lGLU
	EXECUTABLE = PhotoShop
endif

PNG = png
JPEG = jpeg

PNG_PATH = ../libpng-1.6.16
JPEG_PATH = ../jpeg-9a

PNG_LIB = $(PNG_PATH)/lib/lib$(PNG).a
JPEG_LIB = $(JPEG_PATH)/lib/lib$(JPEG).a

LINK_LIBS += -L$(PNG_PATH)/lib/ -l$(PNG)
LINK_LIBS += -L$(JPEG_PATH)/lib/ -l$(JPEG)
LINK_LIBS += -lz


INCLUDE += -I$(PNG_PATH)/include
INCLUDE += -I$(JPEG_PATH)/include

INCLUDE += -I./include/

CPPFLAGS += -c -Wall

LIBPHOTO = -L./lib -lphoto

CPPFILES = $(notdir $(wildcard $(SOURCE_DIR)/*.cpp))

SOURCES=$(addprefix ./src/,  $(CPPFILES))

OBJECTS = $(addprefix ./build/,  $(CPPFILES:.cpp=.o)) 

PHOTO_LIB = lib/libphoto.a

all: setup $(PHOTO_LIB) 

setup: build lib

build:
	mkdir -p build
	mkdir -p lib

$(PHOTO_LIB): $(OBJECTS)
	ar -r $(PHOTO_LIB) $(OBJECTS) 

build/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

.o:
	$(CXX) $(CPPFLAGS) -c $<

clean:
	\rm -rf build lib

sure:
	@echo $(CPPFILES)
	@echo $(SOURCES)
	@echo $(OBJECTS)
