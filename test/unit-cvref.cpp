#include <napp/na.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

constexpr auto& _data = NA::identifier<struct data_tag>;
constexpr auto& _data2 = NA::identifier<struct data2_tag>;
constexpr auto& _data3 = NA::identifier<struct data3_tag>;
using data = typename std::decay_t<decltype(_data)>::identifier_type;
using data2 = typename std::decay_t<decltype(_data2)>::identifier_type;
using data3 = typename std::decay_t<decltype(_data3)>::identifier_type;


struct Foo
{
    Foo() = delete;
    Foo(int a,int b) : M_i(a*b) {}

    Foo( Foo const& ) = delete;
    Foo( Foo && ) = default;
    Foo & operator=( Foo const& ) = delete;
    Foo & operator=( Foo && ) = delete;

    int i() const { return M_i; }

    void setI( int i ) { M_i = i; }
private :
    int M_i;
};


template <typename ... Ts>
std::string test_nonconst( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    auto && data = args.get(_data);
    auto && data2 = args.get_else(_data2,data);
    auto && data3 = args.get_else_invocable(_data3,[&data]() -> decltype(auto) { return data; });

    data.setI(12);
    data2.setI(13);
    data3.setI(14);
    std::ostringstream res;
    res << data.i() << "_" << data2.i() << "_" << data3.i();
    return res.str();
}


template <typename ... Ts>
std::string test_const( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    auto && data = args.get(_data);
    auto && data2 = args.get_else(_data2,data);
    auto && data3 = args.get_else_invocable(_data3,[&data]() -> decltype(auto) { return data; });

    static_assert( std::is_const_v<std::remove_reference_t<decltype(data)>>, "should be const" );
    static_assert( std::is_const_v<std::remove_reference_t<decltype(data2)>>, "should be const" );
    static_assert( std::is_const_v<std::remove_reference_t<decltype(data3)>>, "should be const" );

    std::ostringstream res;
    res << data.i() << "_" << data2.i() << "_" << data3.i();
    return res.str();
}

TEST_CASE( "Test NonConst", "[cvref]" ) {
    Foo foo(1,2);
    Foo foo2(3,4);
    Foo foo3(5,6);
    REQUIRE( test_nonconst(_data=foo) == "14_14_14" );
    REQUIRE( test_nonconst(_data=foo,_data2=foo2) == "14_13_14" );
    REQUIRE( test_nonconst(_data=foo,_data2=foo2,_data3=foo3) == "12_13_14" );
}

TEST_CASE( "Test Const", "[cvref]" ) {
    const Foo foo(3,4);
    const Foo foo2(6,5);
    const Foo foo3(4,9);
    REQUIRE( test_const(_data=foo) == "12_12_12" );
    REQUIRE( test_const(_data=foo,_data2=foo2) == "12_30_12" );
    REQUIRE( test_const(_data=foo,_data2=foo2,_data3=foo3) == "12_30_36" );
}
