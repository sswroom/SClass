#ifndef _SM_PARSER_FILEPARSER_MKVPARSER
#define _SM_PARSER_FILEPARSER_MKVPARSER
#include "IO/IFileParser.h"
#include "Media/AudioFormat.h"
#include "Media/FrameInfo.h"

namespace Parser
{
	namespace FileParser
	{
		///////////////////////////////////////////
		class MKVParser : public IO::IFileParser
		{
		private:
			typedef struct
			{
				IO::IStreamData *fd;
				Int64 nextReadOfst;
				UInt8 *buff;
				OSInt buffSize;
				OSInt currOfst;
			} MKVStatus;

		public:
			MKVParser();
			virtual ~MKVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		private:
			static OSInt ReadDataSize(MKVStatus *status, Int64 *dataSize);
			static OSInt ReadID(MKVStatus *status, UInt32 *eleId);
			static OSInt ReadData(MKVStatus *status, Int64 dataSize, UInt8 *buff);
			static OSInt ReadBuffer(MKVStatus *status);
			static Bool SkipBuffer(MKVStatus *status, OSInt skipSize);
			static Bool ReadHeader(MKVStatus *status, Int64 dataSize);
			static IO::ParsedObject *ReadSegment(MKVStatus *status, Int64 dataSize);
			static Bool ReadTrack(MKVStatus *status, Int64 dataSize);
			static Bool ReadTrackEntry(MKVStatus *status, Int64 dataSize);
			static Bool ReadVideo(MKVStatus *status, Int64 dataSize, Media::FrameInfo *frameInfo);
			static Bool ReadAudio(MKVStatus *status, Int64 dataSize, Media::AudioFormat *audioFmt);
		};
	};
};
#endif
