#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
#include "Media/LUT.h"
#include "Text/MyString.h"

void Media::LUT::Init(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt)
{
	this->inputCh = inputCh;
	this->inputLev = inputLev;
	this->outputCh = outputCh;
	this->fmt = fmt;
	this->remark = nullptr;
	UOSInt tableSize;
	UOSInt i;
	if (fmt == Media::LUT::DF_UINT8)
	{
		tableSize = 1;
	}
	else if (fmt == Media::LUT::DF_UINT16)
	{
		tableSize = 2;
	}
	else if (fmt == Media::LUT::DF_SINGLE)
	{
		tableSize = 4;
	}
	else
	{
		tableSize = 1;
	}
	i = inputCh;
	while (i-- > 0)
	{
		tableSize *= inputLev;
	}
	tableSize = tableSize * this->outputCh;
	this->luTable.ChangeSizeAndClear(tableSize);
}

Media::LUT::LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
	this->Init(inputCh, inputLev, outputCh, fmt);
}

Media::LUT::LUT(UOSInt inputCh, UOSInt inputLev, UOSInt outputCh, DataFormat fmt, Text::CStringNN sourceName) : IO::ParsedObject(sourceName)
{
	this->Init(inputCh, inputLev, outputCh, fmt);
}

Media::LUT::~LUT()
{
	OPTSTR_DEL(this->remark);
}

IO::ParserType Media::LUT::GetParserType() const
{
	return IO::ParserType::LUT;
}

void Media::LUT::SetRemark(Optional<Text::String> remark)
{
	OPTSTR_DEL(this->remark);
	this->remark = Text::String::CopyOrNull(remark);
}

void Media::LUT::SetRemark(Text::CString remark)
{
	OPTSTR_DEL(this->remark);
	this->remark = Text::String::NewOrNull(remark);
}

Optional<Text::String> Media::LUT::GetRemark() const
{
	return this->remark;
}

UOSInt Media::LUT::GetInputCh() const
{
	return this->inputCh;
}

Media::LUT::DataFormat Media::LUT::GetFormat() const
{
	return this->fmt;
}

UOSInt Media::LUT::GetInputLevel() const
{
	return this->inputLev;
}

UOSInt Media::LUT::GetOutputCh() const
{
	return this->outputCh;
}

UInt8 *Media::LUT::GetTablePtr()
{
	return this->luTable.Arr().Ptr();
}

const UInt8 *Media::LUT::GetTablePtrRead() const
{
	return this->luTable.Arr().Ptr();
}

Data::ByteArray Media::LUT::GetTableArray() const
{
	return this->luTable;
}

void Media::LUT::GetValueUInt8(UInt32 *inputVals, UInt8 *outVals) const
{
	UOSInt indexBase = 1;
	UOSInt index = 0;
	UOSInt ofst;
	UOSInt i;
	i = 0;
	while (i < this->inputCh)
	{
		index += inputVals[i] * indexBase;
		indexBase = indexBase * this->inputLev;
		i++;
	}

	if (this->fmt == DF_UINT8)
	{
		ofst = index * this->outputCh;
		i = 0;
		while (i < this->outputCh)
		{
			outVals[i] = this->luTable[ofst + i];
			i++;
		}
	}
	else if (this->fmt == DF_UINT16)
	{
		ofst = index * this->outputCh * 2;
		i = 0;
		while (i < this->outputCh)
		{
			outVals[i] = this->luTable[ofst + i * 2 + 1];
			i++;
		}
	}
	else if (this->fmt == DF_SINGLE)
	{
		Double v;
		ofst = index * this->outputCh * 4;
		i = 0;
		while (i < this->outputCh)
		{
			v = ReadFloat(&this->luTable[ofst + i * 4]) * 255;
			if (v > 255)
			{
				outVals[i] = 255;
			}
			else if (v < 0)
			{
				outVals[i] = 0;
			}
			else
			{
				outVals[i] = (UInt8)Double2Int32(v);
			}
			i++;
		}
	}
}

void Media::LUT::GetValueUInt16(UInt32 *inputVals, UInt16 *outVals) const
{
	UOSInt indexBase = 1;
	UOSInt index = 0;
	UOSInt ofst;
	UOSInt i;
	i = 0;
	while (i < this->inputCh)
	{
		index += inputVals[i] * indexBase;
		indexBase = indexBase * this->inputLev;
		i++;
	}

	if (this->fmt == DF_UINT8)
	{
		ofst = index * this->outputCh;
		i = 0;
		while (i < this->outputCh)
		{
			UInt8 v = this->luTable[ofst + i];
			outVals[i] = (UInt16)(v | (UInt16)(v << 8));
			i++;
		}
	}
	else if (this->fmt == DF_UINT16)
	{
		ofst = index * this->outputCh * 2;
		i = 0;
		while (i < this->outputCh)
		{
			outVals[i] = ReadUInt16(&this->luTable[ofst + i * 2]);
			i++;
		}
	}
	else if (this->fmt == DF_SINGLE)
	{
		Double v;
		ofst = index * this->outputCh * 4;
		i = 0;
		while (i < this->outputCh)
		{
			v = ReadFloat(&this->luTable[ofst + i * 4]) * 65535.0;
			if (v > 65535)
			{
				outVals[i] = 65535;
			}
			else if (v < 0)
			{
				outVals[i] = 0;
			}
			else
			{
				outVals[i] = (UInt16)Double2Int32(v);
			}
			i++;
		}
	}
}

void Media::LUT::GetValueSingle(UInt32 *inputVals, Single *outVals) const
{
	UOSInt indexBase = 1;
	UOSInt index = 0;
	UOSInt ofst;
	UOSInt i;
	i = 0;
	while (i < this->inputCh)
	{
		index += inputVals[i] * indexBase;
		indexBase = indexBase * this->inputLev;
		i++;
	}

	if (this->fmt == DF_UINT8)
	{
		ofst = index * this->outputCh;
		i = 0;
		while (i < this->outputCh)
		{
			UInt8 v = this->luTable[ofst + i];
			outVals[i] = (Single)(v / 255.0);
			i++;
		}
	}
	else if (this->fmt == DF_UINT16)
	{
		ofst = index * this->outputCh * 2;
		i = 0;
		while (i < this->outputCh)
		{
			UInt16 v = ReadUInt16(&this->luTable[ofst + i * 2]);
			outVals[i] = (Single)(v / 65535.0);
			i++;
		}
	}
	else if (this->fmt == DF_SINGLE)
	{
		ofst = index * this->outputCh * 4;
		i = 0;
		while (i < this->outputCh)
		{
			outVals[i] = ReadFloat(&this->luTable[ofst + i * 4]);
			i++;
		}
	}
}

Media::LUT *Media::LUT::Clone() const
{
	Media::LUT *newLut;
	NEW_CLASS(newLut, Media::LUT(this->inputCh, this->inputLev, this->outputCh, this->fmt, this->sourceName));
	if (!this->remark.IsNull())
	{
		newLut->SetRemark(this->remark);
	}
/*	UOSInt tableSize;
	UOSInt i;
	if (fmt == Media::LUT::DF_UINT8)
	{
		tableSize = 1;
	}
	else if (fmt == Media::LUT::DF_UINT16)
	{
		tableSize = 2;
	}
	else if (fmt == Media::LUT::DF_SINGLE)
	{
		tableSize = 4;
	}
	else
	{
		tableSize = 1;
	}
	i = inputCh;
	while (i-- > 0)
	{
		tableSize *= inputLev;
	}
	tableSize = tableSize * this->outputCh;*/
	newLut->luTable.CopyFrom(this->luTable);
	return newLut;
}

Bool Media::LUT::Equals(Media::LUT *lut) const
{
	if (this->fmt != lut->fmt)
		return false;
	if (this->inputLev != lut->inputLev)
		return false;
	if (this->outputCh != lut->outputCh || this->inputCh != lut->inputCh)
		return false;
	UOSInt i;
	UOSInt j = 1;
	i = inputCh;
	while (i-- > 0)
	{
		j *= this->inputLev;
	}
	j = j * this->outputCh;
	if (this->fmt == Media::LUT::DF_UINT8)
	{
		UInt8 *stab = (UInt8*)this->luTable.Arr().Ptr();
		UInt8 *dtab = (UInt8*)lut->luTable.Arr().Ptr();
		while (i < j)
		{
			if (stab[i] != dtab[i])
				return false;
			i++;
		}
	}
	else if (this->fmt == Media::LUT::DF_UINT16)
	{
		UInt16 *stab = (UInt16*)this->luTable.Arr().Ptr();
		UInt16 *dtab = (UInt16*)lut->luTable.Arr().Ptr();
		while (i < j)
		{
			if (stab[i] != dtab[i])
				return false;
			i++;
		}
	}
	else if (this->fmt == Media::LUT::DF_SINGLE)
	{
		Single *stab = (Single*)this->luTable.Arr().Ptr();
		Single *dtab = (Single*)lut->luTable.Arr().Ptr();
		while (i < j)
		{
			if (stab[i] != dtab[i])
				return false;
			i++;
		}
	}
	else
	{
		return false;
	}
	return true;
}
