#ifndef _SM_IO_REGISTRY
#define _SM_IO_REGISTRY

namespace IO
{
	class Registry
	{
	public:
		typedef enum
		{
			REG_USER_THIS,
			REG_USER_ALL
		} RegistryUser;

	private:
		struct ClassData;
	
		static void *thisRegistryFile;
		static void *allRegistryFile;
		ClassData *clsData;

		static void *OpenUserType(RegistryUser usr);
		static void CloseInternal(void *reg);
	public:
		static Registry *OpenSoftware(RegistryUser usr, const WChar *compName, const WChar *appName);
		static Registry *OpenSoftware(RegistryUser usr, const WChar *compName);
		static Registry *OpenLocalHardware();
		static Registry *OpenLocalSoftware(const WChar *softwareName);
		static void CloseRegistry(IO::Registry *reg);

	private:
		Registry(void *hand);
		~Registry();

	public:
		Registry *OpenSubReg(UnsafeArray<const WChar> name);
		UnsafeArrayOpt<WChar> GetSubReg(UnsafeArray<WChar> buff, UOSInt index);

		void SetValue(UnsafeArray<const WChar> name, Int32 value);
		void SetValue(UnsafeArray<const WChar> name, UnsafeArray<const WChar> value);
		void DelValue(UnsafeArray<const WChar> name);

		Int32 GetValueI32(UnsafeArray<const WChar> name);
		UnsafeArrayOpt<WChar> GetValueStr(UnsafeArray<const WChar> name, UnsafeArray<WChar> buff);

		Bool GetValueI32(UnsafeArray<const WChar> name, OutParam<Int32> value);

		UnsafeArrayOpt<WChar> GetName(UnsafeArray<WChar> nameBuff, UOSInt index);
	};
}
#endif
