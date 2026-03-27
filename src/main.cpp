#include "simulation/simulation.h"
#include "Utils/UI/CrashLogger.h"

int main()
{
    CrashLogger::set_exception_translator(); 

    try
    {
        Simulation().run_simulation();
    }
    catch (const std::exception& e) { CrashLogger::handle(e); }
    catch (...) { CrashLogger::handle(); }
}

