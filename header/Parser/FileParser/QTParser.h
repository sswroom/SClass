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
			virtual void PrepareSelector(IO::FileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

		private:
			Media::MediaFile *ParseMoovAtom(IO::StreamData *fd, UInt64 ofst, UInt64 size);
			Media::IMediaSource *ParseTrakAtom(IO::StreamData *fd, UInt64 ofst, UInt32 size, Int32 *trackDelay, Int32 *trackSkipMS, UInt32 mvTimeScale);
			Media::IMediaSource *ParseMdiaAtom(IO::StreamData *fd, UInt64 ofst, UInt32 size, UInt32 *timeScale);
			Media::IMediaSource *ParseMinfAtom(IO::StreamData *fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, UInt32 timeScale);
			Media::IMediaSource *ParseStblAtom(IO::StreamData *fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, UInt32 timeScale);
			Bool ParseEdtsAtom(IO::StreamData *fd, UInt64 ofst, UInt32 size, Int32 *delay, Int32 *sampleSkip);
		};
	}
}
#endif
