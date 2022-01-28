#ifndef _SM_IO_PARSEDOBJECT
#define _SM_IO_PARSEDOBJECT
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	enum class ParserType
	{
		Unknown,
		Stream,
//		Image,
		MediaFile,
		PackageFile,
		ImageList,
		EXEFile,
		MapLayer,
		ReadingDB,
		MapEnv,
		FileCheck,
		TextDocument,
		Workbook,
		SectorData,
		LogFile,
		Playlist,
		CoordinateSystem,
		CodeProject,
		VectorDocument,
		LUT,
		FontRenderer,
		MIMEObject,
		EthernetAnalyzer,
		JavaClass,
		Smake,
		ASN1Data,
		BTScanLog,
		SystemInfoLog
	};

	class ParsedObject
	{
	protected:
		Text::String *sourceName;

		ParsedObject(Text::String *sourceName);
		ParsedObject(const UTF8Char *sourceName);
		ParsedObject(Text::CString sourceName);
	public:
		virtual ~ParsedObject();

		virtual IO::ParserType GetParserType() = 0;
		UTF8Char *GetSourceName(UTF8Char *oriStr);
		Text::String *GetSourceNameObj();
		void SetSourceName(Text::String *sourceName);
		void SetSourceName(const UTF8Char *sourceName, UOSInt nameLen);
	};
}

#endif
