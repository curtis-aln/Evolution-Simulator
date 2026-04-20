#include "simulation/simulation.h"
#include "Utils/UI/CrashLogger.h"
#include "settings.h"

int main()
{
    // Globally available settings loaded from toml file
	load_settings(SimulationSettings::settings_file_location);

    // Custom Debugger
    CrashLogger::set_exception_translator(); 

    try
    {
        Simulation().run_simulation();
    }
    catch (const std::exception& e) { CrashLogger::handle(e); }
    catch (...) { CrashLogger::handle(); }
}