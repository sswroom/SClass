#ifndef _SM_UI_GTK_GTKFORM
#define _SM_UI_GTK_GTKFORM
#include "UI/Gtk/GtkClientControl.h"
#include "UI/Gtk/GtkUI.h"

namespace UI
{
	namespace Gtk
	{
		class GtkTimer;

		class GtkForm : public GtkClientControl
		{
		public:
			typedef enum
			{
				FS_NORMAL,
				FS_MAXIMIZED,
				FS_MINIMIZED
			} FormState;

			typedef enum
			{
				DR_UNKNOWN,
				DR_OK,
				DR_CANCEL
			} DialogResult;
		protected:
			GtkUI *ui;
		private:
			Data::ArrayList<GtkTimer*> *timers;
			Bool exitOnClose;

			Bool isDialog;
			DialogResult dialogResult;

			static void EventDestroy(void *window, void *userObj);
		public:
			GtkForm(GtkClientControl *parent, Int32 initW, Int32 initH, GtkUI *ui);
			virtual ~GtkForm();

			virtual void Close();
			virtual void SetText(const WChar *text);
			void SetFormState(FormState fs);
			DialogResult ShowDialog(UI::Gtk::GtkForm *owner);
			void SetDialogResult(DialogResult dr);

			void MakeForeground();
			virtual void SetExitOnClose(Bool exitOnClose);
			virtual void SetNoResize(Bool noResize);
			virtual UI::Gtk::GtkTimer *AddTimer(UInt32 interval, UI::UIEvent handler, void *userObj);
			virtual void RemoveTimer(UI::Gtk::GtkTimer *tmr);

			virtual Bool IsChildVisible();
			
			void ToFullScn();
			void FromFullScn();
		};
	}
}
#endif
