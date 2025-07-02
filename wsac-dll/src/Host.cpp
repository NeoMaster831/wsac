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
        throw HostNotCurrentException();
    }

    return *_current;
}

Host::Host()
{
    if (_current)
    {
        throw HostAlreadyCurrentException();
    }
    _current = this;
}

} // namespace wsac