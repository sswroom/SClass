#ifndef _SM_UI_GTK_GTKDRAGDROP
#define _SM_UI_GTK_GTKDRAGDROP
#include "Data/FastMapNN.hpp"
#include "Data/StringMapNative.hpp"
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
			AnyType widget;
			AnyType context;
			UInt32 time;
			Data::StringMapNative<IntOS> targetMap;
			Data::FastMapNN<Int32, Text::String> targetText;

			static void AppendWC(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF16Char> s, UIntOS slen);

			void LoadData();
		public:
			GTKDropData(AnyType widget, AnyType context, UInt32 time, Bool readData);
			virtual ~GTKDropData();

			virtual UIntOS GetCount();
			virtual UnsafeArrayOpt<const UTF8Char> GetName(UIntOS index);
			virtual Bool GetDataText(UnsafeArray<const UTF8Char> name, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<IO::Stream> GetDataStream(UnsafeArray<const UTF8Char> name);

			void OnDataReceived(AnyType selData);

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

			Int32 EventDragMotion(AnyType context, IntOS x, IntOS y, UInt32 time);
			void EventDragLeave();		
			Bool EventDragDrop(AnyType context, IntOS x, IntOS y, UInt32 time);
		};
	}
}
#endif
