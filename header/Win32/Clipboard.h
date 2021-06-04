#ifndef _SM_WIN32_CLIPBOARD
#define _SM_WIN32_CLIPBOARD
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF.h"

namespace Win32
{
	class Clipboard
	{
	public:
		typedef enum
		{
			FPT_NONE,
			FPT_COPY,
			FPT_MOVE
		} FilePasteType;
	private:
		Bool succ;
		void *clsData;
	public:
		Clipboard(void *hwnd);
		~Clipboard();

		UOSInt GetDataFormats(Data::ArrayList<UInt32> *dataTypes);
		Bool GetDataText(UInt32 fmtId, Text::StringBuilderUTF *sb);
		FilePasteType GetDataFiles(Data::ArrayList<const UTF8Char *> *fileNames);
		void FreeDataFiles(Data::ArrayList<const UTF8Char *> *fileNames);

	public:
		static Bool GetDataTextH(void *hand, UInt32 fmtId, Text::StringBuilderUTF *sb, UInt32 tymed);
		static Bool SetString(void *hWndOwner, const UTF8Char *s);
		static Bool GetString(void *hWndOwner, Text::StringBuilderUTF *sb);
		static UTF8Char *GetFormatName(UInt32 fmtId, UTF8Char *sbuff, UOSInt buffSize);
	};
}
#endif
