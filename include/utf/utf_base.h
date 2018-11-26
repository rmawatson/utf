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
    using u8string = std::string;
    using u16string = std::u16string;
    using u32string = std::u32string;

    template <bool value,typename T,typename Y>
    using conditional_t = typename std::conditional<value,T,Y>::type;
    
    
    template <typename iterator_category>
    using is_input_tag = std::is_same< iterator_category, std::input_iterator_tag >;

    template <typename iterator_category>
    constexpr bool is_input_tag_v = is_input_tag<iterator_category>::value;

    template <typename iterator_category>
    using is_forward_tag = std::is_same< iterator_category, std::forward_iterator_tag>;

    template <typename iterator_category>
    constexpr bool is_forward_tag_v = is_forward_tag<iterator_category>::value;

    template <typename iterator_category>
    using is_bidirectional_tag = std::is_same< iterator_category, std::bidirectional_iterator_tag>;

    template <typename iterator_category>
    constexpr bool is_bidirectional_tag_v = is_bidirectional_tag<iterator_category>::value;

    template <typename iterator_category>
    using is_random_access_tag = std::is_same< iterator_category, std::random_access_iterator_tag>;

    template <typename iterator_category>
    constexpr bool is_random_access_tag_v = is_random_access_tag<iterator_category>::value;

    template<typename...> struct any_of : std::false_type { };
    template<typename T> struct any_of<T> : T {};
    template<typename T, typename ... Ts>
    struct any_of<T, Ts...> : std::conditional<bool(T::value), T, any_of<Ts...>>::type {};

    template<typename... Ts>
    constexpr bool any_of_v = any_of<Ts...>::value;

    template <typename category, template <typename> class... Ts>
    constexpr bool category_any_of = any_of_v< Ts<category>... >;

    template <typename category, template <typename> class... Ts>
    using enable_if_category = typename std::enable_if<category_any_of<category,Ts...>>::type;

    template <typename category, template <typename> class... Ts>
    using enable_if_category_b = typename std::enable_if<category_any_of<category, Ts...>, bool>::type;


    template <typename...>

    struct type_list {};
    template <bool v>
    using bool_c = std::integral_constant<bool, v>;

    template <typename T>
    struct type_c { using type = T; };

    template <typename...>
    using void_t = void;

    template< template <typename...> class tmpl, typename inst>
    struct is_instance_of_template : std::false_type {};

    template<template <typename...> class tmpl, typename... Ts>
    struct is_instance_of_template <tmpl, tmpl<Ts...>> : std::true_type {};

    template< template <typename...> class tmpl, typename... Ts>
    static constexpr bool is_instance_of_template_v = is_instance_of_template<tmpl, Ts...>::value;

    template < typename impl, template <typename...> class tag_tmpl>
    struct policy_default;

    #define UTF_POLICY_DEFAULT(ITER,POLICYTYPE,VALUE) \
    template <typename base_iterator,typename...Ts> struct policy_default<ITER<base_iterator,Ts...>,POLICYTYPE> { using type = VALUE; }  


    template < typename impl, template <typename...> class policy_tmpl, typename selected, typename policy_list>
    struct select_policy_handler_impl;

    template < typename impl, template <typename...> class policy_tmpl, typename... selected, typename... policies>
    struct select_policy_handler_impl<impl, policy_tmpl, type_list<selected...>, type_list<policies...>>
    {
        static_assert(sizeof...(selected) < 2, "Multiple overlapping policies specified, single policy expected.");

		template <typename... >
		struct select_first_or_default {  using type = typename policy_default<impl, policy_tmpl>::type; };

		template <typename selected_, typename... rest>
		struct select_first_or_default<selected_, rest...>
		{ using type = typename selected_::type; };
		
        using type = 
            typename policy_tmpl<void>::template handler<
                impl, 
				typename select_first_or_default<selected...>::type
            >;
    };

    template < typename impl, template <typename...> class policy_tmpl, typename... selected, typename policy, typename... policies>
    struct select_policy_handler_impl<impl, policy_tmpl, type_list< selected...>, type_list<policy, policies...>> :
        select_policy_handler_impl <
        impl,
        policy_tmpl,
        std::conditional_t<
            is_instance_of_template_v<policy_tmpl, policy>,
            type_list<selected..., policy>,
            type_list<selected...>
        >, type_list<policies...> > {};

    template < typename impl, template <typename...> class policy_tmpl, typename... policies>
    using select_policy_handler_t = typename select_policy_handler_impl<impl, policy_tmpl, type_list<>, type_list<policies...>>::type;

    template <template <typename...> class test, typename default_, typename... args>
    struct select_argument_or_default
    {
        using type = default_;
    };

    template <template <typename...> class test, typename default_, typename arg, typename... args>
    struct select_argument_or_default<test, default_, arg, args...> :
        std::conditional_t<test<arg>::value,
        utf::type_c<arg>,
        select_argument_or_default<test, default_, args...>
        > {};

    template <template <typename...> class test,typename...Ts>
    using select_argument_or_default_t = typename select_argument_or_default<test, Ts...>::type;

    template <typename T, typename enable = void>
    struct has_value_type : std::false_type {};

    template <typename T>
    struct has_value_type<T, void_t<typename T::value_type>>
        : std::true_type {};

    template <typename T, typename enable = void>
    struct has_begin_end : std::false_type {};

    template <typename T>
    struct has_begin_end<T, void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
        : std::true_type {};


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
        struct resultof_postfix_increment_impl<T, void_t<typename T::postfix_increment_result_type>>
        {
            using type = typename T::postfix_increment_result_type;
        };

        using type = typename resultof_postfix_increment_impl<resultof_postfix_increment>::type;
    };

    template <typename impl, typename base_iterator, typename impl_catagory,typename fn_category, typename value, typename reference>
    struct utf_iterator_base < impl, base_iterator, impl_catagory, fn_category ,value, reference,
        enable_if_category<fn_category, is_input_tag, is_forward_tag>
    > : utf_iterator_base < impl, base_iterator, impl_catagory, void, value, reference >
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

    template <typename impl, typename base_iterator, typename impl_catagory,typename fn_category, typename value, typename reference>
    struct utf_iterator_base < impl, base_iterator, impl_catagory, fn_category, value, reference,
        enable_if_category<fn_category, is_bidirectional_tag, is_random_access_tag>
    > : utf_iterator_base<impl, base_iterator, impl_catagory, std::forward_iterator_tag, value, reference>
        
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
