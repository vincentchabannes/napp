//!

struct A {};

struct B
{
    constexpr B() = default;
    B(const B&) = delete;
    B& operator=(const B&) = delete;

    template <typename V>
    constexpr A operator=(V && v) const { return A{}; }
};

template <typename Ts>
void func( Ts && v )
{
    // body
}

template <typename Ts>
void func2( Ts const& v )
{

}

// NOTE: g++ compilation error only when use a template
template <typename T>
class Foo
{
public:
    Foo(){
        B b;
        func( b=3 ); // error
        func( b.operator=(3) ); // OK
        func2( b=3 ); // OK
    }
};

int main()
{
    Foo<int> a;
    return 0;
}
