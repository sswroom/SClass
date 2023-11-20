#ifndef _SM_SSWR_AVIREAD_AVIRLINESELECTOR
#define _SM_SSWR_AVIREAD_AVIRLINESELECTOR
#include "Media/ColorConv.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUICustomDrawVScroll.h"
#include "UI/GUIPopupMenu.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLineSelector : public UI::GUICustomDrawVScroll, public Media::IColorHandler
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::ColorConv *colorConv;
			NotNullPtr<Map::MapEnv> env;
			UOSInt currLineStyle;

			UI::GUIPopupMenu *mnuLayers;

		private:
			static void __stdcall OnResized(void *userObj);
			virtual void OnDraw(NotNullPtr<Media::DrawImage> img);
			virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
			virtual void OnKeyDown(UInt32 keyCode);

		public:
			AVIRLineSelector(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, UOSInt initLineStyle, NotNullPtr<Media::ColorManagerSess> colorSess);
			virtual ~AVIRLineSelector();

			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);

			void SetPopupMenu(UI::GUIPopupMenu *mnuLayers);

			void UpdateLineStyles();
			UOSInt GetSelectedLineStyle();
		};
	}
}
#endif
