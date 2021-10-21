
#include "../include/na.hpp"

#include <string>
#include <iostream>
#include <sstream>

#include <cassert>

using first_name = NA::template_type<struct first_name_tag>;
using last_name = NA::template_type<struct last_name_tag>;

using first_name_string = NA::type<std::string, struct first_name_string_tag>;
using last_name_string = NA::type<std::string, struct last_name_string_tag>;

using data = NA::template_type<struct data_tag>;



constexpr auto& _first_name = NA::parameter<first_name>;
constexpr auto& _last_name = NA::parameter<last_name>;
constexpr auto& _data = NA::parameter<data>;

#if 1
std::string test1( NA::args<typename first_name::template required_as_t<std::string const&>,
                            typename last_name::template required_as_t<std::string> > && v )
{
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get<last_name>();

    //std::cout << "test1 : " << res.str() << std::endl;
    return res.str();
}

void test1bis(NA::args<first_name_string, last_name_string> && v)
{

}
#endif



struct Foo
{
    Foo() = delete;
    Foo(int a,int b) : M_i(a*b) {}

    Foo( Foo const& ) = delete;
    Foo( Foo && ) = default;//delete;
    Foo & operator=( Foo const& ) = delete;
    Foo & operator=( Foo && ) = delete;

    int i() const { return M_i; }

    void setI( int i ) { M_i = i; }
private :
    int M_i;
};

std::ostream&
operator<<( std::ostream&o, Foo const& b ) { return o << "i=" << b.i(); }

struct Baz : Foo
{
    Baz() = delete;
    Baz(int a,int b) : Foo(a+b,a-b) {}

    Baz( Baz const& ) = delete;
    Baz( Baz && ) = default;//delete;
    Baz & operator=( Baz const& ) = delete;
    Baz & operator=( Baz && ) = delete;
};


template <typename ... TT>
std::string test2( NA::args<TT...> && v )
{
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get_optional<last_name>("default_last_name")
        << " "
        << v.template get_optional<data>( "default_data_value" )
        << " "
        << v.template get_optional<data>( [](){ return "default_data_lambda"; } );

    std::cout << "test2 : " << res.str() << std::endl;
    return res.str();
}

template <typename ... Ts>
std::string test2( Ts && ... v )
{
  return test2( NA::args<Ts...>{ std::forward<Ts>(v)... } );
}


template <typename ... Ts>
std::string test2b( Ts && ... v )
{
    return test2( NA::args<typename first_name::template required_as_t<std::string const&>,
                  typename last_name::template required_as_t<std::string>,
                  typename data::template required_as_t<Foo &> >{ std::forward<Ts>(v)... } );
}




using test3_arg_type = NA::args<typename first_name::template required_as_t<std::string &>,
                                typename last_name::template required_as_t<std::string &>,
                                typename data::template required_as_t<Foo&> >;
void test3( test3_arg_type && v )
{
    v.template get<first_name>() = "James";
    v.template get<last_name>() = "Bond";
    v.template get<data>().setI( 34 );
}

template <typename ... Ts>
void test3( Ts && ... v )
{
    test3( test3_arg_type{ std::forward<Ts>(v)... } );
}


//void toto( std::string & a ) { std::cout << "a=" << a<< std::endl; }

// template <typename T,std::enable_if_t< std::is_lvalue_reference_v<T&&>, bool> = true>
// void toto(T && t)
// {
//     using vvv = typename T::hola;
// }

int main()
{
#if 1
    assert( test1({_first_name= "James", _last_name="Bond" }) == "James Bond" );
    assert( test1({_last_name= "Bond", _first_name="James" }) == "James Bond" );
    assert( test1({_last_name= "Bond", _first_name=std::string("James") }) == "James Bond" );
    std::string fn= "James";
    assert( test1({_first_name= fn, _last_name="Bond" }) == "James Bond" );


    assert( test2(_first_name="James" ) == "James default_last_name default_data_value default_data_lambda" );
    assert( test2(_first_name=fn, _last_name="Bond" ) == "James Bond default_data_value default_data_lambda" );
    assert( test2(_last_name= "Bond", _first_name="James" ) == "James Bond default_data_value default_data_lambda" );
    Foo b(3,5);
    assert( test2(_first_name= "James", _last_name="Bond",_data=b ) == "James Bond i=15 i=15" );
    assert( test2(_first_name= "James", _last_name="Bond",_data=Foo(4,3) ) == "James Bond i=12 i=12" );

    assert( test2b(_first_name= "James", _last_name="Bond",_data=Foo(6,5) ) == "James Bond i=30 i=30" );
    assert( test2b(_first_name= "James", _last_name="Bond",_data=b ) == "James Bond i=15 i=15" );
    Baz baz(2,5);
    assert( test2b(_first_name= "James", _last_name="Bond",_data=baz ) == "James Bond i=-21 i=-21" );
#endif

    std::string fn3,ln3;Foo foo3(0,0);
    test3( _data=foo3,_last_name=ln3,_first_name=fn3 );
    std::ostringstream res3;
    res3 << fn3 << " " << ln3 << " " << foo3;
    assert( res3.str() == "James Bond i=34" );

    fn3.clear();ln3.clear();
    Baz baz3(0,0);
    test3( _data=baz3,_last_name=ln3,_first_name=fn3 );
    std::ostringstream res3b;
    res3b  << fn3 << " " << ln3 << " i=" << baz3.i();
    assert( res3b.str() == "James Bond i=34" );





    return 0;
}
