// require version.lib
#ifndef _SM_IO_FILEVERSION
#define _SM_IO_FILEVERSION

namespace IO
{
	class FileVersion
	{
	private:
		UInt8 *buff;
		UInt32 buffSize;

	private:
		FileVersion(UInt8 *buff, UInt32 buffSize);

	public:
		~FileVersion();

		Int32 GetFirstLang();
		UTF8Char *GetInternalName(Int32 lang, UTF8Char *sbuff);
		UTF8Char *GetFileDescription(Int32 lang, UTF8Char *sbuff);

		static FileVersion *Open(const UTF8Char *file);
	};
};
#endif
