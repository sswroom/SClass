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
				IO::IStreamData *fd;
				UInt64 nextReadOfst;
				UInt8 *buff;
				UOSInt buffSize;
				UOSInt currOfst;
			} MKVStatus;

		public:
			MKVParser();
			virtual ~MKVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		private:
			static UOSInt ReadDataSize(MKVStatus *status, UInt64 *dataSize);
			static UOSInt ReadID(MKVStatus *status, UInt32 *eleId);
			static UOSInt ReadData(MKVStatus *status, UInt64 dataSize, UInt8 *buff);
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
