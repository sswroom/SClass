#ifndef _SM_NET_MIME
#define _SM_NET_MIME
namespace Net
{
	class MIME
	{
	private:
		typedef struct
		{
			const Char *ext;
			const Char *mime;
		} MIMEEntry;

		static MIMEEntry mimeList[];
	public:
		static const UTF8Char *GetMIMEFromExt(const UTF8Char *ext);
	};
};
#endif
