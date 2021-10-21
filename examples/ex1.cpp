
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


struct BBBB
{
    BBBB() = delete;
    BBBB(int i,double d) : M_i(i), M_d(d) {}

    BBBB( BBBB const& ) = delete;
    //BBBB( BBBB && ) = default;//delete;
    BBBB & operator=( BBBB const& ) = delete;
    BBBB & operator=( BBBB && ) = delete;

    int i() const { return M_i; }
    double d() const { return M_d; }
private :
    int M_i;
    double M_d;
};

std::ostream&
operator<<( std::ostream&o, BBBB const& b ) { return o << "i=" << b.i() << " d="<< b.d(); }




#if 1
void test1( NA::args<typename first_name::template required_as_t<std::string>,
                     typename last_name::template required_as_t<std::string> > && v )
{
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get<last_name>();

    std::cout << "test1 : " << res.str() << std::endl;
}

void test1bis(NA::args<first_name_string, last_name_string> && v)
{

}
#endif



template <typename ... TT>
void test2( NA::args<TT...> && v )
{
#if 1
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get<last_name>()
        << " "
        << v.template get_optional<data>( "blabla" );

    std::cout << "test2 : " << res.str() << std::endl;
#if 1
  //puts(std::get<first_name>(v.values).value.c_str());
  //puts(std::get<first_template_name>(v.values).value.c_str());
  

  //auto && lastName = v.template get<last_name>();
#if 0
  auto && blla2 = v.template get_optional<data>( "blabla" );
  //auto blla = N::get<other_name>( std::forward<N::args<TT...>>(v) );
  std::cout << "blla2="<<blla2<<std::endl;
#endif

  std::cout << "test2 optional value : " << v.template get_optional<data>( "default" ) << std::endl;
  std::cout << "test2 optional lambda : " << v.template get_optional<data>( [](){ return "default"; } ) << std::endl;
  

  //auto blla2 = N::get_optional<other_name>( std::forward<N::args<TT...>>(v), "blabla" );
#endif
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







#if 0

#if 0
template <typename ... TT>
void test3( NA::args<TT...> && v )
{
    auto && blla2 = v.template get_optional<data>( "blabla" );
    //auto blla = N::get<other_name>( std::forward<N::args<TT...>>(v) );
    std::cout << "blla2="<<blla2<<std::endl;
}
#endif

#if 0
template <typename T>
void test3( NA::args<
            typename first_name::template required_as_t<std::string>,
            typename last_name::template required_as_t<std::string>,
            typename last_name::template required_as_t<T>
            > && v )
{

}
#endif
template <typename T>
using toto_type = NA::args<
    typename first_name::template required_as_t<std::string>,
    typename last_name::template required_as_t<std::string>,
    typename data::template required_as_t<T>
    >;

template <typename T>
void test3( toto_type<T> & v )
{
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get<last_name>()
        << " "
        << v.template get<data>();

    std::cout << "test3 : " << res.str() << std::endl;

}

template <typename ... Ts>
void test3( Ts && ... v )
{
#if 1
    NA::default_parameter<first_name>( []() { return "toto"; } );
    NA::default_parameter<first_name>( "toto" );

#if 0
    //test3( NA::args<Ts...>{ std::forward<Ts>(v)... } );

    //NA::args<Ts...> args{ std::forward<Ts>(v)... };


    //args.template extend< toto_type<double> >( _data=v; )


    auto bbb = NA::extend<toto_type<BBBB>>( NA::args<Ts...>{ std::forward<Ts>(v)... },//args,
                                              NA::default_parameter<first_name>{ []() { return "toto"; } },
                                              _data= []() { return "bloiuhbb"; } );

//toto_type<double> res;
#endif
    //auto ccc = NA::args<Ts...>{ std::forward<Ts>(v)... };
    //ccc = 3;
#if 1
    auto && bbb = NA::extend<toto_type<BBBB>>( NA::args<Ts...>{ std::forward<Ts>(v)... },
                                               NA::default_parameter<first_name>( []() { return "toto"; } ),
                                               NA::default_parameter<last_name>( "titi" ) );


    test3( bbb );
#endif
#endif
}


template <typename TTT>
void
testLambda( TTT && t )
{
    using thetype = std::decay_t<TTT>;
#if 0
    using value_type = typename std::conditional<
        std::is_invocable_v<quantity_type>,
        std::invoke_result<quantity_type>,
        Feel::type_identity<quantity_type>
        >::type::type;
#endif
    if constexpr ( std::is_invocable_v<thetype> )
                     std::cout << "invok:" << t() << std::endl;
}
#endif




int main()
{

    //greet3({NA::parameter<last_name> = "Bond", NA::parameter<first_name> = "James"});
    //_last_name= "Bond";
    //test1({_last_name= "Bond", _first_name="James" });
    test1({_last_name= std::move(std::string("Bond")), _first_name=std::string("James") });
    //_last_name=std::string("Bond");

    test1({_first_name= "James", _last_name="Bond" });
    std::string ff= "James";
    test1({_first_name= ff, _last_name="Bond" });

    test2(_first_name=ff/* "James"*/, _last_name="Bond" );

    test2(_last_name= "Bond", _first_name="James" );
    BBBB b(3,12.67);
    int aa=3;
    //test2(_first_name= "Bond", _last_name="James",_data=b );
    test2(_first_name= "James", _last_name="Bond",_data=b );
    //test2(_first_name= "Bond");
#if 0

  
#if 0
  test2(_first_name= "Bond", _last_name="James",_data=std::move(b) );
  test2(_first_name= "Bond", _last_name="James",_data=std::ref(b) );
  test2(_first_name= "Bond", _last_name="James",_data=std::ref(b) );
#endif
  // test2(_first_name= "Bond", _last_name="James",_data=b );
  
  //test3(_last_name= "Bond", _first_name="James" );
  // test3(_last_name= "BOO");//, _first_name="James" );
  //test3(_first_name= "BOO");//, _first_name="James" );

  test3(_first_name= "Bond", _last_name="James",_data=b );

  testLambda( []() { return 3.13; } );
#endif
}
