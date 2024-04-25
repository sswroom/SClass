#ifndef _SM_NET_SNMPMANAGER
#define _SM_NET_SNMPMANAGER
#include "Data/FastMap.h"
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
			Text::String *contact;
			Text::String *name;
			Text::String *location;
			Text::String *model;
			Text::String *vendor;
			Text::String *cpuName;
			UInt8 mac[6];
			Data::ArrayList<ReadingInfo *> *readingList;
		} AgentInfo;
		
	private:
		Net::SNMPClient *cli;
		Sync::Mutex agentMut;
		Data::ArrayList<AgentInfo *> agentList;
		Data::FastMap<UInt32, AgentInfo*> ipv4Agents;

		static void FreeAllItems(Data::ArrayList<Net::SNMPUtil::BindingItem*> *itemList);
	public:
		SNMPManager(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log);
		~SNMPManager();

		Bool IsError();
		void UpdateValues();
		UOSInt GetAgentList(NN<Data::ArrayList<AgentInfo*>> agentList);

		AgentInfo *AddAgent(NN<const Net::SocketUtil::AddressInfo> addr, NN<Text::String> community);
		UOSInt AddAgents(NN<const Net::SocketUtil::AddressInfo> addr, NN<Text::String> community, Data::ArrayList<AgentInfo*> *agentList, Bool scanIP);

		static void Agent2Record(const AgentInfo *agent, SSWR::SMonitor::ISMonitorCore::DevRecord2 *rec, Int64 *cliId);
		static Int64 Agent2CliId(const AgentInfo *agent);
	};
}
#endif
