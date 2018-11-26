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
    utf::u8string u8_data     = read_file<char>("data/data_utf8.txt");

    
    utf::u32string u32_le_data = read_file<char32_t>("data/data_utf32_le.txt");
    utf::u32string u32_be_data = read_file<char32_t>("data/data_utf32_be.txt");

    /*little endian*/
    SECTION( "std::input_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char32_t>;
        
        std::basic_istringstream<char32_t> u32_stringstream( u32_le_data );
        base_iterator base_begin(u32_stringstream);
        base_iterator base_end{};
    
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::little_endian>> u32iter_pos(base_begin);
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::little_endian>> u32iter_end(base_end);
    
        utf::u8string::iterator u8_data_pos = u8_data.begin();
        utf::u8string::iterator u8_data_end = u8_data.end();
        

        while(u32iter_pos != u32iter_end)
        {

            REQUIRE(u8_data_pos != u8_data_end);

			auto u32_to_u8_iter_value = static_cast<uint8_t>(*u32iter_pos++);
			auto u8_data_value = static_cast<uint8_t>(*u8_data_pos++);

            REQUIRE(u32_to_u8_iter_value == u8_data_value);
        }
    }
    
    SECTION( "std::forward_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::forward_list<char32_t>::iterator;
        
        std::forward_list<char32_t> u32_fwdlist(u32_le_data.begin(),u32_le_data.end());
        
        base_iterator base_begin = u32_fwdlist.begin();
        base_iterator base_end = u32_fwdlist.end();
    
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::little_endian>> u32iter_pos(base_begin);
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::little_endian>> u32iter_end(base_end);
    
        utf::u8string::iterator u8_data_pos = u8_data.begin();
        utf::u8string::iterator u8_data_end = u8_data.end();
        
        while(u32iter_pos != u32iter_end)
        {
            REQUIRE(u8_data_pos != u8_data_end);

			auto u32_to_u8_iter_value = static_cast<uint8_t>(*u32iter_pos++);
			auto u8_data_value = static_cast<uint8_t>(*u8_data_pos++);

			REQUIRE(u32_to_u8_iter_value == u8_data_value);
        }
    }
    
    SECTION( "std::bidirectional_iterator_tag from little_endian input" ) {
    
        using namespace utf;
        using base_iterator = utf::u32string::iterator;
        
        base_iterator base_begin = u32_le_data.begin();
        base_iterator base_end = u32_le_data.end();
    
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::little_endian>> u32iter_pos(base_begin);
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::little_endian>> u32iter_end(base_end);
    
        auto u32iter_rpos = std::make_reverse_iterator(u32iter_end);
        auto u32iter_rend = std::make_reverse_iterator(u32iter_pos);
    
        utf::u8string::iterator u8_data_pos = u8_data.begin();
        utf::u8string::iterator u8_data_end = u8_data.end();
        
        auto u8_data_rpos = std::make_reverse_iterator(u8_data_end);
        auto u8_data_rend = std::make_reverse_iterator(u8_data_pos);
    
        while(u32iter_pos != u32iter_end)
        {
            REQUIRE(u8_data_pos != u8_data_end);

			auto u32_to_u8_iter_value = static_cast<uint8_t>(*u32iter_pos++);
			auto u8_data_value = static_cast<uint8_t>(*u8_data_pos++);

			REQUIRE(u32_to_u8_iter_value == u8_data_value);
        }
        
        while(u32iter_rpos != u32iter_rend)
        {
            REQUIRE(u8_data_rpos != u8_data_rend);

			auto u32_to_u8_riter_value = static_cast<uint8_t>(*u32iter_rpos++);
			auto u8_rdata_value = static_cast<uint8_t>(*u8_data_rpos++);

			REQUIRE(u32_to_u8_riter_value == u8_rdata_value);
        }
        
    }
    
    /*big endian*/
    SECTION( "std::input_iterator_tag from big_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char32_t>;
        
        std::basic_istringstream<char32_t> u32_stringstream( u32_be_data );
        base_iterator base_begin(u32_stringstream);
        base_iterator base_end{};
    
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::big_endian>> u32iter_pos(base_begin);
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::big_endian>> u32iter_end(base_end);
    
        utf::u8string::iterator u8_data_pos = u8_data.begin();
        utf::u8string::iterator u8_data_end = u8_data.end();
        

        while(u32iter_pos != u32iter_end)
        {

            REQUIRE(u8_data_pos != u8_data_end);

			auto u32_to_u8_iter_value = static_cast<uint8_t>(*u32iter_pos++);
			auto u8_data_value = static_cast<uint8_t>(*u8_data_pos++);

			REQUIRE(u32_to_u8_iter_value == u8_data_value);
        }
    }
    
    SECTION( "std::forward_iterator_tag from big_endian input" ) {
    
        using namespace utf;
        using base_iterator = std::forward_list<char32_t>::iterator;
        
        std::forward_list<char32_t> u32_fwdlist(u32_be_data.begin(),u32_be_data.end());
        
        base_iterator base_begin = u32_fwdlist.begin();
        base_iterator base_end = u32_fwdlist.end();
    
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::big_endian>> u32iter_pos(base_begin);
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::big_endian>> u32iter_end(base_end);
    
        utf::u8string::iterator u8_data_pos = u8_data.begin();
        utf::u8string::iterator u8_data_end = u8_data.end();
        
        while(u32iter_pos != u32iter_end)
        {
            REQUIRE(u8_data_pos != u8_data_end);

			auto u32_to_u8_iter_value = static_cast<uint8_t>(*u32iter_pos++);
			auto u8_data_value = static_cast<uint8_t>(*u8_data_pos++);

			REQUIRE(u32_to_u8_iter_value == u8_data_value);
        }
    }
    
    SECTION( "std::bidirectional_iterator_tag from big_endian input" ) {
    
        using namespace utf;
        using base_iterator = utf::u32string::iterator;
        
        base_iterator base_begin = u32_be_data.begin();
        base_iterator base_end = u32_be_data.end();
    
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::big_endian>> u32iter_pos(base_begin);
        utf32_to_utf8_iterator<base_iterator,utf::from<utf::big_endian>> u32iter_end(base_end);
    
        auto u32iter_rpos = std::make_reverse_iterator(u32iter_end);
        auto u32iter_rend = std::make_reverse_iterator(u32iter_pos);
    
        utf::u8string::iterator u8_data_pos = u8_data.begin();
        utf::u8string::iterator u8_data_end = u8_data.end();
        
        auto u8_data_rpos = std::make_reverse_iterator(u8_data_end);
        auto u8_data_rend = std::make_reverse_iterator(u8_data_pos);
    
        while(u32iter_pos != u32iter_end)
        {
            REQUIRE(u8_data_pos != u8_data_end);

			auto u32_to_u8_iter_value = static_cast<uint8_t>(*u32iter_pos++);
			auto u8_data_value = static_cast<uint8_t>(*u8_data_pos++);

			REQUIRE(u32_to_u8_iter_value == u8_data_value);
        }
        
        while(u32iter_rpos != u32iter_rend)
        {
            REQUIRE(u8_data_rpos != u8_data_rend);

			auto u32_to_u8_riter_value = static_cast<uint8_t>(*u32iter_rpos++);
			auto u8_rdata_value = static_cast<uint8_t>(*u8_data_rpos++);

			REQUIRE(u32_to_u8_riter_value == u8_rdata_value);
        }
        
    }

}