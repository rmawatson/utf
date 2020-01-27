/* Copyright (c) 2018, rmawatson@hotmail.com
 *
 * Distributed under the Boost Software License, Version 1.0 (found in the
 * LICENSE file in the root directory of this source tree)
 */
 
#pragma once
#include <iterator>
#include <type_traits>

# define UTF_PLATFORM_LITTLE_ENDIAN utf::little_endian
# define UTF_PLATFORM_BIG_ENDIAN utf::big_endian
# ifdef _WIN32
#   define UTF_PLATFORM_ENDIANESS UTF_PLATFORM_LITTLE_ENDIAN
#   define UTF_PLATFORM_BSWAP32(U32) _byteswap_ulong(U32)
#   define UTF_PLATFORM_BSWAP16(U16) _byteswap_ushort(U16)
# else
#   if defined(__linux__)
#     include <endian.h>
#   elif defined(OS_MACOSX)
#     include <machine/endian.h>
#   endif
#   if __BYTE_ORDER == __LITTLE_ENDIAN
#     define UTF_PLATFORM_ENDIANESS UTF_PLATFORM_LITTLE_ENDIAN
#   elif __BYTE_ORDER  == __BIG_ENDIAN
#     define UTF_PLATFORM_ENDIANESS UTF_PLATFORM_BIG_ENDIAN
#   endif
#   define UTF_PLATFORM_BSWAP32(U32) __builtin_bswap32(U32)
#   define UTF_PLATFORM_BSWAP16(U16) __builtin_bswap16(U16)
# endif

namespace utf
{

#ifdef __cpp_char8_t
    using u8string = std::u8string;
#else 
    using char8_t = char;
    using u8string = std::string;
#endif
    using u16string = std::u16string;
    using u32string = std::u32string;

    namespace mp {

        template <typename...>
        using void_t = void;

        template <typename... Ts>
        struct list {};

        template <template <typename...> class Fn, typename... Ts>
        struct defer {
            using type = Fn<Ts...>;
        };

        template <typename T>
        struct identity {
            using type = T;
        };

        template <size_t V>
        using size_c = std::integral_constant<size_t, V>;

        template <bool V>
        using bool_c = std::integral_constant<bool, V>;

        template <bool V>
        using int_c = std::integral_constant<int, V>;

        template <bool V>
        using uint_c = std::integral_constant<unsigned int, V>;

        template <template <typename...> class Fn>
        struct func
        {
            template <typename... Ts>
            using fn = typename defer<Fn, Ts...>::type;
        };

        template <template <typename...> class Fn>
        using wrap = func<Fn>;
        
        template <template <typename...> class T>
        struct trait
        {
            template <typename... Ts>
            using fn = typename T<Ts...>::type;
        };

        namespace detail {
            template <typename, template <typename...> class Fn, typename... Ts >
            struct is_valid_impl : std::false_type {};

            template <template <typename...> class Fn, typename... Ts>
            struct is_valid_impl<void_t<Fn<Ts...>>, Fn, Ts...> : std::true_type {};
        }

        template <template <typename...> class Fn, typename... Ts>
        using is_valid = detail::is_valid_impl<void, Fn, Ts...>;

        template < typename Q, typename... Ts>
        using is_valid_q = is_valid<Q::template fn, Ts...>;


		namespace detail {
			template <typename, template<typename...> class Fn, typename... Ts >
			struct is_instantiable_impl : std::false_type {};

			template <template<typename...> class Fn, typename... Ts>
			struct is_instantiable_impl<mp::void_t<decltype(Fn<Ts...>())>, Fn, Ts...> : std::true_type {};
		}

		template <template<typename...> class Fn, typename... Ts>
		using is_instantiable = detail::is_instantiable_impl<void, Fn, Ts...>;

		template < typename Q, typename... Ts>
		using is_instantiable_q = is_instantiable<Q::template fn, Ts...>;

        template <template <typename...> class F, typename... Ts>
        struct bind_back
        {
            template <typename... Ys>
            using fn = typename defer<F, Ys..., Ts...>::type;
        };

        template <template <typename...> class F, typename... Ts>
        struct bind_front
        {
            template <typename... Ys>
            using fn = typename defer<F, Ts..., Ys...>::type;
        };

        namespace detail
        {
            template <typename F, typename S>
            struct transform_impl;

            template <typename F, template <typename...> class S, typename... Ts>
            struct transform_impl<F, S<Ts...>>
            {
                using type = S<typename defer<F::template fn, Ts>::type...>;
            };
        }

        template <typename F, typename S>
        using transform = typename detail::transform_impl<F, S>::type;

        namespace detail {

            template <typename, template <typename...> class>
            struct rename_impl;

            template <template <typename...> class A, typename... Ts, template <typename...> class B>
            struct rename_impl<A<Ts...>, B>
            {
                using type = B<Ts...>;
            };
        }

        template <typename A, template <typename...> class B>
        using rename = typename detail::rename_impl<A, B>::type;

        namespace detail
        {
            template <bool v, typename T, typename... E>
            struct if_impl;

            template <typename T, typename... E>
            struct if_impl<true, T, E...>
            {
                using type = T;
            };

            template <typename T, typename E>
            struct if_impl<false, T, E>
            {
                using type = E;
            };
        }

        template <typename C, typename T, typename E>
        using if_ = typename detail::if_impl<static_cast<bool>(C::value), T, E>::type;

        template <bool v, typename T, typename E>
        using if_v = typename  detail::if_impl<v, T, E>::type;

        namespace detail
        {
            template<typename S>
            struct first_impl;

            template<template <typename...> class S, typename T, typename... Ts>
            struct first_impl<S<T, Ts...>>
            {
                using type = T;
            };

            template<typename S>
            struct second_impl;

            template<template <typename...> class S, typename T, typename T1, typename... Ts>
            struct second_impl<S<T, T1, Ts...>>
            {
                using type = T1;
            };
        }

        template <typename S>
        using first = typename detail::first_impl<S>::type;

        template <typename S>
        using second = typename detail::second_impl<S>::type;

        namespace detail {

            template <typename... Ts>
            struct inherit : Ts...{};

            template <typename M, typename T>
            struct map_find_impl;

            template <template <typename...> class M, typename... Ys, typename T>
            struct map_find_impl<M<Ys...>, T>
            {
                using _kvs = inherit<identity<Ys>...>;

                template <template <typename...> class P, typename... Vs>
                static constexpr identity<P<T, Vs...>> _f(identity<P<T, Vs...>>*);
                static constexpr identity<void> _f(...);
                using _result = decltype(f(reinterpret_cast<_kvs*>(nullptr)));
                using type = typename _result::type;
            };
        }

        template <typename M, typename T>
        using map_find = typename detail::map_find_impl<M, T>::type;

        namespace detail {

            template<
                class S1 = list<>, class S2 = list<>, class S3 = list<>, class S4 = list<>, class S5 = list<>, class S6 = list<>,
                class S7 = list<>, class S8 = list<>, class S9 = list<>, class S10 = list<>, class S11 = list<>
            > struct append_impl
            {};

            template<
                template<class...> class S1, template<class...> class S2, template<class...> class S3, template<class...> class S4,
                template<class...> class S5, template<class...> class S6, template<class...> class S7, template<class...> class S8,
                template<class...> class S9, template<class...> class S10, template<class...> class S11,
                typename... T1, typename... T2, typename... T3, typename... T4, typename... T5, typename... T6, typename... T7,
                typename... T8, typename... T9, typename... T10, typename... T11>
                struct append_impl<S1<T1...>, S2<T2...>, S3<T3...>, S4<T4...>, S5<T5...>, S6<T6...>, S7<T7...>, S8<T8...>, S9<T9...>, S10<T10...>, S11<T11...>>
            {
                using type = S1<T1..., T2..., T3..., T4..., T5..., T6..., T7..., T8..., T9..., T10..., T11...>;
            };
        }

        template<typename... Ls>
        using append = typename detail::append_impl<Ls...>::type;

        namespace detail {
            template<typename, template <typename...> class >
            struct filter_if_impl;

            template <template <typename...> class S, template <typename...> class F, typename...Ts >
            struct filter_if_impl<S<Ts...>, F>
            {
                template<class Y, class... Ys>
                using _filter = mp::if_<typename F<Y>::type, mp::list<>, mp::list<Y>>;

                using _filtered = mp::transform<mp::func<_filter>, S<Ts...>>;
                using type = mp::rename< mp::transform<mp::func<_filter>, S<Ts...>>, mp::append>;
            };
        }

        template <typename S, template <typename...> class F>
        using filter_if = typename detail::filter_if_impl<S, F>::type;

        template <typename S, typename  F>
        using filter_if_q = typename  detail::filter_if_impl<S, F::template fn>::type;

        namespace detail
        {
            template <typename... >
            struct make_map;

            template <typename... Ts, size_t... Is>
            struct make_map<std::index_sequence<Is...>, Ts...>
            {
                using type = list<list<size_c<Is>, Ts>...>;
            };

            template <typename...>
            struct at_impl;

            template <template <typename...> class S, typename I, typename... Ts>
            struct at_impl<S<Ts...>, I>
            {
                using type = second<map_find<typename make_map<std::index_sequence_for<Ts...>, Ts...>::type, I>>;
            };

        }

        template <typename S, typename I>
        using at = typename detail::at_impl<S, I>::type;

        template <typename S, size_t I>
        using at_v = typename detail::at_impl<S, size_c<I>>::type;

        template <typename S>
        struct size;

        template <template <typename...> class S, typename... Ts>
        struct size<S<Ts...>> : size_c<sizeof...(Ts)> {};

        template <typename S>
        using front = first<S>;

        template <typename S>
        using back = at_v<S, size<S>::value - 1>;

        template <typename B>
        using not_c = bool_c<!static_cast<bool>(B::value)>;

        namespace detail
        {
            constexpr size_t accumulate(...)
            {
                return 0;
            }

            template <typename V, typename ... Vs>
            constexpr size_t accumulate(V v, Vs... vs)
            {
                return static_cast<size_t>(v) + accumulate(vs...);
            }

            template <typename S, typename T>
            struct count_impl;

            template <template <typename...> class S, typename... Ys, typename T>
            struct count_impl<S<Ys...>, T>
            {
                static constexpr size_t _result = accumulate(static_cast<bool>(std::is_same<Ys, T>::value)... );
                using type = size_c<_result>;
            };
        }

        template <typename S, typename T>
        using count = typename detail::count_impl<S, T>::type;

        namespace detail
        {
            template <typename S, template <typename...> class P>
            struct count_if_impl;

            template <template <typename...> class S, typename... Ys, template <typename...> class P>
            struct count_if_impl<S<Ys...>, P>
            {
                static constexpr size_t _result = accumulate(static_cast<bool>(P<Ys>::value)...);
                using type = size_c<_result>;
            };
        }


        template<typename S, template <typename...> class P>
        using count_if = typename detail::count_if_impl<S, P>::type;

        template<typename S, typename P>
        using count_if_q = typename detail::count_if_impl<S, P::template fn>::type;

        template <typename S, template <typename...> class P>
        using any_of = bool_c<static_cast<bool>(count_if<S, P>::value)>;

        template <typename S, typename P>
        using any_of_q = any_of<S, P::template fn>;

        template<typename... Ts>
        using any = bool_c<(count_if_q < mp::list<bool_c<static_cast<bool>(Ts::value)>...>,
            bind_front<std::is_same, bool_c<true> >>::value != 0)>;

        template <typename S, template <typename...> class P>
        using all_of = bool_c<count_if<S, P>::value == size<S>::value>;

        template <typename S, typename P>
        using all_of_q = all_of<S, P::template fn>;

        template<typename... Ts>
        using all = bool_c<(count_if_q < mp::list<bool_c<static_cast<bool>(Ts::value)>...>,
            bind_front<std::is_same, bool_c<true> >>::value == sizeof...(Ts))>;

        template <typename S, template <typename...> class P>
        using none_of = not_c<all_of<S, P>>;

        template <typename S, typename P>
        using none_of_q = none_of<S, P::template fn>;

        template<typename... Ts>
        using none = not_c<all<Ts...>>;

        template <typename S, typename T>
        using contains = bool_c < static_cast<bool>(count<S, T>::value)>;
    }

    template < typename impl, template <typename...> class tag_tmpl>
    struct policy_default;

#define UTF_POLICY_DEFAULT(ITER,POLICYTYPE,VALUE) \
    template <typename base_iterator,typename...Ts> struct policy_default<ITER<base_iterator,Ts...>,POLICYTYPE> { using type = VALUE; }  


    namespace detail
    {
        template <typename category, typename... catagories>
		struct category_any_of : mp::any_of_q<mp::list<catagories...>, mp::bind_front< std::is_same, category>> {};

        template <typename category, typename... categories>
        using enable_if_category = typename std::enable_if<category_any_of<category, categories...>::value>::type;

        template <typename category, typename... catagories>
        using enable_if_category_b = typename std::enable_if<category_any_of<category, catagories...>::value, bool>::type;

        template< template <typename...> class tmpl, typename inst>
        struct is_instance_of_template : mp::bool_c<false> {};

        template<template <typename...> class tmpl, typename... Ts>
        struct is_instance_of_template <tmpl, tmpl<Ts...>> : mp::bool_c<true> {};

        template <typename T>
        struct is_char_type_impl {
            using type = mp::any_of_q< mp::list<char, char8_t, wchar_t, char16_t, char32_t>,
                mp::bind_front<std::is_same, T>>;
        };

        template <typename T>
        using is_char_type = typename is_char_type_impl<T>::type;

        template <typename impl, template <typename...> class policy_tmpl, typename policy_list>
        struct select_policy_handler_impl
        {
            template <typename policy>
            using _is_not_policy_q = mp::not_c<is_instance_of_template<policy_tmpl, policy>>;
            using _filtered = mp::filter_if<policy_list, _is_not_policy_q>;
            static_assert(mp::size<_filtered>::value < 2, "Multiple overlapping policies specified, single policy expected.");

            using type = typename policy_tmpl<void>::template handler< impl,
                typename mp::if_ <
                mp::size<_filtered>,
                mp::defer<mp::first, _filtered>,
                mp::identity<policy_default<impl, policy_tmpl>>
                >::type::type
            >;
        };

        template < typename impl, template <typename...> class policy_tmpl, typename... policies>
        using select_policy_handler = typename select_policy_handler_impl<impl, policy_tmpl, mp::list<policies...>>::type;

        template <template <typename...> class test, typename default_, typename... args>
        struct select_argument_or_default_impl
        {
            template <typename T>
            using not_test = mp::bool_c<!static_cast<bool>(test<T>::value)>;

            using _filtered = mp::filter_if<mp::list<args...>, not_test>;
            using type = typename mp::if_<
                mp::size<_filtered>,
                mp::defer<mp::first, _filtered>,
                mp::identity<default_>
            >::type;
        };

        template <template <typename...> class test, typename...Ts>
        using select_argument_or_default = typename select_argument_or_default_impl<test, Ts...>::type;

        template <typename T, typename enable = void>
        struct has_value_type : std::false_type {};

        template <typename T>
        struct has_value_type<T, mp::void_t<typename T::value_type>>
            : std::true_type {};

        template <typename T, typename enable = void>
        struct has_begin_end : std::false_type {};

        template <typename T>
        struct has_begin_end<T, mp::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
            : std::true_type {};
    }

    template <uint32_t value_>
    struct replace_with {
        static constexpr uint32_t value = value_;
    };
    using replace_with_fffd = replace_with<0xfffd>;

    struct throw_exception {};

    struct little_endian {};
    struct big_endian {};
    using native_endian = UTF_PLATFORM_ENDIANESS;

    namespace detail
    {
        template <typename impl, typename policy, typename enable = void>
        struct onerror_handler;

        template <typename impl, typename policy, typename enable = void>
        struct to_handler;

        template <typename impl, typename policy, typename enable = void>
        struct from_handler;
    }

    template <typename T>
    struct onerror
    {
        using type = T;
        template <typename...Ts>
        using handler = detail::onerror_handler<Ts...>;
    };


    template <typename T>
    struct from
    {
        using type = T;
        template <typename...Ts>
        using handler =  detail::from_handler<Ts...>;
    };

    template <typename T>
    struct to
    {
        using type = T;
        template <typename...Ts>
        using handler = detail::to_handler<Ts...>;
    };

    constexpr uint32_t utf_not_a_char = 0x0000FFFE; // { 0x00, 0x00, 0xFF, 0xFE };
    constexpr uint32_t utf_replacement_char = 0x0000FFFD; //{ 0x00,0x00,0xFF,0xFD };

   template <
        typename to_endianness,
        typename from_endianness = native_endian,
        typename std::enable_if< std::is_convertible<from_endianness, to_endianness>::value,bool>::type = true
    >
    uint32_t codepoint_as(uint32_t codepoint)
    {
       return codepoint;
    }

   template <
       typename to_endianness,
       typename from_endianness = native_endian,
       typename std::enable_if< !std::is_convertible<from_endianness, to_endianness>::value, bool>::type = true
   >
       uint32_t codepoint_as(uint32_t codepoint)
   {
       return UTF_PLATFORM_BSWAP32(codepoint);
   }

   template <
       typename to_endianness,
       typename from_endianness = native_endian,
       typename std::enable_if< std::is_convertible<from_endianness, to_endianness>::value, bool>::type = true
   >
       uint16_t codeunit_as(uint16_t codeunit)
   {
       return codeunit;
   }

   template <
       typename to_endianness,
       typename from_endianness = native_endian,
       typename std::enable_if< !std::is_convertible<from_endianness, to_endianness>::value, bool>::type = true
   >
       uint32_t codeunit_as(uint16_t codeunit)
   {
       return UTF_PLATFORM_BSWAP16(codeunit);
   }

   constexpr uint8_t  utf8_bom[] = { 0xEF,0xBB,0xBF };
   constexpr uint8_t  utf16_little_endian_bom[] = { 0xFF,0xFE };
   constexpr uint8_t  utf16_big_endian_bom[] = { 0xFE,0xFF };
   constexpr uint8_t  utf32_little_endian_bom[] = { 0xFF,0xFE,0x00,0x00 };
   constexpr uint8_t  utf32_big_endian_bom[] = { 0x00,0x00,0xFE,0xFF };

   enum class bom_type
   {
       none,
       utf8,
       utf16_little_endian,
       utf16_big_endian,
       utf32_little_endian,
       utf32_big_endian,
   };
}


namespace utf
{
    struct utf_iterator_access
    {
        template <typename impl_type>
        static void increment(impl_type& impl)
        {
            impl.increment();
        }

        template <typename impl_type>
        static void decrement(impl_type& impl)
        {
            impl.decrement();
        }

        template <typename impl_type>
        static bool equal(const impl_type& impl, const impl_type& other)
        {
            return impl.equal(other);
        }

        template <typename impl_type>
        static typename impl_type::reference dereference(const impl_type& impl)
        {
            return impl.dereference();
        }

    };

    template <
        typename impl,
        typename base_iterator,
        typename impl_category,
        typename fn_category,
        typename value_type_,
        typename reference_type_,
        typename enable=void
    >
     struct utf_iterator_base
    {
        using iterator_category = impl_category;
        using value_type = value_type_;
        using difference_type = std::ptrdiff_t;
        using pointer = typename std::add_pointer<value_type_>::type;
        using reference = reference_type_;

    protected:
        impl& derived()
        {
            return *static_cast<impl*>(this);
        }

        const impl& derived() const
        {
            return *static_cast<const impl*>(this);
        }
    public:


        reference operator*() const
        {
            return utf_iterator_access::dereference(derived());
        }

        friend bool operator==(const impl& first,const impl& second)
        {
            return utf_iterator_access::equal(first,second);
        }

        friend bool operator!=(const impl& first, const impl& second)
        {
            return !utf_iterator_access::equal(first, second);
        }

        impl& operator++()
        {
            utf_iterator_access::increment(this->derived());
            return this->derived();
        }


        template <typename iterator_type = impl>
        impl operator++(int)
        {
            impl result(this->derived());
            ++(*this);
            return result;
        }
    };

    template <typename iterator_type>
    struct resultof_postfix_increment : iterator_type
    {
        template<typename T, typename enable = void>
        struct resultof_postfix_increment_impl 
        { using type = iterator_type; };

        template<typename T>
        struct resultof_postfix_increment_impl<T, mp::void_t<typename T::postfix_increment_result_type>>
        {
            using type = typename T::postfix_increment_result_type;
        };

        using type = typename resultof_postfix_increment_impl<resultof_postfix_increment>::type;
    };

    template <typename impl, typename base_iterator, typename impl_category, typename fn_category,  typename value, typename reference>
    struct utf_iterator_base < impl, base_iterator, impl_category,fn_category,value, reference,
        detail::enable_if_category<fn_category, std::input_iterator_tag, std::forward_iterator_tag>
    > : utf_iterator_base < impl, base_iterator, impl_category, void, value, reference >
    {

        impl& operator++()
        {
            utf_iterator_access::increment(this->derived());
            return this->derived();
        }


        template <typename iterator_type=impl>
        auto operator++(int)
        {
            typename resultof_postfix_increment<impl>::type result(this->derived());
            ++(*this);
            return result;
        }
    };

    template <typename impl, typename base_iterator, typename impl_category,typename fn_category,typename value, typename reference>
    struct utf_iterator_base < impl, base_iterator, impl_category, fn_category, value, reference,
        detail::enable_if_category<fn_category, std::bidirectional_iterator_tag, std::random_access_iterator_tag>
    > : utf_iterator_base<impl, base_iterator, impl_category, std::forward_iterator_tag, value, reference>
        
    {
        impl& operator--()
        {
            utf_iterator_access::decrement(this->derived());
            return this->derived();
        }

        impl operator--(int)
        {
            impl result(this->derived());
            --(*this);
            return result;
        }
    };

    template <typename impl, typename base_iterator, typename impl_category, typename value, typename reference>
    struct utf_iterator : utf_iterator_base<impl,base_iterator,impl_category, impl_category,value,reference>{};
}
