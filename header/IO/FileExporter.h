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
		enum class SupportType
		{
			NotSupported,
			NormalStream,
			MultiFiles,
			PathOnly
		};

		enum class ParamType
		{
			INT32,
			STRINGUTF8,
			SELECTION,
			BOOL
		};

		struct ParamInfo
		{
			Text::CString name;
			ParamType paramType;
			Bool allowNull;
		};
	public:
		virtual ~FileExporter(){};

		virtual Int32 GetName() = 0;
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj) = 0;
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff) = 0;
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param) = 0;
		Bool ExportNewFile(Text::CString fileName, IO::ParsedObject *pobj, void *param);

		virtual void SetCodePage(UInt32 codePage);
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
