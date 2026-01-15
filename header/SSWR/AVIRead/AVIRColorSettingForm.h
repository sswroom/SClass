#ifndef _SM_SSWR_AVIREAD_AVIRCOLORSETTINGFORM
#define _SM_SSWR_AVIREAD_AVIRCOLORSETTINGFORM
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRColorSettingForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManager> colorMgr;
			NN<Media::MonitorColorManager> monColor;
			Bool normalClose;
			Optional<Text::String> monFileName;
			Optional<MonitorHandle> hMon;

			NN<UI::GUIPanel> pnlMonitor;
			NN<UI::GUILabel> lblMonitor;
			NN<UI::GUITextBox> txtMonitor;
			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<UI::GUITabPage> tpSetting;
			NN<UI::GUILabel> lblMonProfile;
			NN<UI::GUIComboBox> cboMonProfile;
			NN<UI::GUIButton> btnMonProfile;
			NN<UI::GUIGroupBox> grpMonProfile;
			NN<UI::GUILabel> lblMonTran;
			NN<UI::GUITextBox> txtMonTran;
			NN<UI::GUILabel> lblMonRGBGamma;
			NN<UI::GUITextBox> txtMonRGBGamma;
			NN<UI::GUILabel> lblMonPrimaries;
			NN<UI::GUITextBox> txtMonPrimaries;
			NN<UI::GUIButton> btnMonCustom;
			NN<UI::GUILabel> lblDefVProfile;
			NN<UI::GUIComboBox> cboDefVProfile;
			NN<UI::GUILabel> lblDefPProfile;
			NN<UI::GUIComboBox> cboDefPProfile;
			NN<UI::GUICheckBox> chk10Bit;
			NN<UI::GUILabel> lblLuminance;
			NN<UI::GUITextBox> txtLuminance;
			NN<UI::GUILabel> lblLuminanceUnit;
			NN<UI::GUIGroupBox> grpYUVDef;
			NN<UI::GUIRadioButton> radYUVDefBT601;
			NN<UI::GUIRadioButton> radYUVDefBT709;
			NN<UI::GUIRadioButton> radYUVDefFCC;
			NN<UI::GUIRadioButton> radYUVDefBT470BG;
			NN<UI::GUIRadioButton> radYUVDefSMPTE170M;
			NN<UI::GUIRadioButton> radYUVDefSMPTE240M;

			NN<UI::GUITabPage> tpCal;
			NN<UI::GUIPanel> pnlCalRed;
			NN<UI::GUILabel> lblCalRedB;
			NN<UI::GUIHScrollBar> hsbCalRedB;
			NN<UI::GUILabel> lblCalRedBV;
			NN<UI::GUILabel> lblCalRedC;
			NN<UI::GUIHScrollBar> hsbCalRedC;
			NN<UI::GUILabel> lblCalRedCV;
			NN<UI::GUILabel> lblCalRedG;
			NN<UI::GUIHScrollBar> hsbCalRedG;
			NN<UI::GUILabel> lblCalRedGV;
			NN<UI::GUIPanel> pnlCalGreen;
			NN<UI::GUILabel> lblCalGreenB;
			NN<UI::GUIHScrollBar> hsbCalGreenB;
			NN<UI::GUILabel> lblCalGreenBV;
			NN<UI::GUILabel> lblCalGreenC;
			NN<UI::GUIHScrollBar> hsbCalGreenC;
			NN<UI::GUILabel> lblCalGreenCV;
			NN<UI::GUILabel> lblCalGreenG;
			NN<UI::GUIHScrollBar> hsbCalGreenG;
			NN<UI::GUILabel> lblCalGreenGV;
			NN<UI::GUIPanel> pnlCalBlue;
			NN<UI::GUILabel> lblCalBlueB;
			NN<UI::GUIHScrollBar> hsbCalBlueB;
			NN<UI::GUILabel> lblCalBlueBV;
			NN<UI::GUILabel> lblCalBlueC;
			NN<UI::GUIHScrollBar> hsbCalBlueC;
			NN<UI::GUILabel> lblCalBlueCV;
			NN<UI::GUILabel> lblCalBlueG;
			NN<UI::GUIHScrollBar> hsbCalBlueG;
			NN<UI::GUILabel> lblCalBlueGV;

			NN<UI::GUITabPage> tpRGB;
			NN<UI::GUILabel> lblMonVBrig;
			NN<UI::GUIHScrollBar> hsbMonVBrig;
			NN<UI::GUILabel> lblMonVBrigV;
			NN<UI::GUILabel> lblMonPBrig;
			NN<UI::GUIHScrollBar> hsbMonPBrig;
			NN<UI::GUILabel> lblMonPBrigV;
			NN<UI::GUILabel> lblMonRBrig;
			NN<UI::GUIHScrollBar> hsbMonRBrig;
			NN<UI::GUILabel> lblMonRBrigV;
			NN<UI::GUILabel> lblMonGBrig;
			NN<UI::GUIHScrollBar> hsbMonGBrig;
			NN<UI::GUILabel> lblMonGBrigV;
			NN<UI::GUILabel> lblMonBBrig;
			NN<UI::GUIHScrollBar> hsbMonBBrig;
			NN<UI::GUILabel> lblMonBBrigV;

			NN<UI::GUITabPage> tpYUV;
			NN<UI::GUILabel> lblYUVBrig;
			NN<UI::GUIHScrollBar> hsbYUVBrig;
			NN<UI::GUILabel> lblYUVBrigV;
			NN<UI::GUILabel> lblYUVCont;
			NN<UI::GUIHScrollBar> hsbYUVCont;
			NN<UI::GUILabel> lblYUVContV;
			NN<UI::GUILabel> lblYUVSat;
			NN<UI::GUIHScrollBar> hsbYUVSat;
			NN<UI::GUILabel> lblYUVSatV;
			NN<UI::GUILabel> lblYUVYGamma;
			NN<UI::GUIHScrollBar> hsbYUVYGamma;
			NN<UI::GUILabel> lblYUVYGammaV;
			NN<UI::GUILabel> lblYUVCGamma;
			NN<UI::GUIHScrollBar> hsbYUVCGamma;
			NN<UI::GUILabel> lblYUVCGammaV;

			static void __stdcall OnMonProfileChg(AnyType userObj);
			static void __stdcall OnMonProfileClicked(AnyType userObj);
			static void __stdcall OnMonCustomClicked(AnyType userObj);
			static void __stdcall OnDefVProfileChg(AnyType userObj);
			static void __stdcall OnDefPProfileChg(AnyType userObj);
			static void __stdcall On10BitChanged(AnyType userObj, Bool newVal);

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);

			static void __stdcall OnCalRedBChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalRedCChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalRedGChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalGreenBChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalGreenCChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalGreenGChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalBlueBChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalBlueCChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnCalBlueGChg(AnyType userObj, UIntOS newPos);

			static void __stdcall OnMonVBrigChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnMonPBrigChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnMonRBrigChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnMonGBrigChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnMonBBrigChg(AnyType userObj, UIntOS newPos);

			static void __stdcall OnYUVBrigChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnYUVContChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnYUVSatChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnYUVYGammaChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnYUVCGammaChg(AnyType userObj, UIntOS newPos);
			static void __stdcall OnYUVDefBT601Chg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefBT709Chg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefFCCChg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefBT470BGChg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefSMPTE170MChg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefSMPTE240MChg(AnyType userObj, Bool checked);
			
		public:
			AVIRColorSettingForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<MonitorHandle> hMon);
			virtual ~AVIRColorSettingForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
