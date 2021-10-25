
#include <napp/na.hpp>

#include <iostream>

constexpr auto& _elt1 = NA::identifier<struct elt1_tag>;
constexpr auto& _elt2 = NA::identifier<struct elt2_tag>;
constexpr auto& _elt3 = NA::identifier<struct elt3_tag>;


template <typename ... Ts>
void func( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );

    std::cout << "elt1 : " << args.get(_elt1) << "\n"
              << "elt2 : " << args.get_else(_elt2, "a default value" ) << "\n"
              << "elt3 : " << args.get_else_invocable(_elt3, []() { return 3.1415; } )
              << std::endl;
}


int main()
{
    func( _elt2=36,_elt1="toto",_elt3=std::string("string as rvalue") );

    func( _elt1=36 );

    return 0;
}
