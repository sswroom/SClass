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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::ColorConv> colorConv;
			NN<Map::MapEnv> env;
			UOSInt currLineStyle;

			Optional<UI::GUIPopupMenu> mnuLayers;

		private:
			static void __stdcall OnResized(AnyType userObj);
			virtual void OnDraw(NN<Media::DrawImage> img);
			virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
			virtual void OnKeyDown(UInt32 keyCode);

		public:
			AVIRLineSelector(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, UOSInt initLineStyle, NN<Media::ColorManagerSess> colorSess);
			virtual ~AVIRLineSelector();

			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);

			void SetPopupMenu(Optional<UI::GUIPopupMenu> mnuLayers);

			void UpdateLineStyles();
			UOSInt GetSelectedLineStyle();
		};
	}
}
#endif
