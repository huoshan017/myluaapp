#include "myapp.h"
#include "HSSystem.h"

int main()
{
	int res = 0;
	MyApp app;
	if (!app.Init())
		return -1;
	while (true) {
		if (!app.Run())
			return -1;
		HSSystem::MSleep(10);
	}
	app.Close();
	return res;
}