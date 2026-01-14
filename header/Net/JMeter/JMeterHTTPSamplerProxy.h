#ifndef _SM_NET_JMETER_JMETERHTTPSAMPLERPROXY
#define _SM_NET_JMETER_JMETERHTTPSAMPLERPROXY
#include "Data/ByteBuffer.h"
#include "Net/WebUtil.h"
#include "Net/JMeter/JMeterStep.h"

namespace Net
{
	namespace JMeter
	{
		class JMeterHTTPSamplerProxy : public JMeterStep
		{
		private:
			Net::WebUtil::RequestMethod method;
			NN<Text::String> url;
			Bool followRedirects;
			Bool useKeepalive;
			Bool enableGZip;
			Optional<Text::String> postType;
			Optional<Data::ByteBuffer> postData;
		public:
			JMeterHTTPSamplerProxy(Text::CStringNN name, Net::WebUtil::RequestMethod method, Text::CStringNN url, Bool followRedirects, Bool useKeepalive, Bool enableGZip) : JMeterStep(name) { this->method = method; this->url = Text::String::New(url); this->followRedirects = followRedirects; this->useKeepalive = useKeepalive; this->postData = nullptr; this->postType = nullptr; this->enableGZip = enableGZip; }
			virtual ~JMeterHTTPSamplerProxy() {this->url->Release(); OPTSTR_DEL(postType); this->postData.Delete(); };

			virtual Bool Step(NN<JMeterIteration> iter, NN<JMeterResult> result) const;

			void SetPostContent(Text::CStringNN postType, Data::ByteArrayR postData) {OPTSTR_DEL(this->postType); this->postData.Delete(); this->postType = Text::String::New(postType); NEW_CLASSOPT(this->postData, Data::ByteBuffer(postData)); }
			void ToString(NN<Text::StringBuilderUTF8> sb) const;

		};
	}
}
#endif
