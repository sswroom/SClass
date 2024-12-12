#ifndef _SM_NET_JMETER_JMETERELEMENT
#define _SM_NET_JMETER_JMETERELEMENT
#include "Text/String.h"

namespace Net
{
	namespace JMeter
	{
		class JMeterElement
		{
		protected:
			NN<Text::String> name;

		public:
			JMeterElement(Text::CStringNN name) { this->name = Text::String::New(name); }
			virtual ~JMeterElement() { this->name->Release(); }
		};
	}
}
#endif
