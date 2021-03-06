// cc65_charmap.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

void printMapping(bool asmFlag, unsigned char from, unsigned char to)
{
	if (asmFlag)
		printf(".charmap $%02X, $%02X\n", from, to);
	else
		printf("#pragma charmap ($%02X, $%02X)\n", from, to);
}

void convertChar(bool asmFlag, unsigned char start, unsigned char end, unsigned char remap)
{
	if (asmFlag)
		printf("\n; ");
	else
		printf("\n\\\\ ");

	if (remap == 0)
		printf("Char $%02X ... $%02X -> c = c\n", start, end);
	else
	{
		if(remap & 0x80)
			printf("Char $%02X ... $%02X -> c + $%02X\n", start, end, remap);
		else
			printf("Char $%02X ... $%02X -> c - $%02X\n", start, end, remap);
	}

	for (unsigned char i = start; i <= end; i++)
		printMapping(asmFlag, i, i + remap);
}

void createCharmap(bool asmFlag)
{
	convertChar(asmFlag, 0x00, 0x1F, 0x80);
	convertChar(asmFlag, 0x20, 0x3F, 0x00);
	convertChar(asmFlag, 0x40, 0x5F, 0xC0);
	convertChar(asmFlag, 0x60, 0x7F, 0xE0);
	convertChar(asmFlag, 0x80, 0x9F, 0x40);
	convertChar(asmFlag, 0xA0, 0xBF, 0xC0);
	convertChar(asmFlag, 0xC0, 0xDF, 0x80);
	convertChar(asmFlag, 0xE0, 0xFE, 0x80);

	printf("\n");
	printMapping(asmFlag, 0xff, 0x5e);
}

#include <string>
#include <iostream>

using namespace std;

int main()
{
	createCharmap(true);
	createCharmap(false);

	return 0;
}
