#pragma once

#include "Include.hpp"

namespace wsac
{

struct HostNotCurrentException final : std::exception
{
};

struct HostAlreadyCurrentException final : std::exception
{
};

class Host
{
    std::unordered_map<std::string, std::pair<void *, void (*)(void *)>> _registered;
    std::stop_source _ss;
    bool _disposed = false;

  public:
    static Host &Current();

    Host();

    template <typename T> void Add()
    {
        auto *v = new T(_ss.get_token());
        _registered[typeid(T).name()] = std::make_pair(v, [](void *vp) { delete static_cast<T *>(vp); });
    }

    template <typename T> T &Get()
    {
        return *static_cast<T *>(_registered.at(typeid(T).name()).first);
    }

    ~Host()
    {
        if (_disposed)
            return;
        _disposed = true;

        _ss.request_stop();
        for (auto [v, del] : _registered | std::views::values)
        {
            del(v);
        }
    }
};

} // namespace wsac
