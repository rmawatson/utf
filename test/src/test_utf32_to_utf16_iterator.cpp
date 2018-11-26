#include "catch.hpp"
#include "test.h"
#include <utf/utf.h>
#include <string>
#include <iterator>
#include <sstream>
#include <forward_list>
#include <iostream>
TEST_CASE("utf32_to_utf8_iterator" )
{
    utf::u16string u16_data_le  = read_file<char16_t>("data/data_utf16_le.txt");
    utf::u16string u16_data_be  = read_file<char16_t>("data/data_utf16_be.txt");
    
    utf::u32string u32_le_data = read_file<char32_t>("data/data_utf32_le.txt");
    utf::u32string u32_be_data = read_file<char32_t>("data/data_utf32_be.txt");

    /*little endian -> little endian*/
    SECTION( "std::input_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char32_t>;
        
        std::basic_istringstream<char32_t> u32_stringstream( u32_le_data );
        base_iterator base_begin(u32_stringstream);
        base_iterator base_end{};
    
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::little_endian>> u32iter_pos(base_begin);
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::little_endian>> u32iter_end(base_end);
    
        utf::u16string::iterator u16_data_pos = u16_data_le.begin();
        utf::u16string::iterator u16_data_end = u16_data_le.end();
        

        while(u32iter_pos != u32iter_end)
        {

            REQUIRE(u16_data_pos != u16_data_end);

			auto u32_to_u16_iter_value = static_cast<uint16_t>(*u32iter_pos++);
			auto u16_data_le_value = static_cast<uint16_t>(*u16_data_pos++);

            REQUIRE(u32_to_u16_iter_value == u16_data_le_value);
        }
    }
    
    SECTION( "std::forward_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::forward_list<char32_t>::iterator;
        
        std::forward_list<char32_t> u32_fwdlist(u32_le_data.begin(),u32_le_data.end());
        
        base_iterator base_begin = u32_fwdlist.begin();
        base_iterator base_end = u32_fwdlist.end();
    
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::little_endian>> u16iter_pos(base_begin);
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::little_endian>> u16iter_end(base_end);
    
        utf::u16string::iterator u16_data_pos = u16_data_le.begin();
        utf::u16string::iterator u16_data_end = u16_data_le.end();
        
        while(u16iter_pos != u16iter_end)
        {

            REQUIRE(u16_data_pos != u16_data_end);

			auto u32_to_u16_iter_value = static_cast<uint16_t>(*u16iter_pos++);
			auto u16_data_le_value = static_cast<uint16_t>(*u16_data_pos++);

			REQUIRE(u32_to_u16_iter_value == u16_data_le_value);
        }
    }
    
    SECTION( "std::bidirectional_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = utf::u32string::iterator;
        
        base_iterator base_begin = u32_le_data.begin();
        base_iterator base_end = u32_le_data.end();
    
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::little_endian>> u16iter_pos(base_begin);
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::little_endian>> u16iter_end(base_end);
    
        auto u16iter_rpos = std::make_reverse_iterator(u16iter_end);
        auto u16iter_rend = std::make_reverse_iterator(u16iter_pos);
    
        utf::u16string::iterator u16_data_pos = u16_data_le.begin();
        utf::u16string::iterator u16_data_end = u16_data_le.end();
        
        auto u16_data_rpos = std::make_reverse_iterator(u16_data_end);
        auto u16_data_rend = std::make_reverse_iterator(u16_data_pos);
    
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u16_data_pos != u16_data_end);

			auto u32_to_u16_iter_value = static_cast<uint16_t>(*u16iter_pos++);
			auto u16_data_le_value = static_cast<uint16_t>(*u16_data_pos++);

			REQUIRE(u32_to_u16_iter_value == u16_data_le_value);
        }
        
        while(u16iter_rpos != u16iter_rend)
        {
            REQUIRE(u16_data_rpos != u16_data_rend);

			auto u32_to_u16_riter_value = static_cast<uint16_t>(*u16iter_rpos++);
			auto u16_rdata_le_value = static_cast<uint16_t>(*u16_data_rpos++);

			REQUIRE(u32_to_u16_riter_value == u16_rdata_le_value);
        }
    }
    
    
    /*little endian -> big endian*/
    SECTION( "std::input_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char32_t>;
        
        std::basic_istringstream<char32_t> u32_stringstream( u32_le_data );
        base_iterator base_begin(u32_stringstream);
        base_iterator base_end{};
    
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::big_endian>> u16iter_pos(base_begin);
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::big_endian>> u16iter_end(base_end);
    
        utf::u16string::iterator u16_data_pos = u16_data_be.begin();
        utf::u16string::iterator u16_data_end = u16_data_be.end();
        

        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u16_data_pos != u16_data_end);

			auto u32_to_u16_iter_value = static_cast<uint16_t>(*u16iter_pos++);
			auto u16_data_be_value = static_cast<uint16_t>(*u16_data_pos++);

			REQUIRE(u32_to_u16_iter_value == u16_data_be_value);
        }
    }
    
    SECTION( "std::forward_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::forward_list<char32_t>::iterator;
        
        std::forward_list<char32_t> u32_fwdlist(u32_le_data.begin(),u32_le_data.end());
        
        base_iterator base_begin = u32_fwdlist.begin();
        base_iterator base_end = u32_fwdlist.end();
    
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::big_endian>> u16iter_pos(base_begin);
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::big_endian>> u16iter_end(base_end);
    
        utf::u16string::iterator u16_data_pos = u16_data_be.begin();
        utf::u16string::iterator u16_data_end = u16_data_be.end();
        
        while(u16iter_pos != u16iter_end)
        {

            REQUIRE(u16_data_pos != u16_data_end);

			auto u32_to_u16_iter_value = static_cast<uint16_t>(*u16iter_pos++);
			auto u16_data_be_value = static_cast<uint16_t>(*u16_data_pos++);

			REQUIRE(u32_to_u16_iter_value == u16_data_be_value);
        }
    }
    
    SECTION( "std::bidirectional_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = utf::u32string::iterator;
        
        base_iterator base_begin = u32_le_data.begin();
        base_iterator base_end = u32_le_data.end();
    
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::big_endian>> u16iter_pos(base_begin);
        utf32_to_utf16_iterator<base_iterator,utf::from<utf::little_endian>,utf::to<utf::big_endian>> u16iter_end(base_end);
    
        auto u16iter_rpos = std::make_reverse_iterator(u16iter_end);
        auto u16iter_rend = std::make_reverse_iterator(u16iter_pos);
    
        utf::u16string::iterator u16_data_pos = u16_data_be.begin();
        utf::u16string::iterator u16_data_end = u16_data_be.end();
        
        auto u16_data_rpos = std::make_reverse_iterator(u16_data_end);
        auto u16_data_rend = std::make_reverse_iterator(u16_data_pos);
    
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u16_data_pos != u16_data_end);

			auto u32_to_u16_iter_value = static_cast<uint16_t>(*u16iter_pos++);
			auto u16_data_be_value = static_cast<uint16_t>(*u16_data_pos++);

			REQUIRE(u32_to_u16_iter_value == u16_data_be_value);
        }
        
        while(u16iter_rpos != u16iter_rend)
        {
            REQUIRE(u16_data_rpos != u16_data_rend);

			auto u32_to_u16_riter_value = static_cast<uint16_t>(*u16iter_rpos++);
			auto u16_rdata_be_value = static_cast<uint16_t>(*u16_data_rpos++);

			REQUIRE(u32_to_u16_riter_value == u16_rdata_be_value);
        }
    }
    
}