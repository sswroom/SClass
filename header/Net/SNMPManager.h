#ifndef _SM_NET_SNMPMANAGER
#define _SM_NET_SNMPMANAGER
#include "Data/FastMapNN.h"
#include "Net/SNMPClient.h"
#include "SSWR/SMonitor/ISMonitorCore.h"
#include "SSWR/SMonitor/SAnalogSensor.h"

namespace Net
{
	class SNMPManager
	{
	public:
		typedef struct
		{
			NN<Text::String> name;
			UOSInt index;
			UInt8 objId[64];
			UOSInt objIdLen;
			Double mulVal;
			Int32 invVal;
			SSWR::SMonitor::SAnalogSensor::ReadingType readingType;
			Bool valValid;
			Double currVal;
		} ReadingInfo;
		
		typedef struct
		{
			Net::SocketUtil::AddressInfo addr;
			NN<Text::String> community;
			UInt8 objId[64];
			UOSInt objIdLen;
			NN<Text::String> descr;
			Optional<Text::String> contact;
			Optional<Text::String> name;
			Optional<Text::String> location;
			Optional<Text::String> model;
			Optional<Text::String> vendor;
			Optional<Text::String> cpuName;
			UInt8 mac[6];
			Data::ArrayListNN<ReadingInfo> readingList;
		} AgentInfo;
		
	private:
		Net::SNMPClient *cli;
		Sync::Mutex agentMut;
		Data::ArrayListNN<AgentInfo> agentList;
		Data::FastMapNN<UInt32, AgentInfo> ipv4Agents;

		static void FreeAllItems(NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList);
	public:
		SNMPManager(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log);
		~SNMPManager();

		Bool IsError();
		void UpdateValues();
		UOSInt GetAgentList(NN<Data::ArrayListNN<AgentInfo>> agentList);

		Optional<AgentInfo> AddAgent(NN<const Net::SocketUtil::AddressInfo> addr, NN<Text::String> community);
		UOSInt AddAgents(NN<const Net::SocketUtil::AddressInfo> addr, NN<Text::String> community, NN<Data::ArrayListNN<AgentInfo>> agentList, Bool scanIP);

		static void Agent2Record(NN<const AgentInfo> agent, NN<SSWR::SMonitor::ISMonitorCore::DevRecord2> rec, OutParam<Int64> cliId);
		static Int64 Agent2CliId(NN<const AgentInfo> agent);
	};
}
#endif
