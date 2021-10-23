
#include <iostream>

#include <napp/na.hpp>

#include <string>
#include <sstream>

#include <cassert>

//using first_name = NA::template_type<struct first_name_tag>;
using first_name = NA::named_argument_t<struct first_name_tag>;
using last_name = NA::named_argument_t<struct last_name_tag>;

//using first_name_string = NA::type<std::string, struct first_name_string_tag>;
//using last_name_string = NA::type<std::string, struct last_name_string_tag>;
using first_name_string = NA::named_argument_t<struct first_name_string_tag,std::string>;
//using last_name_string = NA::named_argument_t<struct last_name_string_tag,std::string>;
//using first_name_string = NA::named_argument_t<first_name,std::string>;
using last_name_string = NA::named_argument_t<last_name,std::string>;

using data = NA::named_argument_t<struct data_tag>;


constexpr auto& _first_name = NA::identifier<first_name>;
constexpr auto& _last_name = NA::identifier<last_name>;
constexpr auto& _data = NA::identifier<data>;

#if 1
std::string test1( NA::args<typename first_name::template required_as_t<std::string const&>,
                            typename last_name::template required_as_t<std::string> > && v )
{
    using tototo = NA::args<typename first_name::template required_as_t<std::string const&>,
                            typename last_name::template required_as_t<std::string> >;
    std::string bbb ;
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        //<< v.template get_optional<data>( "default_data_value" )
        //<< std::forward<tototo>( v ).template get_optional<data>( std::string("AAAAdefault_data_value") )
        //<< std::move( NA::default_parameter<data>( std::string("default_data_value") ).value() )
        << v.template get<last_name>();


    //std::cout << "test1 : " << res.str() << std::endl;

    //v.template get_optional<data>( std::string("default_data_value") );

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

    Foo( Foo const& ) = delete;//default;// delete;
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

std::string const& uu = "toto";


template <typename ... TT>
std::string test2( NA::args<TT...> && v )
{
    //std::string hol="default_last_name";
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get_else<last_name>("default_last_name")
        //<< v.template get_else<last_name>(std::string(hol))
        << " "
        << v.template get_else<data>( "default_data_value" )
        << " "
        << v.template get_else_invocable<data>( [](){ return "default_data_lambda"; } );

    //std::cout << "test2 : " << res.str() << std::endl;
    return res.str();
}

template <typename ... Ts>
std::string test2( Ts && ... v )
{
    return test2( NA::make_arguments( std::forward<Ts>(v)... ) );
}


template <typename ... Ts>
std::string test2b( Ts && ... v )
{
    return test2( NA::args<typename first_name::template required_as_t<std::string const&>,
                  typename last_name::template required_as_t<std::string>,
                  typename data::template required_as_t<Foo &> >{ std::forward<Ts>(v)... } );
}
#if 1
template <typename ... Ts>
void test2bbb( Ts && ... v )
{
    NA::args<typename first_name::template required_as_t<std::string /*const&*/>,
             typename last_name::template required_as_t<std::string>,
             typename data::template required_as_t<Foo &> >{ std::forward<Ts>(v)... };
}

#endif


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
    //test3( test3_arg_type{ std::forward<Ts>(v)... } );
    test3( NA::make_arguments<test3_arg_type>( std::forward<Ts>(v)... ) );

}


//void toto( std::string & a ) { std::cout << "a=" << a<< std::endl; }

// template <typename T,std::enable_if_t< std::is_lvalue_reference_v<T&&>, bool> = true>
// void toto(T && t)
// {
//     using vvv = typename T::hola;
// }

template <typename T>
using test4_arg_type = NA::args<
    typename first_name::template required_as_t<std::string>,
    typename last_name::template required_as_t<std::string>,
    typename data::template required_as_t<T&>
    >;

template <typename T>
void test4( test4_arg_type<T> && v )
{
    std::cout << "fn="<<v.template get<first_name>() <<std::endl;
    std::cout << "data="<<v.template get<data>() <<std::endl;

    if constexpr ( NA::has_v<first_name_string,test4_arg_type<T> > )
        std::cout << "fn="<<v.template get<first_name_string>() <<std::endl;


    // std::cout << "fn arg="<<v.template getArgument<first_name>() <<std::endl;


}

template <typename ... Ts>
void test4( Ts && ... v )
{
#if 0
    test4( NA::extend<test4_arg_type<Foo>>( NA::args<Ts...>{ std::forward<Ts>(v)... },
                                            NA::default_parameter<first_name>( []() { return "toto"; } ),
                                            NA::default_parameter<last_name>( "titi" ) ) );
#endif
    // auto && aa = NA::extend<test4_arg_type</*double*/Foo>>( NA::args<Ts...>{ std::forward<Ts>(v)... } );

    NA::args<Ts...> a{ std::forward<Ts>(v)... };
    using thedata_arg_type = std::decay_t<decltype(a.template get<data>())>;
    test4<thedata_arg_type>( test4_arg_type<thedata_arg_type>::create( std::move(a), NA::default_parameter<first_name>( std::string("aaa") ) ) );
    //test4<thedata_arg_type>( test4_arg_type<thedata_arg_type>::create( std::move(a), NA::default_parameter<first_name>( "aaa" ) ) );
}


template <typename ... TT>
constexpr auto test5( NA::args<TT...> && v )
{
#if 0
    //constexpr auto hola = [v = std::move(v) ]() constexpr { return v; };
    auto hola = []( auto && q ) constexpr {
                    //std::cout << std::integral_constant<int, q.template get<first_name>() >::value << std::endl;
                    auto && fn = q.template get<first_name>();
                    auto && ln = q.template get<last_name>();
                    return fn+ln;
                };

    //constexpr auto hole = hola(v);
    //constexpr auto hola = v.template get<first_name>();
    //std::integral_constant<int, test5( aa )>::value;
    //return v.template get<first_name>();
    //std::cout << std::integral_constant<int, hola(v)>::value << std::endl;

    return hola(v);
#endif
    auto && fn = v.template get<first_name>();
    auto && ln = v.template get<last_name>();
    return fn+ln;

}

template <typename ... Ts>
constexpr auto test5(  Ts && ... v )
{
    //return NA::args<Ts...>{ std::forward<Ts>(v)... }.template get<first_name>();
    //constexpr auto vv = NA::args<Ts...>{ std::forward<Ts>(v)... };//.template get<first_name>();
    //constexpr auto vv = NA::args<Ts...>{ v... };//.template get<first_name>();
    //constexpr auto vvvv = NA::make_arguments( std::forward<Ts>(v)... );
    //constexpr auto vvvva = NA::make_arguments( std::forward<Ts>(v) );
    //return 0;

    //return NA::make_arguments( std::forward<Ts>(v) );


    return test5( NA::make_arguments( std::forward<Ts>(v)... ) );

}

void abc( typename first_name::template required_as_t<std::string&> &&  v ) {}

int main()
{
#if 1
    // std::string f;
    // test2bbb(_first_name= "James", _last_name="Bond",_data=Foo(6,5) );
    // test2bbb(_first_name= std::string(f), _last_name=std::string(f),_data=Foo(6,5) );


    //assert( test1({_first_name= "James", _last_name="Bond" }) == "James Bond" );
    //assert( test1({_first_name= std::string("James"), _last_name="Bond" }) == "James Bond" );
    //assert( test1({_first_name= "James", _last_name=std::string("Bond") }) == "James Bond" );

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


    std::string fn4="a",ln4="b";double d=3; Foo foo4(4,3);

    test4(_first_name= fn4, _last_name=ln4,_data=d/*Foo(4,3)*/ );
    test4(_first_name= fn4, _last_name=ln4,_data=foo4/*Foo(4,3)*/ );
    test4(_first_name=std::string(fn4), _last_name=std::string(ln4),_data=foo4/*Foo(4,3)*/ );
    test4(_first_name=fn4, _last_name=std::string(ln4),_data=foo4/*Foo(4,3)*/ );

    test4(/*_first_name=fn4,*/ _last_name=std::string(ln4),_data=foo4 );

    //test4(_first_name= fn4, _last_name=ln4,_data=[]() { return "toto"; } );


    //auto hola = NA::make_argument<first_name>( 36 );
    //auto hola2 = NA::make_argument<first_name>( fn );
    auto hola2 = NA::make_argument<first_name>( "toto" );
    //hola2 = 3;
    test4( std::move(hola2), _last_name=std::string(ln4),_data=foo4/*Foo(4,3)*/ );
#endif


#if 0
    static constexpr auto aa = NA::make_argument<first_name>(5);
    constexpr auto vvvv = NA::make_arguments( aa );
    constexpr auto yyyy = vvvv.template get<first_name>();
    std::cout << std::integral_constant<int, yyyy>::value << std::endl;

    // std::cout << std::integral_constant<int, test5( aa )>::value << std::endl;
    //std::cout << std::integral_constant<int, test5( aa ).template get<first_name>()>::value << std::endl;
    //std::cout << std::integral_constant<int, test5( NA::make_argument<first_name>(5) )>::value << std::endl;
    std::cout << std::integral_constant<int, test5( _first_name=5, _last_name=10 )>::value << std::endl;
 #endif

    //abc( NA::default_parameter<first_name>( "aaa" ) );
    // std::string a;
    // abc( NA::default_parameter<first_name>( a ) );

    return 0;
}
