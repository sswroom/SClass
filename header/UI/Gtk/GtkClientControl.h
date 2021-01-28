#ifndef _SM_UI_GTK_GTKCLIENTCONTROL
#define _SM_UI_GTK_GTKCLIENTCONTROL
#include "Data/ArrayList.h"
#include "UI/Gtk/GtkControl.h"

namespace UI
{
	namespace Gtk
	{
		class GtkClientControl : public UI::Gtk::GtkControl
		{
		protected:
			void *container;
			Data::ArrayList<UI::Gtk::GtkControl*> *children;
			Int32 undockLeft;
			Int32 undockTop;
			Int32 undockRight;
			Int32 undockBottom;

		private:
			void InitContainer();
		protected:
			GtkClientControl(UI::Gtk::GtkClientControl *parent);
		public:
			virtual ~GtkClientControl();

			virtual void GetClientOfst(Int32 *x, Int32 *y);
			virtual void GetClientSize(Int32 *w, Int32 *h);
			virtual void AddChild(GtkControl *child);
			virtual Bool IsChildVisible() = 0;

			OSInt GetChildCount();
			UI::Gtk::GtkControl *GetChild(OSInt index);
			void SetChildRect(GtkControl *child, Int32 left, Int32 top, Int32 width, Int32 height);
			void FocusChild(GtkControl *child);

			void UpdateChildrenSize(Bool redraw);
		};
	}
}
#endif
