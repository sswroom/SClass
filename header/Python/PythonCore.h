#ifndef _SM_PYTHON_PYTHONCORE
#define _SM_PYTHON_PYTHONCORE

namespace Python
{
	class PythonCore
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;

	public:
		PythonCore();
		~PythonCore();

		UnsafeArrayOpt<const WChar> GetProgramName() const;
	};
}
#endif
