#include <iostream>
#include <Windows.h>

int main() {
    const auto wsac = LoadLibraryA("wsac-dll.dll");

    if (!wsac) {
        std::cout << "Couldn't load AC library!" << '\n';
        std::cout << std::hex << GetLastError() << std::dec << '\n';
        return 1;
    }

    using pEnableAc = BOOLEAN(*)();
    using pDisableAc = void(*)();
    const auto enableAc = reinterpret_cast<pEnableAc>(GetProcAddress(wsac, "Enable"));
    const auto disableAc = reinterpret_cast<pDisableAc>(GetProcAddress(wsac, "Disable"));

    if (!enableAc || !disableAc) {
        std::cout << "Couldn't get module export!" << '\n';
        return 1;
    }

    if (!enableAc()) {
        std::cout << "Couldn't enable anticheat!" << '\n';
        return 1;
    }

    while (true) {
        int idle = 1;
        std::cin >> idle;
        if (idle == 0) {
            break;
        }
    }

    disableAc();

    return 0;
}