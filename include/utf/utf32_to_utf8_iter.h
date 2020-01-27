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

        using error_policy = detail::select_policy_handler<impl, onerror, policies...>;
        using from_policy = detail::select_policy_handler<impl, from, policies...>;

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
        detail::enable_if_category< typename std::iterator_traits<base_iterator>::iterator_category, std::input_iterator_tag>
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
        detail::enable_if_category< typename std::iterator_traits<base_iterator>::iterator_category, std::forward_iterator_tag>
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
        detail::enable_if_category<  typename std::iterator_traits<base_iterator>::iterator_category, std::bidirectional_iterator_tag, std::random_access_iterator_tag>
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

