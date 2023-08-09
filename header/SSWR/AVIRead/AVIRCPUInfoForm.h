#ifndef _SM_SSWR_AVIREAD_AVIRCPUINFOFORM
#define _SM_SSWR_AVIREAD_AVIRCPUINFOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCPUInfoForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpCPU;
			UI::GUIListView *lvMain;
			UI::GUIPanel *pnlMain;
			UI::GUIButton *btnUpload;
			UI::GUIButton *btnCopyInfo;

			UI::GUITabPage *tpCache;
			UI::GUIListBox *lbCache;

			UI::GUITabPage *tpFeature;
			UI::GUIListView *lvFeature;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;

			static void __stdcall OnUploadClick(void *userObj);
			static void __stdcall OnCopyInfoClick(void *userObj);
		public:
			AVIRCPUInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCPUInfoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
