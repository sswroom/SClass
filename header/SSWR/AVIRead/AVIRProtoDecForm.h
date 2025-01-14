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
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUILabel> lblDecoder;
			NN<UI::GUIComboBox> cboDecoder;
			NN<UI::GUIButton> btnLoad;
			NN<UI::GUIListView> lvLogs;
			NN<UI::GUIVSplitter> vspLogs;
			NN<UI::GUITextBox> txtLogs;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::FileStream> currFile;
			Optional<IO::ProtoDec::IProtocolDecoder> currDec;
			IO::ProtoDec::ProtoDecList decList;
			Data::ArrayListNN<ProtocolItem> itemList;

			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnLoadClicked(AnyType userObj);
			static void __stdcall OnProtocolEntry(AnyType userObj, UInt64 fileOfst, UOSInt size, Text::CStringNN typeName);
			void ClearList();
		public:
			AVIRProtoDecForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProtoDecForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
