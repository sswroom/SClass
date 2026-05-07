#ifndef _SM_PYTHON_PYTHONUNICODE
#define _SM_PYTHON_PYTHONUNICODE
#include "Python/PythonObject.h"

namespace Python
{
	class PythonUnicode : public PythonObject
	{
	public:
		PythonUnicode(AnyType unicode);
		PythonUnicode(Text::CStringNN str);
		virtual ~PythonUnicode();

		virtual ObjectType GetObjectType() const;
	};
}
#endif