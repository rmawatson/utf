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