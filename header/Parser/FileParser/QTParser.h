#ifndef _SM_PARSER_FILEPARSER_QTPARSER
#define _SM_PARSER_FILEPARSER_QTPARSER
#include "IO/IFileParser.h"
#include "Media/MediaFile.h"

namespace Parser
{
	namespace FileParser
	{
		class QTParser : public IO::IFileParser
		{
		public:
			QTParser();
			virtual ~QTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);

		private:
			Media::MediaFile *ParseMoovAtom(IO::IStreamData *fd, UInt64 ofst, Int64 size);
			Media::IMediaSource *ParseTrakAtom(IO::IStreamData *fd, UInt64 ofst, UInt32 size, Int32 *trackDelay, Int32 *trackSkipMS, Int32 mvTimeScale);
			Media::IMediaSource *ParseMdiaAtom(IO::IStreamData *fd, UInt64 ofst, UInt32 size, Int32 *timeScale);
			Media::IMediaSource *ParseMinfAtom(IO::IStreamData *fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, Int32 timeScale);
			Media::IMediaSource *ParseStblAtom(IO::IStreamData *fd, UInt64 ofst, UInt32 size, Media::MediaType mtyp, Int32 timeScale);
			Bool ParseEdtsAtom(IO::IStreamData *fd, UInt64 ofst, UInt32 size, Int32 *delay, Int32 *sampleSkip);
		};
	};
};
#endif
