#include <cstdlib>
#include "stdafx.h"
#include "Bodemonderzoek.h"

// --- Lib Stuff --- //
#include <cstdlib>
#include <VrLib/Kernel.h>
#include <VrLib\ServerConnection.h>

int main(int argc, char* argv[])
{
	vrlib::Kernel* kernel = vrlib::Kernel::getInstance();  // Get the kernel
	Bodemonderzoek* application = nullptr;

	// Load any config files specified on the command line
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--config") == 0)
		{
			i++;
			kernel->loadConfig(argv[i]);
		}
	}
	if (!application)
		application = new Bodemonderzoek();

	kernel->setApp(application);
	kernel->start();
	return 0;
}

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
