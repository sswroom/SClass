#ifndef _SM_PARSER_FILEPARSER_MKVPARSER
#define _SM_PARSER_FILEPARSER_MKVPARSER
#include "IO/FileParser.h"
#include "Media/AudioFormat.h"
#include "Media/FrameInfo.h"

namespace Parser
{
	namespace FileParser
	{
		///////////////////////////////////////////
		class MKVParser : public IO::FileParser
		{
		private:
			typedef struct
			{
				NN<IO::StreamData> fd;
				UInt64 nextReadOfst;
				Data::ByteArray buff;
				UOSInt buffSize;
				UOSInt currOfst;
			} MKVStatus;

		public:
			MKVParser();
			virtual ~MKVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		private:
			static UOSInt ReadDataSize(MKVStatus *status, UInt64 *dataSize);
			static UOSInt ReadID(MKVStatus *status, UInt32 *eleId);
			static UOSInt ReadData(MKVStatus *status, UInt64 dataSize, Data::ByteArray buff);
			static UOSInt ReadBuffer(MKVStatus *status);
			static Bool SkipBuffer(MKVStatus *status, UOSInt skipSize);
			static Bool ReadHeader(MKVStatus *status, UInt64 dataSize);
			static IO::ParsedObject *ReadSegment(MKVStatus *status, UInt64 dataSize);
			static Bool ReadTrack(MKVStatus *status, UInt64 dataSize);
			static Bool ReadTrackEntry(MKVStatus *status, UInt64 dataSize);
			static Bool ReadVideo(MKVStatus *status, UInt64 dataSize, Media::FrameInfo *frameInfo);
			static Bool ReadAudio(MKVStatus *status, UInt64 dataSize, Media::AudioFormat *audioFmt);
		};
	}
}
#endif
