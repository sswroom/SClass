#ifndef _SM_MAP_LEICA_LEICAGSIFILE
#define _SM_MAP_LEICA_LEICAGSIFILE
#include "IO/SeekableStream.h"

namespace Map
{
	namespace Leica
	{
		class LeicaGSIFile
		{
		public:
			typedef enum
			{
				FT_LEVEL
			} FileType;

		protected:
			LeicaGSIFile();
		public:
			virtual ~LeicaGSIFile();

			virtual FileType GetFileType() = 0;
			virtual Bool ExportExcel(IO::SeekableStream *stm, const WChar *fileName) = 0;

			static LeicaGSIFile *Parse(IO::Stream *fs);
			static void ParseHeader(WChar **sarr, OSInt colCount, LeicaGSIFile *file);
		};
	}
}
#endif
