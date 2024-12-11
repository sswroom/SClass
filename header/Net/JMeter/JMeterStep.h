#ifndef _SM_NET_JMETER_JMETERSTEP
#define _SM_NET_JMETER_JMETERSTEP
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/JMeter/JMeterIteration.h"

namespace Net
{
	namespace JMeter
	{
		struct JMeterResult
		{
			UInt64 dataSize;
		};

		class JMeterStep
		{
		protected:
			NN<Text::String> name;

		public:
			JMeterStep(Text::CStringNN name) { this->name = Text::String::New(name); }
			virtual ~JMeterStep() { this->name->Release(); }

			virtual Bool Step(NN<JMeterIteration> iter, NN<JMeterResult> result) const = 0;
		};
	}
}
#endif
