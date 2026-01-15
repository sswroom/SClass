#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBFFixWriter.h"
#include "DB/DBFFile.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

DB::DBFFixWriter::DBFFixWriter(NN<IO::SeekableStream> stm, UIntOS nCol, UnsafeArray<NN<Text::String>> colNames, UnsafeArray<const UIntOS> colSize, UnsafeArray<const UIntOS> dp, UnsafeArray<DB::DBUtil::ColType> colTypes, UInt32 codePage)
{
	UInt8 buff[128];
	UIntOS i;
	UIntOS j;
	UIntOS k;
	NEW_CLASSNN(this->enc, Text::Encoding(codePage));
	this->stm = stm;
	this->rowCnt = 0;
	this->colCnt = nCol;
	this->refPos = stm->GetPosition();
	this->columns = MemAllocArr(DB::DBFFixWriter::DBFColumn, nCol);

	Data::DateTime dt;
	dt.SetCurrTime();

	buff[0] = 3;
	buff[1] = (UInt8)(dt.GetYear() - 1900);
	buff[2] = dt.GetMonth();
	buff[3] = dt.GetDay();
	*(Int32*)&buff[4] = (Int32)this->rowCnt;
	*(Int16*)&buff[8] = (Int16)((this->colCnt << 5) + 33);

	i = 0;
	j = this->colCnt;
	k = 1;
	while (i < j)
	{
		this->columns[i].colType = colTypes[i];
		this->columns[i].decimalPoint = dp[i];
		this->columns[i].colOfst = k;
		if (colTypes[i] == DB::DBUtil::CT_DateTime)
		{
			this->columns[i].colSize = 8;
		}
		else if (colTypes[i] == DB::DBUtil::CT_Bool)
		{
			this->columns[i].colSize = 1;
		}

		this->columns[i].colSize = colSize[i];
		k += colSize[i];
		i++;
	}
	*(Int16*)&buff[10] = (Int16)k;
	*(Int32*)&buff[12] = 0;
	*(Int32*)&buff[16] = 0;
	*(Int32*)&buff[20] = 0;
	*(Int32*)&buff[24] = 0;
	buff[28] = 0;
	buff[29] = DB::DBFFile::GetLangDriver(codePage);
	*(Int16*)&buff[30] = 0;
	stm->Write(Data::ByteArrayR(buff, 32));
	this->rec = MemAlloc(UInt8, k);
	this->rec[0] = ' ';
	this->recSize = k;

	i = 0;
	k = nCol;
	while (i < k)
	{
		j = enc->UTF8ToBytes((UInt8*)buff, colNames[i]->v);
		while (j < 11)
		{
			buff[j++] = 0;
		}

		if (colTypes[i] == DB::DBUtil::CT_DateTime)
		{
			buff[11] = 'D';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Double)
		{
			buff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Byte)
		{
			buff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Int16)
		{
			buff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Int32)
		{
			buff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Int64)
		{
			buff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_UInt32)
		{
			buff[11] = 'N';
		}
		else if (colTypes[i] == DB::DBUtil::CT_Bool)
		{
			buff[11] = 'L';
		}
		else
		{
			buff[11] = 'C';
		}
		*(Int32*)&buff[12] = (Int32)this->columns[i].colOfst;
		buff[16] = (UInt8)this->columns[i].colSize;
		buff[17] = (UInt8)this->columns[i].decimalPoint;
		buff[18] = 0;
		buff[19] = 0;
		*(Int32*)&buff[20] = 0;
		*(Int32*)&buff[24] = 0;
		*(Int32*)&buff[28] = 0;
		stm->Write(Data::ByteArrayR(buff, 32));
		i++;
	}
	buff[0] = 13;
	stm->Write(Data::ByteArrayR(buff,  1));

}

DB::DBFFixWriter::~DBFFixWriter()
{
	UInt8 buff = 26;
	stm->Write(Data::ByteArrayR(&buff, 1));

	stm->SeekFromBeginning(refPos + 4);
	stm->Write(Data::ByteArrayR((UInt8*)&this->rowCnt, 4));
	MemFreeArr(this->columns);
	MemFreeArr(this->rec);
	this->enc.Delete();
}

void DB::DBFFixWriter::AddRecord(UnsafeArray<UnsafeArray<const UTF8Char>> rowValues)
{
	UIntOS k;
	UIntOS j;
	UIntOS l;
	UInt8 buff[512];

	buff[0] = 32; 
	stm->Write(Data::ByteArrayR(buff, 1));
	j = 0;
	while (j < this->colCnt)
	{
		k = enc->UTF8ToBytes(buff, rowValues[j]);
		if (this->columns[j].colType == DB::DBUtil::CT_DateTime || this->columns[j].colType == DB::DBUtil::CT_Double || this->columns[j].colType == DB::DBUtil::CT_Byte || this->columns[j].colType == DB::DBUtil::CT_Int16 || this->columns[j].colType == DB::DBUtil::CT_Int32 || this->columns[j].colType == DB::DBUtil::CT_Int64 || this->columns[j].colType == DB::DBUtil::CT_UInt32 || this->columns[j].colType == DB::DBUtil::CT_Bool)
		{
			if (this->columns[j].colSize <= k)
			{
				stm->Write(Data::ByteArrayR(buff, this->columns[j].colSize));
			}
			else
			{
				l = this->columns[j].colSize - k;
				while (l-- > 0)
				{
					buff[l + k] = 32;
				}
				stm->Write(Data::ByteArrayR(&buff[k], this->columns[j].colSize - k));
				stm->Write(Data::ByteArrayR(buff, k));
			}
		}
		else
		{
			while (k < this->columns[j].colSize)
				buff[k++] = 32;
			stm->Write(Data::ByteArrayR(buff, this->columns[j].colSize));
		}
		j += 1;
	}
	rowCnt++;
}

Bool DB::DBFFixWriter::SetColumnDT(UIntOS index, NN<Data::DateTime> val)
{
	UTF8Char sbuff[10];
	UInt8 outBuff[10];
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_DateTime)
		return false;
	val->ToString(sbuff, "yyyyMMdd");
	enc->UTF8ToBytesC(outBuff, sbuff, 8);
	MemCopyNO(&this->rec[this->columns[index].colOfst], outBuff, 8);
	return true;
}

Bool DB::DBFFixWriter::SetColumnTS(UIntOS index, const Data::Timestamp &val)
{
	UTF8Char sbuff[10];
	UInt8 outBuff[10];
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_DateTime)
		return false;
	val.ToString(sbuff, "yyyyMMdd");
	enc->UTF8ToBytesC(outBuff, sbuff, 8);
	MemCopyNO(&this->rec[this->columns[index].colOfst], outBuff, 8);
	return true;
}

Bool DB::DBFFixWriter::SetColumnF64(UIntOS index, Double val)
{
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_Double)
		return false;
	UTF8Char sbuff[60];
	UnsafeArray<UTF8Char> sptr;
	UInt8 outBuff[60];
	UIntOS i;
	UIntOS j;
	Text::StringBuilderUTF8 sb;
	i = this->columns[index].decimalPoint;
	if (i == 0)
	{
		sptr = Text::StrDouble(sbuff, val);
	}
	else
	{
		sptr = Text::StrDoubleDP(sbuff, val, i, i);
	}
	i = this->enc->UTF8ToBytesC(outBuff, sbuff, (UIntOS)(sptr - sbuff));
	if (i >= this->columns[index].colSize)
	{
		MemCopyNO(&this->rec[this->columns[index].colOfst], &outBuff[i - this->columns[index].colSize], this->columns[index].colSize);
	}
	else
	{
		j = this->columns[index].colOfst;
		MemCopyNO(&this->rec[j + this->columns[index].colSize - i], outBuff, i);
		i = this->columns[index].colSize - i;
		while (i-- > 0)
		{
			this->rec[j + i] = ' ';
		}
	}
	return true;
}

Bool DB::DBFFixWriter::SetColumnI16(UIntOS index, Int16 val)
{
	UTF8Char sbuff[12];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_Int16)
		return false;
	sptr = Text::StrInt32(sbuff, val);
	if ((UIntOS)(sptr - sbuff) >= this->columns[index].colSize)
	{
		MemCopyNO(&this->rec[this->columns[index].colOfst], sptr.Ptr() - this->columns[index].colSize, this->columns[index].colSize);
	}
	else
	{
		j = this->columns[index].colOfst;
		MemCopyNO(&this->rec[j + this->columns[index].colSize - (UIntOS)(sptr - sbuff)], sbuff, (UIntOS)(sptr - sbuff));
		i = this->columns[index].colSize - (UIntOS)(sptr - sbuff);
		while (i-- > 0)
		{
			this->rec[j + i] = ' ';
		}
	}
	return true;
}

Bool DB::DBFFixWriter::SetColumnI32(UIntOS index, Int32 val)
{
	UTF8Char sbuff[12];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_Int32)
		return false;
	sptr = Text::StrInt32(sbuff, val);
	if ((UIntOS)(sptr - sbuff) >= this->columns[index].colSize)
	{
		MemCopyNO(&this->rec[this->columns[index].colOfst], sptr.Ptr() - this->columns[index].colSize, this->columns[index].colSize);
	}
	else
	{
		j = this->columns[index].colOfst;
		MemCopyNO(&this->rec[j + this->columns[index].colSize - (UIntOS)(sptr - sbuff)], sbuff, (UIntOS)(sptr - sbuff));
		i = this->columns[index].colSize - (UIntOS)(sptr - sbuff);
		while (i-- > 0)
		{
			this->rec[j + i] = ' ';
		}
	}
	return true;
}

Bool DB::DBFFixWriter::SetColumnI64(UIntOS index, Int64 val)
{
	UTF8Char sbuff[24];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_Int64)
		return false;
	sptr = Text::StrInt64(sbuff, val);
	if ((UIntOS)(sptr - sbuff) >= this->columns[index].colSize)
	{
		MemCopyNO(&this->rec[this->columns[index].colOfst], sptr.Ptr() - this->columns[index].colSize, this->columns[index].colSize);
	}
	else
	{
		j = this->columns[index].colOfst;
		MemCopyNO(&this->rec[j + this->columns[index].colSize - (UIntOS)(sptr - sbuff)], sbuff, (UIntOS)(sptr - sbuff));
		i = this->columns[index].colSize - (UIntOS)(sptr - sbuff);
		while (i-- > 0)
		{
			this->rec[j + i] = ' ';
		}
	}
	return true;
}

Bool DB::DBFFixWriter::SetColumnU32(UIntOS index, UInt32 val)
{
	UTF8Char sbuff[24];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_UInt32)
		return false;
	sptr = Text::StrInt64(sbuff, val);
	if ((UIntOS)(sptr - sbuff) >= this->columns[index].colSize)
	{
		MemCopyNO(&this->rec[this->columns[index].colOfst], sptr.Ptr() - this->columns[index].colSize, this->columns[index].colSize);
	}
	else
	{
		j = this->columns[index].colOfst;
		MemCopyNO(&this->rec[j + this->columns[index].colSize - (UIntOS)(sptr - sbuff)], sbuff, (UIntOS)(sptr - sbuff));
		i = this->columns[index].colSize - (UIntOS)(sptr - sbuff);
		while (i-- > 0)
		{
			this->rec[j + i] = ' ';
		}
	}
	return true;
}

Bool DB::DBFFixWriter::SetColumnBool(UIntOS index, Bool val)
{
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_Bool)
		return false;
	if (val)
	{
		this->rec[this->columns[index].colOfst] = 'T';
	}
	else
	{
		this->rec[this->columns[index].colOfst] = 'F';
	}
	return true;
}

Bool DB::DBFFixWriter::SetColumnStr(UIntOS index, Text::CStringNN val)
{
	UInt8 buff[512];
	if (index >= this->colCnt)
		return false;
	if (this->columns[index].colType != DB::DBUtil::CT_UTF8Char && this->columns[index].colType != DB::DBUtil::CT_VarUTF8Char)
		return false;
	UIntOS i;
	if (val.leng >= 256)
	{
		i = enc->UTF8ToBytesC(buff, val.v, 256);
	}
	else
	{
		i = enc->UTF8ToBytes(buff, val.v);
	}
	i--;
	while (i < this->columns[index].colSize)
		buff[i++] = 32;
	MemCopyNO(&this->rec[this->columns[index].colOfst], buff, this->columns[index].colSize);
	return true;
}

void DB::DBFFixWriter::WriteRecord()
{
	stm->Write(Data::ByteArrayR(this->rec, this->recSize));
	this->rowCnt++;
}
