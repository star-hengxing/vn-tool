#include <platform/process_manager.hpp>
#include <platform/os.hpp>
#include "Program.hpp"

int main()
{
    process_manager process_m;

    while (true)
    {
        process_m.get_process_list();
        os::sleep(1000);
    }
}
