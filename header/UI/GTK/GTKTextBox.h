#ifndef _SM_UI_GTK_GTKTEXTBOX
#define _SM_UI_GTK_GTKTEXTBOX
#include "UI/GUITextBox.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKTextBox : public GUITextBox
		{
		private:
			Bool multiLine;
			GtkWidget *widget;


			static void SignalChanged(GtkTextBuffer *textbuffer, gpointer user_data);
			static void SignalDelText(GtkEntryBuffer *buffer, guint position, guint n_chars, gpointer user_data);
			static void SignalInsText(GtkEntryBuffer *buffer, guint position, char *chars, guint n_chars, gpointer user_data);
			static gboolean SignalKeyDown(GtkWidget *widget, GdkEvent *event, gpointer user_data);
			void InitTextBox(Text::CStringNN lbl, Bool multiLine);
		public:
			GTKTextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText);
			GTKTextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline);
			virtual ~GTKTextBox();

			virtual UI::EventState EventKeyDown(UInt32 osKey);

			virtual void SetReadOnly(Bool isReadOnly);
			virtual void SetPasswordChar(UTF32Char c);

			virtual void SetText(Text::CStringNN text);
			virtual UnsafeArrayOpt<UTF8Char> GetText(UnsafeArray<UTF8Char> buff);
			virtual Bool GetText(NN<Text::StringBuilderUTF8> sb);
			virtual IntOS OnNotify(UInt32 code, void *lParam);

			virtual void SetWordWrap(Bool wordWrap);
			virtual void SelectAll();

			virtual Optional<ControlHandle> GetDisplayHandle();
		};
	}
}
#endif
