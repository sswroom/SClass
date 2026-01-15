#ifndef _SM_DB_DBREADER
#define _SM_DB_DBREADER
#include "Data/ByteBuffer.h"
#include "Data/Class.h"
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
		virtual UIntOS ColCount() = 0;
		virtual IntOS GetRowChanged() = 0; //-1 = error
		virtual Int32 GetInt32(UIntOS colIndex) = 0;
		NInt32 GetNInt32(UIntOS colIndex)
		{
			if (IsNull(colIndex))
				return nullptr;
			else
				return GetInt32(colIndex);
		}
		virtual Int64 GetInt64(UIntOS colIndex) = 0;
		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff) = 0;
		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb) = 0;
		Bool GetStrN(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb) { sb->ClearStr(); return GetStr(colIndex, sb); }
		virtual Optional<Text::String> GetNewStr(UIntOS colIndex) = 0;
		NN<Text::String> GetNewStrNN(UIntOS colIndex) { return Text::String::OrEmpty(GetNewStr(colIndex)); }
		
		Optional<Text::String> GetNewStrB(UIntOS colIndex, NN<Text::StringBuilderUTF8> tmpBuff)
		{
			tmpBuff->ClearStr();
			if (GetStr(colIndex, tmpBuff))
				return Text::String::New(tmpBuff->ToCString());
			else
				return nullptr;
		}

		NN<Text::String> GetNewStrBNN(UIntOS colIndex, NN<Text::StringBuilderUTF8> tmpBuff)
		{
			tmpBuff->ClearStr();
			if (GetStr(colIndex, tmpBuff))
				return Text::String::New(tmpBuff->ToCString());
			else
				return Text::String::NewEmpty();
		}
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize) = 0;
		virtual Data::Timestamp GetTimestamp(UIntOS colIndex) = 0;
		virtual Data::Date GetDate(UIntOS colIndex) { return GetTimestamp(colIndex).ToDate(); }

		DateErrType GetAsDateTime(UIntOS colIndex, NN<Data::DateTime> outVal)
		{
			if (IsNull(colIndex))
				return DateErrType::Null;
			Data::Timestamp ts = GetTimestamp(colIndex);
			if (ts.inst.sec == 0)
				return DateErrType::Error;
			outVal->SetValue(ts.inst, ts.tzQhr);
			return DateErrType::Ok;
		}
		Int64 GetTicks(UIntOS colIndex) { return GetTimestamp(colIndex).ToTicks(); }

		virtual Double GetDblOrNAN(UIntOS colIndex) = 0;
		Double GetDblOr(UIntOS colIndex, Double v) { Double ret = this->GetDblOrNAN(colIndex); return Math::IsNAN(ret)?v:ret; }

		virtual Bool GetBool(UIntOS colIndex) = 0;
		virtual UIntOS GetBinarySize(UIntOS colIndex) = 0;
		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff) = 0;
		Optional<Data::ByteBuffer> GetNewByteBuff(UIntOS colIndex)
		{
			if (IsNull(colIndex))
				return nullptr;
			UIntOS byteSize = GetBinarySize(colIndex);
			NN<Data::ByteBuffer> buff;
			NEW_CLASSNN(buff, Data::ByteBuffer(byteSize));
			if (GetBinary(byteSize, buff->Arr()) == byteSize)
				return buff;
			buff.Delete();
			return nullptr;
		}

		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex) = 0;
		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid) = 0;
		virtual Bool GetVariItem(UIntOS colIndex, NN<Data::VariItem> item);

		virtual Bool IsNull(UIntOS colIndex) = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff) = 0;
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize) = 0;
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef) = 0;
		virtual UInt64 GetRowFileOfst() const { return 0; }

		NN<TableDef> GenTableDef(Text::CString schemaName, Text::CStringNN tableName);
		NN<Data::VariObject> CreateVariObject();
		NN<Data::Class> CreateClass();
	};
}
#endif
