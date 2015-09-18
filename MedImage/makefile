#  Created by Umang Agarwal

# Include nvidia drivers if necessary:
	NVIDIA_LIB = $(wildcard /usr/lib/nvidia-304)
	ifneq ($(NVIDIA_LIB), )
		LINK_LIBS += -L$(NVIDIA_LIB)
	endif

UNAME = $(shell uname)
CC 		= g++
CFLAGS 	= -c -Wall
LDFLAGS = 

GLUI = glui
PNG = png
JPEG = jpeg
PHOTO = photo



GLUI_PATH = ../glui
PNG_PATH = ../libpng-1.6.16
JPEG_PATH = ../jpeg-9a
PHOTO_PATH = ../photolib

GLUI_LIB = $(GLUI_PATH)/lib/lib$(GLUI).a
PNG_LIB = $(PNG_PATH)/lib/lib$(PNG).a
JPEG_LIB = $(JPEG_PATH)/lib/lib$(JPEG).a
PHOTO_LIB = $(PHOTO_PATH)/lib/lib$(PHOTO).a

LINK_LIBS += -L$(PHOTO_PATH)/lib/ -l$(PHOTO)
LINK_LIBS += -L$(GLUI_PATH)/lib/ -l$(GLUI)
LINK_LIBS += -L$(PNG_PATH)/lib/ -l$(PNG)
LINK_LIBS += -L$(JPEG_PATH)/lib/ -l$(JPEG)
LINK_LIBS += -lz

OBJECT_DIR = object_files
SOURCE_DIR = src
LIB_DIR = ../photolib/include/

INCLUDE += -I$(GLUI_PATH)/include 
INCLUDE += -I$(PNG_PATH)/include
INCLUDE += -I$(JPEG_PATH)/include
INCLUDE += -I$(PHOTO_PATH)/include

OBJECTS = $(addprefix $(OBJECT_DIR)/,  $(FILENAMES:.cpp=.o)) 
CPPFILES = $(wildcard $(SOURCE_DIR)/*.cpp)
FILENAMES =  $(notdir $(CPPFILES))

ifeq ($(UNAME), Darwin) # Mac OSX 
	LINK_LIBS += -framework glut -framework opengl
	EXECUTABLE = medimage
else # LINUX
	LINK_LIBS += -lglut -lGL -lGLU
	EXECUTABLE = medimage
endif


build: setup $(EXECUTABLE)

rebuild: clean build

# Create the object_files directory only if it does not exist. 
setup: | $(OBJECT_DIR)

$(OBJECT_DIR): 
	mkdir -p $(OBJECT_DIR)

$(EXECUTABLE): $(GLUI_LIB) $(PNG_LIB) $(JPEG_LIB) $(PHOTO_LIB) $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LINK_LIBS) -o $@

# Explicitly defined build targets because of dependancy on other header files
$(OBJECT_DIR)/MedImageGUI.o: $(SOURCE_DIR)/MedImageGUI.cpp $(SOURCE_DIR)/MedImageGUI.h $(LIB_DIR)/ColorData.h $(LIB_DIR)/PixelBuffer.h
	$(CC) $(CFLAGS) $(INCLUDE)  -o $@ $<

$(OBJECT_DIR)/PixelBuffer.o: $(LIB_DIR)/PixelBuffer.cpp $(LIB_DIR)/PixelBuffer.h $(LIB_DIR)/ColorData.h 
	$(CC) $(CFLAGS) $(INCLUDE)  -o $@ $<

# Pattern build targets
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(SOURCE_DIR)/%.h
	#@echo 'Compiling arbitrary .cpp file with .h'
	$(CC) $(CFLAGS) $(INCLUDE)  -o $@ $<

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	#@echo 'Compiling arbitrary .cpp file without .h'
	$(CC) $(CFLAGS) $(INCLUDE)  -o $@ $<

clean:
	\rm -rf $(OBJECT_DIR) $(EXECUTABLE) 

cleanglui:	
	 $(MAKE)  -C ../$(GLUI)/ clean

$(GLUI_LIB): 
	$(MAKE) -C ../$(GLUI)/ all

$(PNG_LIB):
	cd $(PNG_PATH); ./configure --prefix=$(OLDPWD)/libpng-1.6.16 --enable-shared=no 
	cd $(PNG_PATH); $(MAKE) / check 
	cd $(PNG_PATH); $(MAKE) / install

$(JPEG_LIB):
	cd $(JPEG_PATH); ./configure --prefix=$(OLDPWD)/jpeg-9a --enable-shared=no
	cd $(JPEG_PATH); $(MAKE) / all  
	cd $(JPEG_PATH); $(MAKE) / install

$(PHOTO_LIB):
	cd $(PHOTO_PATH); $(MAKE)

sure:
	@echo $(LIB_DIR)
