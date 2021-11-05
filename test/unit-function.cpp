
#include <napp/na.hpp>

#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>


using first_name = NA::named_argument_t<struct first_name_tag>;
using last_name = NA::named_argument_t<struct last_name_tag>;
using data = NA::named_argument_t<struct data_tag>;

using first_name_string = NA::named_argument_t<struct first_name_string_tag,std::string>;
using last_name_string = NA::named_argument_t<last_name,std::string>;

constexpr auto& _first_name = NA::identifier<first_name>;
constexpr auto& _last_name = NA::identifier<last_name>;
constexpr auto& _data = NA::identifier<data>;

constexpr auto& _data2 = NA::identifier<struct data2_tag>;
constexpr auto& _data3 = NA::identifier<struct data3_tag>;
using data2 = typename std::decay_t<decltype(_data2)>::identifier_type;
using data3 = typename std::decay_t<decltype(_data3)>::identifier_type;

std::string test1( NA::arguments<typename first_name::template required_as_t<std::string const&>,
                                 typename last_name::template required_as_t<std::string> > && v )
{
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.get(_last_name);

    //std::cout << "test1 : " << res.str() << std::endl;
    return res.str();
}

void test1bis(NA::arguments<first_name_string, last_name_string> && v)
{

}


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

template <typename ... TT>
std::string test2( NA::arguments<TT...> && v )
{
    std::ostringstream res;
    res << v.template get<first_name>()
        << " "
        << v.template get_else<last_name>("default_last_name")
        << " "
        << v.template get_else<data>(  "default_data_value"  )
        << " "
        //<< v.template get_else<data>(  "default_data_lambda"  );
        << v.template get_else_invocable<data>( [](){ return "default_data_lambda"; } );
        //<< v.template get_else_invocable<data>( [](){ return std::string("default_data_lambda"); } );

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
    return test2( NA::arguments<typename first_name::template required_as_t<std::string const&>,
                           typename last_name::template required_as_t<std::string>,
                           typename data::template required_as_t<Foo &> >{ std::forward<Ts>(v)... } );
}


using test3_arg_type = NA::arguments<typename first_name::template required_as_t<std::string &>,
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


template <typename T>
using test4_arg_type = NA::arguments<
    typename first_name::template required_as_t<std::string const&>,
    typename last_name::template required_as_t<std::string>,
    typename data::template required_as_t<T&>
    >;

template <typename T>
std::string test4( test4_arg_type<T> && v )
{
    if constexpr ( NA::has_v<first_name_string,test4_arg_type<T> > )
        std::cout << "fn="<<v.template get<first_name_string>() <<std::endl;

    std::ostringstream res;
    res << v.get(_first_name)
        << " "
        << v.get(_last_name)
        << " "
        << v.get(_data);

    //std::cout << "test4=" << res.str() << std::endl;
    
    if constexpr ( std::is_same_v<std::decay_t<decltype( v.get(_data) )>,Foo > )
         v.get(_data).setI( v.get(_data).i()+1 );

    return res.str();


}

template <typename ... Ts>
std::string test4( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    using thedata_arg_type = std::decay_t<decltype(args.template get<data>())>;

    std::string default_ln = "default_ln";
    return test4<thedata_arg_type>( test4_arg_type<thedata_arg_type>::create( std::move(args),
                                                                              NA::make_default_argument<first_name>( "default_fn" ),
                                                                              NA::make_default_argument(_last_name, default_ln)
                                                                              ) );
}


template <typename ... TT>
std::string test5( NA::arguments<TT...> && v )
{
    std::string fn = "default_first_name";
    std::string const& ln = "default_last_name";
    Foo foo(6,5);
    std::ostringstream res;
    res << v.get_else(_first_name,fn)
        << " "
        << v.get_else(_last_name, ln)
        << " "
        << v.get_else(_data, foo )
         << " "
        << v.get_else_invocable(_data2, [](){ return Foo(3,2); } )
        << " "
        << v.get_else_invocable(_data3, [&foo]() -> auto && { return foo; } );

    //std::cout << "test2 : " << res.str() << std::endl;
    return res.str();
}

template <typename ... Ts>
std::string test5( Ts && ... v )
{
    return test5( NA::make_arguments( std::forward<Ts>(v)... ) );
}



std::string test6( std::string const& fn, std::string const& ln )
{
    std::ostringstream res;
    res << "[v1] " << fn << " "<< ln;
    return res.str();
}

template <typename ... Ts,typename  = typename std::enable_if_t< sizeof...(Ts) != 2 || ( NA::is_named_argument_v<Ts> && ...) > >
std::string test6( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    auto && fn = args.get(_first_name);
    auto && ln = args.get(_last_name);
    std::ostringstream res;
    res << "[v2] " << fn << " "<< ln;
    return res.str();
}


struct FooString
{
    FooString( std::string const& s) : M_s(new std::string(s)) {}
    ~FooString() { delete M_s; }
    std::string const& s() const { return *M_s; }
private:
    std::string * M_s;
};
FooString
operator+(FooString const& t1, FooString const& t2) { return FooString(t1.s()+t2.s()); }

template <typename ... Ts>
std::string test7( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    // check default arg which return ref from temporary object
    auto && fn = args.get_else_invocable(_first_name, []() { return (FooString("fn0")+FooString("fn1")).s(); } );
    auto && ln = args.get_else(_last_name, std::string((FooString("ln0")+FooString("ln1")).s()));
    std::ostringstream res;
    res << fn << " "<< ln;
    return res.str();
}

// constraints
template <typename ... Ts>
std::string test8( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    auto && data = args.template get<std::is_integral>(_data);
    auto && data2 = args.template get_else<std::is_floating_point>(_data2,2.3);
    //auto && data3 = args.template get_else_invocable<std::is_arithmetic>(_data3,[](){return 5.7;});
    auto && data3 = args.template get_else_invocable<std::is_arithmetic>(_data3,[](){return 5.7;});
    auto && fn = args.template get_else<NA::constraint::is_convertible<std::string>::apply>(_first_name,"fn");
    std::ostringstream res;
    res << data << " "<< std::lround(data2) << " " << std::lround(data3) << " " << fn;
    return res.str();
}




TEST_CASE( "Test Function 1", "[function]" ) {

    REQUIRE( test1({_first_name= "James", _last_name="Bond" }) == "James Bond" );
    REQUIRE( test1({_last_name= "Bond", _first_name="James" }) == "James Bond" );
    REQUIRE( test1({_last_name= "Bond", _first_name=std::string("James") }) == "James Bond" );
    std::string fn= "James";
    REQUIRE( test1({_first_name= fn, _last_name="Bond" }) == "James Bond" );
}

TEST_CASE( "Test Function 2", "[function]" ) {
    std::string fn= "James";
    REQUIRE( test2(_first_name="James" ) == "James default_last_name default_data_value default_data_lambda" );
    REQUIRE( test2(_first_name=fn, _last_name="Bond" ) == "James Bond default_data_value default_data_lambda" );
    REQUIRE( test2(_last_name= "Bond", _first_name="James" ) == "James Bond default_data_value default_data_lambda" );
    Foo b(3,5);
    REQUIRE( test2(_first_name= "James", _last_name="Bond",_data=b ) == "James Bond i=15 i=15" );
    REQUIRE( test2(_first_name= "James", _last_name="Bond",_data=Foo(4,3) ) == "James Bond i=12 i=12" );
    REQUIRE( test2b(_first_name= "James", _last_name="Bond",_data=Foo(6,5) ) == "James Bond i=30 i=30" );
    REQUIRE( test2b(_first_name= "James", _last_name="Bond",_data=b ) == "James Bond i=15 i=15" );
    Baz baz(2,5);
    REQUIRE( test2b(_first_name= "James", _last_name="Bond",_data=baz ) == "James Bond i=-21 i=-21" );
}

TEST_CASE( "Test Function 3", "[function]" ) {
    std::string fn3,ln3;Foo foo3(0,0);
    test3( _data=foo3,_last_name=ln3,_first_name=fn3 );
    std::ostringstream res3;
    res3 << fn3 << " " << ln3 << " " << foo3;
    REQUIRE( res3.str() == "James Bond i=34" );

    fn3.clear();ln3.clear();
    Baz baz3(0,0);
    test3( _data=baz3,_last_name=ln3,_first_name=fn3 );
    std::ostringstream res3b;
    res3b  << fn3 << " " << ln3 << " i=" << baz3.i();
    REQUIRE( res3b.str() == "James Bond i=34" );
}

TEST_CASE( "Test Function 4", "[function]" ) {
    std::string fn4="James",ln4="Bond";int i4=3; Foo foo4(4,3);
    REQUIRE( test4(_first_name= fn4, _last_name=ln4,_data=i4 ) == "James Bond 3" );
    REQUIRE( test4(_first_name= fn4, _last_name=ln4,_data=foo4/*Foo(4,3)*/) == "James Bond i=12" );
    REQUIRE( test4(_first_name=std::string(fn4), _last_name=std::string(ln4),_data=Foo(4,3) ) == "James Bond i=12" );
 
    REQUIRE( test4( _last_name=ln4,_data=foo4 ) == "default_fn Bond i=13" );
    REQUIRE( test4( _data=foo4 ) == "default_fn default_ln i=14" );

    //test4(_first_name= fn4, _last_name=ln4,_data=[]() { return "toto"; } );

    REQUIRE( test4( NA::make_argument<first_name>( "James" ), _last_name=std::string(ln4),_data=foo4 ) == "James Bond i=15" );
    REQUIRE( test4( NA::make_argument(_first_name, "James" ), _last_name=std::string(ln4),_data=foo4 ) == "James Bond i=16" );
}

TEST_CASE( "Test Function 5", "[function]" ) {
    test5(_first_name= "James", _last_name="Bond",_data=Foo(4,3),_data2=Foo(6,3),_data3=Foo(9,3));
}

TEST_CASE( "Test Function 6", "[function]" ) {
    REQUIRE( test6(_first_name= "James", _last_name="Bond") == "[v2] James Bond" );
    REQUIRE( test6("James","Bond") == "[v1] James Bond" );
    REQUIRE( test6(std::string("James"),std::string("Bond")) == "[v1] James Bond" );
}


TEST_CASE( "Test Function 7", "[function]" ) {
    REQUIRE( test7(_first_name="James",_last_name="Bond") == "James Bond" );
    REQUIRE( test7(_first_name="James") == "James ln0ln1" );
    REQUIRE( test7(_last_name="Bond") == "fn0fn1 Bond" );
    REQUIRE( test7() == "fn0fn1 ln0ln1" );
}

TEST_CASE( "Test Function 8", "[function]" ) {
     REQUIRE( test8(_data=12) == "12 2 6 fn" );
     REQUIRE( test8(_data=8,_data2=9.3) == "8 9 6 fn" );
     REQUIRE( test8(_first_name="James",_data=8,_data2=9.3) == "8 9 6 James" );
     REQUIRE( test8(_first_name="James",_data3=4,_data=8,_data2=9.3) == "8 9 4 James" );
}
