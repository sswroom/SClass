#ifndef _SM_SSWR_AVIREAD_AVIRGISPROPFORM
#define _SM_SSWR_AVIREAD_AVIRGISPROPFORM
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISPropForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			UI::GUITextBox *txtMinScale;
			UI::GUITextBox *txtMaxScale;
			UI::GUICheckBox *chkHide;
			UI::GUIPictureBox *pbFontStyle;
			UI::GUIButton *btnFontModify;
			UI::GUIButton *btnFontStyle;
			UI::GUILabel *lblLineStyle;
			UI::GUIPictureBox *pbLineStyle;
			UI::GUIButton *btnLineModify;
			UI::GUIButton *btnLineStyle;
			UI::GUILabel *lblFillStyle;
			UI::GUIPictureBox *pbFillStyle;
			UI::GUIComboBox *cboColName;
			UI::GUICheckBox *chkShowLabel;
			UI::GUITextBox *txtPriority;
			UI::GUILabel *lblIcon;
			UI::GUIPictureBox *pbIcon;

			UI::GUIGroupBox *grpLabel;
			UI::GUICheckBox *chkSmart;
			UI::GUICheckBox *chkRotate;
			UI::GUICheckBox *chkAlign;
			UI::GUICheckBox *chkTrim;
			UI::GUICheckBox *chkCapital;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			Media::DrawEngine *eng;
			Media::StaticImage *imgLine;
			Media::StaticImage *imgFont;
			Map::MapEnv *env;
			Media::Resizer::LanczosResizer8_C8 *resizer;
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Media::ColorConv *colorConv;
			Map::MapEnv::GroupItem *group;
			OSInt index;
			Int32 lineType;
			UOSInt lineStyle;
			UOSInt lineThick;
			UInt32 lineColor;
			Int32 fillStyle;
			Int32 fontType;
			UOSInt fontStyle;
			const UTF8Char *fontName;
			Double fontSizePt;
			UInt32 fontColor;
			UOSInt imgIndex;

		private:

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static Bool __stdcall OnFillClicked(void *userObj, OSInt x, OSInt y, UI::GUIPictureBox::MouseButton btn);
			static Bool __stdcall OnLineDown(void *userObj, OSInt x, OSInt y, UI::GUIPictureBox::MouseButton btn);
			static void __stdcall OnLineModifyClicked(void *userObj);
			static void __stdcall OnLineStyleClicked(void *userObj);
			static Bool __stdcall OnIconClicked(void *userObj, OSInt x, OSInt y, UI::GUIPictureBox::MouseButton btn);
			static Bool __stdcall OnFontModifyDown(void *userObj, OSInt x, OSInt y, UI::GUIPictureBox::MouseButton btn);
			static void __stdcall OnFontModifyClicked(void *userObj);
			static void __stdcall OnFontStyleClicked(void *userObj);
/*
			System::Void LayerPropForm_Load(System::Object *  sender, System::EventArgs *  e);
			System::Void LayerPropForm_Closed(System::Object *  sender, System::EventArgs *  e);
*/

		public:
			AVIRGISPropForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, Map::MapEnv::GroupItem *group, OSInt index);
			virtual ~AVIRGISPropForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		};
	}
}
#endif
