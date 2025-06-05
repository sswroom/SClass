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
			static UOSInt ReadDataSize(NN<MKVStatus> status, OutParam<UInt64> dataSize);
			static UOSInt ReadID(NN<MKVStatus> status, OutParam<UInt32> eleId);
			static UOSInt ReadData(NN<MKVStatus> status, UInt64 dataSize, Data::ByteArray buff);
			static UOSInt ReadBuffer(NN<MKVStatus> status);
			static Bool SkipBuffer(NN<MKVStatus> status, UOSInt skipSize);
			static Bool ReadHeader(NN<MKVStatus> status, UInt64 dataSize);
			static Optional<IO::ParsedObject> ReadSegment(NN<MKVStatus> status, UInt64 dataSize);
			static Bool ReadTrack(NN<MKVStatus> status, UInt64 dataSize);
			static Bool ReadTrackEntry(NN<MKVStatus> status, UInt64 dataSize);
			static Bool ReadVideo(NN<MKVStatus> status, UInt64 dataSize, NN<Media::FrameInfo> frameInfo);
			static Bool ReadAudio(NN<MKVStatus> status, UInt64 dataSize, NN<Media::AudioFormat> audioFmt);
		};
	}
}
#endif
