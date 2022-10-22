#include "Image.h"

using namespace std;
int main()
{
	Image test("starting.bmp");
	test.Save("out.bmp");
	return 0;
}
