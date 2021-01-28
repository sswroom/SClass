//require Strmiids.lib

namespace Media
{
	class VideoCapSource
	{
	private:

	public:
		static Int32 GetDeviceCount();
		static WChar *GetDeviceName(WChar *buff, Int32 devNo);

		VideoCapSource(WChar *devName, Int32 freq, Int16 nbits, Int16 nChannels);
		VideoCapSource(Int32 devId, Int32 freq, Int16 nbits, Int16 nChannels);
		~VideoCapSource();
	};
};
