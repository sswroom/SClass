#ifndef _SM_IO_FILEEXPORTER
#define _SM_IO_FILEEXPORTER
#include "IO/SeekableStream.h"
#include "IO/ParsedObject.h"
#include "Text/EncodingFactory.h"

namespace IO
{
	class FileExporter
	{
	public:
		typedef enum
		{
			ST_NOT_SUPPORTED = 0,
			ST_NORMAL_STREAM = 1,
			ST_MULTI_FILES = 2,
			ST_PATH_ONLY = 3
		} SupportType;

		typedef enum
		{
			PT_INT32,
			PT_STRINGUTF8,
			PT_SELECTION,
			PT_BOOL
		} ParamType;

		typedef struct
		{
			const UTF8Char *name;
			ParamType paramType;
			Bool allowNull;
		} ParamInfo;
	public:
		virtual ~FileExporter(){};

		virtual Int32 GetName() = 0;
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj) = 0;
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff) = 0;
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param) = 0;

		virtual void SetCodePage(Int32 codePage);
		virtual void SetEncFactory(Text::EncodingFactory *encFact);

		virtual UOSInt GetParamCnt();
		virtual void *CreateParam(IO::ParsedObject *pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(UOSInt index, ParamInfo *info);
		virtual Bool SetParamStr(void *param, UOSInt index, const UTF8Char *val);
		virtual Bool SetParamInt32(void *param, UOSInt index, Int32 val);
		virtual Bool SetParamSel(void *param, UOSInt index, UOSInt selCol);
		virtual Bool SetParamBool(void *param, UOSInt index, Bool val);
		virtual UTF8Char *GetParamStr(void *param, UOSInt index, UTF8Char *buff);
		virtual Int32 GetParamInt32(void *param, UOSInt index);
		virtual Int32 GetParamSel(void *param, UOSInt index);
		virtual UTF8Char *GetParamSelItems(void *param, UOSInt index, UOSInt itemIndex, UTF8Char *buff);
		virtual Bool GetParamBool(void *param, UOSInt index);
	};
}
#endif
