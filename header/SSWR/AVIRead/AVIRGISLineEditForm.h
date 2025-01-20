#ifndef _SM_SSWR_AVIREAD_AVIRGISLINEEDITFORM
#define _SM_SSWR_AVIREAD_AVIRGISLINEEDITFORM
#include "Media/ColorManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRLineSelector.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISLineEditForm : public UI::GUIForm, public Media::ColorHandler
		{
		private:
			typedef struct
			{
				UInt32 color;
				Double thick;
				UnsafeArrayOpt<UInt8> pattern;
				UOSInt nPattern;
			} LineLayer;

		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Map::MapEnv> env;
			NN<Media::DrawEngine> eng;
			UOSInt lineStyle;
			Optional<LineLayer> currLayer;
			Data::ArrayListNN<LineLayer> lineLayers;
			Optional<Media::DrawImage> prevImage;
			Optional<Media::StaticImage> prevsImage;
			Bool thickChging;

			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUIPanel> pnlStyle;
			NN<UI::GUIGroupBox> grpLayer;
			NN<UI::GUIPanel> pnlLayers;
			NN<UI::GUIPanel> pnlLayersButton;
			NN<UI::GUIPanel> pnlLayer;

			NN<UI::GUIPictureBox> pbPreview;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUIListBox> lbLayer;
			NN<UI::GUIButton> btnNewLayer;
			NN<UI::GUIButton> btnRemoveLayer;

			NN<UI::GUILabel> lblColor;
			NN<UI::GUILabel> lblAlpha;
			NN<UI::GUILabel> lblThick;
			NN<UI::GUILabel> lblPattern;
			NN<UI::GUIPictureBox> pbColor;
			NN<UI::GUIHScrollBar> hsbAlpha;
			NN<UI::GUITextBox> txtThick;
			NN<UI::GUIHScrollBar> hsbThick;
			NN<UI::GUITextBox> txtPattern;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			void LineStyleUpdated();
			void UpdatePreview();

			static void __stdcall NewLayerClicked(AnyType userObj);
			static void __stdcall RemoveLayerClicked(AnyType userObj);
			static void __stdcall LayerSelChanged(AnyType userObj);
			static void __stdcall ThickChanged(AnyType userObj);
			static void __stdcall OnThickScrolled(AnyType userObj, UOSInt newPos);
			static Bool __stdcall ColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn);
			static void __stdcall PatternChanged(AnyType userObj);
			static void __stdcall OKClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);

			static void __stdcall FreeLayer(NN<LineLayer> lyr);
		public:
			AVIRGISLineEditForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Media::DrawEngine> eng, UOSInt lineStyle);
			virtual ~AVIRGISLineEditForm();

			virtual void OnMonitorChanged();
			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
		};
	}
}
#endif
