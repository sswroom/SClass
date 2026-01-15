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
			virtual Bool ExportExcel(NN<IO::SeekableStream> stm, Text::CStringNN fileName) = 0;

			static Optional<LeicaGSIFile> Parse(NN<IO::Stream> fs);
			static void ParseHeader(UnsafeArray<Text::PString> sarr, UIntOS colCount, NN<LeicaGSIFile> file);
		};
	}
}
#endif
