#include <napp/na.hpp>
#include <tuple>
#include <iostream>

constexpr auto& _parameters = NA::identifier<struct parameters_tag>;
constexpr auto& _task = NA::identifier<struct task_tag>;

// namespace na {
// using parameters = typename std::decay_t<decltype(_parameters)>::identifier_type;
// using task = typename std::decay_t<decltype(_task)>::identifier_type;
// }

namespace Backend{

template<typename ...T, size_t... I>
auto extractParametersAsTuple( std::tuple<T...> && t, std::index_sequence<I...>)
{
    return std::forward_as_tuple( std::get<I>(t).getValue()...);
}

struct Runtime{
    template <typename ... Ts>
    void task(Ts && ... ts ) {
        auto t = std::make_tuple( std::forward<Ts>(ts)... );
        auto callback = std::get<sizeof...(Ts) - 1>(t);
        auto parameters = extractParametersAsTuple( std::move(t), std::make_index_sequence<sizeof...(Ts)-1>{} );
        std::apply( callback, std::move(parameters) );
    }
};

template <typename T,bool b>
class SpData
{
    static_assert(std::is_reference<T>::value,
                  "The given type must be a reference");
public:
    using value_type = T;
    static constexpr bool isWrite = b;

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U,T>> >
    constexpr explicit SpData( U && u ) : M_val( std::forward<U>(u) ) {}

    constexpr value_type getValue() { return M_val; }
private:
    value_type M_val;
};

template <typename T>
auto spRead( T && t )
{
    return SpData<T,false>{ std::forward<T>( t ) };
}
template <typename T>
auto spWrite( T && t )
{
    return SpData<T,true>{ std::forward<T>( t ) };
}

template<typename T>
auto toSpData( T && t )
{
    if constexpr ( std::is_const_v<std::remove_reference_t<T>> )
        return spRead( std::forward<T>( t ) );
    else
        return spWrite( std::forward<T>( t ) );
}

template<typename ...T, size_t... I>
auto makeSpDataHelper( std::tuple<T...>& t, std::index_sequence<I...>)
{
    return std::make_tuple( toSpData(std::get<I>(t))...);
}
template<typename ...T>
auto makeSpData( std::tuple<T...>& t ){
    return makeSpDataHelper<T...>(t, std::make_index_sequence<sizeof...(T)>{});
}
}

namespace pm
{
template <typename ... Ts>
void
runTask( Ts && ... ts )
{
    auto args = NA::make_arguments( std::forward<Ts>(ts)... );
    auto && task = args.get(_task);
    auto && parameters = args.get_else(_parameters,std::make_tuple());

    Backend::Runtime runtime;
    std::apply( [&runtime](auto... args){ runtime.task(args...); }, std::tuple_cat( Backend::makeSpData( parameters ), std::make_tuple( task ) ) );
}

template <typename ... Ts>
auto parameters(Ts && ... ts)
{
    return std::forward_as_tuple( std::forward<Ts>(ts)... );
}
}

int main()
{
    const int i1=4;
    double d1=2.34;

    // use variables state (if non const -> SpWrite else SpRead).
    pm::runTask( _parameters=pm::parameters(i1,d1),
                 _task=[](int p1,double & p2){
                           p2 *= p1;
                       } );
    std::cout << "d1="<< d1 << std::endl;

    // use std::ref/std::cref (if non const -> SpWrite else SpRead).
    int i2 = 123;
    std::string s2 = "string value : ";
    pm::runTask( _parameters=pm::parameters(std::cref(i2),std::ref(s2)),
                 _task=[](int p1,std::string & p2){
                           p2 += std::to_string(p1);
                       } );
    std::cout << "s2="<< s2 << std::endl;

    return 0;
}
