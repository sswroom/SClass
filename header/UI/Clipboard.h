#ifndef _SM_UI_CLIPBOARD
#define _SM_UI_CLIPBOARD
#include "Handles.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace UI
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
		struct ClassData;
		
		Bool succ;
		ClassData *clsData;
	public:
		Clipboard(void *hwnd);
		~Clipboard();

		UOSInt GetDataFormats(Data::ArrayList<UInt32> *dataTypes);
		Bool GetDataText(UInt32 fmtId, Text::StringBuilderUTF8 *sb);
		FilePasteType GetDataFiles(Data::ArrayListNN<Text::String> *fileNames);
		void FreeDataFiles(Data::ArrayListNN<Text::String> *fileNames);

	public:
		static Bool GetDataTextH(void *hand, UInt32 fmtId, Text::StringBuilderUTF8 *sb, UInt32 tymed);
		static Bool SetString(ControlHandle *hWndOwner, Text::CString s);
		static Bool GetString(ControlHandle *hWndOwner, Text::StringBuilderUTF8 *sb);
		static UTF8Char *GetFormatName(UInt32 fmtId, UTF8Char *sbuff, UOSInt buffSize);
	};
}
#endif
