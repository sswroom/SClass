#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORALERTER
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORALERTER
#include "Text/StringBuilderUTF8.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorAlerter
		{
		public:
			virtual ~ServerMonitorAlerter() {};

			virtual Bool HasError() const = 0;
			virtual Bool Send(Text::CStringNN serverName) = 0;
			virtual Bool BuildSetting(NN<Text::StringBuilderUTF8> setting) = 0;
			virtual Bool BuildTarget(NN<Text::StringBuilderUTF8> target) = 0;
		};
	}
}
#endif