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