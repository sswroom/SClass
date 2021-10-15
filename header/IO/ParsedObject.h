#ifndef _SM_IO_PARSEDOBJECT
#define _SM_IO_PARSEDOBJECT

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
		const UTF8Char *sourceName;

		ParsedObject(const UTF8Char *sourceName);
	public:
		virtual ~ParsedObject();

		virtual IO::ParserType GetParserType() = 0;
		UTF8Char *GetSourceName(UTF8Char *oriStr);
		const UTF8Char *GetSourceNameObj();
		void SetSourceName(const UTF8Char *sourceName);
	};
}

#endif
