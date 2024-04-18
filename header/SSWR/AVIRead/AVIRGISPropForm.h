#ifndef _SM_SSWR_AVIREAD_AVIRGISPROPFORM
#define _SM_SSWR_AVIREAD_AVIRGISPROPFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/String.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISPropForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			NotNullPtr<UI::GUITextBox> txtMinScale;
			NotNullPtr<UI::GUITextBox> txtMaxScale;
			NotNullPtr<UI::GUICheckBox> chkHide;
			NotNullPtr<UI::GUIPictureBox> pbFontStyle;
			NotNullPtr<UI::GUIButton> btnFontModify;
			NotNullPtr<UI::GUIButton> btnFontStyle;
			NotNullPtr<UI::GUILabel> lblLineStyle;
			NotNullPtr<UI::GUIPictureBox> pbLineStyle;
			NotNullPtr<UI::GUIButton> btnLineModify;
			NotNullPtr<UI::GUIButton> btnLineStyle;
			NotNullPtr<UI::GUILabel> lblFillStyle;
			NotNullPtr<UI::GUIPictureBox> pbFillStyle;
			NotNullPtr<UI::GUIComboBox> cboColName;
			NotNullPtr<UI::GUICheckBox> chkShowLabel;
			NotNullPtr<UI::GUITextBox> txtPriority;
			NotNullPtr<UI::GUILabel> lblIcon;
			NotNullPtr<UI::GUIPictureBox> pbIcon;

			NotNullPtr<UI::GUIGroupBox> grpLabel;
			NotNullPtr<UI::GUICheckBox> chkSmart;
			NotNullPtr<UI::GUICheckBox> chkRotate;
			NotNullPtr<UI::GUICheckBox> chkAlign;
			NotNullPtr<UI::GUICheckBox> chkTrim;
			NotNullPtr<UI::GUICheckBox> chkCapital;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<Media::DrawEngine> eng;
			Media::StaticImage *imgLine;
			Media::StaticImage *imgFont;
			NotNullPtr<Map::MapEnv> env;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			NotNullPtr<Media::ColorConv> colorConv;
			Optional<Map::MapEnv::GroupItem> group;
			UOSInt index;
			Int32 lineType;
			UOSInt lineStyle;
			Double lineThick;
			UInt32 lineColor;
			UInt32 fillStyle;
			Map::MapEnv::FontType fontType;
			UOSInt fontStyle;
			Text::String *fontName;
			Double fontSizePt;
			UInt32 fontColor;
			UOSInt imgIndex;

		private:

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static Bool __stdcall OnFillClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
			static Bool __stdcall OnLineDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
			static void __stdcall OnLineModifyClicked(AnyType userObj);
			static void __stdcall OnLineStyleClicked(AnyType userObj);
			static Bool __stdcall OnIconClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
			static Bool __stdcall OnFontModifyDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
			static void __stdcall OnFontModifyClicked(AnyType userObj);
			static void __stdcall OnFontStyleClicked(AnyType userObj);
/*
			System::Void LayerPropForm_Load(System::Object *  sender, System::EventArgs *  e);
			System::Void LayerPropForm_Closed(System::Object *  sender, System::EventArgs *  e);
*/

		public:
			AVIRGISPropForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, Optional<Map::MapEnv::GroupItem> group, UOSInt index);
			virtual ~AVIRGISPropForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);
		};
	}
}
#endif
