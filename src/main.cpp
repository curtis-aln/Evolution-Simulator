#include "simulation/simulation.h"
#include "Utils/UI/CrashLogger.h"
#include "settings.h"

int main()
{
	load_settings("media/aria_settings.toml");

    CrashLogger::set_exception_translator(); 

    try
    {
        Simulation().run_simulation();
    }
    catch (const std::exception& e) { CrashLogger::handle(e); }
    catch (...) { CrashLogger::handle(); }
}