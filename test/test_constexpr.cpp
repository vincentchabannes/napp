
#include <napp/na.hpp>

using element1 = NA::named_argument_t<struct element1_tag>;
using element2 = NA::named_argument_t<struct element2_tag>;

constexpr auto& _element1 = NA::identifier<element1>;
constexpr auto& _element2 = NA::identifier<element2>;


template <typename ... TT>
constexpr auto test1( NA::args<TT...> && v )
{
    auto && fn = v.template get<element1>();
    auto && ln = v.template get<element2>();
    return fn+ln;
}

template <typename ... Ts>
constexpr auto test1(  Ts && ... v )
{
    return test1( NA::make_arguments( std::forward<Ts>(v)... ) );
}

int main()
{
    static_assert( test1( _element1=5, _element2=10 ) == 15, "wrong result" );
}
