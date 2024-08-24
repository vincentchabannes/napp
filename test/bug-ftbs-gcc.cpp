#include <napp/na.hpp>

using element1 = NA::named_argument_t<struct element1_tag>;
constexpr auto& _element1 = NA::identifier<element1>;

template <typename Ts>
void func( Ts && v )
{
    // body
}

// NOTE: g++ compilation error only when use a template
template <typename T>
class Foo
{
public:
    Foo(){
        func( _element1=("toto") ); // error
        func( _element1.operator=("toto") ); // OK
    }
};

int main()
{
    Foo<int> a;
    return 0;
}
