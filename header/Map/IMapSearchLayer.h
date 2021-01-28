#ifndef _SM_MAP_IMAPSEARCHLAYER
#define _SM_MAP_IMAPSEARCHLAYER

namespace Map
{
	class IMapSearchLayer
	{
	public:
		virtual ~IMapSearchLayer() {};

		virtual Bool IsError() = 0;

		virtual UTF8Char *GetPGLabelD(UTF8Char *buff, Double xpos, Double ypos) = 0;
		virtual UTF8Char *GetPLLabelD(UTF8Char *buff, Double xpos, Double ypos, Double *xposOut, Double *yposOut) = 0;
	};
};
#endif
