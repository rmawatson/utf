/* Copyright (c) 2018, rmawatson@hotmail.com
 *
 * Distributed under the Boost Software License, Version 1.0 (found in the
 * LICENSE file in the root directory of this source tree)
 */
 
#pragma once
#include "utf_base.h"

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

        using error_policy = detail::select_policy_handler<impl, onerror, policies...>;
        using from_policy = detail::select_policy_handler<impl, from, policies...>;
        using to_policy = detail::select_policy_handler<impl, to, policies...>;

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
        detail::enable_if_category< base_category, std::input_iterator_tag >
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
        detail::enable_if_category< base_category, std::forward_iterator_tag>, policies...>
        : utf16_to_utf32_iterator_base<impl, base_iterator, base_category, uint32_t, const uint32_t&, policies...>
    {

    protected:
        using base_type = typename utf16_to_utf32_iterator_impl::utf16_to_utf32_iterator_base;
        using base_type::base_type;

    };

    /*** bidirectional & random access iterator implementation */
    template <typename impl, typename base_iterator, typename base_category, typename... policies>
    struct utf16_to_utf32_iterator_impl<impl, base_iterator, base_category,
        detail::enable_if_category<
        base_category, std::bidirectional_iterator_tag, std::random_access_iterator_tag>
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
            typename bidirectional_or_random_access = detail::enable_if_category<category_type, std::bidirectional_iterator_tag, std::random_access_iterator_tag>>
        utf16_to_utf32_iterator(base_iterator pos, base_iterator start, base_iterator end) : impl_type(pos, start, end) {}

        template<typename category_type=typename std::iterator_traits<base_iterator>::iterator_category,
            typename input_or_forward = detail::enable_if_category<category_type, std::input_iterator_tag, std::forward_iterator_tag>>
        utf16_to_utf32_iterator(base_iterator pos, base_iterator end) : impl_type(pos, end) {}

    };

    template <typename... policies, typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        detail::enable_if_category_b<category_type, std::bidirectional_iterator_tag, std::random_access_iterator_tag> = true>
    auto make_utf16_to_utf32_iterator(base_iterator begin, base_iterator end)
    {
        return utf16_to_utf32_iterator<base_iterator, policies...>{begin, begin, end};
    }

    template <typename... policies, typename base_iterator,
        typename category_type = typename std::iterator_traits<base_iterator>::iterator_category,
        detail::enable_if_category_b<category_type, std::input_iterator_tag, std::forward_iterator_tag> = true>
    auto make_utf16_to_utf32_iterator(base_iterator begin, base_iterator end)
    {
        return utf16_to_utf32_iterator<base_iterator, policies...>{begin, end};
    }

}