#ifndef _SM_IO_UDPLOG
#define _SM_IO_UDPLOG
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListInt64.h"
#include "IO/StreamData.h"
#include "IO/LogFile.h"

namespace IO
{
	class UDPLog : public IO::LogFile
	{
	protected:
		Bool ParseLog(UInt8 *dataBuff, UIntOS dataSize, NN<Text::StringBuilderUTF8> sb, Bool detail) const;
		UDPLog(NN<Text::String> sourceName);
	public:
		virtual ~UDPLog();
	};
}
#endif
