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
		class AVIRGISLineEditForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			typedef struct
			{
				UInt32 color;
				Double thick;
				UInt8 *pattern;
				UOSInt nPattern;
			} LineLayer;

		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			NotNullPtr<Map::MapEnv> env;
			NotNullPtr<Media::DrawEngine> eng;
			UOSInt lineStyle;
			LineLayer *currLayer;
			Data::ArrayList<LineLayer*> *lineLayers;
			Media::DrawImage *prevImage;
			Media::StaticImage *prevsImage;
			Bool thickChging;

			NotNullPtr<UI::GUIPanel> pnlButtons;
			NotNullPtr<UI::GUIPanel> pnlStyle;
			NotNullPtr<UI::GUIGroupBox> grpLayer;
			NotNullPtr<UI::GUIPanel> pnlLayers;
			NotNullPtr<UI::GUIPanel> pnlLayersButton;
			NotNullPtr<UI::GUIPanel> pnlLayer;

			NotNullPtr<UI::GUIPictureBox> pbPreview;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUIListBox> lbLayer;
			NotNullPtr<UI::GUIButton> btnNewLayer;
			NotNullPtr<UI::GUIButton> btnRemoveLayer;

			NotNullPtr<UI::GUILabel> lblColor;
			NotNullPtr<UI::GUILabel> lblAlpha;
			NotNullPtr<UI::GUILabel> lblThick;
			NotNullPtr<UI::GUILabel> lblPattern;
			NotNullPtr<UI::GUIPictureBox> pbColor;
			NotNullPtr<UI::GUIHScrollBar> hsbAlpha;
			NotNullPtr<UI::GUITextBox> txtThick;
			NotNullPtr<UI::GUIHScrollBar> hsbThick;
			NotNullPtr<UI::GUITextBox> txtPattern;

			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

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

			static void FreeLayer(LineLayer *lyr);
		public:
			AVIRGISLineEditForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, NotNullPtr<Media::DrawEngine> eng, UOSInt lineStyle);
			virtual ~AVIRGISLineEditForm();

			virtual void OnMonitorChanged();
			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);
		};
	}
}
#endif
