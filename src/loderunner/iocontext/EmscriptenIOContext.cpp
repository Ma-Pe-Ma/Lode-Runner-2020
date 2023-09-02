#include "EmscriptenIOContext.h"

#ifdef __EMSCRIPTEN__
namespace EmscriptenHelper {
	EM_JS(int, canvas_get_width, (), {
		return canvas.width;
		});

	EM_JS(int, canvas_get_height, (), {
		return canvas.height;
		});

	EM_JS(int, screen_get_width, (), {
		return screen.width;
		});

	EM_JS(int, screen_get_height, (), {
		return screen.height;
		});

	EM_JS(bool, is_mobile, (), {
		var a = false;

		if (navigator.userAgent.match(/Android/i)
			 || navigator.userAgent.match(/webOS/i)
			 || navigator.userAgent.match(/iPhone/i)
			 || navigator.userAgent.match(/iPad/i)
			 || navigator.userAgent.match(/iPod/i)
			 || navigator.userAgent.match(/BlackBerry/i)
			 || navigator.userAgent.match(/Windows Phone/i)
			 ) {
				a = true;
		}

		return a;
		});

	EM_JS(void, set_cookie, (const char* key, const char* value), {
			//console.log("Setting cookie: " + UTF8ToString(key) + "=" + UTF8ToString(value) + ";");
			document.cookie = UTF8ToString(key) + "=" + UTF8ToString(value) + ";path=/;";
		});
	
	EM_JS(char*, get_cookies, (), {
			//console.log("Getting cookies: " + document.cookie);
			return stringToNewUTF8(document.cookie);
		});
}
#endif // __EMSCRIPTEN__