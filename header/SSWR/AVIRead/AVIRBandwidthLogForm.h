#ifndef _SM_SSWR_AVIREAD_AVIRBANDWIDTHLOGFORM
#define _SM_SSWR_AVIREAD_AVIRBANDWIDTHLOGFORM
#include "Data/StringMapNN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBandwidthLogForm : public UI::GUIForm
		{
		public:
			struct ThreadStatus
			{
				NN<Text::String> name;
				Int64 startTime;
				Int64 endTime;
			};

			struct BandwidthItem
			{
				Int64 time;
				UInt32 recvCnt;
				UInt64 recvBytes;
				UInt32 sendCnt;
				UInt64 sendBytes;
			};

			struct BandwidthIP
			{
				NN<Text::String> ip;
				Data::ArrayListNN<BandwidthItem> items;
			};
		private:
			NN<UI::GUIGroupBox> grpFile;
			NN<UI::GUILabel> lblBandwidthLog;
			NN<UI::GUITextBox> txtBandwidthLog;
			NN<UI::GUILabel> lblJMeterLog;
			NN<UI::GUITextBox> txtJMeterLog;
			NN<UI::GUILabel> lblSIDELog;
			NN<UI::GUITextBox> txtSIDELog;

			NN<UI::GUIGroupBox> grpDetail;
			NN<UI::GUILabel> lblStartTime;
			NN<UI::GUITextBox> txtStartTime;
			NN<UI::GUILabel> lblEndTime;
			NN<UI::GUITextBox> txtEndTime;
			NN<UI::GUILabel> lblThreadsCount;
			NN<UI::GUITextBox> txtThreadsCount;
			NN<UI::GUIButton> btnThreadsExport;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUIComboBox> cboIP;
			NN<UI::GUIButton> btnExport;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<ThreadStatus> jmeterThreadList;
			Data::ArrayListNN<ThreadStatus> sideThreadList;
			Data::StringMapNN<BandwidthIP> bandwidthMap;
			Int64 startTime;
			Int64 endTime;

			static void __stdcall ThreadStatusFree(NN<ThreadStatus> thread);
			static void __stdcall DropFilesHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnExportClicked(AnyType userObj);
			static void __stdcall OnThreadsExportClicked(AnyType userObj);
			void LoadBandwidthLog(Text::CStringNN fileName);
			void LoadJMeterLog(Text::CStringNN fileName);
			void LoadSIDELog(Text::CStringNN filePath);
			void ClearBandwidthLog();
			void ClearJMeterLog();
			void ClearSIDELog();
		public:
			AVIRBandwidthLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBandwidthLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
