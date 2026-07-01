#ifndef _SM_IO_WINDOWZIP
#define _SM_IO_WINDOWZIP
#include "Data/ArrayListArr.hpp"

namespace IO
{
	class WindowZIP
	{
	private:
		UnsafeArray<const WChar> filePath;
		Bool error;

	public:
		WindowZIP(UnsafeArray<const WChar> zipFile);
		~WindowZIP();

		Bool AddFile(UnsafeArray<const WChar> sourceFile);
		IntOS GetFileList(NN<Data::ArrayListArr<const WChar>> fileList);
		void FreeFileList(NN<Data::ArrayListArr<const WChar>> fileList);
		Bool ExtractFile(UnsafeArray<const WChar> fileName, UnsafeArray<const WChar> destPath);
	};
};
#endif
