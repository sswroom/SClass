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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ColorManager *colorMgr;
			NotNullPtr<Media::MonitorColorManager> monColor;
			Bool normalClose;
			Text::String *monFileName;
			MonitorHandle *hMon;

			NotNullPtr<UI::GUIPanel> pnlMonitor;
			NotNullPtr<UI::GUILabel> lblMonitor;
			NotNullPtr<UI::GUITextBox> txtMonitor;
			NotNullPtr<UI::GUIPanel> pnlButtons;
			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<UI::GUITabPage> tpSetting;
			NotNullPtr<UI::GUILabel> lblMonProfile;
			NotNullPtr<UI::GUIComboBox> cboMonProfile;
			NotNullPtr<UI::GUIButton> btnMonProfile;
			NotNullPtr<UI::GUIGroupBox> grpMonProfile;
			NotNullPtr<UI::GUILabel> lblMonTran;
			NotNullPtr<UI::GUITextBox> txtMonTran;
			NotNullPtr<UI::GUILabel> lblMonRGBGamma;
			NotNullPtr<UI::GUITextBox> txtMonRGBGamma;
			NotNullPtr<UI::GUILabel> lblMonPrimaries;
			NotNullPtr<UI::GUITextBox> txtMonPrimaries;
			NotNullPtr<UI::GUIButton> btnMonCustom;
			NotNullPtr<UI::GUILabel> lblDefVProfile;
			NotNullPtr<UI::GUIComboBox> cboDefVProfile;
			NotNullPtr<UI::GUILabel> lblDefPProfile;
			NotNullPtr<UI::GUIComboBox> cboDefPProfile;
			NotNullPtr<UI::GUICheckBox> chk10Bit;
			NotNullPtr<UI::GUILabel> lblLuminance;
			NotNullPtr<UI::GUITextBox> txtLuminance;
			NotNullPtr<UI::GUILabel> lblLuminanceUnit;
			NotNullPtr<UI::GUIGroupBox> grpYUVDef;
			NotNullPtr<UI::GUIRadioButton> radYUVDefBT601;
			NotNullPtr<UI::GUIRadioButton> radYUVDefBT709;
			NotNullPtr<UI::GUIRadioButton> radYUVDefFCC;
			NotNullPtr<UI::GUIRadioButton> radYUVDefBT470BG;
			NotNullPtr<UI::GUIRadioButton> radYUVDefSMPTE170M;
			NotNullPtr<UI::GUIRadioButton> radYUVDefSMPTE240M;

			NotNullPtr<UI::GUITabPage> tpCal;
			NotNullPtr<UI::GUIPanel> pnlCalRed;
			NotNullPtr<UI::GUILabel> lblCalRedB;
			NotNullPtr<UI::GUIHScrollBar> hsbCalRedB;
			NotNullPtr<UI::GUILabel> lblCalRedBV;
			NotNullPtr<UI::GUILabel> lblCalRedC;
			NotNullPtr<UI::GUIHScrollBar> hsbCalRedC;
			NotNullPtr<UI::GUILabel> lblCalRedCV;
			NotNullPtr<UI::GUILabel> lblCalRedG;
			NotNullPtr<UI::GUIHScrollBar> hsbCalRedG;
			NotNullPtr<UI::GUILabel> lblCalRedGV;
			NotNullPtr<UI::GUIPanel> pnlCalGreen;
			NotNullPtr<UI::GUILabel> lblCalGreenB;
			NotNullPtr<UI::GUIHScrollBar> hsbCalGreenB;
			NotNullPtr<UI::GUILabel> lblCalGreenBV;
			NotNullPtr<UI::GUILabel> lblCalGreenC;
			NotNullPtr<UI::GUIHScrollBar> hsbCalGreenC;
			NotNullPtr<UI::GUILabel> lblCalGreenCV;
			NotNullPtr<UI::GUILabel> lblCalGreenG;
			NotNullPtr<UI::GUIHScrollBar> hsbCalGreenG;
			NotNullPtr<UI::GUILabel> lblCalGreenGV;
			NotNullPtr<UI::GUIPanel> pnlCalBlue;
			NotNullPtr<UI::GUILabel> lblCalBlueB;
			NotNullPtr<UI::GUIHScrollBar> hsbCalBlueB;
			NotNullPtr<UI::GUILabel> lblCalBlueBV;
			NotNullPtr<UI::GUILabel> lblCalBlueC;
			NotNullPtr<UI::GUIHScrollBar> hsbCalBlueC;
			NotNullPtr<UI::GUILabel> lblCalBlueCV;
			NotNullPtr<UI::GUILabel> lblCalBlueG;
			NotNullPtr<UI::GUIHScrollBar> hsbCalBlueG;
			NotNullPtr<UI::GUILabel> lblCalBlueGV;

			NotNullPtr<UI::GUITabPage> tpRGB;
			NotNullPtr<UI::GUILabel> lblMonVBrig;
			NotNullPtr<UI::GUIHScrollBar> hsbMonVBrig;
			NotNullPtr<UI::GUILabel> lblMonVBrigV;
			NotNullPtr<UI::GUILabel> lblMonPBrig;
			NotNullPtr<UI::GUIHScrollBar> hsbMonPBrig;
			NotNullPtr<UI::GUILabel> lblMonPBrigV;
			NotNullPtr<UI::GUILabel> lblMonRBrig;
			NotNullPtr<UI::GUIHScrollBar> hsbMonRBrig;
			NotNullPtr<UI::GUILabel> lblMonRBrigV;
			NotNullPtr<UI::GUILabel> lblMonGBrig;
			NotNullPtr<UI::GUIHScrollBar> hsbMonGBrig;
			NotNullPtr<UI::GUILabel> lblMonGBrigV;
			NotNullPtr<UI::GUILabel> lblMonBBrig;
			NotNullPtr<UI::GUIHScrollBar> hsbMonBBrig;
			NotNullPtr<UI::GUILabel> lblMonBBrigV;

			NotNullPtr<UI::GUITabPage> tpYUV;
			NotNullPtr<UI::GUILabel> lblYUVBrig;
			NotNullPtr<UI::GUIHScrollBar> hsbYUVBrig;
			NotNullPtr<UI::GUILabel> lblYUVBrigV;
			NotNullPtr<UI::GUILabel> lblYUVCont;
			NotNullPtr<UI::GUIHScrollBar> hsbYUVCont;
			NotNullPtr<UI::GUILabel> lblYUVContV;
			NotNullPtr<UI::GUILabel> lblYUVSat;
			NotNullPtr<UI::GUIHScrollBar> hsbYUVSat;
			NotNullPtr<UI::GUILabel> lblYUVSatV;
			NotNullPtr<UI::GUILabel> lblYUVYGamma;
			NotNullPtr<UI::GUIHScrollBar> hsbYUVYGamma;
			NotNullPtr<UI::GUILabel> lblYUVYGammaV;
			NotNullPtr<UI::GUILabel> lblYUVCGamma;
			NotNullPtr<UI::GUIHScrollBar> hsbYUVCGamma;
			NotNullPtr<UI::GUILabel> lblYUVCGammaV;

			static void __stdcall OnMonProfileChg(AnyType userObj);
			static void __stdcall OnMonProfileClicked(AnyType userObj);
			static void __stdcall OnMonCustomClicked(AnyType userObj);
			static void __stdcall OnDefVProfileChg(AnyType userObj);
			static void __stdcall OnDefPProfileChg(AnyType userObj);
			static void __stdcall On10BitChanged(AnyType userObj, Bool newVal);

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);

			static void __stdcall OnCalRedBChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalRedCChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalRedGChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalGreenBChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalGreenCChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalGreenGChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalBlueBChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalBlueCChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnCalBlueGChg(AnyType userObj, UOSInt newPos);

			static void __stdcall OnMonVBrigChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnMonPBrigChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnMonRBrigChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnMonGBrigChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnMonBBrigChg(AnyType userObj, UOSInt newPos);

			static void __stdcall OnYUVBrigChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnYUVContChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnYUVSatChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnYUVYGammaChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnYUVCGammaChg(AnyType userObj, UOSInt newPos);
			static void __stdcall OnYUVDefBT601Chg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefBT709Chg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefFCCChg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefBT470BGChg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefSMPTE170MChg(AnyType userObj, Bool checked);
			static void __stdcall OnYUVDefSMPTE240MChg(AnyType userObj, Bool checked);
			
		public:
			AVIRColorSettingForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, MonitorHandle *hMon);
			virtual ~AVIRColorSettingForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
