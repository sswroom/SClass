#ifndef _SM_PARSER_FILEPARSER_MAPSFORGEPARSER
#define _SM_PARSER_FILEPARSER_MAPSFORGEPARSER
#include "IO/FileParser.h"
#include "Map/OSM/OSMData.h"

namespace Parser
{
	namespace FileParser
	{
		class MapsforgeParser : public IO::FileParser
		{
		private:
			struct TagInfo
			{
				NN<Text::String> k;
				NN<Text::String> v;
			};
		public:
			MapsforgeParser();
			virtual ~MapsforgeParser();
			
			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		private:
			static void ParseSubFile(NN<IO::StreamData> fd, NN<Map::OSM::OSMData> osmData, UnsafeArray<TagInfo> nodeTags, UnsafeArray<TagInfo> wayTags, UInt8 baseZoomLevel, UInt8 minZoomLevel, UInt8 maxZoomLevel, UInt64 subfileOfst, UInt64 subfileSize, InOutParam<UIntOS> nodeCnt, InOutParam<UIntOS> wayCnt, UIntOS nodeTagsCnt, UIntOS wayTagsCnt);
		};
	}
}
#endif
