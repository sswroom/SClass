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
		class AVIRGISPropForm : public UI::GUIForm, public Media::ColorHandler
		{
		private:
			NN<UI::GUITextBox> txtMinScale;
			NN<UI::GUITextBox> txtMaxScale;
			NN<UI::GUICheckBox> chkHide;
			NN<UI::GUIPictureBox> pbFontStyle;
			NN<UI::GUIButton> btnFontModify;
			NN<UI::GUIButton> btnFontStyle;
			NN<UI::GUILabel> lblLineStyle;
			NN<UI::GUIPictureBox> pbLineStyle;
			NN<UI::GUIButton> btnLineModify;
			NN<UI::GUIButton> btnLineStyle;
			NN<UI::GUILabel> lblFillStyle;
			NN<UI::GUIPictureBox> pbFillStyle;
			NN<UI::GUIComboBox> cboColName;
			NN<UI::GUICheckBox> chkShowLabel;
			NN<UI::GUITextBox> txtPriority;
			NN<UI::GUILabel> lblIcon;
			NN<UI::GUIPictureBox> pbIcon;

			NN<UI::GUIGroupBox> grpLabel;
			NN<UI::GUICheckBox> chkSmart;
			NN<UI::GUICheckBox> chkRotate;
			NN<UI::GUICheckBox> chkAlign;
			NN<UI::GUICheckBox> chkTrim;
			NN<UI::GUICheckBox> chkCapital;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<Media::DrawEngine> eng;
			Optional<Media::StaticImage> imgLine;
			Optional<Media::StaticImage> imgFont;
			NN<Map::MapEnv> env;
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::ColorConv> colorConv;
			Optional<Map::MapEnv::GroupItem> group;
			UOSInt index;
			Int32 lineType;
			UOSInt lineStyle;
			Double lineThick;
			UInt32 lineColor;
			UInt32 fillStyle;
			Map::MapEnv::FontType fontType;
			UOSInt fontStyle;
			Optional<Text::String> fontName;
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

		public:
			AVIRGISPropForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Optional<Map::MapEnv::GroupItem> group, UOSInt index);
			virtual ~AVIRGISPropForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
		};
	}
}
#endif
