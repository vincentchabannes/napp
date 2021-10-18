
#include "../include/na.hpp"

#include <string>
#include <iostream>
#include <sstream>


using first_name = NA::template_type<struct first_name_tag>;
using last_name = NA::template_type<struct last_name_tag>;

using first_name_string = NA::type<std::string, struct first_name_string_tag>;
using last_name_string = NA::type<std::string, struct last_name_string_tag>;

using data = NA::template_type<struct data_tag>;



constexpr auto& _first_name = NA::parameter<first_name>;
constexpr auto& _last_name = NA::parameter<last_name>;
constexpr auto& _data = NA::parameter<data>;


struct BBBB {};

std::ostream&
operator<<( std::ostream&o, BBBB const& b ) { return o << "BBBB"; }


void test1( NA::args<typename first_name::template required_as_t<std::string>,
                     typename last_name::template required_as_t<std::string> > && v )
{
    std::cout <<" hola" << std::endl;
    puts( v.template get<first_name>().c_str() );
    puts( v.template get<last_name>().c_str() );
}


void test1bis(NA::args<first_name_string, last_name_string> && v)
{

}


template <typename ... TT>
void test2( NA::args<TT...> && v )
{
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get<last_name>()
        << " "
        << v.template get_optional<data>( "blabla" );

    std::cout << res.str() << std::endl;
#if 1
  //puts(std::get<first_name>(v.values).value.c_str());
  //puts(std::get<first_template_name>(v.values).value.c_str());
  

  //auto && lastName = v.template get<last_name>();

  auto && blla2 = v.template get_optional<data>( "blabla" );
  //auto blla = N::get<other_name>( std::forward<N::args<TT...>>(v) );
  std::cout << "blla2="<<blla2<<std::endl;

  //auto blla2 = N::get_optional<other_name>( std::forward<N::args<TT...>>(v), "blabla" );

#endif
}

template <typename ... Ts>
void test2( Ts && ... v )
{
  //greet2( N::args<Ts...>{ v... } );
  //greet( N::args<first_name, last_name>{ std::forward<Ts>(v)... } );
  //greet2( { std::forward<Ts>(v)... } );

  //greet2( NA::args<Ts...>{ std::forward<Ts>(v)... } );
  test2( NA::args<Ts...>{ std::forward<Ts>(v)... } );
  //greet0( { std::forward<Ts>(v)... } );
}


int main()
{

  //greet3({NA::parameter<last_name> = "Bond", NA::parameter<first_name> = "James"});
  test1({_last_name= "Bond", _first_name="James" });
  test1({_first_name= "Bond", _last_name="James" });

  test2(_first_name= "James", _last_name="Bond" );
  test2(_last_name= "Bond", _first_name="James" );

  BBBB b;
  test2(_first_name= "Bond", _last_name="James",_data=b );
}
