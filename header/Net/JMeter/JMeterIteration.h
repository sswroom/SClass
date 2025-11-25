#ifndef _SM_NET_JMETER_JMETERITERATION
#define _SM_NET_JMETER_JMETERITERATION
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Net/JMeter/JMeterListener.h"

namespace Net
{
	namespace JMeter
	{
		class JMeterIteration
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Data::ArrayListNN<JMeterListener> listeners;
			Data::ArrayList<AnyType> listenerData;

		public:
			JMeterIteration(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : listeners(2), listenerData(2) { this->clif = clif; this->ssl = ssl; }
			~JMeterIteration() { UOSInt i = listeners.GetCount(); while (i-- > 0) listeners.GetItemNoCheck(i)->IterationEnd(NNTHIS, listenerData.GetItem(i)); }

			NN<Net::TCPClientFactory> GetTCPClientFactory() const { return this->clif; }
			Optional<Net::SSLEngine> GetSSLEngine() const { return this->ssl; }
			void AddListener(NN<JMeterListener> listener) { listeners.Add(listener); listenerData.Add(listener->IterationBegin(NNTHIS)); }
			void HTTPBegin(NN<HTTPClient> cli) const { UOSInt i = listeners.GetCount(); while (i-- > 0) listeners.GetItemNoCheck(i)->HTTPBegin(NNTHIS, listenerData.GetItem(i), cli); }
			void HTTPEnd(NN<HTTPClient> cli, Bool succ) const { UOSInt i = listeners.GetCount(); while (i-- > 0) listeners.GetItemNoCheck(i)->HTTPEnd(NNTHIS, listenerData.GetItem(i), cli, succ); }
		};
	}
}
#endif
