#ifndef _SM_UI_CLIPBOARD
#define _SM_UI_CLIPBOARD
#include "Handles.h"
#include "Data/ArrayList.hpp"
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
		NN<ClassData> clsData;
	public:
		Clipboard(Optional<ControlHandle> hwnd);
		~Clipboard();

		UOSInt GetDataFormats(NN<Data::ArrayList<UInt32>> dataTypes);
		Bool GetDataText(UInt32 fmtId, NN<Text::StringBuilderUTF8> sb);
		FilePasteType GetDataFiles(NN<Data::ArrayListStringNN> fileNames);
		void FreeDataFiles(NN<Data::ArrayListStringNN> fileNames);

	public:
		static Bool GetDataTextH(void *hand, UInt32 fmtId, NN<Text::StringBuilderUTF8> sb, UInt32 tymed);
		static Bool SetString(Optional<ControlHandle> hWndOwner, Text::CStringNN s);
		static Bool GetString(Optional<ControlHandle> hWndOwner, NN<Text::StringBuilderUTF8> sb);
		static UnsafeArray<UTF8Char> GetFormatName(UInt32 fmtId, UnsafeArray<UTF8Char> sbuff, UOSInt buffSize);
	};
}
#endif
