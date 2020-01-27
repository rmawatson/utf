/* Copyright (c) 2018, rmawatson@hotmail.com
 *
 * Distributed under the Boost Software License, Version 1.0 (found in the
 * LICENSE file in the root directory of this source tree)
 */
 
#pragma once
#include <cstring>
#include <tuple>
#include "utf_base.h"


namespace utf
{
    template <typename T>
    struct result_type : mp::identity<T> {};

    namespace detail
    {

        template <typename T, typename = void>
        struct has_push_back : mp::bool_c<false>
        {};


        template <typename T>
        struct has_push_back<T, mp::void_t<decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()))>>
            : mp::bool_c<true>{};


        template <typename T>
        using is_error_policy = mp::bool_c< is_instance_of_template<onerror, T>::value >;

        template <typename T>
        using is_result_type = mp::bool_c< has_begin_end<T>::value && has_value_type<T>::value>;


        struct _utf8;
        struct _utf16;
        struct _utf32;

        template <typename,typename>
        struct utf_conversion_traits;

        template <>
        struct utf_conversion_traits<_utf8, _utf16>
        {
            using iterators = mp::list<
                mp::wrap<utf8_to_utf32_iterator>,
                mp::wrap<utf32_to_utf16_iterator>>;
            using string_type = std::u16string;
        };

        template <>
        struct utf_conversion_traits<_utf8, _utf32>
        {
            using iterators = mp::list<mp::wrap<utf8_to_utf32_iterator>>;
            using string_type = std::u32string;
        };

        template <>
        struct utf_conversion_traits<_utf16, _utf8>
        {
            using iterators = mp::list<
                mp::wrap<utf16_to_utf32_iterator>,
                mp::wrap<utf32_to_utf8_iterator>>;
            using string_type = std::string;
        };

        template <>
        struct utf_conversion_traits<_utf16, _utf32>
        {
            using iterators = mp::list<mp::wrap<utf16_to_utf32_iterator>>;
            using string_type = std::u32string;
        };

        template <>
        struct utf_conversion_traits<_utf32, _utf8>
        {
            using iterators = mp::list<mp::wrap<utf32_to_utf8_iterator>>;
            using string_type = std::string;
        };

        template <>
        struct utf_conversion_traits<_utf32, _utf16>
        {
            using iterators = mp::list<mp::wrap<utf32_to_utf16_iterator>>;
            using string_type = std::u16string;
        };


        template<typename iterator_type,
            typename base_iterator,
            typename iterator_value_type = typename std::iterator_traits< iterator_type >::value_type,
            typename base_iterator_value_type = typename std::iterator_traits< base_iterator>::value_type,
            typename iterator_category = typename std::iterator_traits< iterator_type >::iterator_category,
            typename std::enable_if<
            category_any_of<iterator_category, std::forward_iterator_tag, std::input_iterator_tag>::value &&
            (sizeof(iterator_value_type) > sizeof(base_iterator_value_type))
            , bool>::type = true>
            std::tuple<iterator_type, iterator_type> make_utf_iterators_for_iterable(base_iterator begin, base_iterator end)
        {
            return std::make_pair(iterator_type{ begin,end }, iterator_type{ end,end });
        }

        template<typename iterator_type,
            typename base_iterator,
            typename iterator_value_type = typename std::iterator_traits< iterator_type >::value_type,
            typename base_iterator_value_type = typename std::iterator_traits< base_iterator>::value_type,
            typename iterator_category = typename std::iterator_traits< iterator_type >::iterator_category,
            typename std::enable_if<
            category_any_of<iterator_category, std::bidirectional_iterator_tag, std::random_access_iterator_tag>::value &&
            (sizeof(iterator_value_type) > sizeof(base_iterator_value_type))
            , bool>::type = true>
            std::tuple<iterator_type, iterator_type> make_utf_iterators_for_iterable(base_iterator begin, base_iterator end)
        {
            return std::make_pair(iterator_type{ begin,begin,end }, iterator_type{ end,begin,end });
        }

        template<typename iterator_type,
            typename base_iterator,
            typename iterator_value_type = typename std::iterator_traits< iterator_type >::value_type,
            typename base_iterator_value_type = typename std::iterator_traits< base_iterator>::value_type,
            typename std::enable_if< (sizeof(iterator_value_type) < sizeof(base_iterator_value_type))
            , bool>::type = true>
            std::tuple<iterator_type, iterator_type> make_utf_iterators_for_iterable(base_iterator begin, base_iterator end)
        {
            return std::make_pair(iterator_type{ begin }, iterator_type{ end });
        }

        template<typename result_type,typename iterator_type,
            typename std::enable_if_t<has_push_back<result_type>::value,int> = 0>
        result_type apply_iterators(iterator_type begin, iterator_type end)
        {
            result_type result;
            std::copy(begin, end, std::back_inserter(result));
            return result;
        }

        template<typename result_type, typename iterator_type,
            typename std::enable_if_t<!has_push_back<result_type>::value && has_begin_end<result_type>::value, int> = 0>
            result_type apply_iterators(iterator_type iter, iterator_type end)
        {
            result_type result;
            
            auto riter = result.begin();
            auto rend = result.end();
            while (iter != end || riter != rend)
                *riter++ = *iter++;

            return result;
        }

        template<typename result_type, typename iterator_type,
            typename std::enable_if_t<std::is_constructible<result_type,typename iterator_type::value_type>::value, int> = 0>
            result_type apply_iterators(iterator_type iter, iterator_type end)
        {
            return (iter != end) ? result_type(*iter) : result_type{};
        }

        template <  typename result_type, template <typename...> class iterator_tmpl,typename base_iterator, typename... policy>
        result_type utf_convert_impl(base_iterator begin, base_iterator end, mp::list<mp::wrap<iterator_tmpl>>, mp::list< policy...>)
        {
            using iterator_type = iterator_tmpl<base_iterator, policy...>;
            iterator_type upos, uend;
            std::tie(upos, uend) = make_utf_iterators_for_iterable<iterator_type>(begin, end);
            return apply_iterators<result_type>(upos, uend);
        }

        template < typename result_type, template <typename...> class iterator_tmpl_first, template <typename...> class iterator_tmpl_second, typename base_iterator, typename... policy>
        result_type utf_convert_impl(base_iterator begin, base_iterator end, mp::list<mp::wrap<iterator_tmpl_first>,mp::wrap<iterator_tmpl_second>>  ,mp::list< policy...>)
        {
            using error_policy = select_argument_or_default< is_error_policy, void, policy...>;

            using iterator_type_first = iterator_tmpl_first<base_iterator, error_policy>;
            iterator_type_first uposf, uendf;
            std::tie(uposf, uendf) = make_utf_iterators_for_iterable<iterator_type_first>(begin, end);

            using iterator_type_second = iterator_tmpl_second<iterator_type_first, error_policy, policy...>;
            iterator_type_second uposs, uends;
            std::tie(uposs, uends) = make_utf_iterators_for_iterable<iterator_type_second>(uposf, uendf);

            return apply_iterators<result_type>(uposs, uends);
        }

        template <typename F, typename T, typename... args, typename iter_type>
        auto  utf_convert(iter_type begin, iter_type end)
        {
            using traits = utf_conversion_traits<F,T>;

            using result_type = select_argument_or_default<
                is_result_type,
                typename traits::string_type,
                args...>;

            return utf_convert_impl<result_type>(begin, end,
                traits::iterators(),
                mp::list<args...>());
        }

        template <typename F, typename T, typename... args, typename source_type,
            typename std::enable_if<is_char_type<source_type>::value, int>::type = 0>
            auto utf_convert(source_type source)
        {
            using traits = utf_conversion_traits<F,T>;
            using result_type = select_argument_or_default<
                is_result_type,
                typename traits::string_type::value_type,
                args...>;

            return utf_convert_impl<result_type>(&source, &source + 1,
                traits::iterators(),
                mp::list<args...>());
        }

        template <typename F, typename T, typename... args, typename source_type,
            typename std::enable_if<has_begin_end<source_type>::value, int>::type = 0>
            auto  utf_convert(const source_type& source)
        {
            return utf_convert<F, T, args...>(source.begin(), source.end());
        }

        template <typename F, typename T, typename... args, typename source_type,
            typename std::enable_if<is_char_type<source_type>::value, int>::type = 0>
            auto  utf_convert(const source_type* source)
        {
            return utf_convert<F, T, args...>(source, source + std::char_traits<source_type>::length(source));

        }
    }

    template <typename...Ts, typename iter_type>
    auto utf8_to_utf32(iter_type first, iter_type second)
    {
        return detail::utf_convert<detail::_utf8, detail::_utf32, Ts...>(
            first,
            second);
    }
    template <typename...Ts, typename source_type>
    auto utf8_to_utf32(source_type&& source)
    {
        return detail::utf_convert<detail::_utf8, detail::_utf32, Ts...>(
            std::forward<source_type>(source));
    }

    template <typename...Ts, typename iter_type>
    auto utf8_to_utf16(iter_type first, iter_type second)
    {
        return detail::utf_convert<detail::_utf8, detail::_utf16, Ts...>(
            first,
            second);
    }

    template <typename...Ts, typename source_type>
    auto utf8_to_utf16(source_type&& source)
    {
        return detail::utf_convert<detail::_utf8, detail::_utf16, Ts...>(
            std::forward<source_type>(source));
    }

    template <typename...Ts, typename iter_type>
    auto utf16_to_utf8(iter_type first, iter_type second)
    {
        return detail::utf_convert<detail::_utf16, detail::_utf8, Ts...>(
            first,
            second);
    }

    template <typename...Ts, typename source_type>
    auto utf16_to_utf8(source_type&& source)
    {
        return detail::utf_convert<detail::_utf16, detail::_utf8, Ts...>(
            std::forward<source_type>(source));
    }

    template <typename...Ts, typename iter_type>
    auto utf16_to_utf32(iter_type first, iter_type second)
    {
        return detail::utf_convert<detail::_utf16, detail::_utf32, Ts...>(
            first,
            second);
    }

    template <typename...Ts, typename source_type>
    auto utf16_to_utf32(source_type&& source)
    {
        return detail::utf_convert<detail::_utf16, detail::_utf32, Ts...>(
            std::forward<source_type>(source));
    }

    template <typename...Ts, typename iter_type>
    auto utf32_to_utf16(iter_type first, iter_type second)
    {
        return detail::utf_convert<detail::_utf32, detail::_utf16, Ts...>(
            first,
            second);
    }

    template <typename...Ts, typename source_type>
    auto utf32_to_utf16(source_type&& source)
    {
        return detail::utf_convert<detail::_utf32, detail::_utf16, Ts...>(
            std::forward<source_type>(source));
    }

    template <typename...Ts, typename iter_type>
    auto utf32_to_utf8(iter_type first, iter_type second)
    {
        return detail::utf_convert<detail::_utf32, detail::_utf8, Ts...>(
            first,
            second);
    }

    template <typename...Ts, typename source_type>
    auto utf32_to_utf8(source_type&& source)
    {
        return detail::utf_convert<detail::_utf32, detail::_utf8, Ts...>(
            std::forward<source_type>(source));
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
            typename std::iterator_traits<iterator_type>::value_type value = *pos;
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