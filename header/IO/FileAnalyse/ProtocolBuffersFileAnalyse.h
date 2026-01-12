#ifndef _SM_IO_FILEANALYSE_PROTOCOLBUFFERSFILEANALYSE
#define _SM_IO_FILEANALYSE_PROTOCOLBUFFERSFILEANALYSE
#include "IO/ProtocolBuffersMessage.h"
#include "IO/FileAnalyse/FileAnalyserCreator.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class ProtocolBuffersFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			NN<IO::ProtocolBuffersMessage> msg;
			NN<IO::StreamData> fd;
		public:
			ProtocolBuffersFileAnalyse(NN<IO::StreamData> fd, NN<IO::ProtocolBuffersMessage> msg);
			virtual ~ProtocolBuffersFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};

		class ProtocolBuffersFileAnalyseCreator : public IO::FileAnalyse::FileAnalyserCreator
		{
		private:
			NN<IO::ProtocolBuffersMessage> msg;
		public:
			ProtocolBuffersFileAnalyseCreator(NN<IO::ProtocolBuffersMessage> msg);
			virtual ~ProtocolBuffersFileAnalyseCreator();

			virtual Optional<IO::FileAnalyse::FileAnalyser> Create(NN<IO::StreamData> fd);
		};
	}
}
#endif
