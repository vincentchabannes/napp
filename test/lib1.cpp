
#include "lib1.hpp"
#include <sstream>

const NA::identifier_t<last_name>& _last_name = NA::identifier<last_name>;

std::string test1Impl( NA::arguments<typename first_name::template required_as_t<std::string const&>,
                       typename last_name::template required_as_t<std::string> > && args )
{
    auto && fn = args.get(_first_name);
    auto && ln = args.get(_last_name);
    std::ostringstream res;
    res << fn << " " << ln;
    return res.str();
}

