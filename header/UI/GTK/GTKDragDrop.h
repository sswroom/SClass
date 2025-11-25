#ifndef _SM_UI_GTK_GTKDRAGDROP
#define _SM_UI_GTK_GTKDRAGDROP
#include "Data/FastMapNN.hpp"
#include "Data/StringUTF8Map.hpp"
#include "Text/String.h"
#include "UI/GUIControl.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKDropData : public UI::GUIDropData
		{
		private:
			void *widget;
			void *context;
			UInt32 time;
			Data::StringUTF8Map<OSInt> targetMap;
			Data::FastMapNN<Int32, Text::String> targetText;

			static void AppendWC(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF16Char> s, UOSInt slen);

			void LoadData();
		public:
			GTKDropData(void *widget, void *context, UInt32 time, Bool readData);
			virtual ~GTKDropData();

			virtual UOSInt GetCount();
			virtual UnsafeArrayOpt<const UTF8Char> GetName(UOSInt index);
			virtual Bool GetDataText(UnsafeArray<const UTF8Char> name, NN<Text::StringBuilderUTF8> sb);
			virtual IO::Stream *GetDataStream(UnsafeArray<const UTF8Char> name);

			void OnDataReceived(void *selData);

		};

		class GTKDragDrop
		{
		private:
			NN<UI::GUIDropHandler> hdlr;
			Optional<ControlHandle> hWnd;
			UI::GUIDropHandler::DragEffect currEff;
			GTKDropData *dragData;

			static gboolean OnDragMotion(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data);
			static void OnDragLeave(GtkWidget *widget, GdkDragContext *context, guint time, gpointer user_data);
			static gboolean OnDragDrop(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data);
			static void OnDropData(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data);
		public:
			GTKDragDrop(Optional<ControlHandle> hWnd, NN<UI::GUIDropHandler> hdlr);
			~GTKDragDrop();

			void SetHandler(NN<UI::GUIDropHandler> hdlr);

			Int32 EventDragMotion(void *context, OSInt x, OSInt y, UInt32 time);
			void EventDragLeave();		
			Bool EventDragDrop(void *context, OSInt x, OSInt y, UInt32 time);
		};
	}
}
#endif
