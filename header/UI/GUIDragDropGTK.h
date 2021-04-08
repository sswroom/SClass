#ifndef _SM_UI_GUIDRAGDROPGTK
#define _SM_UI_GUIDRAGDROPGTK
#include "Data/Int32Map.h"
#include "Data/StringUTF8Map.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIDragDataGTK : public UI::GUIDropData
	{
	private:
		void *widget;
		void *context;
		UInt32 time;
		Data::StringUTF8Map<OSInt> *targetMap;
		Data::Int32Map<const UTF8Char*> *targetText;

		void LoadData();
	public:
		GUIDragDataGTK(void *widget, void *context, UInt32 time, Bool readData);
		virtual ~GUIDragDataGTK();

		virtual OSInt GetCount();
		virtual const UTF8Char *GetName(OSInt index);
		virtual Bool GetDataText(const UTF8Char *name, Text::StringBuilderUTF *sb);
		virtual IO::Stream *GetDataStream(const UTF8Char *name);

		void OnDataReceived(void *selData);
	};

	class GUIDragDropGTK
	{
	private:
		UI::GUIDropHandler *hdlr;
		void *hWnd;
		UI::GUIDropHandler::DragEffect currEff;
		Bool dragStarted;

	public:
		GUIDragDropGTK(void *hWnd, UI::GUIDropHandler *hdlr);
		~GUIDragDropGTK();

		void SetHandler(UI::GUIDropHandler *hdlr);

		Int32 OnDragMotion(void *context, OSInt x, OSInt y, UInt32 time);
		void OnDragLeave();		
		Bool OnDragDrop(void *context, OSInt x, OSInt y, UInt32 time);
	};
};

#endif
