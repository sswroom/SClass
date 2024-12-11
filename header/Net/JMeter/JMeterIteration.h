#ifndef _SM_NET_JMETER_JMETERITERATION
#define _SM_NET_JMETER_JMETERITERATION
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"

namespace Net
{
	namespace JMeter
	{
		class JMeterIteration
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;

		public:
			JMeterIteration(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) { this->clif = clif; this->ssl = ssl; }
			~JMeterIteration() { }

			NN<Net::TCPClientFactory> GetTCPClientFactory() const { return this->clif; }
			Optional<Net::SSLEngine> GetSSLEngine() const { return this->ssl; }
		};
	}
}
#endif
