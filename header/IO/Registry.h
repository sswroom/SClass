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
		void *hand;

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
		Registry *OpenSubReg(const WChar *name);
		WChar *GetSubReg(WChar *buff, UOSInt index);

		void SetValue(const WChar *name, Int32 value);
		void SetValue(const WChar *name, const WChar *value);
		void DelValue(const WChar *name);

		Int32 GetValueI32(const WChar *name);
		WChar *GetValueStr(const WChar *name, WChar *buff);

		Bool GetValueI32(const WChar *name, Int32 *value);

		WChar *GetName(WChar *nameBuff, UOSInt index);
	};
}
#endif
