#include "Host.hpp"

namespace
{
wsac::Host *_current;
}

namespace wsac
{

Host &Host::Current()
{
    if (!_current)
    {
        // TODO : use custom exception
        throw std::exception();
    }

    return *_current;
}

Host::Host()
{
    if (_current)
    {
        // TODO : use custom exception
        throw std::exception();
    }

    _current = this;
}

} // namespace wsac