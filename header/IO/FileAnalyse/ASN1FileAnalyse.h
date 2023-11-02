#ifndef _SM_IO_FILEANALYSE_ASN1FILEANALYSE
#define _SM_IO_FILEANALYSE_ASN1FILEANALYSE
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "Net/ASN1Names.h"

namespace IO
{
	namespace FileAnalyse
	{
		class ASN1FileAnalyse : public IO::FileAnalyse::IFileAnalyse
		{
		private:
			NotNullPtr<IO::StreamData> fd;
			Net::ASN1Names *names;
		public:
			ASN1FileAnalyse(NotNullPtr<IO::StreamData> fd, Net::ASN1Names *names);
			virtual ~ASN1FileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UOSInt GetFrameCount();
			virtual Bool GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual UOSInt GetFrameIndex(UInt64 ofst);
			virtual FrameDetail *GetFrameDetail(UOSInt index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
