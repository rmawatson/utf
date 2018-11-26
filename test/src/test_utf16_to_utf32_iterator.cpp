#include "catch.hpp"
#include "test.h"
#include <utf/utf.h>
#include <string>
#include <iterator>
#include <sstream>
#include <forward_list>

TEST_CASE("utf16_to_utf32_iterator" )
{
    utf::u16string u16data     = read_file<char16_t>("data/data_utf16_le.txt");

    
    utf::u32string u32_le_data = read_file<char32_t>("data/data_utf32_le.txt");
    utf::u32string u32_be_data = read_file<char32_t>("data/data_utf32_be.txt");

    /*little endian*/
    SECTION( "std::input_iterator_tag with little_endian output" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char16_t>;
        
        std::basic_istringstream<char16_t> u16_stringstream( u16data );
        
        base_iterator base_begin(u16_stringstream);
        base_iterator base_end{};
    
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u16iter_pos(base_begin,base_end);
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u16iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();
        
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);

			auto u16_to_u32_iter_value = static_cast<uint32_t>(*u16iter_pos++);
			auto u32_le_data_value = static_cast<uint32_t>(*u32_le_data_pos++);

			REQUIRE(u16_to_u32_iter_value == u32_le_data_value);
        }
    
    }
    SECTION( "std::forward_iterator_tag with little_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = std::forward_list<char16_t>::iterator;
        
        std::forward_list<char16_t> u16_fwd_list( u16data.begin(),u16data.end());
        
        base_iterator base_begin = u16_fwd_list.begin();
        base_iterator base_end   = u16_fwd_list.end();
    
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u16iter_pos(base_begin,base_end);
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u16iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();
        
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);

			auto u16_to_u32_iter_value = static_cast<uint32_t>(*u16iter_pos++);
			auto u32_le_data_value = static_cast<uint32_t>(*u32_le_data_pos++);

			REQUIRE(u16_to_u32_iter_value == u32_le_data_value);

        }
    
    }
    SECTION( "std::bidirectional_iterator_tag with little_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = utf::u16string::iterator;
        
        base_iterator base_begin = u16data.begin();
        base_iterator base_end   = u16data.end();
    
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u16iter_pos(base_begin,base_begin,base_end);
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>> u16iter_end(base_end,base_begin,base_end);
    
        auto u16iter_rpos = std::make_reverse_iterator(u16iter_end);
        auto u16iter_rend = std::make_reverse_iterator(u16iter_pos);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();
        
        auto u32_le_data_rpos = std::make_reverse_iterator(u32_le_data_end);
        auto u32_le_data_rend = std::make_reverse_iterator(u32_le_data_pos);
        
        
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);


			auto u16_to_u32_iter_value = static_cast<uint32_t>(*u16iter_pos++);
			auto u32_le_data_value = static_cast<uint32_t>(*u32_le_data_pos++);


            REQUIRE(u16_to_u32_iter_value == u32_le_data_value);
        }
    
        while(u16iter_rpos != u16iter_rend )
        {
            REQUIRE(u32_le_data_rpos != u32_le_data_rend);

			auto u16_to_u32_riter_value = static_cast<uint32_t>(*u16iter_rpos++);
			auto u32_le_rdata_value = static_cast<uint32_t>(*u32_le_data_rpos++);

            REQUIRE(u16_to_u32_riter_value == u32_le_rdata_value );
        }

    }
    /* big endian */
    SECTION( "iterate forward over underlying std::input_iterator_tag iterator with big_endian output" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char16_t>;
        
        std::basic_istringstream<char16_t> u16_stringstream( u16data );
        
        base_iterator base_begin(u16_stringstream);
        base_iterator base_end{};
    
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u16iter_pos(base_begin,base_end);
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u16iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_be_data_pos = u32_be_data.begin();
        utf::u32string::iterator u32_be_data_end = u32_be_data.end();
        
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32_be_data_pos != u32_be_data_end);

			auto u16_to_u32_iter_value = static_cast<uint32_t>(*u16iter_pos++);
			auto u32_be_data_value = static_cast<uint32_t>(*u32_be_data_pos++);

            REQUIRE( u16_to_u32_iter_value == u32_be_data_value );
        }
    
    }
    SECTION( "iterate forward over underlying std::forward_iterator_tag iterator with big_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = std::forward_list<char16_t>::iterator;
        
        std::forward_list<char16_t> u16_fwd_list( u16data.begin(),u16data.end());
        
        base_iterator base_begin = u16_fwd_list.begin();
        base_iterator base_end   = u16_fwd_list.end();
    
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u16iter_pos(base_begin,base_end);
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u16iter_end(base_end,base_end);
    
        utf::u32string::iterator u32_be_data_pos = u32_be_data.begin();
        utf::u32string::iterator u32_be_data_end = u32_be_data.end();
        
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32_be_data_pos != u32_be_data_end);

			auto u16_to_u32_iter_value = static_cast<uint32_t>(*u16iter_pos++);
			auto u32_be_data_value = static_cast<uint32_t>(*u32_be_data_pos++);

            REQUIRE( u16_to_u32_iter_value  == u32_be_data_value );
        }
    
    }
    SECTION("iterate forward and reverse over underlying std::bidirectional_iterator_tag iterator with big_endian output" ) {
    
        using namespace utf;
        
        using base_iterator = utf::u16string::iterator;
        
        base_iterator base_begin = u16data.begin();
        base_iterator base_end   = u16data.end();
    
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u16iter_pos(base_begin,base_begin,base_end);
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::big_endian>> u16iter_end(base_end,base_begin,base_end);
    
        auto u16iter_rpos = std::make_reverse_iterator(u16iter_end);
        auto u16iter_rend = std::make_reverse_iterator(u16iter_pos);
    
        utf::u32string::iterator u32_be_data_pos = u32_be_data.begin();
        utf::u32string::iterator u32_be_data_end = u32_be_data.end();
        
        auto u32_be_data_rpos = std::make_reverse_iterator(u32_be_data_end);
        auto u32_be_data_rend = std::make_reverse_iterator(u32_be_data_pos);
        
        
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32_be_data_pos != u32_be_data_end);

			auto u16_to_u32_iter_value = static_cast<uint32_t>(*u16iter_pos++);
			auto u32_be_data_value = static_cast<uint32_t>(*u32_be_data_pos++);

			REQUIRE(u16_to_u32_iter_value == u32_be_data_value);
        }
    
        while(u16iter_rpos != u16iter_rend )
        {

			REQUIRE(u32_be_data_rpos != u32_be_data_rend);

			auto u16_to_u32_riter_value = static_cast<uint32_t>(*u16iter_rpos++);
			auto u32_be_rdata_value = static_cast<uint32_t>(*u32_be_data_rpos++);

            REQUIRE(u16_to_u32_riter_value == u32_be_rdata_value);
        }
    }
    
    SECTION( "detect utf16 data with bom" ) {
    
        using namespace utf;
        using base_iterator = std::istreambuf_iterator<char>;
        
        std::ifstream u16data_bom("data/data_utf16_le_bom.txt",std::ios::binary);
        std::noskipws(u16data_bom);
        
        base_iterator base_begin{ u16data_bom };
        base_iterator base_end{};

        utf::bom_type bom = utf::detect_bom(base_begin,base_end);

        REQUIRE(bom == utf::bom_type::utf16_little_endian);
        
        u16data_bom.clear();
        u16data_bom.seekg(0, std::ios::beg);

        std::advance(base_begin, sizeof(utf::utf16_little_endian_bom));
        
        using converting_iterator = utf::bytes_to_short_iterator<std::istreambuf_iterator<char>>;
        converting_iterator b2s_pos(base_begin,base_end);
        converting_iterator b2s_end(base_end,base_end);
        
        
        utf16_to_utf32_iterator<converting_iterator,utf::to<utf::little_endian>> u16iter_pos(b2s_pos,b2s_end);
        utf16_to_utf32_iterator<converting_iterator,utf::to<utf::little_endian>> u16iter_end(b2s_end,b2s_end);
    
        utf::u32string::iterator u32_le_data_pos = u32_le_data.begin();
        utf::u32string::iterator u32_le_data_end = u32_le_data.end();

        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32_le_data_pos != u32_le_data_end);

			auto u16_to_u32_iter_value = static_cast<uint32_t>(*u16iter_pos++);
			auto u32_le_data_value = static_cast<uint32_t>(*u32_le_data_pos++);

            REQUIRE(u16_to_u32_iter_value == u32_le_data_value);
        }
        
    }

    
    /*
    SECTION( "replace bad data" ) {
    
        using namespace utf;
        
        using base_iterator = utf::u16string::iterator;
        utf::u16string u16data_bad        = read_file<char>("data/data_utf16_bad.txt");
        utf::u32string u32data_replaced = read_file<char32_t>("data/data_utf32_utf16_bad_replaced.txt");

        base_iterator base_begin = u16data_bad.begin();
        base_iterator base_end   = u16data_bad.end();
    
        auto u32data_replaced_iter = u32data_replaced.begin();
        auto u32data_replaced_end = u32data_replaced.end();
    
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>,utf::onerror<utf::replace_with_fffd>> u16iter_pos(base_begin,base_begin,base_end);
        utf16_to_utf32_iterator<base_iterator,utf::to<utf::little_endian>,utf::onerror<utf::replace_with_fffd>> u16iter_end(base_end,base_begin,base_end);
        
        while(u16iter_pos != u16iter_end)
        {
            REQUIRE(u32data_replaced_iter != u32data_replaced_end);
            REQUIRE( static_cast<uint32_t>(*u16iter_pos++) ==  static_cast<uint32_t>(*u32data_replaced_iter++) );
        }

    }

    */
 
}