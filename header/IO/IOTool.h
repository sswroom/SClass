#ifndef _SM_IO_IOTOOL
#define _SM_IO_IOTOOL

namespace IO
{
	class IOTool
	{
	public:
		static Bool EchoFile(const UTF8Char *fileName, const Char *msg);
	};
}
#endif
