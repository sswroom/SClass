#ifndef _SM_PYTHON_PYTHONCORE
#define _SM_PYTHON_PYTHONCORE

namespace Python
{
	class PythonModule;
	class PythonObject;
	class PythonCore
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;

	public:
		PythonCore();
		~PythonCore();

		UnsafeArrayOpt<const UTF8Char> GetVersion() const;
		UnsafeArrayOpt<const UTF8Char> GetPlatform() const;
		UnsafeArrayOpt<const UTF8Char> GetCopyright() const;
		UnsafeArrayOpt<const UTF8Char> GetCompiler() const;
		UnsafeArrayOpt<const UTF8Char> GetBuildInfo() const;

		Optional<PythonObject> GetConstNone() const;
		Optional<PythonObject> GetConstFalse() const;
		Optional<PythonObject> GetConstTrue() const;
		Optional<PythonObject> GetConstEllipsis() const;
		Optional<PythonObject> GetConstNotImplemented() const;
		Optional<PythonObject> GetConstZero() const;
		Optional<PythonObject> GetConstOne() const;
		Optional<PythonObject> GetConstEmptyStr() const;
		Optional<PythonObject> GetConstEmptyBytes() const;
		Optional<PythonObject> GetConstEmptyTuple() const;

		Int32 RunString(UnsafeArray<const UTF8Char> s);
		Optional<PythonModule> ImportModule(UnsafeArray<const UTF8Char> name);
	};
}
#endif
