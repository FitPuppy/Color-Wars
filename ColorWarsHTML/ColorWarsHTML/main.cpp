#define WEB

#ifdef WEB
#include <emscripten.h>
#endif

int main() {

#ifdef WEB
	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			EM_ASM({ (setCell([$1], [$2],[$0], "black", 3); )},0, x, y);
		}
	}
#endif
}