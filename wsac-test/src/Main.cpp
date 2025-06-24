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
    const auto enableAc = reinterpret_cast<pEnableAc>(GetProcAddress(wsac, "Enable"));

    if (!enableAc) {
        std::cout << "Couldn't get Enable export!" << '\n';
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

    return 0;
}