
#include <napp/na.hpp>


using element1 = NA::named_argument_t<struct element1_tag>;
using element2 = NA::named_argument_t<struct element2_tag>;
constexpr auto& _element1 = NA::identifier<element1>;
constexpr auto& _element2 = NA::identifier<element2>;
constexpr auto& _element3 = NA::identifier<struct element3_tag>;
constexpr auto& _element4 = NA::identifier<struct element4_tag>;


template <typename ... TT>
constexpr auto test1( NA::arguments<TT...> && v )
{
    auto && e1 = v.template get<element1>();
    auto && e2 = v.template get_else<element2>( 4 );
    auto && e3 = v.get(_element3);
    auto && e4 = v.get_else( _element4, 7 );
    return e1+e2+e3+e4;
}

template <typename ... Ts>
constexpr auto test1( Ts && ... v )
{
    return test1( NA::make_arguments( std::forward<Ts>(v)... ) );
}

int main()
{
    static_assert( test1( _element1=5, _element3=10 ) == 5+4+10+7, "wrong result" );
    static_assert( test1( _element3=10, _element1=5 ) == 5+4+10+7, "wrong result" );
    static_assert( test1( _element1=5, _element2=9, _element3=10 ) == 5+9+10+7, "wrong result" );
    static_assert( test1( _element4=12, _element1=5, _element2=9, _element3=10 ) == 5+9+10+12, "wrong result" );
}
