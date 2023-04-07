#pragma once
#define _CRT_SECURE_NO_WARNINGS 1 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int initStream(const char* pStreamFile);
int nextNalLength(int Pos);
void readOneNal(unsigned char* pout, int Pos, int nLen);

