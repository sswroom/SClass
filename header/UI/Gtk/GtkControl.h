#ifndef _SM_UI_GTK_GTKCONTROL
#define _SM_UI_GTK_GTKCONTROL

namespace UI
{
	namespace Gtk
	{
		class GtkClientControl;

		class GtkControl
		{
		protected:
			void *window;
			GtkClientControl *parent;

			GtkControl(GtkClientControl *parent);
		public:
			virtual ~GtkControl();

			virtual void SetText(const WChar *text) = 0;
			virtual void Show();
			virtual void Close();
			virtual void SetArea(Int32 left, Int32 top, Int32 right, Int32 bottom, Bool updateScn);
			void SetRect(Int32 left, Int32 top, Int32 width, Int32 height, Bool updateScn);
			virtual void SetFont(const WChar *name, Single size, Bool isBold);
			virtual void SetVisible(Bool isVisible);
			virtual Bool GetVisible();
			virtual void SetEnabled(Bool isEnable);
			virtual void SetBGColor(Int32 bgColor);
			
			void *GetHandle();
		};
	}
}
#endif
