
#ifndef NA_TEST_LIB_H
#define NA_TEST_LIB_H 1

#include <napp/na.hpp>
#include <string>

using first_name = NA::named_argument_t<struct first_name_tag>;
using last_name = NA::named_argument_t<struct last_name_tag>;

// using inline
inline constexpr auto& _first_name = NA::identifier<first_name>;

// using extern
extern const NA::identifier_t<last_name>& _last_name;

std::string test1Impl( NA::arguments<typename first_name::template required_as_t<std::string const&>,
                                     typename last_name::template required_as_t<std::string> > && args );

template <typename ... Ts>
std::string test1a( Ts && ... v )
{
    return test1Impl( NA::make_arguments( std::forward<Ts>(v)... ) );
}

template <typename ... Ts>
std::string test1b( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... ).add_default_arguments( NA::make_default_argument( _first_name, "default_first_name" ),
                                                                                    NA::make_default_argument( _last_name, std::string("default_last_name") ) );

    return test1Impl( std::move( args ) );
}


#endif
