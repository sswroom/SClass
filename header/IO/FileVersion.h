// require version.lib
#ifndef _SM_IO_FILEVERSION
#define _SM_IO_FILEVERSION

namespace IO
{
	class FileVersion
	{
	private:
		UnsafeArray<UInt8> buff;
		UInt32 buffSize;

	private:
		FileVersion(UnsafeArray<UInt8> buff, UInt32 buffSize);

	public:
		~FileVersion();

		Int32 GetFirstLang();
		UnsafeArrayOpt<UTF8Char> GetInternalName(Int32 lang, UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetFileDescription(Int32 lang, UnsafeArray<UTF8Char> sbuff);

		static Optional<FileVersion> Open(UnsafeArrayOpt<const UTF8Char> file);
	};
}
#endif
