#include "catch.hpp"
#include "test.h"
#include <utf/utf.h>
#include <string>
#include <iterator>
#include <sstream>
#include <forward_list>

TEST_CASE("utf8_to_utf32_iterator" )
{
    utf::u8string u8data     = read_file<char>("data/data_utf8.txt");

    
    utf::u32string u32_le_data = read_file<char32_t>("data/data_utf32_le.txt");
    utf::u32string u32_be_data = read_file<char32_t>("data/data_utf32_be.txt");

    /*little endian*/
    SECTION( "std::input_iterator_tag with little_endian output" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char>;
        
        std::istringstream u8_stringstream( u8data );
        base_iterator base_begin(u8_stringstream);
        base_iterator base_end{};
    
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_pos(base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();
        
        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);

			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_data_le_value = static_cast<uint32_t>(*u32_le_data_pos++);

            REQUIRE(u8_to_u32_iter_value == u32_data_le_value);
        }
    
    }
    SECTION( "std::forward_iterator_tag with little_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = std::forward_list<char>::iterator;
        
        std::forward_list<char> u8_fwd_list( u8data.begin(),u8data.end());
        
        base_iterator base_begin = u8_fwd_list.begin();
        base_iterator base_end   = u8_fwd_list.end();
    
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_pos(base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();
        
        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);

			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_data_le_value = static_cast<uint32_t>(*u32_le_data_pos++);

            REQUIRE(u8_to_u32_iter_value == u32_data_le_value);
        }
    
    }
    SECTION( "std::bidirectional_iterator_tag with little_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = utf::u8string::iterator;
        
        base_iterator base_begin = u8data.begin();
        base_iterator base_end   = u8data.end();
    
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_pos(base_begin,base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_end(base_end,base_begin,base_end);
    
        auto u8iter_rpos = std::make_reverse_iterator(u8iter_end);
        auto u8iter_rend = std::make_reverse_iterator(u8iter_pos);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();
        
        auto u32_le_data_rpos = std::make_reverse_iterator(u32_le_data_end);
        auto u32_le_data_rend = std::make_reverse_iterator(u32_le_data_pos);
        
        
        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);

			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_data_le_value = static_cast<uint32_t>(*u32_le_data_pos++);

			REQUIRE(u8_to_u32_iter_value == u32_data_le_value);
        }
    
        while(u8iter_rpos != u8iter_rend )
        {
            REQUIRE(u32_le_data_rpos != u32_le_data_rend);

			auto u8_to_u32_riter_value = static_cast<uint32_t>(*u8iter_rpos++);
			auto u32_rdata_le_value = static_cast<uint32_t>(*u32_le_data_rpos++);

			REQUIRE(u8_to_u32_riter_value == u32_rdata_le_value);
        }

    }
    /* big endian */
    SECTION( "iterate forward over underlying std::input_iterator_tag iterator with big_endian output" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char>;
        
        std::istringstream u8_stringstream( u8data );
        base_iterator base_begin(u8_stringstream);
        base_iterator base_end{};
    
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u8iter_pos(base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u8iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_be_data_pos = u32_be_data.begin();
        utf::u32string::iterator u32_be_data_end = u32_be_data.end();
        
        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32_be_data_pos != u32_be_data_end);

			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_data_be_value = static_cast<uint32_t>(*u32_be_data_pos++);

			REQUIRE(u8_to_u32_iter_value == u32_data_be_value);
        }
    
    }
    SECTION( "iterate forward over underlying std::forward_iterator_tag iterator with big_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = std::forward_list<char>::iterator;
        
        std::forward_list<char> u8_fwd_list( u8data.begin(),u8data.end());
        
        base_iterator base_begin = u8_fwd_list.begin();
        base_iterator base_end   = u8_fwd_list.end();
    
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u8iter_pos(base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u8iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_be_data_pos = u32_be_data.begin();
        utf::u32string::iterator u32_be_data_end = u32_be_data.end();
        
        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32_be_data_pos != u32_be_data_end);
			
			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_data_be_value = static_cast<uint32_t>(*u32_be_data_pos++);

			REQUIRE(u8_to_u32_iter_value == u32_data_be_value);
        }
    
    }
    SECTION("iterate forward and reverse over underlying std::bidirectional_iterator_tag iterator with big_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = utf::u8string::iterator;
        
        base_iterator base_begin = u8data.begin();
        base_iterator base_end   = u8data.end();
    
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u8iter_pos(base_begin,base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u8iter_end(base_end,base_begin,base_end);
    
        auto u8iter_rpos = std::make_reverse_iterator(u8iter_end);
        auto u8iter_rend = std::make_reverse_iterator(u8iter_pos);
    
        utf::u32string::iterator u32_be_data_pos = u32_be_data.begin();
        utf::u32string::iterator u32_be_data_end = u32_be_data.end();
        
        auto u32_be_data_rpos = std::make_reverse_iterator(u32_be_data_end);
        auto u32_be_data_rend = std::make_reverse_iterator(u32_be_data_pos);
        
        
        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32_be_data_pos != u32_be_data_end);

			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_data_be_value = static_cast<uint32_t>(*u32_be_data_pos++);

			REQUIRE(u8_to_u32_iter_value == u32_data_be_value);
        }
    
        while(u8iter_rpos != u8iter_rend )
        {
            REQUIRE(u32_be_data_rpos != u32_be_data_rend);

			auto u8_to_u32_riter_value = static_cast<uint32_t>(*u8iter_rpos++);
			auto u32_rdata_be_value = static_cast<uint32_t>(*u32_be_data_rpos++);

			REQUIRE(u8_to_u32_riter_value == u32_rdata_be_value);
        }
    }
    
    SECTION( "detect utf8 data with bom" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char>;
        
        std::ifstream u8data_bom("data/data_utf8_bom.txt",std::ios::binary);
        std::noskipws(u8data_bom);
        
        base_iterator base_begin{ u8data_bom };
        base_iterator base_end{};
        
        utf::bom_type bom = utf::detect_bom(base_begin,base_end);
        
        REQUIRE(bom == utf::bom_type::utf8);
        
        u8data_bom.clear();
        u8data_bom.seekg(0, std::ios::beg);

        std::advance(base_begin, sizeof(utf::utf8_bom));
        
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_pos(base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u8iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();

        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);

			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_data_le_value = static_cast<uint32_t>(*u32_le_data_pos++);

			REQUIRE(u8_to_u32_iter_value == u32_data_le_value);
        }

    }
  
    
    SECTION( "replace bad data" ) {
    
        using namespace utf;
        
        using base_iterator = utf::u8string::iterator;
        utf::u8string u8data_bad        = read_file<char>("data/data_utf8_bad.txt");
        utf::u32string u32data_replaced = read_file<char32_t>("data/data_utf32_utf8_bad_replaced.txt");

        base_iterator base_begin = u8data_bad.begin();
        base_iterator base_end   = u8data_bad.end();
    
        auto u32data_replaced_iter = u32data_replaced.begin();
        auto u32data_replaced_end = u32data_replaced.end();
    
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>,utf::onerror<utf::replace_with_fffd>> u8iter_pos(base_begin,base_begin,base_end);
        utf8_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>,utf::onerror<utf::replace_with_fffd>> u8iter_end(base_end,base_begin,base_end);
        
        while(u8iter_pos != u8iter_end)
        {
            REQUIRE(u32data_replaced_iter != u32data_replaced_end);


			auto u8_to_u32_iter_value = static_cast<uint32_t>(*u8iter_pos++);
			auto u32_replaced_data_le_value = static_cast<uint32_t>(*u32data_replaced_iter++);

			REQUIRE(u8_to_u32_iter_value == u32_replaced_data_le_value);

        }

    }
    
 
}