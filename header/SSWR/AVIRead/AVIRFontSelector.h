#ifndef _SM_SSWR_AVIREAD_AVIRFONTSELECTOR
#define _SM_SSWR_AVIREAD_AVIRFONTSELECTOR
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUICustomDrawVScroll.h"
#include "UI/GUIPopupMenu.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFontSelector : public UI::GUICustomDrawVScroll, public Media::IColorHandler
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Media::ColorConv *colorConv;
			Map::MapEnv *env;
			UOSInt currFontStyle;

			UI::GUIPopupMenu *mnuLayers;

		private:
			static void __stdcall OnResized(void *userObj);
			virtual void OnDraw(Media::DrawImage *img);
			virtual void OnMouseDown(OSInt scrollY, Int32 xPos, Int32 yPos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
			virtual void OnKeyDown(UInt32 keyCode);

		public:
			AVIRFontSelector(UI::GUICore *ui, UI::GUIClientControl *parent, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, UOSInt initFontStyle, Media::ColorManagerSess *colorSess);
			virtual ~AVIRFontSelector();

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);

			void SetPopupMenu(UI::GUIPopupMenu *mnuLayers);

			void UpdateFontStyles();
			UOSInt GetSelectedFontStyle();
		};
	}
}
#endif
