// tests.h
// (c) 2015 Christian Wohlwend


#ifndef TESTS_H
#define TESTS_H

#include "PixelBuffer.h"
#include <string>

class ColorData;
class PixelBuffer;
class Tool;
class Filter;

class tests
{
public:
	tests();

private:
	bool comparePixelBuffers(PixelBuffer* firstBuffer, PixelBuffer* secondBuffer);



	void generate_tests();
	void generate_test_pen();
	void generate_test_eraser();
	void generate_test_spraycan();
	void generate_test_caligraphypen();
	void generate_test_highlighter();
	void generate_test_chalk();
	void generate_test_blurtool();

	void runTests();
	void test_pen();
	void test_eraser();
	void test_spraycan();
	void test_caligraphypen();
	void test_highlighter();
	void test_chalk();
	void test_blurtool();


	bool hasSuffix(const std::string & str, const std::string & suffix);
	bool isValidImageFileName(const std::string & name);
	bool isValidImageFile(const std::string & name);

	Tool **m_tools;




};

#endif