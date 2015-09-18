# created by Christian Wohlwend

	NVIDIA_LIB = $(wildcard /usr/lib/nvidia-304)
	ifneq ($(NVIDIA_LIB), )
		LINK_LIBS += -L$(NVIDIA_LIB)
	endif

UNAME = $(shell uname)
CC 		= g++
CFLAGS 	= -c -Wall
LDFLAGS = 

PNG = png
JPEG = jpeg
PHOTO = photo

PNG_PATH = ../libpng-1.6.16
JPEG_PATH = ../jpeg-9a
PHOTO_PATH = ../photolib

PNG_LIB = $(PNG_PATH)/lib/lib$(PNG).a
JPEG_LIB = $(JPEG_PATH)/lib/lib$(JPEG).a
PHOTO_LIB = $(PHOTO_PATH)/lib/lib$(PHOTO).a

LINK_LIBS += -L$(PHOTO_PATH)/lib/ -l$(PHOTO)
LINK_LIBS += -L$(PNG_PATH)/lib/ -l$(PNG)
LINK_LIBS += -L$(JPEG_PATH)/lib/ -l$(JPEG)
LINK_LIBS += -lz

OBJECT_DIR = object_files
SOURCE_DIR = src
LIB_DIR = ../photolib/include/

INCLUDE += -I$(PNG_PATH)/include
INCLUDE += -I$(JPEG_PATH)/include
INCLUDE += -I$(PHOTO_PATH)/include

OBJECTS = $(addprefix $(OBJECT_DIR)/,  $(FILENAMES:.cpp=.o)) 
CPPFILES = $(wildcard $(SOURCE_DIR)/*.cpp)
FILENAMES =  $(notdir $(CPPFILES))

ifeq ($(UNAME), Darwin) # Mac OSX 
	LINK_LIBS += -framework glut -framework opengl
	EXECUTABLE = tests
else # LINUX
	LINK_LIBS += -lglut -lGL -lGLU
	EXECUTABLE = tests
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



all:
	$(MAKE) test_blur
	$(MAKE) test_sharpen
	$(MAKE) test_edgedetect
	$(MAKE) test_saturate
	$(MAKE) test_thresh
	$(MAKE) test_quantize
	$(MAKE) test_multirgb
	$(MAKE) test_stamp
	$(MAKE) test_tools
	$(MAKE) test_commandline

test_blur:
	@echo 'testing blur filter'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -blur 5 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_blur.png
test_sharpen:
	@echo 'testing sharpen filter'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -sharpen 5 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_sharpen.png
test_edgedetect:
	@echo 'testing edgedetect filter'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -edgedetect ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_edgedetect.png
test_saturate:
	@echo 'testing saturation filter'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -saturate 2 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_saturate.png
test_thresh:
	@echo 'testing threshold filter'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -thresh .5 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_thresh.png
test_quantize:
	@echo 'testing quanatize filter'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -quantize 4 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_quantize.png
test_multirgb:
	@echo 'testing multirgb filter'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -multrgb .75 .6 .5 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_multrgb.png
test_stamp:
	@echo 'testing stamp tool'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -stamp 256 256 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_stamp.png

test_tools:
	./tests

test_commandline:
	@echo 'simple command line test'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -stamp 300 300 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_simple.png
	@echo 'more advanced command line test'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -stamp 300 300 -thresh .5 -edgedetect ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_advanced.png
	@echo 'very complex command line test'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -stamp 300 300 -thresh .5 -edgedetect -blur 4 -saturate 3 -quantize 5 -multrgb .6 1.0 .8 ../tests/testImages/grumpyTestout.png
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTestout.png -compare ../tests/testImages/preComputedImages/grumpyTest_complex.png
	@echo ' testing -h'
	cd ../MedImage; ./medimage -h
	@echo 'misspelling'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -thresbold .5 ../tests/testImages/grumpyTestout.png
	@echo 'wrong argument count'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyTest.png -threshold .5 .5 ../tests/testImages/grumpyTestout.png
	@echo 'bad image'
	cd ../MedImage; ./medimage ../tests/testImages/grumpyBest.png -threshold .5 .5 ../tests/testImages/grumpyTestout.png


