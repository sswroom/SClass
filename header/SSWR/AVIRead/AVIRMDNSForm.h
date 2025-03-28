#ifndef _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#define _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#include "Net/MDNSClient.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMDNSForm : public UI::GUIForm
		{
		private:
			struct ServiceInfo
			{
				NN<Text::String> instanceName;
				NN<Text::String> serviceName;
				UInt16 port;
				UInt32 ptrTTL;
				UInt32 txtTTL;
				UInt32 srvTTL;
				Data::ArrayListStringNN txtList;
			};

			struct DeviceInfo
			{
				NN<Text::String> ip;
				Optional<Text::String> name;
				Data::FastStringMapNN<ServiceInfo> svcMap;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::MDNSClient> mdns;

			NN<UI::GUIListBox> lbDev;
			NN<UI::GUIHSplitter> hspDev;
			NN<UI::GUIListView> lvServices;
			NN<UI::GUIVSplitter> vspServices;
			NN<UI::GUITextBox> txtServices;

			Sync::Mutex devMut;
			Data::StringMapNN<DeviceInfo> devMap;
			Bool devUpdated;
			Bool svcUpdated;
			Optional<DeviceInfo> currDev;
			Optional<ServiceInfo> currSvc;

			Sync::Mutex ptrMut;
			Data::StringMap<Int32> ptrMap;

			static void __stdcall DNSRecordRecv(AnyType userData, NN<const Net::SocketUtil::AddressInfo> addr, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> answers);
			static void __stdcall OnDevSelChg(AnyType userData);
			static void __stdcall OnServicesSelChg(AnyType userData);
			static void __stdcall OnTimerTick(AnyType userData);
			static void __stdcall FreeServiceInfo(NN<ServiceInfo> svc);
			static void __stdcall FreeDeviceInfo(NN<DeviceInfo> dev);
			void DisplayServices();
			void DisplayTXTList();
		public:
			AVIRMDNSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMDNSForm();
		
			virtual void OnMonitorChanged();
		};
	}
}
#endif
