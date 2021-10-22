/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t  -*-
 */

#ifndef _NAMED_ARGUMENTS_HPP
#define _NAMED_ARGUMENTS_HPP 1

#include <iostream>
#include <tuple>
#include <optional>
#include <variant>
#include <any>
#include <vector>

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

namespace detail
{

struct owner_object {};
struct reference_object {};

template <typename T, typename tag,typename ObjectType>
struct type;

#if 1
template <typename T>
struct ownership_object : std::conditional<
    std::is_lvalue_reference_v<T>,
    reference_object,
    owner_object>
{};

template <typename T,typename tag>
struct infer_type
{
    using object_type = typename ownership_object<T>::type;
    using value_type = std::remove_reference_t<T>;
    using type = type<value_type,tag,object_type>;
};

template <typename T,typename tag>
using infer_type_t = typename infer_type<T,tag>::type;

#endif

template <typename T, typename tag>
struct type<T,tag,owner_object>
{
    static_assert(!std::is_reference_v<T>,
                  "T must not be a reference. Rather set the category!");
    using tag_type = tag;
    using value_type = T;

    template <typename T2>
    using apply_t = type<T,tag,owner_object>;

    template <typename T2>
    using required_as_t = typename detail::infer_type_t<T2,tag>;


    type(type &&) = default;
    type(type const&) = delete;

    template <typename V>
    type(V && t) : M_value(std::forward<V>(t)) { std::cout << "owner_object" << std::endl; }


    template <typename T2>
    type( type<T2,tag,reference_object> && t ) : M_value(std::forward<type<T2,tag,reference_object>>(t).value()) { std::cout << "owner_object-2-" << std::endl; } // TODO here no apply copy

    constexpr T & value() & { return M_value; }
    constexpr T const & value() const & { return M_value; }
    constexpr T && value() && { return std::move( M_value ); }

private :
    T M_value;
};

template <typename T, typename tag>
struct type<T,tag,reference_object>
{
    using tag_type = tag;
    using value_type = T;

    template <typename T2>
    using apply_t = type<T,tag,reference_object>;

    template <typename T2>
    using required_as_t = typename detail::infer_type_t<T2,tag>;

    type(T & t) : M_value(t) { std::cout << "ref_object-0-" << std::endl; }

    type(type &&) = default;//delete;
    type(type const&) = delete;

    template <typename V,std::enable_if_t< std::is_lvalue_reference_v<V&&> && std::is_base_of_v<std::decay_t<T>,std::decay_t<V>> , bool> = true>
    type(V && t) : M_value(std::forward<V>(t)) { std::cout << "ref_object" << std::endl; }

    template <typename V,std::enable_if_t< std::is_rvalue_reference_v<V&&> /*&& !std::is_same_v<std::decay_t<T>,std::decay_t<V>>*/, bool> = true>
    type(V && t) : M_tmp(std::make_shared<T>(std::forward<V>(t))), M_value( *std::any_cast<std::shared_ptr<T>&>(M_tmp).get() ) { std::cout << "BIS ref_object-1-" << std::endl; }

    template <typename V,std::enable_if_t< std::is_lvalue_reference_v<V&&> && !std::is_base_of_v<std::decay_t<T>,std::decay_t<V>>, bool> = true>
    type(V && t) : M_tmp(std::make_shared<T>(std::forward<V>(t))), M_value( *std::any_cast<std::shared_ptr<T>&>(M_tmp).get() ) { std::cout << "BIS ref_object-2-" << std::endl; }


    template <typename T2, typename tag2,typename ObjectType2> friend class type;


    template <typename T2>
    type( type<T2,tag,owner_object> && t ) :
        M_tmp(std::make_shared<T>(std::forward<type<T2,tag,owner_object>>(t).value())),
        M_value( *std::any_cast<std::shared_ptr<T>&>(M_tmp).get() ) { std::cout << "BIS ref_object-3-" << std::endl; }


    // TODO HERE : maybe some case where we can avoid the copy
    template <typename T2>
    //type( type<T2,tag,reference_object> && t ) : M_tmp( std::move( t.M_tmp ) ), M_value( std::move(std::forward<type<T2,tag,reference_object>>(t).M_value) ) {}
    //type( type<T2,tag,reference_object> && t ) : M_tmp( std::move( t.M_tmp ) ), M_value( std::move( static_cast<type<T2,tag,reference_object>&&>(t).M_value) ) {}
    //type( type<T2,tag,reference_object> && t ) : type( std::forward<type<T2,tag,reference_object>>(t).M_value ) {}
    type( type<T2,tag,reference_object> && t ) : type( std::move(t).M_value ) {}


    T & value()  { return M_value; }
    T const& value() const  { return M_value; }
    //T & value() && { return M_value; } // NOT ALWAYS GOOD!!
private :
    std::any M_tmp;
    T & M_value;

};

} // detail


template <typename T,typename Tag>
using type = typename detail::infer_type_t<T,Tag>;

//! make a new arg type with value \t
template <typename Tag,typename T>
constexpr auto/*type<T,Tag>*/ make_argument(T&& t)
{
    return detail::infer_type_t<T,typename Tag::tag_type>(std::forward<T>(t));
}


struct template_type_base {};
template <typename tag>
struct template_type : template_type_base
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
#if 0
    template <typename V>
    constexpr typename T::template apply_t<V> operator=(V v) const { return typename T::template apply_t<V>(v);}
#endif
    template <typename V>
    constexpr typename T::template apply_t<V> operator=(V && v) const { return typename T::template apply_t<V>( std::forward<V>( v ) );}
};

template <typename T>
constexpr named_parameter<T> parameter;




template <typename Tag,typename ValueType>
struct DefaultParameter : type<ValueType,Tag>
{
    DefaultParameter() = delete;
    template <typename U>
    DefaultParameter( U && v ) : type<ValueType,Tag>( std::forward<U>( v ) ) {}
    DefaultParameter( DefaultParameter const& ) = delete;
    DefaultParameter( DefaultParameter && ) = default;//delete;
};


template <typename T,typename ValueType>
auto default_parameter( ValueType && val )
{
    return DefaultParameter<T,ValueType>{ std::forward<ValueType>( val ) };
}





template<typename U, int Id, int MaxId, typename TupleType>
constexpr auto&& getImplBIS(TupleType && t)
{
    static_assert( Id< MaxId );
    if constexpr ( std::is_same_v<U, typename std::tuple_element_t<Id, std::decay_t<TupleType> >::tag_type > )
                     return std::get<Id>( std::forward<TupleType>(t) );//.value();
    else
        return getImplBIS<U,Id+1,MaxId,TupleType>( std::forward<TupleType>(t ));
}


template <typename ...T>
struct to_tuple : std::tuple<T...>
{
    using std::tuple<T...>::tuple;

    using super_type = std::tuple<T...>;
#if 1
    // conversion operator
#if 0
    template <typename ... Ts>
    operator std::tuple<Ts...> () { return { std::get<Ts>(*this)...}; }
#else
    template <typename ... Ts>
    //operator std::tuple<Ts...> () { return std::tuple<Ts...>{ getImpl<typename Ts::tag_type,0, sizeof...(T) >( static_cast<super_type&&>( *this ) )...};}
    operator std::tuple<Ts...> () { return std::tuple<Ts...>{ getImplBIS<typename Ts::tag_type,0, sizeof...(T) >( static_cast<super_type&&>( *this ) )...}; }
#endif
#endif
};

template <typename ... T>
to_tuple(T...) -> to_tuple<T...>;



template <typename ... T>
struct args
{
    // using tuple_type = std::tuple<T...>;
    // template <typename Tag>
    // constexpr bool has = has_type<Tag::>
    template <typename Tag>
    using has_t = std::disjunction<std::is_same<typename T::tag_type, typename Tag::tag_type>...>;
#if 0
    template <typename ... U>
    args(U&& ... u) : values(to_tuple(std::forward<U>(u)...)) {}
#else
    template <typename ... U>
    args(U&& ... u) : values(to_tuple(std::forward<U>(u)...)) {}

    // TODO CHECK U IS NOT NA::args
    template <typename U>
    args(U&&  u) : values(std::forward<U>(u)) {}
#endif
    args() = delete;
    args( args const& ) = delete;
    //args( args && ) = delete;


    template <typename NamedArgType>
    constexpr auto const& get() const { return this->getArgument<NamedArgType>().value(); }

    template <typename NamedArgType>
    constexpr auto & get() { return this->getArgument<NamedArgType>().value(); }

    template <typename NamedArgType,typename DefaultType>
    constexpr auto && /*const&*/ /*&&*/ get_optional( DefaultType && defaultValue ) const// &
        {
            if constexpr ( has_t<NamedArgType>::value )
                return this->get<NamedArgType>();
            else
            {
                auto defaultArg = NA::default_parameter<NamedArgType>( std::forward<DefaultType>( defaultValue ) );
                using the_default_arg_type = std::decay_t<decltype(defaultArg)>;

                if constexpr ( std::is_invocable_v<typename the_default_arg_type::value_type> )
                {
                    // TODO : here we guess lambda return valuue (not a ref)
                    auto defaultArg2 = NA::default_parameter<NamedArgType>( defaultArg.value()() );
                    using the_default_arg2_type = std::decay_t<decltype(defaultArg2)>;
                    M_tmps.push_back( std::make_shared<the_default_arg2_type>( std::move(defaultArg2) ) );
                    return std::any_cast<std::shared_ptr<the_default_arg2_type> const&>(M_tmps.back()).get()->value();
                }
                else
                {
                    M_tmps.push_back( std::make_shared<the_default_arg_type>( std::move(defaultArg) ) );
                    return std::any_cast<std::shared_ptr<the_default_arg_type> const&>(M_tmps.back()).get()->value();
                }
            }
        }

    template <typename NamedArgType>
    constexpr auto & getArgument() &
        {
            if constexpr ( false ) // non template type
                return std::get<NamedArgType>( this->values );
            else
                return getImplBIS<typename NamedArgType::tag_type,0,sizeof...(T)>( this->values );
        }
    template <typename NamedArgType>
    constexpr auto const& getArgument() const &
    {
        if constexpr ( false ) // non template type
            return std::get<NamedArgType>( this->values ).value;
        else
            return getImplBIS<typename NamedArgType::tag_type,0,sizeof...(T)>( this->values );
    }
    template <typename NamedArgType>
    constexpr auto && getArgument() &&
        {
            if constexpr ( false ) // non template type
                return std::get<NamedArgType>( std::move( this->values ) );
            else
                return getImplBIS<typename NamedArgType::tag_type,0,sizeof...(T)>( std::move( this->values ) );
        }

#if 1
    template <typename TheType>
    static auto New( TheType && a )
        {
            return args<T...>{ std::forward<TheType>( a ).template getArgument<T>() ... };
        }
#endif


private :
    std::tuple<T...> values;
    mutable std::vector<std::any> M_tmps; // store optional arg given in get_optional
};




template <typename ... T>
constexpr NA::args<T...>  make_arguments(T&& ... t)
{
    return NA::args<T...>{ std::forward<T>(t)... };
}


// TODO check U is NA::args
template <typename U,typename ... T>
constexpr U  make_arguments(T&& ... t)
{
    return U{ std::forward<T>(t)... };
}


// TODO check U is NA::args
template <typename Tag,typename U>
constexpr bool has_v = U::template has_t<Tag>::value;





#if 1


template <typename ArgsType,typename ... T>
auto extendImpl( args<T...> && args )
{


}

#if 1
template <typename NewArgType, typename ArgsType, typename ... T>
auto /*&&*/ extend( /*args<ArgsType>*/ArgsType && args, T && ... defaultParams )
{
    return NewArgType::New( std::forward<ArgsType>( args )/*.values*/ );
}

#endif

#endif
} //  namespace NA

#endif
