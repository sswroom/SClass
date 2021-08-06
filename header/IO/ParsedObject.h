#ifndef _SM_IO_PARSEDOBJECT
#define _SM_IO_PARSEDOBJECT

namespace IO
{
	class ParsedObject
	{
	public:
		typedef enum
		{
			PT_UNKNOWN,
			PT_STREAM,
	//		PT_IMAGE_PARSER,
			PT_VIDEO_PARSER,
			PT_PACKAGE_PARSER,
			PT_IMAGE_LIST_PARSER,
			PT_EXE_PARSER,
			PT_MAP_LAYER_PARSER,
			PT_READINGDB_PARSER,
			PT_MAP_ENV_PARSER,
			PT_FILE_CHECK,
			PT_TEXT_DOCUMENT,
			PT_WORKBOOK,
			PT_SECTOR_DATA,
			PT_LOG_FILE,
			PT_PLAYLIST,
			PT_COORDINATE_SYSTEM,
			PT_CODEPROJECT,
			PT_VECTOR_DOCUMENT,
			PT_LUT,
			PT_FONT_RENDERER,
			PT_MIME_OBJECT,
			PT_ETHERNET_ANALYZER,
			PT_JAVA_CLASS,
			PT_SMAKE,
			PT_ASN1_DATA,
			PT_BTSCANLOG
		} ParserType;

	protected:
		const UTF8Char *sourceName;

		ParsedObject(const UTF8Char *sourceName);
	public:
		virtual ~ParsedObject();

		virtual IO::ParsedObject::ParserType GetParserType() = 0;
		UTF8Char *GetSourceName(UTF8Char *oriStr);
		const UTF8Char *GetSourceNameObj();
		void SetSourceName(const UTF8Char *sourceName);
	};
}

#endif
