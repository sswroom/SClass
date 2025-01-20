#ifndef _SM_PARSER_FILEPARSER_QTPARSER
#define _SM_PARSER_FILEPARSER_QTPARSER
#include "IO/FileParser.h"
#include "Media/MediaFile.h"

namespace Parser
{
	namespace FileParser
	{
		class QTParser : public IO::FileParser
		{
		public:
			QTParser();
			virtual ~QTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

		private:
			Media::MediaFile *ParseMoovAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt64 size);
			Media::MediaSource *ParseTrakAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Int32 *trackDelay, Int32 *trackSkipMS, UInt32 mvTimeScale);
			Media::MediaSource *ParseMdiaAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, UInt32 *timeScale);
			Media::MediaSource *ParseMinfAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, UInt32 timeScale);
			Media::MediaSource *ParseStblAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, UInt32 timeScale);
			Bool ParseEdtsAtom(NN<IO::StreamData> fd, UInt64 ofst, UInt32 size, Int32 *delay, Int32 *sampleSkip);
		};
	}
}
#endif
