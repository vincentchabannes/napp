/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t  -*-
 */

#ifndef _NAMED_ARGUMENTS_HPP
#define _NAMED_ARGUMENTS_HPP 1

#include <tuple>

#if 0
#if 0
template<typename U, typename... T>
constexpr bool contains(std::tuple<T...>) {
  return (std::is_same_v<U, T> || ...);
}

#else
template <typename T, typename Tuple>
struct has_type;

template <typename T, typename... Us>
struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};
#endif
#endif


namespace NA {

template <typename T, typename tag>
struct type
{
    using tag_type = tag;

    template <typename T2>
    using apply_t = type<T,tag>;

    type(T t) : value(t) {}
    T value;
};

template <typename tag>
struct template_type
{
    using tag_type = tag;

    template <typename T>
    using required_as_t = type<T,tag>;

    template <typename T>
    using apply_t = type<T,tag>;
};

template <typename T>
struct named_parameter
{
    constexpr named_parameter() = default;
    named_parameter(const named_parameter&) = delete;
    named_parameter& operator=(const named_parameter&) = delete;

    template <typename V>
    constexpr typename T::template apply_t<V> operator=(V v) const { return typename T::template apply_t<V>(v);}
};

template <typename T>
constexpr named_parameter<T> parameter;


template<typename U, int Id, int MaxId, typename TupleType>
constexpr auto&& getImpl(TupleType && t)
{
    static_assert( Id< MaxId );
    if constexpr ( std::is_same_v<U, typename std::tuple_element_t<Id, std::decay_t<TupleType> >::tag_type > )
        return std::get<Id>( std::forward<TupleType>(t) ).value;
    else
        return getImpl<U,Id+1,MaxId,TupleType>( std::forward<TupleType>(t ));
}
template<typename U, int Id, int MaxId, typename TupleType,typename DefaultType>
constexpr auto&& getOptionalImpl(TupleType && t, DefaultType && defaultValue )
{
    if constexpr ( Id>= MaxId )
        return std::forward<DefaultType>( defaultValue );
    else if constexpr ( std::is_same_v<U, typename std::tuple_element_t<Id, std::decay_t<TupleType> >::tag_type > )
        return std::get<Id>( std::forward<TupleType>(t) ).value;
    else
        return getOptionalImpl<U,Id+1,MaxId>( std::forward<TupleType>(t), std::forward<DefaultType>( defaultValue ) );
}

template <typename ...T>
struct to_tuple : std::tuple<T...>
{
    using std::tuple<T...>::tuple;

    using super_type = std::tuple<T...>;

    // conversion operator
#if 0
    template <typename ... Ts>
    operator std::tuple<Ts...> () { return { std::get<Ts>(*this)...};}
#else
    template <typename ... Ts>
    operator std::tuple<Ts...> () { return std::tuple<Ts...>{ getImpl<typename Ts::tag_type,0, sizeof...(T) >( static_cast<super_type&&>( *this ) )...};}
#endif
};

template <typename ... T>
to_tuple(T...) -> to_tuple<T...>;



template <typename ... T>
struct args
{
    template <typename ... U>
    args(U&& ... u) : values(to_tuple(std::forward<U>(u)...)) {}

    args() = delete;
    args( args const& ) = delete;
    //args( args && ) = delete;


    template <typename TheType>
    constexpr auto && get() const
        {
            if constexpr ( false ) // non template type
                             return std::get<TheType>( this->values ).value;
            else
                return getImpl<typename TheType::tag_type,0,sizeof...(T)>( this->values );
        }

    template <typename TheType,typename DefaultType>
    constexpr auto && get_optional( DefaultType && defaultValue ) const
        {
            if constexpr ( false ) // non template type
                             return std::get<TheType>( this->values ).value;
            else
                return getOptionalImpl<typename TheType::tag_type,0,sizeof...(T)>( this->values, std::forward<DefaultType>( defaultValue ) );
        }

    std::tuple<T...> values;

};


} //  namespace NA

#endif
