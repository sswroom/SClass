#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/ColDef.h"
#include "DB/CSVMonthGroupedReader.h"
#include "IO/Path.h"

DB::CSVMonthGroupedReader::CSVMonthGroupedReader(Text::CStringNN filePart1, Text::CStringNN filePart2, Text::CStringNN filePart3, Data::DataArray<Text::CStringNN> tableColNames, UIntOS dateCol, UIntOS dataOfst, UIntOS maxCount, Optional<Data::QueryConditions> condition, NN<Data::ArrayListNative<UIntOS>> colIndex, Bool noHeader)
{
	this->filePart1 = Text::String::New(filePart1);
	this->filePart2 = Text::String::New(filePart2);
	this->filePart3 = Text::String::New(filePart3);
	this->currDate = Data::Date::UTCToday();
	this->tableColNames = tableColNames;
	this->dateCol = dateCol;
	this->dataOfst = dataOfst;
	this->countLeft = maxCount;
	if (maxCount == 0)
	{
		this->countLeft = INVALID_INDEX;
	}
	NN<Data::QueryConditions> nncond;
	if (condition.SetTo(nncond))
	{
		this->condition = nncond->Clone();
	}
	else
	{
		this->condition = nullptr;
	}
	this->colIndex.AddAll(colIndex);
	this->noHeader = noHeader;
	this->currReader = nullptr;

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	sptr = filePart1.ConcatTo(sbuff);
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		UInt32 month;
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, nullptr, pt, nullptr).SetTo(sptr2))
		{
			if (pt == IO::Path::PathType::Directory && (sptr2 - sptr) == 6 && Text::StrToUInt32(sptr, month))
			{
				this->monthList.Add(month);
			}
		}
		Data::Sort::ArtificialQuickSort::Sort<UInt32>(this->monthList, this->monthList);
		IO::Path::FindFileClose(sess);
		printf("Found %d months\n", (Int32)this->monthList.GetCount());
	}
	///////////////////////////////
}

DB::CSVMonthGroupedReader::~CSVMonthGroupedReader()
{
	this->filePart1->Release();
	this->filePart2->Release();
	this->filePart3->Release();
	this->currReader.Delete();
	this->condition.Delete();
}

Text::CStringNN DB::CSVMonthGroupedReader::GetClassName() const
{
	return CSTR("CSVMonthGroupedReader");
}

Bool DB::CSVMonthGroupedReader::ReadNext()
{
	NN<DB::CSVReader> reader;
	if (this->countLeft == 0)
		return false;
	if (this->currReader.SetTo(reader))
	{
		if (reader->ReadNext())
			return true;
		this->currReader.Delete();
	}
	
	/////////////////////
	return false;
}

UIntOS DB::CSVMonthGroupedReader::ColCount()
{
	return this->colIndex.GetCount();
}

IntOS DB::CSVMonthGroupedReader::GetRowChanged()
{
	return 0;
}

Int32 DB::CSVMonthGroupedReader::GetInt32(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetInt32(this->colIndex.GetItem(colIndex));
	return 0;
}

Int64 DB::CSVMonthGroupedReader::GetInt64(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetInt64(this->colIndex.GetItem(colIndex));
	return 0;
}

UnsafeArrayOpt<WChar> DB::CSVMonthGroupedReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetStr(this->colIndex.GetItem(colIndex), buff);
	return nullptr;
}

Bool DB::CSVMonthGroupedReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetStr(this->colIndex.GetItem(colIndex), sb);
	return false;
}

Optional<Text::String> DB::CSVMonthGroupedReader::GetNewStr(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetNewStr(this->colIndex.GetItem(colIndex));
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> DB::CSVMonthGroupedReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetStr(this->colIndex.GetItem(colIndex), buff, buffSize);
	return nullptr;
}

Data::Timestamp DB::CSVMonthGroupedReader::GetTimestamp(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetTimestamp(this->colIndex.GetItem(colIndex));
	return nullptr;
}

Double DB::CSVMonthGroupedReader::GetDblOrNAN(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetDblOrNAN(this->colIndex.GetItem(colIndex));
	return NAN;
}

Bool DB::CSVMonthGroupedReader::GetBool(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetBool(this->colIndex.GetItem(colIndex));
	return false;
}

UIntOS DB::CSVMonthGroupedReader::GetBinarySize(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetBinarySize(this->colIndex.GetItem(colIndex));
	return 0;
}

UIntOS DB::CSVMonthGroupedReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetBinary(this->colIndex.GetItem(colIndex), buff);
	return 0;
}

Optional<Math::Geometry::Vector2D> DB::CSVMonthGroupedReader::GetVector(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetVector(this->colIndex.GetItem(colIndex));
	return nullptr;
}

Bool DB::CSVMonthGroupedReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetUUID(this->colIndex.GetItem(colIndex), uuid);
	return false;
}

Bool DB::CSVMonthGroupedReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->GetVariItem(this->colIndex.GetItem(colIndex), item);
	return false;
}

UnsafeArrayOpt<UTF8Char> DB::CSVMonthGroupedReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
{
	if (colIndex < this->colIndex.GetCount())
	{
		return this->tableColNames[this->colIndex.GetItem(colIndex)].ConcatTo(buff);
	}
	return nullptr;
}

Bool DB::CSVMonthGroupedReader::IsNull(UIntOS colIndex)
{
	NN<DB::CSVReader> reader;
	if (colIndex < this->colIndex.GetCount() && this->currReader.SetTo(reader))
		return reader->IsNull(this->colIndex.GetItem(colIndex));
	return false;
}

DB::DBUtil::ColType DB::CSVMonthGroupedReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	if (colIndex < this->colIndex.GetCount())
	{
		colSize.Set(0xffff);
		return DB::DBUtil::CT_VarUTF8Char;
	}
	return DB::DBUtil::CT_Unknown;
}

Bool DB::CSVMonthGroupedReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
{
	if (colIndex < this->colIndex.GetCount())
	{
		colDef->SetColName(this->tableColNames[this->colIndex.GetItem(colIndex)]);
		colDef->SetColSize(0xffff);
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		return true;
	}
	return false;
}

NN<Data::VariItem> DB::CSVMonthGroupedReader::GetNewItem(Text::CStringNN name)
{
	NN<DB::CSVReader> reader;
	if (this->currReader.SetTo(reader))
		return reader->GetNewItem(name);
	return Data::VariItem::NewNull();
}
