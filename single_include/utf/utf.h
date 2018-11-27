/* Copyright (c) 2018, rmawatson@hotmail.com
 *
 * Distributed under the Boost Software License, Version 1.0 (found in the
 * LICENSE file in the root directory of this source tree)
 */
 
/* this file has been automatically generated on 2018-11-26 18:00:16 and should not be edited manually. */
 
#pragma once
#include <algorithm>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <cstring>
#include <tuple>

 

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



namespace utf
{
    template <typename base_iterator, typename... policies>
    class utf8_to_utf32_iterator;

    UTF_POLICY_DEFAULT(utf8_to_utf32_iterator, onerror, throw_exception);
    UTF_POLICY_DEFAULT(utf8_to_utf32_iterator, to, little_endian);

    namespace detail {

        template<uint32_t value, typename base_iterator,typename... Ts>
        struct onerror_handler< utf8_to_utf32_iterator<base_iterator,Ts...>, replace_with<value>, void>
        {
            static uint32_t apply(const std::string& errmsg) {
                return value;
            }
        };

        template <typename base_iterator,typename... Ts>
        struct onerror_handler< utf8_to_utf32_iterator<base_iterator,Ts...>, throw_exception, void>
        {
            static uint32_t apply(const std::string& errmsg) { throw std::runtime_error(errmsg); }

        };

        template<typename endianness, typename base_iterator,typename... Ts>
        struct to_handler < utf8_to_utf32_iterator<base_iterator,Ts...>, endianness>
        {
            static uint32_t apply(uint32_t codepoint) {
                return codepoint_as<endianness>(codepoint);
            }
        };
    }

    /*** common iterator implementation */
    template <typename impl, typename base_iterator, typename impl_catagory, typename value_type, typename reference_type, typename... policies>
    struct utf8_to_utf32_iterator_base :
        utf_iterator< impl, base_iterator, impl_catagory, value_type, reference_type>
    {
     protected:


        using base_type = typename utf8_to_utf32_iterator_base::utf_iterator;

        friend utf_iterator_access;

        using error_policy = select_policy_handler_t<impl, onerror, policies...>;
        using to_policy    = select_policy_handler_t<impl, to, policies...>;
        
        utf8_to_utf32_iterator_base() {};
        
        utf8_to_utf32_iterator_base(base_iterator pos, const base_iterator end) :
            pos_(pos),
            end_(end),
            current_(0),
            replace_(0),
            size_(5)
        {};

        static inline size_t codepoint_bytes(uint8_t value)
        {
            if (value < 0x80)
                return 1;

            for (size_t i = 0; i < 5; ++i)
                if (!(value & 0x80 >> i))
                    return i;

            return 0;
        }

        template < typename onerror_policy, typename to_policy>
        static uint32_t read_next_impl(base_iterator& pos, const base_iterator& end, onerror_policy, to_policy, int8_t& replace, int8_t& consumed)
        {

            uint8_t start_byte = *pos++; ++consumed;

            if ((start_byte & 0xC0) == 0x80) {
                return onerror_policy::apply("invalid start byte decoding utf8 string.");
            }

            size_t byte_count = codepoint_bytes(start_byte);
            if (!byte_count)
                return onerror_policy::apply("invalid byte count decoding utf8 string.");

            uint32_t codepoint = start_byte;
            for (size_t i = 1; i < byte_count; ++i, ++pos, ++consumed)
            {
                if (pos == end)
                    return (replace = static_cast<int8_t>(i-1),onerror_policy::apply("unexpected eof deocding of ut8 string."));
                else if ((*pos & 0xC0) != 0x80)
                    return (replace = static_cast<int8_t>(i-1),onerror_policy::apply("invalid byte deocding of ut8 string. expected continuation byte."));

                codepoint <<= 6;
                codepoint |= static_cast<uint32_t>(*pos) & 0x3F;
            }

            static const uint32_t masks[] =
            {
               0x7Fu,
               0x7FFu,
               0xFFFFu,
               0x1FFFFFu,
            };

            return codepoint & masks[byte_count - 1];
        }

        template < typename onerror_policy, typename to_policy>
        static uint32_t read_next(base_iterator& pos, const base_iterator& end, onerror_policy, to_policy, int8_t& replace,int8_t& consumed)
        {
            consumed = 0;
            replace  = 0;
            return to_policy::apply(read_next_impl(pos, end, onerror_policy(), to_policy(), replace, consumed));
        }

        template < typename onerror_policy, typename to_policy>
        static uint32_t read_prev_impl(base_iterator& pos, const  base_iterator& start, onerror_policy, to_policy, int8_t& replace, int8_t& consumed)
        {
            base_iterator prev(pos);
            base_iterator end(pos);
            
            size_t byte_count = 1;
            for (--prev; byte_count < 5; ++byte_count, --prev)
            {
                if ((*prev & 0xC0) != 0x80)
                {
                    if (codepoint_bytes(*prev) == byte_count)
                        break;

                    return (--pos, onerror_policy::apply("invalid byte count decoding utf8 string."));
                }

                if (prev == start)
                    return (--pos, onerror_policy::apply("unexpected sof deocding of ut8 string."));
            }

            pos = prev;

            return read_next_impl(prev, end, onerror_policy(), to_policy(), replace, consumed);
        }

        template < typename onerror_policy, typename to_policy>
        static uint32_t read_prev(base_iterator& pos, const base_iterator& end, onerror_policy, to_policy, int8_t& replace, int8_t& consumed)
        {
            consumed = 0;
            replace = 0;
            return to_policy::apply(read_prev_impl(pos, end, onerror_policy(), to_policy(), replace, consumed));
        }

        typename base_type::reference dereference() const
        {
            if (!replace_ && read_required())
            {
                base_iterator pos(pos_);
                read(pos);
            }
            return this->current_;
        }

        bool equal(const impl& other) const
        {
            return pos_ == other.pos_;
        }

        void increment()
        {
            if (replace_ > 0)
                return (void)--replace_;

            if (read_required())
                read(pos_);
            else
                std::advance(pos_, size_);

            size_ = 5;
        }

        bool read_required() const
        {
            return size_ == 5;
        }

        void read(base_iterator& pos) const
        {
            current_ = read_next(
                pos,
                end_,
                error_policy(), to_policy(),
                replace_,
                size_
            );

        }

        mutable base_iterator pos_;
        base_iterator end_;

        mutable uint32_t current_;
        mutable int8_t size_;
        mutable int8_t replace_;
        

    };

    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf8_to_utf32_iterator_impl;

    /*** input iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf8_to_utf32_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< base_category, is_input_tag >
        , policies...> :
        utf8_to_utf32_iterator_base<impl, base_iterator, std::input_iterator_tag, uint32_t, const uint32_t&, policies...>
    {

    protected:
        using base_type = typename utf8_to_utf32_iterator_impl::utf8_to_utf32_iterator_base;
        friend utf_iterator_access;

        utf8_to_utf32_iterator_impl()
        {}

        utf8_to_utf32_iterator_impl(base_iterator pos, const base_iterator end) :
            base_type(pos,end)
        {
            this->size_ = 0;
            if (pos != end)
                this->read(this->pos_);
        }

        void increment()
        {
            if (this->replace_ > 0)
                return (void)--this->replace_;

            if (this->pos_ != this->end_)
                this->read(this->pos_);
            else 
                this->size_ = 0;
        }

        bool equal(const impl& other) const
        {
            return other.pos_ == this->pos_ && other.size_ == this->size_ && this->replace_ == other.replace_;
        }

        typename base_type::reference dereference() const
        {
            return this->current_;
        }

    };

    /***  forward iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf8_to_utf32_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< base_category, is_forward_tag>, policies...> 
        : utf8_to_utf32_iterator_base<impl, base_iterator, std::forward_iterator_tag, uint32_t, const uint32_t&, policies...>
    {
    protected:
        using base_type = typename utf8_to_utf32_iterator_impl::utf8_to_utf32_iterator_base;
        using base_type::base_type;
    };

    /*** bidirectional & random access iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf8_to_utf32_iterator_impl<impl, base_iterator, base_category,
        enable_if_category<base_category, is_bidirectional_tag, is_random_access_tag>, policies...>
        : utf8_to_utf32_iterator_base<impl, base_iterator, std::bidirectional_iterator_tag, uint32_t, const uint32_t&, policies...>
    {
    protected:
        friend utf_iterator_access;

        using base_type = typename utf8_to_utf32_iterator_impl::utf8_to_utf32_iterator_base;

        using typename base_type::error_policy;
        using typename base_type::to_policy;

        utf8_to_utf32_iterator_impl()
        {}

        utf8_to_utf32_iterator_impl(base_iterator pos, base_iterator start, base_iterator end) :
            base_type(pos,end),
            start_(start)
        {}

        void decrement()
        {
            if (this->replace_ < 0)
                return (void)++this->replace_;

            this->derived().current_ = this->read_prev(
                this->pos_,
                this->start_,
                error_policy(), to_policy(),
                this->replace_,
                this->size_
            );
        }

        base_iterator start_;
    };

    template <typename base_iterator, typename... policies>
    class utf8_to_utf32_iterator :
        public utf8_to_utf32_iterator_impl< 
            utf8_to_utf32_iterator<base_iterator, policies...>, 
            base_iterator, 
            typename std::iterator_traits<base_iterator>::iterator_category, 
            void, policies...
        >
    {
        using impl_type = typename utf8_to_utf32_iterator::utf8_to_utf32_iterator_impl;

        friend typename impl_type::base_type;

    public:

        utf8_to_utf32_iterator() {};

        template< typename category_type=typename std::iterator_traits<base_iterator>::iterator_category,
            typename bidirectional_or_random_access=enable_if_category<category_type,is_bidirectional_tag,is_random_access_tag>>
        utf8_to_utf32_iterator(base_iterator pos, base_iterator start, base_iterator end) : impl_type(pos, start, end) {}

        template< typename category_type=typename std::iterator_traits<base_iterator>::iterator_category,
            typename input_or_forward=enable_if_category<category_type, is_input_tag, is_forward_tag>>
        utf8_to_utf32_iterator(base_iterator pos, base_iterator end) : impl_type(pos, end) {}
    };

    template <typename... policies,typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_bidirectional_tag, is_random_access_tag> = true>
    auto make_utf8_to_utf32_iterator(base_iterator begin, base_iterator end)
    {
        return utf8_to_utf32_iterator<base_iterator, policies...>{begin, begin,end};
    }

    template <typename... policies, typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_input_tag, is_forward_tag> = true>
    auto make_utf8_to_utf32_iterator(base_iterator begin, base_iterator end)
    {
        return utf8_to_utf32_iterator<base_iterator, policies...>{begin, end};
    }
}

 

namespace utf
{

    template <typename base_iterator, typename... policies>
    class utf32_to_utf8_iterator;

    UTF_POLICY_DEFAULT(utf32_to_utf8_iterator, onerror, throw_exception);
    UTF_POLICY_DEFAULT(utf32_to_utf8_iterator, from,    little_endian);

    namespace detail {

        template<uint32_t value,typename base_iterator, typename... Ts>
        struct onerror_handler< utf32_to_utf8_iterator<base_iterator,Ts...>, replace_with<value>, void>
        {
            static uint32_t apply(const std::string& errmsg,uint32_t& codepoint) {
                return codepoint = value;
            }
        };

        template <typename base_iterator,typename... Ts>
        struct onerror_handler< utf32_to_utf8_iterator<base_iterator,Ts...>, throw_exception, void>
        {
            static uint32_t apply(const std::string& errmsg, uint32_t& codepoint) {
                throw std::runtime_error(errmsg); 
            }

        };

        template<typename endianness,typename base_iterator, typename... Ts>
        struct from_handler < utf32_to_utf8_iterator<base_iterator,Ts...>, endianness>
        {
            static uint32_t apply(uint32_t codepoint) {
                return codepoint_as<native_endian, endianness>(codepoint);
            }
        };
    }

    /*** common iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename value_type, typename reference_type, typename... policies>
    struct utf32_to_utf8_iterator_base :
        utf_iterator< impl, base_iterator, base_category, value_type, reference_type>
    {
        friend utf_iterator_access;
    protected:

        utf32_to_utf8_iterator_base() {};
        utf32_to_utf8_iterator_base(base_iterator pos) :
            current_{ 0 },
            size_(0),
            index_(4),
            pos_(pos)
        {}

        using error_policy = select_policy_handler_t<impl, onerror, policies...>;
        using from_policy = select_policy_handler_t<impl, from, policies...>;

        template <typename onerror_policy, typename from_policy>
        static size_t read_next(base_iterator& pos,  uint8_t* bytes,onerror_policy, from_policy)
        {
            uint32_t codepoint = from_policy::apply(*pos++);

            if (codepoint > 0x10FFFF)
                onerror_policy::apply("error encoding utf8 string, invalid codepoint.", codepoint);

            if (codepoint <= 0x007F)
            {
                bytes[0] = static_cast<uint8_t>(codepoint);
                bytes[1] = 0;
                bytes[2] = 0;
                bytes[3] = 0;

                return 1;
            }
            if (codepoint <= 0x07FF)
            {
                bytes[0] = static_cast<uint8_t>((codepoint >> 6 & 0x1F) | 0xC0);
                bytes[1] = static_cast<uint8_t>((codepoint & 0x3F) | 0x80);
                bytes[2] = 0;
                bytes[3] = 0;
                return 2;
            }

            if (codepoint <= 0xFFFF)
            {
                bytes[0] = static_cast<uint8_t>((codepoint >> 12 & 0x0F) | 0xE0);
                bytes[1] = static_cast<uint8_t>((codepoint >> 6 & 0x3F) | 0x80);
                bytes[2] = static_cast<uint8_t>((codepoint & 0x3F) | 0x80);
                bytes[3] = 0;
                return 3;
            }
            if (codepoint <= 0x10FFFF)
            {
                bytes[0] = static_cast<uint8_t>((codepoint >> 18 & 0x0E) | 0xF0);
                bytes[1] = static_cast<uint8_t>((codepoint >> 12 & 0x3F) | 0x80);
                bytes[2] = static_cast<uint8_t>((codepoint >> 6 & 0x3F) | 0x80);
                bytes[3] = static_cast<uint8_t>((codepoint & 0x3F) | 0x80);
                return 4;
            }
            return 0;
        }

        template < typename onerror_policy, typename from_policy>
        static size_t read_prev(base_iterator& pos, uint8_t* bytes,onerror_policy, from_policy)
        {
            base_iterator prev(--pos);
            return read_next(prev,bytes, onerror_policy(), from_policy());
        }

        using base_type = typename utf32_to_utf8_iterator_base::utf_iterator;


        typename base_type::reference dereference() const
        {
            read_pending();
            return this->current_[this->index_];
        }

        bool equal(const impl& other) const
        {
            return this->pos_ == other.pos_ && ((this->index_ | other.index_) & 3) == 0;
        }

        void increment()
        {
            read_pending();
            ++this->index_;

            if (this->index_ == this->size_)
                this->index_ = 4;
        }

        inline void read_pending() const
        {
            if (this->index_ == 4) {
                this->size_ = read_next(this->pos_, this->current_, error_policy(), from_policy());
                this->index_ = 0;
            }
        }

        mutable uint8_t current_[4];
        mutable size_t index_;
        mutable size_t size_;
        mutable base_iterator pos_;

    };

    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf8_iterator_impl;

    /*** input iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf8_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< typename std::iterator_traits<base_iterator>::iterator_category, is_input_tag>
        , policies...> :
        utf32_to_utf8_iterator_base<impl, base_iterator, base_category, uint8_t, const uint8_t&, policies...>
    {

    protected:

        struct postfix_increment_result_type {

            postfix_increment_result_type(const impl& iter) :
                value_(*iter)
            {}

            uint8_t operator*() const { return value_; }

        private:
            uint8_t value_;
        };


        friend utf_iterator_access;


        using base_type = typename utf32_to_utf8_iterator_impl::utf32_to_utf8_iterator_base;
        using base_type::base_type;




        typename base_type::reference dereference() const
        {
            this->read_pending();
            return this->current_[this->index_];
        }

        bool equal(const impl& other) const
        {
            return this->pos_ == other.pos_ && ((this->index_ | other.index_) & 3) == 0;
        }

        void increment()
        {
            this->read_pending();
            ++this->index_;

            if (this->index_ == this->size_)
                this->index_ = 4;
        }


        

    };

    /*** forward iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf8_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< typename std::iterator_traits<base_iterator>::iterator_category, is_forward_tag>
        , policies...> :
        utf32_to_utf8_iterator_base<impl, base_iterator, base_category, uint8_t, const uint8_t&, policies...>
    {
    protected:
        using base_type = typename utf32_to_utf8_iterator_impl::utf32_to_utf8_iterator_base;
        using base_type::base_type;

    };


    /*** bidirectional & random access iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf8_iterator_impl<impl, base_iterator, base_category,
        enable_if_category<  typename std::iterator_traits<base_iterator>::iterator_category, is_bidirectional_tag, is_random_access_tag>
        , policies...> :
        utf32_to_utf8_iterator_base<impl, base_iterator, std::bidirectional_iterator_tag, uint8_t, const uint8_t&, policies...>
    {
    protected:
        friend utf_iterator_access;

        using base_type = typename utf32_to_utf8_iterator_impl::utf32_to_utf8_iterator_base;
        using base_type::base_type;
        using typename base_type::error_policy;
        using typename base_type::from_policy;
        
        void decrement()
        {
            if ((this->index_ & 3) == 0) {
                this->size_ = base_type::read_prev(this->pos_, this->current_, error_policy(), from_policy());
                this->index_ = this->size_;
            }
            --this->index_;
        }
    };

    template <typename base_iterator, typename... policies>
    class utf32_to_utf8_iterator :
        public utf32_to_utf8_iterator_impl<
        utf32_to_utf8_iterator<base_iterator, policies...>,
        base_iterator,
        typename std::iterator_traits<base_iterator>::iterator_category,
        void, policies...
        >
    {

        using impl_type = typename utf32_to_utf8_iterator::utf32_to_utf8_iterator_impl;
    public:
        utf32_to_utf8_iterator() {};
        utf32_to_utf8_iterator(base_iterator pos) : impl_type(pos) {};

    };

    template <typename... policies, typename base_iterator>
    auto make_utf32_to_utf8_iterator(base_iterator iter)
    {
        return utf32_to_utf8_iterator<base_iterator, policies...>{iter};
    }
}


 

namespace utf
{
    template <typename base_iterator, typename... policies>
    class utf16_to_utf32_iterator;

    UTF_POLICY_DEFAULT(utf16_to_utf32_iterator, onerror, throw_exception);
    UTF_POLICY_DEFAULT(utf16_to_utf32_iterator, to, little_endian);
    UTF_POLICY_DEFAULT(utf16_to_utf32_iterator, from, little_endian);

    namespace detail {

        template<uint32_t value,typename base_iterator, typename... Ts>
        struct onerror_handler< utf16_to_utf32_iterator<base_iterator,Ts...>, replace_with<value>, void>
        {
            static uint32_t apply(const std::string& errmsg) {
                return value;
            }
        };

        template <typename base_iterator,typename... Ts>
        struct onerror_handler< utf16_to_utf32_iterator<base_iterator,Ts...>, throw_exception, void>
        {
            static uint32_t apply(const std::string& errmsg) { throw std::runtime_error(errmsg); }

        };

        template<typename endianness,typename base_iterator, typename... Ts>
        struct to_handler < utf16_to_utf32_iterator<base_iterator,Ts...>, endianness>
        {
            static uint32_t apply(uint32_t codepoint) {
                return codepoint_as<endianness>(codepoint);
            }
        };

        template<typename endianness,typename base_iterator, typename... Ts>
        struct from_handler < utf16_to_utf32_iterator<base_iterator,Ts...>, endianness>
        {
            static uint16_t apply(uint16_t codeunit) {
                return codeunit_as<native_endian,endianness>(codeunit);
            }
        };

    }

    /*** common iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename value_type, typename reference_type, typename... policies>
    struct utf16_to_utf32_iterator_base :
        utf_iterator< impl, base_iterator, base_category, value_type, reference_type>
    {

    protected:
        utf16_to_utf32_iterator_base() {};

        using base_type = typename utf16_to_utf32_iterator_base::utf_iterator;

        friend utf_iterator_access;

        using error_policy = select_policy_handler_t<impl, onerror, policies...>;
        using from_policy = select_policy_handler_t<impl, from, policies...>;
        using to_policy = select_policy_handler_t<impl, to, policies...>;

        template < typename onerror_policy,typename from_policy, typename to_policy>
        static uint32_t read_next_impl(base_iterator& pos, const base_iterator& end, onerror_policy, from_policy,to_policy, int8_t& replace, int8_t& consumed)
        {
            uint32_t value = from_policy::apply(*pos++); ++consumed;

            if ((value & 0xFC00) == 0xD800)
            {
                if (pos == end)
                    return (replace=0,onerror_policy::apply("unexpected end of utf16 string"));

                uint32_t lowsurrogate = from_policy::apply(*pos); 

                if ((lowsurrogate & 0xFC00) != 0xDC00)
                    return (replace=0,onerror_policy::apply("unpaired surrogate found"));

                value  = (value - 0xD7C0) << 10;
                value |= (lowsurrogate & 0x3FF);

                ++pos; ++consumed;

            }
            else {

                if ((value & 0xF800) == 0xD800)
                    return (replace=0,onerror_policy::apply("unpaired surrogate found"));

            }
            return value;
        }

        template <typename onerror_policy, typename from_policy,typename to_policy>
        static uint32_t read_next(base_iterator& pos, const base_iterator& end, onerror_policy, from_policy,to_policy, int8_t& replace, int8_t& consumed)
        {
            consumed = 0;
            replace = 0;
            return to_policy::apply(read_next_impl(pos, end, onerror_policy(), from_policy(),to_policy(), replace, consumed));
        }

        template <  typename onerror_policy, typename from_policy, typename to_policy>
        static uint32_t read_prev_impl(base_iterator& pos,const base_iterator& start, onerror_policy, from_policy, to_policy, int8_t& replace, int8_t& consumed)
        {
            base_iterator prev(pos);
            base_iterator end(pos);

            uint16_t value = from_policy::apply(*--prev);

            if ((value & 0xFC00) == 0xDC00) {
                if (prev == start)
                    return (--pos, onerror_policy::apply("unexpected start of utf16 string"));
                value = *--prev;
                if ((value & 0xFC00) != 0xD800)
                    return (--pos, onerror_policy::apply("unexpected start of utf16 string"));
            }
            else if ((value & 0xF800) == 0xD800)
                return (--pos,onerror_policy::apply("unpaired surrogate found"));
            
            pos = prev;
            return read_next_impl(prev, end, onerror_policy(),from_policy(), to_policy(), replace, consumed);
        }

        template < typename onerror_policy, typename from_policy, typename to_policy>
        static uint32_t read_prev(base_iterator& pos, const base_iterator& start,onerror_policy, from_policy, to_policy, int8_t& replace, int8_t& consumed)
        {
            consumed = 0;
            replace = 0;
            return to_policy::apply(read_prev_impl(pos, start, onerror_policy(), from_policy(), to_policy(),replace,consumed));
        }

        utf16_to_utf32_iterator_base(base_iterator pos,  const base_iterator end) :
            pos_(pos),
            end_(end),
            current_(0),
            replace_(0),
            size_(2)
        {};

        typename base_type::reference dereference() const
        {
            if (!replace_ && read_required())
            {
                base_iterator pos(pos_);
                read(pos);
            }
            return this->current_;
        }

        bool equal(const impl& other) const
        {
            return pos_ == other.pos_;
        }

        void increment()
        {
            if (replace_ > 0)
                return (void)--replace_;

            if (read_required())
                read(pos_);
            else
                std::advance(pos_, size_);

            size_ = 2;
        }

        bool read_required() const
        {
            return size_ == 2;
        }

        void read(base_iterator& pos) const
        {
            current_ = this->read_next(
                pos,
                end_,
                error_policy(), from_policy(), to_policy(),
                replace_,
                size_
            );

        }

        mutable base_iterator pos_;
        base_iterator end_;

        mutable uint32_t current_;
        mutable int8_t size_;
        mutable int8_t replace_;
    };

    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf16_to_utf32_iterator_impl;

    ///*** input iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf16_to_utf32_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< base_category, is_input_tag >
        , policies...> :
        utf16_to_utf32_iterator_base<impl, base_iterator, base_category, uint32_t, const uint32_t&, policies...>
    {

    protected:
        using base_type = typename utf16_to_utf32_iterator_impl::utf16_to_utf32_iterator_base;
        friend utf_iterator_access;

        utf16_to_utf32_iterator_impl() {};

        utf16_to_utf32_iterator_impl(base_iterator pos, const base_iterator end) :
            base_type(pos, end)
        {
            this->size_ = 0;
            if (pos != end)
                this->read(this->pos_);
        }

        void increment()
        {
            if (this->replace_ > 0)
                return (void)--this->replace_;

            if (this->pos_ != this->end_)
                this->read(this->pos_);
            else
                this->size_ = 0;
        }

        bool equal(const impl& other) const
        {
            return other.pos_ == this->pos_ && other.size_ == this->size_ && this->replace_ == other.replace_;
        }

        typename base_type::reference dereference() const
        {
            return this->current_;
        }
    };

    /***  forward iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf16_to_utf32_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< base_category, is_forward_tag>, policies...>
        : utf16_to_utf32_iterator_base<impl, base_iterator, base_category, uint32_t, const uint32_t&, policies...>
    {

    protected:
        using base_type = typename utf16_to_utf32_iterator_impl::utf16_to_utf32_iterator_base;
        using base_type::base_type;

    };

    /*** bidirectional & random access iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf16_to_utf32_iterator_impl<impl, base_iterator, base_category,
        enable_if_category<
        base_category, is_bidirectional_tag, is_random_access_tag>
        , policies...> :
        utf16_to_utf32_iterator_base<impl, base_iterator, std::bidirectional_iterator_tag, uint32_t, const uint32_t&, policies...>
    {
    protected:

        using base_type = typename utf16_to_utf32_iterator_impl::utf16_to_utf32_iterator_base;
        using typename base_type::error_policy;
        using typename base_type::to_policy;
        using typename base_type::from_policy;
        
        friend utf_iterator_access;

        utf16_to_utf32_iterator_impl() {};

        utf16_to_utf32_iterator_impl(base_iterator pos,const base_iterator start, const base_iterator end) :
            base_type(pos,end),
            start_(start)
        {};

        void decrement()
        {
            if (this->replace_ < 0)
                return (void)++this->replace_;

            this->derived().current_ = this->read_prev(
                this->pos_,
                this->start_,
                error_policy(), from_policy(),to_policy(),
                this->replace_,
                this->size_
            );
        }

        base_iterator start_;

    };

    template <typename base_iterator, typename... policies>
    class utf16_to_utf32_iterator :
        public utf16_to_utf32_iterator_impl<
        utf16_to_utf32_iterator<base_iterator, policies...>,
        base_iterator,
        typename std::iterator_traits<base_iterator>::iterator_category,
        void, policies...
        >
    {
        using impl_type = typename utf16_to_utf32_iterator::utf16_to_utf32_iterator_impl;

        friend typename impl_type::base_type;


    public:
        utf16_to_utf32_iterator() {};

        template<typename category_type=typename std::iterator_traits<base_iterator>::iterator_category,
            typename bidirectional_or_random_access = enable_if_category<category_type, is_bidirectional_tag, is_random_access_tag>>
        utf16_to_utf32_iterator(base_iterator pos, base_iterator start, base_iterator end) : impl_type(pos, start, end) {}

        template<typename category_type=typename std::iterator_traits<base_iterator>::iterator_category,
            typename input_or_forward = enable_if_category<category_type, is_input_tag, is_forward_tag>>
        utf16_to_utf32_iterator(base_iterator pos, base_iterator end) : impl_type(pos, end) {}

    };

    template <typename... policies, typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_bidirectional_tag, is_random_access_tag> = true>
    auto make_utf16_to_utf32_iterator(base_iterator begin, base_iterator end)
    {
        return utf16_to_utf32_iterator<base_iterator, policies...>{begin, begin, end};
    }

    template <typename... policies, typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_input_tag, is_forward_tag> = true>
    auto make_utf16_to_utf32_iterator(base_iterator begin, base_iterator end)
    {
        return utf16_to_utf32_iterator<base_iterator, policies...>{begin, end};
    }

}


namespace utf
{

    template <typename base_iterator, typename... policies>
    class utf32_to_utf16_iterator;

    UTF_POLICY_DEFAULT(utf32_to_utf16_iterator, onerror, throw_exception);
    UTF_POLICY_DEFAULT(utf32_to_utf16_iterator, from, little_endian);
    UTF_POLICY_DEFAULT(utf32_to_utf16_iterator, to, little_endian);

    namespace detail {

        template<uint32_t value, typename base_iterator, typename... Ts>
        struct onerror_handler< utf32_to_utf16_iterator<base_iterator,Ts...>, replace_with<value>, void>
        {
            static uint32_t apply(const std::string& errmsg, uint32_t& codepoint) {
                return codepoint = value;
            }
        };

        template <typename base_iterator, typename... Ts>
        struct onerror_handler< utf32_to_utf16_iterator<base_iterator,Ts...>, throw_exception, void>
        {
            static uint32_t apply(const std::string& errmsg, uint32_t& codepoint) {
                throw std::runtime_error(errmsg);
            }
        };

        template<typename endianness,typename base_iterator,  typename... Ts>
        struct to_handler < utf32_to_utf16_iterator<base_iterator,Ts...>, endianness>
        {
            static uint16_t apply(uint16_t codeunit) {
                return codeunit_as<endianness>(codeunit);
            }
        };

        template<typename endianness,typename base_iterator, typename... Ts>
        struct from_handler < utf32_to_utf16_iterator<base_iterator,Ts...>, endianness>
        {
            static uint32_t apply(uint32_t codepoint) {
                return codepoint_as<native_endian, endianness>(codepoint);
            }
        };
    }

    /*** common iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename value_type, typename reference_type, typename... policies>
    struct utf32_to_utf16_iterator_base :
        utf_iterator< impl, base_iterator, base_category, value_type, reference_type>
    {
        friend utf_iterator_access;
    protected:

        utf32_to_utf16_iterator_base() {};
        utf32_to_utf16_iterator_base(base_iterator pos) :
            current_{ 0 },
            size_(0),
            index_(2),
            pos_(pos)
        {}

        using error_policy = select_policy_handler_t<impl, onerror, policies...>;
        using from_policy  = select_policy_handler_t<impl, from, policies...>;
        using to_policy    = select_policy_handler_t<impl, to, policies...>;

        template <typename onerror_policy, typename from_policy,typename to_policy>
        static size_t read_next(base_iterator& pos, uint16_t* bytes, onerror_policy, from_policy, to_policy)
        {

            uint32_t codepoint = from_policy::apply(*pos++);
            size_t size = 0;
            if (codepoint > 0x10FFFF)
                codepoint = onerror_policy::apply("error encoding utf16 string, invalid codepoint.", codepoint);
            

            if (codepoint >= 0x10000)
            {
                bytes[0] = static_cast<uint16_t>(codepoint >> 10) + 0xD7C0;
                bytes[1] = static_cast<uint16_t>(codepoint & 0x3FF) + 0xDC00;
                size = 2;
            }
            else
            {
                if ((codepoint & 0xFFFFF800) == 0xD800)
                    codepoint = onerror_policy::apply("error encoding utf16 string, invalid surrogate codepoint.", codepoint);

                bytes[0] = static_cast<uint16_t>(codepoint);
                bytes[1] = 0;
                size  = 1;
            }

            for (size_t i = 0; i < size; ++i)
                bytes[i] = to_policy::apply(bytes[i]);

            return size;
        }


        template <  typename onerror_policy, typename from_policy, typename to_policy>
        static size_t read_prev(base_iterator& pos, uint16_t* bytes, onerror_policy, from_policy,to_policy)
        {
            base_iterator prev(--pos);
            return read_next(prev, bytes, onerror_policy(), from_policy(), to_policy());
        }

        using base_type = typename utf32_to_utf16_iterator_base::utf_iterator;
        using base_type::base_type;

        typename base_type::reference dereference() const
        {
            read_pending();
            return this->current_[this->index_];
        }

        bool equal(const impl& other) const
        {
            return this->pos_ == other.pos_ && ((this->index_ | other.index_) & 1) == 0;
        }

        void increment()
        {
            read_pending();
            ++this->index_;

            if (this->index_ == this->size_)
                this->index_ = 2;
        }

        inline void read_pending() const
        {
            if (this->index_ == 2) {
                this->size_ = read_next(this->pos_, this->current_, error_policy(), from_policy(), to_policy());
                this->index_ = 0;
            }
        }

        mutable uint16_t current_[2];
        mutable size_t index_;
        mutable size_t size_;
        mutable base_iterator pos_;

    };

    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf16_iterator_impl;

    /*** input iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf16_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< typename std::iterator_traits<base_iterator>::iterator_category, is_input_tag>
        , policies...> :
        utf32_to_utf16_iterator_base<impl, base_iterator, base_category, uint16_t, const uint16_t&, policies...>
    {

    protected:

        struct postfix_increment_result_type {

            postfix_increment_result_type(const impl& iter) :
                value_(*iter)
            {}

            uint16_t operator*() const { return value_; }

        private:
            uint16_t value_;
        };


        friend utf_iterator_access;

        using base_type = typename utf32_to_utf16_iterator_impl::utf32_to_utf16_iterator_base;
        using base_type::base_type;

        typename base_type::reference dereference() const
        {
            this->read_pending();
            return this->current_[this->index_];
        }

        bool equal(const impl& other) const
        {
            return this->pos_ == other.pos_ && ((this->index_ | other.index_) & 1) == 0;
        }

        void increment()
        {
            this->read_pending();
            ++this->index_;

            if (this->index_ == this->size_)
                this->index_ = 2;
        }




    };

    /*** forward iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf16_iterator_impl<impl, base_iterator, base_category,
        enable_if_category< typename std::iterator_traits<base_iterator>::iterator_category, is_forward_tag>
        , policies...> :
        utf32_to_utf16_iterator_base<impl, base_iterator, base_category, uint16_t, const uint16_t&, policies...>
    {
    protected:
        using base_type = typename utf32_to_utf16_iterator_impl::utf32_to_utf16_iterator_base;
        using base_type::base_type;

    };

    /*** bidirectional & random access iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf32_to_utf16_iterator_impl<impl, base_iterator, base_category,
        enable_if_category<  typename std::iterator_traits<base_iterator>::iterator_category, is_bidirectional_tag, is_random_access_tag>
        , policies...> :
        utf32_to_utf16_iterator_base<impl, base_iterator, std::bidirectional_iterator_tag, uint16_t, const uint16_t&, policies...>
    {
    protected:
        friend utf_iterator_access;

        using base_type = typename utf32_to_utf16_iterator_impl::utf32_to_utf16_iterator_base;
        using base_type::base_type;
        using typename base_type::error_policy;
        using typename base_type::from_policy;
        using typename base_type::to_policy;
        
        void decrement()
        {
            if ((this->index_ & 1) == 0) {
                this->size_ = base_type::read_prev(this->pos_, this->current_, error_policy(), from_policy(), to_policy());
                this->index_ = this->size_;
            }
            --this->index_;
        }
    };

    template <typename base_iterator, typename... policies>
    class utf32_to_utf16_iterator :
        public utf32_to_utf16_iterator_impl<
        utf32_to_utf16_iterator<base_iterator, policies...>,
        base_iterator,
        typename std::iterator_traits<base_iterator>::iterator_category,
        void, policies...
        >
    {

        using impl_type = typename utf32_to_utf16_iterator::utf32_to_utf16_iterator_impl;
    public:
        utf32_to_utf16_iterator() {};
        utf32_to_utf16_iterator(base_iterator pos) : impl_type(pos) {};

    };

    template <typename... policies, typename base_iterator>
    auto make_utf32_to_utf16_iterator(base_iterator iter)
    {
        return utf32_to_utf16_iterator<base_iterator, policies...>{iter};
    }
}


 

namespace utf
{
    template <typename T,typename base_iterator>
    class bytes_to_type_iterator;

    /*** common iterator implementation */
    template <typename T,typename impl, typename base_iterator, typename impl_catagory, typename value_type, typename reference_type>
    struct bytes_to_type_iterator_base :
        utf_iterator< impl, base_iterator, impl_catagory, value_type, reference_type>
    {
    protected:

        using base_type = typename bytes_to_type_iterator_base::utf_iterator;

        friend utf_iterator_access;

        bytes_to_type_iterator_base() {};

        bytes_to_type_iterator_base(base_iterator pos, const base_iterator end) :
            pos_(pos),
            end_(end),
            current_(0),
            pending_(true)
        {

        };


        typename base_type::reference dereference() const
        {
            if (pending_) {
                base_iterator pos(pos_);
                read_next(pos, end_);
            }

            return current_;
        }

        bool equal(const impl& other) const
        {
            return pos_ == other.pos_;
        }

        void increment()
        {
            for (size_t i = 0; i < sizeof(T); ++i,++pos_) {
                if (pos_ == end_)
                    throw std::runtime_error("invalid byte count, byte count must be divisbile by 2");
            }
            pending_ = true;
        }

        void read_next(base_iterator& pos, base_iterator end) const
        {
            current_ = 0;
            for (size_t i = 0; i < sizeof(T); ++i)
            {
                if (pos == end)
                    throw std::runtime_error("invalid byte count, byte count must be divisbile by 2");
                auto val = static_cast<typename std::make_unsigned<typename base_iterator::value_type>::type>(*pos++);
                current_ |=  val << (8 * i);
            }
        }

        mutable base_iterator pos_;
        mutable bool pending_;
        base_iterator end_;

        mutable T current_;
    };

    template <typename T,typename impl, typename base_iterator, typename base_category,typename enable=void>
    struct bytes_to_type_iterator_impl;

    /*** input iterator implementation */
    template <typename T,typename impl, typename base_iterator, typename base_category>
    struct bytes_to_type_iterator_impl<T,impl, base_iterator, base_category,
        enable_if_category< base_category, is_input_tag >
    > : bytes_to_type_iterator_base<T,impl, base_iterator, std::input_iterator_tag, T, const T&>
    {

    protected:
        using base_type = typename bytes_to_type_iterator_impl::bytes_to_type_iterator_base;
        friend utf_iterator_access;

        bytes_to_type_iterator_impl()
        {}

        bytes_to_type_iterator_impl(base_iterator pos, const base_iterator end) :
            base_type(pos, end)
        {
            if (pos != end)
            {
                this->read_next(this->pos_, this->end_);
                this->pending_ = false;
            } 
        }

        void increment()
        {
            if (this->pos_ != this->end_)
                this->read_next(this->pos_,this->end_);
            else
                this->pending_ = true;
        }

        bool equal(const impl& other) const
        {
            return this->pos_ == other.pos_ && this->pending_ == other.pending_;
        }

        typename base_type::reference dereference() const
        {
            return this->current_;
        }

    };

    /***  forward/input iterator implementation */
    template <typename T,typename impl, typename base_iterator, typename base_category>
    struct bytes_to_type_iterator_impl<T,impl, base_iterator, base_category,
        enable_if_category< base_category, is_forward_tag>>
        : bytes_to_type_iterator_base<T,impl, base_iterator, base_category,T, const T&>
    {
    protected:
        using base_type = typename bytes_to_type_iterator_impl::bytes_to_type_iterator_base;
        using base_type::base_type;
    };

    /*** bidirectional & random access iterator implementation */
    template <typename T,typename impl, typename base_iterator, typename base_category>
    struct bytes_to_type_iterator_impl<T,impl, base_iterator, base_category,
        enable_if_category<base_category, is_bidirectional_tag, is_random_access_tag>
    > : bytes_to_type_iterator_base<T,impl, base_iterator, std::bidirectional_iterator_tag, T, const T&>
    {
    protected:
        friend utf_iterator_access;

        using base_type = typename bytes_to_type_iterator_impl::bytes_to_type_iterator_base;

        bytes_to_type_iterator_impl()
        {}

        bytes_to_type_iterator_impl(base_iterator pos, base_iterator start, base_iterator end) :
            base_type(pos, end),
            start_(start)
        {}

        void decrement()
        {
            this->current_ = 0;
            for (size_t i = 0; i < sizeof(T); ++i, --this->pos_){
                if (this->pos_ == start_)
                    throw std::runtime_error("invalid byte count, byte count must be divisbile by 2");
            }
        }

        base_iterator start_;
    };

    template <typename T,typename base_iterator>
    class bytes_to_type_iterator :
        public bytes_to_type_iterator_impl<T,bytes_to_type_iterator<T,base_iterator>,base_iterator,typename std::iterator_traits<base_iterator>::iterator_category>
    {
        using impl_type = typename bytes_to_type_iterator::bytes_to_type_iterator_impl;

        friend typename impl_type::base_type;

    public:

        bytes_to_type_iterator() {};

        template<typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
            typename bidirectional_or_random_access = enable_if_category<category_type, is_bidirectional_tag, is_random_access_tag>>
        bytes_to_type_iterator(base_iterator pos, base_iterator start, base_iterator end) : impl_type(pos, start, end) {}

        template<typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
            typename input_or_forward = enable_if_category<category_type, is_input_tag, is_forward_tag>>
        bytes_to_type_iterator(base_iterator pos, base_iterator end) : impl_type(pos, end) {}
    };

    template <typename base_iterator>
    using bytes_to_short_iterator = bytes_to_type_iterator<uint16_t, base_iterator>;

    template <typename base_iterator>
    using bytes_to_long_iterator = bytes_to_type_iterator<uint32_t, base_iterator>;

    template<typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_bidirectional_tag, is_random_access_tag> = true>
    auto make_bytes_to_long_iterator(base_iterator start, base_iterator end)
    {
        return bytes_to_long_iterator<base_iterator>(start, start, end);
    }

    template<typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_input_tag, is_forward_tag> = true>
        auto make_bytes_to_long_iterator(base_iterator start, base_iterator end)
    {
        return bytes_to_long_iterator<base_iterator>(start, end);
    }

    template<typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_bidirectional_tag, is_random_access_tag> = true>
        auto make_bytes_to_short_iterator(base_iterator start, base_iterator end)
    {
        return bytes_to_short_iterator<base_iterator>(start, start, end);
    }

    template<typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        enable_if_category_b<category_type, is_input_tag, is_forward_tag> = true>
        auto make_bytes_to_short_iterator(base_iterator start, base_iterator end)
    {
        return bytes_to_short_iterator<base_iterator>(start, end);
    }

}
 

namespace utf
{
    template <typename T>
    struct result_type : type_c<T> {};

    namespace detail
    {

        template <typename T>
        using is_error_policy = bool_c< is_instance_of_template<onerror, T>::value >;

        template <typename T>
        using is_result_type = bool_c< has_begin_end<T>::value && has_value_type<T>::value>;

        template<typename iterator_type,
            typename base_iterator,
            typename iterator_category = typename std::iterator_traits< iterator_type >::iterator_category,
            typename std::enable_if<
                category_any_of<iterator_category, is_forward_tag, is_input_tag> &&
                (sizeof(typename iterator_type::value_type) > sizeof(typename base_iterator::value_type))
            , bool>::type = true>
            std::tuple<iterator_type, iterator_type> make_utf_iterators_for_iterable( base_iterator begin, base_iterator end)
        {
            return std::make_pair(iterator_type{ begin,end }, iterator_type{ end,end });
        }

        template<typename iterator_type,
            typename base_iterator,
            typename iterator_category = typename std::iterator_traits< iterator_type >::iterator_category,
            typename std::enable_if<
                category_any_of<iterator_category, is_bidirectional_tag, is_random_access_tag> &&
                (sizeof(typename iterator_type::value_type) > sizeof(typename base_iterator::value_type))
            , bool>::type = true>
            std::tuple<iterator_type, iterator_type> make_utf_iterators_for_iterable(base_iterator begin, base_iterator end)
        {
            return std::make_pair(iterator_type{ begin,begin,end }, iterator_type{ end,begin,end });
        }

        template<typename iterator_type,
            typename base_iterator,
            typename std::enable_if< (sizeof(typename iterator_type::value_type) < sizeof(typename base_iterator::value_type))
            , bool>::type = true>
            std::tuple<iterator_type, iterator_type> make_utf_iterators_for_iterable(base_iterator begin, base_iterator end)
        {
            return std::make_pair(iterator_type{ begin }, iterator_type{ end });
        }

        template < template <typename...> class iterator_tmpl, typename result_type, typename base_iterator, typename... policy>
        result_type utfn_to_utfm(base_iterator begin, base_iterator end, type_list< policy...>)
        {
            using iterator_type = iterator_tmpl<base_iterator, policy...>;
            iterator_type upos, uend;
            std::tie(upos, uend) = detail::make_utf_iterators_for_iterable<iterator_type>(begin,end);

            result_type result;
            std::copy(upos, uend, std::back_inserter(result));

            return result;
        }

        template < template <typename...> class iterator_tmpl_first, template <typename...> class iterator_tmpl_second, typename result_type, typename base_iterator, typename... policy>
        result_type utfn_to_utfm(base_iterator begin, base_iterator end, type_list< policy...>)
        {
            using error_policy = select_argument_or_default_t< is_error_policy, void, policy...>;

            using iterator_type_first = iterator_tmpl_first<base_iterator, error_policy>;
            iterator_type_first uposf, uendf;
            std::tie(uposf, uendf) = detail::make_utf_iterators_for_iterable<iterator_type_first>(begin, end);

            using iterator_type_second = iterator_tmpl_second<iterator_type_first, error_policy, policy...>;
            iterator_type_second uposs, uends;
            std::tie(uposs, uends) = detail::make_utf_iterators_for_iterable<iterator_type_second>(uposf, uendf);

            result_type result;
            std::copy(uposs, uends, std::back_inserter(result));

            return result;
        }
    }


    template <typename... arguments,typename iterator_type>
    auto utf8_to_utf32(iterator_type begin, iterator_type end)
    {
        using result_type = select_argument_or_default_t<detail::is_result_type, u32string, arguments...>;
        return detail::utfn_to_utfm< utf8_to_utf32_iterator, result_type>(begin,end,type_list<arguments...>());
    }

    template <typename... Ts,typename source_type>
    auto utf8_to_utf32(const source_type& src)
    {
        return utf8_to_utf32<Ts...>(src.begin(), src.end());
    }

    template <typename... arguments, typename iterator_type>
    auto utf32_to_utf8(iterator_type begin, iterator_type end)
    {
        using result_type = select_argument_or_default_t<detail::is_result_type, u8string, arguments...>;
        return detail::utfn_to_utfm< utf32_to_utf8_iterator, result_type>(begin, end, type_list<arguments...>());
    }

    template <typename... Ts, typename source_type>
    auto utf32_to_utf8(const source_type& src)
    {
        return utf32_to_utf8<Ts...>(src.begin(), src.end());
    }

    template <typename... arguments, typename iterator_type>
    auto utf16_to_utf32(iterator_type begin, iterator_type end)
    {
        using result_type = select_argument_or_default_t<detail::is_result_type, u32string, arguments...>;
        return detail::utfn_to_utfm< utf16_to_utf32_iterator, result_type>(begin, end, type_list<arguments...>());
    }

    template <typename... Ts, typename source_type>
    auto utf16_to_utf32(const source_type& src)
    {
        return utf16_to_utf32<Ts...>(src.begin(), src.end());
    }


    template <typename... arguments, typename iterator_type>
    auto utf32_to_utf16(iterator_type begin, iterator_type end)
    {
        using result_type = select_argument_or_default_t<detail::is_result_type, u16string, arguments...>;
        return detail::utfn_to_utfm< utf32_to_utf16_iterator, result_type>(begin, end, type_list<arguments...>());
    }

    template <typename... Ts, typename source_type>
    auto utf32_to_utf16(const source_type& src)
    {
        return utf32_to_utf16<Ts...>(src.begin(), src.end());
    }




    template <typename... arguments, typename base_iterator>
    auto utf8_to_utf16(base_iterator begin, base_iterator end)
    {
        using result_type = select_argument_or_default_t<detail::is_result_type, u16string, arguments...>;
        return detail::utfn_to_utfm< utf8_to_utf32_iterator, utf32_to_utf16_iterator, result_type>(begin, end, type_list<arguments...>());
    }

    template <typename... Ts, typename source_type>
    auto utf8_to_utf16(const source_type& src)
    {
        return utf8_to_utf16<Ts...>(src.begin(), src.end());
    }

    template <typename... arguments, typename base_iterator>
    auto utf16_to_utf8(base_iterator begin, base_iterator end)
    {
        using result_type = select_argument_or_default_t<detail::is_result_type, u8string, arguments...>;
        return detail::utfn_to_utfm< utf16_to_utf32_iterator, utf32_to_utf8_iterator, result_type>(begin, end, type_list<arguments...>());
    }

    template <typename... Ts, typename source_type>
    auto utf16_to_utf8(const source_type& src)
    {
        return utf16_to_utf8<Ts...>(src.begin(), src.end());
    }


    /*** Potential ambiguity with utf16 and utf32 if the utf16 string
     starts with two null characters it would be detected as utf32. */
    template <typename iterator_type>
    bom_type detect_bom(iterator_type pos, iterator_type end)
    {
        size_t byte_count = 0;
        uint8_t bytes[4]{};

        for (; byte_count < 4; ++pos) {
            if (pos == end) break;
            typename iterator_type::value_type value = *pos;
            std::memcpy(bytes + byte_count, &value, sizeof(value));
            byte_count += sizeof(value);
        }

        if (byte_count > 2 && !std::memcmp(bytes, utf8_bom, 3))
            return bom_type::utf8;

        if (byte_count > 3 && !std::memcmp(bytes, utf32_little_endian_bom, 4))
            return bom_type::utf32_little_endian;

        if (byte_count > 3 && !std::memcmp(bytes, utf32_big_endian_bom, 4))
            return bom_type::utf32_big_endian;

        if (byte_count > 1 && !std::memcmp(bytes, utf16_little_endian_bom, 2))
            return bom_type::utf16_little_endian;

        if (byte_count > 1 && !std::memcmp(bytes, utf16_big_endian_bom, 2))
            return bom_type::utf16_big_endian;

        return bom_type::none;
    }



}
