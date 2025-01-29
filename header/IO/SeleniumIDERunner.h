#ifndef _SM_IO_SELENIUMIDERUNNER
#define _SM_IO_SELENIUMIDERUNNER
#include "IO/SeleniumIDE.h"

namespace IO
{
	class SeleniumIDERunner
	{
	public:
		SeleniumIDERunner();
		~SeleniumIDERunner();

		Bool Run(NN<SeleniumTest> test);
	};
}
#endif
