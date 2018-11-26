/* Copyright (c) 2018, rmawatson@hotmail.com
 * 
 * Distributed under the Boost Software License, Version 1.0 (found in the
 * LICENSE file in the root directory of this source tree)
 */

#pragma once
#include  <algorithm>
#include "utf_base.h"

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
