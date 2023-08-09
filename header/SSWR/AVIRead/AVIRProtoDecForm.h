#ifndef _SM_SSWR_AVIREAD_AVIRPROTODECFORM
#define _SM_SSWR_AVIREAD_AVIRPROTODECFORM
#include "IO/FileStream.h"
#include "IO/LogFile.h"
#include "IO/ProtoDec/ProtoDecList.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRProtoDecForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UOSInt size;
			} ProtocolItem;
		private:
			UI::GUIPanel *pnlCtrl;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnFile;
			UI::GUILabel *lblDecoder;
			UI::GUIComboBox *cboDecoder;
			UI::GUIButton *btnLoad;
			UI::GUIListView *lvLogs;
			UI::GUIVSplitter *vspLogs;
			UI::GUITextBox *txtLogs;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::FileStream *currFile;
			IO::ProtoDec::IProtocolDecoder *currDec;
			IO::ProtoDec::ProtoDecList *decList;
			Data::ArrayList<ProtocolItem*> *itemList;

			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnProtocolEntry(void *userObj, UInt64 fileOfst, UOSInt size, Text::CString typeName);
			void ClearList();
		public:
			AVIRProtoDecForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProtoDecForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
