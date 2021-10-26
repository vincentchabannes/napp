#include <napp/na.hpp>

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <sstream>

constexpr auto& _elt1 = NA::identifier<struct elt1_tag>;
constexpr auto& _elt2 = NA::identifier<struct elt2_tag>;
constexpr auto& _elt3 = NA::identifier<struct elt3_tag>;

using elt1 = typename std::decay_t<decltype(_elt1)>::identifier_type;
using elt2 = typename std::decay_t<decltype(_elt2)>::identifier_type;
using elt3 = typename std::decay_t<decltype(_elt3)>::identifier_type;


template <typename ... Ts>
std::string test1( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );

    int * a = new int(3);
    int * b = new int(17);
    auto && elt1 = args.get(_elt1);
    auto && elt2 = args.get_else(_elt2, a );
    auto && elt3 = args.get_else_invocable(_elt3, [&b]() { return b; } );

    std::ostringstream res;
    res << *elt1;
    if constexpr ( std::is_same_v<const char*, std::decay_t<decltype(elt2)> > )
        res << " "  << elt2;
    else
        res << " "  << *elt2;
    res << " " << *elt3;
    delete a;
    delete b;

    std::string oldElt1 = *elt1;
    delete elt1;
    elt1 = new std::string( oldElt1+ "_n" );
    std::cout << "res=" << res.str() << std::endl;
    return res.str();
}


struct Foo
{
    static int nAlloc;
    Foo() = delete;
    Foo(int a,int b) : M_i(a*b) { ++nAlloc; std::cout << "[Foo] nAlloc=" << nAlloc << std::endl;  }
    ~Foo() { --nAlloc; }
    Foo( Foo const& ) = delete;
    Foo( Foo && ) = delete;
    Foo & operator=( Foo const& ) = delete;
    Foo & operator=( Foo && ) = delete;

    int i() const { return M_i; }

    void setI( int i ) { M_i = i; }
private :
    int M_i;
};

int Foo::nAlloc = 0;

template <typename T>
using test2_arg_type = NA::arguments<
    typename elt1::template required_as_t<std::shared_ptr<T> >,
    typename elt2::template required_as_t<std::shared_ptr<T> const&>,
    typename elt3::template required_as_t<std::shared_ptr<T> &>
    >;

std::string test2Impl( test2_arg_type<Foo> && args )
{
    auto && elt1 = args.get(_elt1);
    auto && elt2 = args.get(_elt2);
    auto && elt3 = args.get(_elt3);

    if ( !elt1 )
        elt1 = std::make_shared<Foo>(1,1);

    std::ostringstream res;
    res << elt1->i() << " " << elt2->i() << " " << elt3->i();

    elt1->setI( elt1->i()+1 );
    elt2->setI( elt2->i()+2 );
    elt3->setI( elt3->i()+3 );
    return res.str();
}

template <typename ... Ts>
std::string test2( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... )/*.add_default_argument()*/;
    return test2Impl( std::move( args ) );
}

TEST_CASE( "Test Native Pointer", "[pointer]" ) {

    std::string* s_str = new std::string( "toto" );
    REQUIRE( test1(_elt1=s_str) == "toto 3 17" );
    REQUIRE( *s_str == "toto_n" );
    const char* s_char = "titi";
    REQUIRE( test1(_elt1=s_str,_elt2=s_char) == "toto_n titi 17" );
    REQUIRE( *s_str == "toto_n_n" );

    delete s_str;
}

TEST_CASE( "Test Smart Pointer", "[pointer]" ) {

    auto foo2 = std::make_shared<Foo>(6,9);
    REQUIRE( test2(_elt1=new Foo(3,4),_elt3=std::make_shared<Foo>(4,5),_elt2=foo2) == "12 54 20" );
    REQUIRE( foo2->i() == 56 );
    
}
