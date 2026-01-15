#ifndef _SM_IO_WINDOWZIP
#define _SM_IO_WINDOWZIP
#include "Data/ArrayList.hpp"

namespace IO
{
	class WindowZIP
	{
	private:
		const WChar *filePath;
		Bool error;

	public:
		WindowZIP(const WChar *zipFile);
		~WindowZIP();

		Bool AddFile(const WChar *sourceFile);
		IntOS GetFileList(Data::ArrayList<const WChar *> *fileList);
		void FreeFileList(Data::ArrayList<const WChar *> *fileList);
		Bool ExtractFile(const WChar *fileName, const WChar *destPath);
	};
};
#endif
