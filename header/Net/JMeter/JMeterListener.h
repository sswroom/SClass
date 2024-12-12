#ifndef _SM_NET_JMETER_JMETERLISTENER
#define _SM_NET_JMETER_JMETERLISTENER
#include "Net/HTTPClient.h"
#include "Net/JMeter/JMeterElement.h"

namespace Net
{
	namespace JMeter
	{
		class JMeterIteration;
		class JMeterListener : public JMeterElement
		{
		public:
			JMeterListener(Text::CStringNN name) : JMeterElement(name) { }
			virtual ~JMeterListener() { }

			virtual AnyType IterationBegin(NN<JMeterIteration> iter) const = 0;
			virtual void IterationEnd(NN<JMeterIteration> iter, AnyType data) const = 0;
			virtual void HTTPBegin(NN<const JMeterIteration> iter, AnyType data, NN<Net::HTTPClient> cli) const = 0;
			virtual void HTTPEnd(NN<const JMeterIteration> iter, AnyType data, NN<Net::HTTPClient> cli, Bool succ) const = 0;
		};
	}
}
#endif
