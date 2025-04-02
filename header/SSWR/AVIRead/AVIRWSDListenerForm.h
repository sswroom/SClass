#ifndef _SM_SSWR_AVIREAD_AVIRWSDLISTENERFORM
#define _SM_SSWR_AVIREAD_AVIRWSDLISTENERFORM
#include "Net/UDPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWSDListenerForm : public UI::GUIForm
		{
		private:
			struct PacketInfo
			{
				Data::Timestamp ts;
				Net::SocketUtil::AddressInfo addr;
				UInt16 port;
				NN<Data::ByteBuffer> buff;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::UDPServer> listener;
			IO::LogTool log;
			Sync::Mutex packetMut;
			Data::ArrayListNN<PacketInfo> packetList;
			Bool packetUpdated;

			NN<UI::GUIListView> lvPackets;
			NN<UI::GUIHSplitter> hspPackets;
			NN<UI::GUITextBox> txtRAW;

			static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
			static void __stdcall OnTimerTick(AnyType userData);
			static void __stdcall OnPacketsSelChg(AnyType userData);
			static void __stdcall PacketInfoFree(NN<PacketInfo> packet);
		public:
			AVIRWSDListenerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWSDListenerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
