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

