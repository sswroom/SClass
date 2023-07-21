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
				UOSInt thick;
				UInt8 *pattern;
				UOSInt nPattern;
			} LineLayer;

		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Map::MapEnv *env;
			NotNullPtr<Media::DrawEngine> eng;
			UOSInt lineStyle;
			LineLayer *currLayer;
			Data::ArrayList<LineLayer*> *lineLayers;
			Media::DrawImage *prevImage;
			Media::StaticImage *prevsImage;
			Bool thickChging;

			UI::GUIPanel *pnlButtons;
			UI::GUIPanel *pnlStyle;
			UI::GUIGroupBox *grpLayer;
			UI::GUIPanel *pnlLayers;
			UI::GUIPanel *pnlLayersButton;
			UI::GUIPanel *pnlLayer;

			UI::GUIPictureBox *pbPreview;
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUIListBox *lbLayer;
			UI::GUIButton *btnNewLayer;
			UI::GUIButton *btnRemoveLayer;

			UI::GUILabel *lblColor;
			UI::GUILabel *lblAlpha;
			UI::GUILabel *lblThick;
			UI::GUILabel *lblPattern;
			UI::GUIPictureBox *pbColor;
			UI::GUIHScrollBar *hsbAlpha;
			UI::GUITextBox *txtThick;
			UI::GUIHScrollBar *hsbThick;
			UI::GUITextBox *txtPattern;

			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			void LineStyleUpdated();
			void UpdatePreview();

			static void __stdcall NewLayerClicked(void *userObj);
			static void __stdcall RemoveLayerClicked(void *userObj);
			static void __stdcall LayerSelChanged(void *userObj);
			static void __stdcall ThickChanged(void *userObj);
			static void __stdcall OnThickScrolled(void *userObj, UOSInt newPos);
			static Bool __stdcall ColorClicked(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn);
			static void __stdcall PatternChanged(void *userObj);
			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);

			static void FreeLayer(LineLayer *lyr);
		public:
			AVIRGISLineEditForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, NotNullPtr<Media::DrawEngine> eng, UOSInt lineStyle);
			virtual ~AVIRGISLineEditForm();

			virtual void OnMonitorChanged();
			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		};
	};
};
#endif
