#ifndef _SM_IO_JMETERLOG
#define _SM_IO_JMETERLOG
#include "Data/FastStringMapNN.h"
#include "Data/ArrayListStringNN.h"

namespace IO
{
	struct JMeterLogItem;
	struct JMeterGroup
	{
		NN<Text::String> label;
		NN<Text::String> url;
		Data::ArrayListNN<JMeterLogItem> logs;
	};

	struct JMeterThread
	{
		NN<Text::String> name;
		Data::ArrayListNN<JMeterLogItem> logs;
	};

	struct JMeterLogItem
	{
		Int64 startTime;
		Int64 endTime;
		NN<JMeterGroup> group;
		NN<Text::String> responseCode;
		NN<Text::String> responseMessage;
		NN<Text::String> threadName;
		Bool success;
		NN<Text::String> failureMessage;
		UInt32 bytes;
		UInt32 sentBytes;
		UInt32 grpThreads;
		UInt32 allThreads;
		UInt32 latency;
		UInt32 idleTime;
		UInt32 connect;
	};

	class JMeterLog
	{
	private:
		Data::FastStringMapNN<JMeterGroup> groups;
		Data::FastStringMapNN<JMeterThread> threads;
		Data::ArrayListNN<JMeterLogItem> logs;
		Data::ArrayListStringNN responseCodes;
		Int64 minTime;
		Int64 maxTime;

		static void FreeGroup(NN<JMeterGroup> group);
		static void FreeThread(NN<JMeterThread> thread);
		static void FreeLog(NN<JMeterLogItem> log);
		NN<JMeterGroup> GetOrCreateGroup(Text::CStringNN label, Text::CStringNN url);
		NN<Text::String> NewResponseCode(Text::CStringNN responseCode);
		NN<JMeterThread> GetOrCreateThread(Text::CStringNN threadName);
	public:
		JMeterLog(Text::CStringNN fileName);
		~JMeterLog();

		Int64 GetMinTime() const { return this->minTime; }
		Int64 GetMaxTime() const { return this->maxTime; }
		void GetGroups(NN<Data::ArrayListNN<JMeterGroup>> groups) const { groups->AddAll(this->groups); }
		void GetThreads(NN<Data::ArrayListNN<JMeterThread>> threads) const { threads->AddAll(this->threads); }
		UOSInt GetConcurrCnt(Int64 ts) const;
	};
}
#endif
