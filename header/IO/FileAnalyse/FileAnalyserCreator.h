#ifndef _SM_IO_FILEANALYSE_FILEANALYSERCREATOR
#define _SM_IO_FILEANALYSE_FILEANALYSERCREATOR
#include "IO/FileAnalyse/FileAnalyser.h"

namespace IO
{
	namespace FileAnalyse
	{
		class FileAnalyserCreator
		{
		public:
			virtual ~FileAnalyserCreator() {};

			virtual Optional<IO::FileAnalyse::FileAnalyser> Create(NN<IO::StreamData> fd) = 0;
		};
	}
}
#endif
