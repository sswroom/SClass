#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "DB/DBFWrite.h"
#include "IO/FileStream.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

DB::DBFWrite::DBFWrite(Int32 nCol, const UTF8Char **colNames, Int32 outputCodePage, const DB::DBUtil::ColType *colTypes, const Int32 *colSizes) : enc(outputCodePage)
{
	Int32 i;
	this->colCnt = nCol;
	i = nCol;
	this->cols = MemAlloc(const UTF8Char*, nCol);
	this->strSize = MemAlloc(OSInt, nCol);
	this->colTypes = MemAlloc(DB::DBUtil::ColType, nCol);
	this->colSizes = MemAlloc(Int32, nCol);
	while (i-- > 0)
	{
		this->cols[i] = Text::StrCopyNew(colNames[i]);
		this->strSize[i] = colSizes[i];
		this->colTypes[i] = colTypes[i];
		this->colSizes[i] = colSizes[i];
	}
}

DB::DBFWrite::~DBFWrite()
{
	OSInt i = this->values.GetCount();
	Int32 j;
	Char **val;
	while (i-- > 0)
	{
		val = this->values.GetItem(i);
		j = this->colCnt;
		while (j-- > 0)
		{
			MemFree(val[j]);
		}
		MemFree(val);
	}

	i = this->colCnt;
	while (i-- > 0)
	{
		Text::StrDelNew(this->cols[i]);
	}
	MemFree(this->colTypes);
	MemFree(this->colSizes);
	MemFree(this->cols);
	MemFree(this->strSize);
}

void DB::DBFWrite::AddRecord(const UTF8Char **rowValues)
{
	UInt8 buff[1024];
	OSInt colSize;
	Int32 i = this->colCnt;
	const UTF8Char *wPtr;
	Char **row;
	Char *src;
	Char *dest;
	row = MemAlloc(Char*, this->colCnt);
	while (i-- > 0)
	{
		wPtr = rowValues[i];
		while (*wPtr++);
		colSize = this->enc.UTF8ToBytesC(buff, rowValues[i], wPtr - rowValues[i] - 1);
		dest = row[i] = MemAlloc(Char, colSize + 1);
		src = (Char*)buff;

		if (this->strSize[i] < colSize)
			this->strSize[i] = colSize;

		while (colSize--)
			*dest++ = *src++;
		*dest = 0;
	}
	this->values.Add(row);
}

void DB::DBFWrite::Save(IO::Stream *stm)
{
	UInt8 byteBuff[1024];
	Data::DateTime dt;
	byteBuff[0] = 3;
	byteBuff[1] = dt.GetYear() - 1900;
	byteBuff[2] = dt.GetMonth();
	byteBuff[3] = dt.GetDay();
	*(Int32*)&byteBuff[4] = (Int32)this->values.GetCount();
	*(Int16*)&byteBuff[8] = (this->colCnt << 5) + 33;
	OSInt i = 0;
	OSInt j = this->colCnt;
	while (j-- > 0)
	{
		i += strSize[j];
	}
	*(Int16*)&byteBuff[10] = (Int16)(i + 1);
	*(Int32*)&byteBuff[12] = 0;
	*(Int32*)&byteBuff[16] = 0;
	*(Int32*)&byteBuff[20] = 0;
	*(Int32*)&byteBuff[24] = 0;
	*(Int32*)&byteBuff[28] = 0;
	stm->Write(byteBuff, 32);

	i = 0;
	j = this->colCnt;
	while (i < j)
	{
		OSInt retSize;
		retSize = this->enc.UTF8ToBytes(byteBuff, cols[i]);
		if (retSize > 10)
			retSize = 10;
		while (retSize < 11)
			byteBuff[retSize++] = 0;
		if (colTypes[i] == DB::DBUtil::CT_DateTime)
		{
			byteBuff[11] = 'D';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Double)
		{
			byteBuff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Byte)
		{
			byteBuff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Int16)
		{
			byteBuff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Int32)
		{
			byteBuff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Int64)
		{
			byteBuff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_UInt32)
		{
			byteBuff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Bool)
		{
			byteBuff[11] = 'L';
		}
		else
		{
			byteBuff[11] = 'C';
		}
		*(Int32*)&byteBuff[16] = (Int32)strSize[i];
		stm->Write(byteBuff, 32);
		i += 1;
	}
	byteBuff[0] = 13;
	stm->Write(byteBuff,  1);

	OSInt k;
	OSInt l;
	i = 0;
	while (i < this->values.GetCount())
	{
		byteBuff[0] = 32;
		stm->Write(byteBuff, 1);
		Char **rec = this->values.GetItem(i);
		j = 0;
		while (j < this->colCnt)
		{
			stm->Write((UInt8*)rec[j], k = Text::StrCharCnt(rec[j]));
			k = strSize[j] - k;
			l = 0;
			while (l < k)
			{
				byteBuff[l++] = 0x20;
			}
			stm->Write(byteBuff, k);
			j += 1;
		}
		i += 1;
	}
	byteBuff[0] = 26;
	stm->Write(byteBuff, 1);
}
