#ifndef _SM_UI_CLIPBOARD
#define _SM_UI_CLIPBOARD
#include "Handles.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListStringNN.h"
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
		Bool GetDataText(UInt32 fmtId, NN<Text::StringBuilderUTF8> sb);
		FilePasteType GetDataFiles(Data::ArrayListStringNN *fileNames);
		void FreeDataFiles(Data::ArrayListStringNN *fileNames);

	public:
		static Bool GetDataTextH(void *hand, UInt32 fmtId, NN<Text::StringBuilderUTF8> sb, UInt32 tymed);
		static Bool SetString(ControlHandle *hWndOwner, Text::CString s);
		static Bool GetString(ControlHandle *hWndOwner, NN<Text::StringBuilderUTF8> sb);
		static UnsafeArray<UTF8Char> GetFormatName(UInt32 fmtId, UnsafeArray<UTF8Char> sbuff, UOSInt buffSize);
	};
}
#endif
