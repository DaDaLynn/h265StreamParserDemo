#include "h265StreamParser.h"

static int FindStartCode3Bytes(unsigned char* scData);
static int FindStartCode4Bytes(unsigned char* scData);
static unsigned char GetNaluType(unsigned char* pInBuffer, int s32NaluStartPos);
static unsigned char IsSliceNalu(unsigned char u8Nal_unit_type);
static int GetNaluDataLen(int startPos, int h265BitsSize, unsigned char* h265Bits);
static int GetSliceNaluDataLen(int startPos, int h265BitsSize, unsigned char* h265Bits);
static void ParseNaluData(const unsigned int naluLen, unsigned char* const naluData);

#define streamSpace 1024*1024*300
unsigned char pStream[streamSpace];
int nStreamLen = 0;

int initStream(const char* pStreamFile)
{
	FILE* fp = fopen(pStreamFile, "rb");
	if (!fp)
	{
		printf("open file[%s] error\n", pStreamFile);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	int fileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (streamSpace < fileLen)
	{
		fclose(fp);
		return -1;
	}

	if (fread(pStream, 1, fileLen, fp) == fileLen)
	{
		printf("Load stream file success!\n");
		nStreamLen = fileLen;
		fclose(fp);
		return fileLen;
	}
	else
	{
		fclose(fp);
		return -1;
	}
}

int nextNalLength(int Pos)
{
	int h265BitsPos = Pos;
	int naluLen = 0;
	int sliceLen = 0;

	while (h265BitsPos < (nStreamLen - 6))
	{
		int startCode3 = FindStartCode3Bytes(&pStream[h265BitsPos]);
		int startCode4 = FindStartCode4Bytes(&pStream[h265BitsPos]);
		if (!(startCode3 || startCode4))
			h265BitsPos++;
		else
		{
			int startCodeLen = startCode3 ? 3 : 4;
			unsigned char uNaluType = GetNaluType(&pStream[h265BitsPos], startCodeLen);

			if (IsSliceNalu(uNaluType))
			{
				naluLen = GetSliceNaluDataLen(h265BitsPos+startCodeLen, nStreamLen, pStream) + startCodeLen;
				sliceLen += naluLen;
				//ParseNaluData(sliceLen, pStream + h265BitsPos + 4);
				break;
			}
			else
			{
				naluLen = GetNaluDataLen(h265BitsPos + startCodeLen, nStreamLen, pStream) + startCodeLen;
				h265BitsPos += naluLen;
				sliceLen += naluLen;
				continue;
			}
		}
	}
	return sliceLen;
}

void readOneNal(unsigned char* pout, int Pos, int nLen)
{
	memcpy(pout, pStream + Pos, nLen);
}


static int FindStartCode3Bytes(unsigned char* scData)
{
	int isFind = 0;

	if ((0 == scData[0]) && (0 == scData[1]) && (1 == scData[2]))
	{
		isFind = 1;
	}

	return isFind;
}

static int FindStartCode4Bytes(unsigned char* scData)
{
	int isFind = 0;

	if ((0 == scData[0]) && (0 == scData[1]) && (0 == scData[2]) && (1 == scData[3]))
	{
		isFind = 1;
	}

	return isFind;
}

static unsigned char GetNaluType(unsigned char* pInBuffer, int s32NaluStartPos)
{
	return (pInBuffer[s32NaluStartPos] >> 1) & 0x3f;
}

static unsigned char IsSliceNalu(unsigned char u8Nal_unit_type)
{
	return ((u8Nal_unit_type <= 9) || ((u8Nal_unit_type >= 16) && (u8Nal_unit_type <= 21)));
}

static int GetNaluDataLen(int startPos, int h265BitsSize, unsigned char* h265Bits)
{
	int parsePos = startPos;

	while (parsePos < h265BitsSize)
	{
		if (FindStartCode3Bytes(&h265Bits[parsePos]) || FindStartCode4Bytes(&h265Bits[parsePos]))
			return parsePos - startPos;
		else
			parsePos++;
	}
	return parsePos - startPos;
}

static int GetSliceNaluDataLen(int startPos, int h265BitsSize, unsigned char* h265Bits)
{
	int parsePos = startPos;

	while (parsePos < h265BitsSize - 6)
	{
		if (FindStartCode3Bytes(&h265Bits[parsePos]))
		{
			unsigned char u8First_slice_in_pic_flag = h265Bits[parsePos + 5] >> 7;
			if (u8First_slice_in_pic_flag == 0)
			{
				parsePos += 3;
				continue;
			}
			else
				return parsePos - startPos;
		}
		else if (FindStartCode4Bytes(&h265Bits[parsePos]))
		{
			unsigned char u8First_slice_in_pic_flag = h265Bits[parsePos + 6] >> 7;
			if (u8First_slice_in_pic_flag == 0)
			{
				parsePos += 4;
				continue;
			}
			else
				return parsePos - startPos;
		}
		else
		{
			parsePos++;
			if (parsePos == h265BitsSize - 6)
				parsePos = h265BitsSize;
		}
	}

	return parsePos - startPos; 
}

static void ParseNaluData(const unsigned int naluLen, unsigned char* const nuluData)
{
	static int naluNum = 0;

	char typeStr[20] = { 0 };

	int nal_unit_type = GetNaluType(nuluData, 0);

	naluNum++;

#ifdef PRINTF_DEBUG
	switch (nal_unit_type)
	{
	case 0:
		sprintf(typeStr, "B SLICE");
		break;

	case 1:
		sprintf(typeStr, "P SLICE");
		break;

	case 19:
		sprintf(typeStr, "IDR");
		break;

	case 32:
		sprintf(typeStr, "VPS");
		break;

	case 33:
		sprintf(typeStr, "SPS");
		break;

	case 34:
		sprintf(typeStr, "PPS");
		break;

	case 39:
		sprintf(typeStr, "SEI");
		break;

	default:
		sprintf(typeStr, "NTYPE(%d)", nal_unit_type);
		break;
	}

	printf("%5d| %7s| %8d|\n", naluNum, typeStr, naluLen);
#endif
}