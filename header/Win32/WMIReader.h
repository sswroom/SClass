#ifndef _SM_WIN32_WMIREADER
#define _SM_WIN32_WMIREADER
#include "Data/ArrayList.h"
#include "DB/DBReader.h"

namespace Win32
{
	class WMIReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			const WChar *name;
			Int32 colType;
		} WMIColumn;

	private:
		void *pEnum;
		void *pObject;
		void *fObject;
		Bool isFirst;

		Data::ArrayList<WMIColumn*> *columns;
	public:
		WMIReader(void *pEnum);
		virtual ~WMIReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

		Int32 GetInt32(const WChar *colName);
		WChar *GetStr(const WChar *colName, WChar *buff);
	};
}
#endif
