#ifndef _SM_UI_GUIPANELBASE
#define _SM_UI_GUIPANELBASE

namespace UI
{
	class GUIPanel;

	class GUIPanelBase
	{
	public:
		virtual ~GUIPanelBase(){};

		virtual Math::Coord2DDbl GetClientOfst() = 0;
		virtual Math::Size2DDbl GetClientSize() = 0;
		virtual void SetMinSize(Int32 minW, Int32 minH) = 0;
	};
}
#endif
