#ifndef _SM_DB_DBREADER
#define _SM_DB_DBREADER
#include "Data/ByteBuffer.h"
#include "Data/Class.h"
#include "Data/StringMap.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class ColDef;
	class TableDef;
	class DBReader
	{
	public:
		enum class DateErrType
		{
			Ok = 0,
			Null = 1,
			Error = 2
		};

	protected:
		DBReader(){};
		virtual ~DBReader(){};

	public:
		virtual Bool ReadNext() = 0;
		virtual UOSInt ColCount() = 0;
		virtual OSInt GetRowChanged() = 0; //-1 = error
		virtual Int32 GetInt32(UOSInt colIndex) = 0;
		NInt32 GetNInt32(UOSInt colIndex)
		{
			if (IsNull(colIndex))
				return nullptr;
			else
				return GetInt32(colIndex);
		}
		virtual Int64 GetInt64(UOSInt colIndex) = 0;
		virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff) = 0;
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb) = 0;
		Bool GetStrN(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb) { sb->ClearStr(); return GetStr(colIndex, sb); }
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex) = 0;
		NN<Text::String> GetNewStrNN(UOSInt colIndex)
		{
			return Text::String::OrEmpty(GetNewStr(colIndex));
		}
		
		Optional<Text::String> GetNewStrB(UOSInt colIndex, NN<Text::StringBuilderUTF8> tmpBuff)
		{
			tmpBuff->ClearStr();
			if (GetStr(colIndex, tmpBuff))
				return Text::String::New(tmpBuff->ToCString());
			else
				return 0;
		}

		NN<Text::String> GetNewStrBNN(UOSInt colIndex, NN<Text::StringBuilderUTF8> tmpBuff)
		{
			tmpBuff->ClearStr();
			if (GetStr(colIndex, tmpBuff))
				return Text::String::New(tmpBuff->ToCString());
			else
				return Text::String::NewEmpty();
		}
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize) = 0;
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex) = 0;
		virtual Data::Date GetDate(UOSInt colIndex)
		{
			return GetTimestamp(colIndex).ToDate();
		}

		DateErrType GetAsDateTime(UOSInt colIndex, NN<Data::DateTime> outVal)
		{
			if (IsNull(colIndex))
				return DateErrType::Null;
			Data::Timestamp ts = GetTimestamp(colIndex);
			if (ts.inst.sec == 0)
				return DateErrType::Error;
			outVal->SetValue(ts.inst, ts.tzQhr);
			return DateErrType::Ok;
		}

		Int64 GetTicks(UOSInt colIndex)
		{
			return GetTimestamp(colIndex).ToTicks();
		}

		virtual Double GetDbl(UOSInt colIndex) = 0;
		virtual Bool GetBool(UOSInt colIndex) = 0;
		virtual UOSInt GetBinarySize(UOSInt colIndex) = 0;
		virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff) = 0;
		Optional<Data::ByteBuffer> GetNewByteBuff(UOSInt colIndex)
		{
			if (IsNull(colIndex))
				return 0;
			UOSInt byteSize = GetBinarySize(colIndex);
			NN<Data::ByteBuffer> buff;
			NEW_CLASSNN(buff, Data::ByteBuffer(byteSize));
			if (GetBinary(byteSize, buff->Arr()) == byteSize)
				return buff;
			buff.Delete();
			return 0;
		}

		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex) = 0;
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid) = 0;
		virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item);

		virtual Bool IsNull(UOSInt colIndex) = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff) = 0;
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize) = 0;
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef) = 0;

		NN<TableDef> GenTableDef(Text::CString schemaName, Text::CStringNN tableName);
		NN<Data::VariObject> CreateVariObject();
		NN<Data::Class> CreateClass();
	};
}
#endif
