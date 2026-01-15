#ifndef _SM_WIN32_WMIREADER
#define _SM_WIN32_WMIREADER
#include "Data/ArrayListObj.hpp"
#include "DB/DBReader.h"

namespace Win32
{
	class WMIReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			UnsafeArray<const WChar> name;
			Int32 colType;
		} WMIColumn;

	private:
		void *pEnum;
		void *pObject;
		void *fObject;
		Bool isFirst;

		Data::ArrayListObj<WMIColumn*> *columns;
	public:
		WMIReader(void *pEnum);
		virtual ~WMIReader();

		virtual Bool ReadNext();
		virtual UIntOS ColCount();
		virtual IntOS GetRowChanged();

		virtual Int32 GetInt32(UIntOS colIndex);
		virtual Int64 GetInt64(UIntOS colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize);
		virtual Data::Timestamp GetTimestamp(UIntOS colIndex);
		virtual Double GetDblOrNAN(UIntOS colIndex);
		virtual Bool GetBool(UIntOS colIndex);
		virtual UIntOS GetBinarySize(UIntOS colIndex);
		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex);
		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid);

		virtual Bool IsNull(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

		Int32 GetInt32(UnsafeArray<const WChar> colName);
		UnsafeArrayOpt<WChar> GetStr(UnsafeArray<const WChar> colName, UnsafeArray<WChar> buff);
	};
}
#endif
