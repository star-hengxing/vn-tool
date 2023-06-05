#include <Windows.h>
#include <psapi.h>

#include <base/unsafe/buffer_view.hpp>
#include "process_manager.hpp"

void process_manager::get_process_list() noexcept
{
    DWORD cbNeeded;
    id_size = (::EnumProcesses(id_list, sizeof(id_list), &cbNeeded) == 0) ? 0 : cbNeeded / sizeof(DWORD);
}

bool process_manager::contain(usize target_id) const noexcept
{
    // TODO: sort id_list and use binary search
    for (auto const id : unsafe::buffer_view{id_list, id_size})
    {
        if (id == target_id)
        {
            return true;
        }
    }
    return false;
}

DWORD process_manager::get_current_process() noexcept
{
    DWORD pid;
    HWND hwnd = ::GetForegroundWindow();
    ::GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}

std::wstring_view process_manager::get_process_filename(usize target_id) noexcept
{
    HANDLE process = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(target_id));
    if (process)
    {
        DWORD size = ::GetModuleFileNameExW(process, NULL, full_path, MAX_PATH);
        if (size != 0)
        {
            return {full_path, size};
        }

        ::CloseHandle(process);
    }
    return {};
}
