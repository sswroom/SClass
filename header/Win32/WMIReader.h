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
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
		virtual const UTF8Char *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal); //return 0 = ok, 1 = NULL, 2 = Error
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual void DelNewStr(const UTF8Char *s);

		Int32 GetInt32(const WChar *colName);
		WChar *GetStr(const WChar *colName, WChar *buff);
	};
}
#endif
