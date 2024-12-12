#ifndef _SM_NET_JMETER_JMETERCOOKIEMANAGER
#define _SM_NET_JMETER_JMETERCOOKIEMANAGER
#include "Net/JMeter/JMeterListener.h"

namespace Net
{
	namespace JMeter
	{
		class JMeterCookieManager : public JMeterListener
		{
		public:
			JMeterCookieManager(Text::CStringNN name) : JMeterListener(name) { }
			virtual ~JMeterCookieManager() { }

			virtual AnyType IterationBegin(NN<JMeterIteration> iter) const;
			virtual void IterationEnd(NN<JMeterIteration> iter, AnyType data) const;
			virtual void HTTPBegin(NN<const JMeterIteration> iter, AnyType data, NN<Net::HTTPClient> cli) const;
			virtual void HTTPEnd(NN<const JMeterIteration> iter, AnyType data, NN<Net::HTTPClient> cli, Bool succ) const;
		};
	}
}
#endif
