[![Build status](https://ci.appveyor.com/api/projects/status/ix95xf1mv55v9pag/branch/master?svg=true)](https://ci.appveyor.com/project/rmawatson/utf/branch/master)
[![Build Status](https://travis-ci.com/rmawatson/utf.svg?branch=master)](https://travis-ci.com/rmawatson/utf)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3287a3623db942fa887042e2624c5a6e)](https://www.codacy.com/app/rmawatson/utf?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=rmawatson/utf&amp;utm_campaign=Badge_Grade)
[![GitHub License](https://img.shields.io/badge/license-Boost%201.0-blue.svg)](https://github.com/rmawatson/utf/blob/master/LICENSE)
![platform](https://img.shields.io/badge/platform-visual%20studio-blue.svg?logo=windows&longCache=true&style=flat&logoColor=white)
![platform](https://img.shields.io/badge/platform-xcode-blue.svg?logo=apple&longCache=true&style=flat&logoColor=white)
![platform](https://img.shields.io/badge/platform-gcc%20%2F%20clang-blue.svg?logo=linux&longCache=true&style=flat&logoColor=white)
# UTF Iterators for modern C++

## Introduction

**UTF** is a header only library providing simple unicode iterator adapters to for converting between various unicode formats and differing endianness. Similar to those available with boost, but without the boost dependency. 

Utility functions that build on top of these iterators are available to provide standard conversions and byte order mark detection. Additional support is provided for replacing invalid byte sequences with a user provide replacement character, or the unicode default (U+FFFD �) or throwing an exception.

* [Iterators](#iterators)
* [Utility Functions](#utility-functions)
* [Examples](#examples)
* [CMake](#cmake)
* [License](#license)

## Iterators

Each iterator adapter models the catagory of the underlying base_iterator. To ensure iterators do not iterate past the end when consuming multiple elements from the base_iterator, different constructors are used for certain iterators depending on the catagory of base_iterator.

#### **|```utf8_to_utf32_iterator<base_iterator,policies...>```**


***constructors:***

*base_iterator::iterator_category is bidrectional_iterator or random_access_iterator*<br/>
**```utf8_to_utf32_iterator(start_iterator,range_begin_iterator,range_end_iterator)```**
 
*base_iterator::iterator_category is forward_iterator or input_iterator*<br/>
**```utf8_to_utf32_iterator(start_iterator,range_end_iterator)```**
 
***policies:***

**```to<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```utf8_to_utf16_iterator<base_iterator,policies...>```**

***constructors:***

*base_iterator::iterator_category is bidrectional_iterator*<br/>
**```utf8_to_utf16_iterator(start_iterator,range_begin_iterator,range_end_iterator)```**
 
*base_iterator::iterator_category is forward_iterator or input_iterator*<br/>
**```utf8_to_utf16_iterator(start_iterator,range_end_iterator)```**
 
***policies:***

**```to<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```utf32_to_utf8_iterator<base_iterator,policies...>```**

***constructors:***

**```utf32_to_utf8_iterator(base_iterator)```**
 
***policies:***

**```from<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```ut32_to_utf16_iterator<base_iterator,policies...>```**

***constructors:***

**```utf32_to_utf16_iterator(base_iterator)```**
 
***policies:***

**```from<big_endian|little_endian>```**<br/>
**```to<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**


## Utility Functions

The ```utfX_to_utfY()``` utility functions provide an easy way to convert between different unicode types. the ```base_iterator``` 
dereference operator should yeild a type convertible to uintX_t.

Each of ```utfX_to_utfY()``` use a default return value of uYstring, which can be changed by providing any type that has begin() end() and value_type as one of the tempalte parameters.
for example,

```c++
std::string u8text = u8"text";
uint8_to_uint16<std::vector<char16_t>>(text);
```

Two overloads exist for each ```utfX_to_utfY()``` conversion function, one whos arguments are a start and end iterator range to convert, the other an iterable object.
The same set of policies that would be used with the respective iterator can be passed as template parameters to these functions. The order is unimportant. However polices
are not checked and unused policies will be silently ignored (for example a to<big_endian> in a to_utf8 conversion has no meaning).

```c++
uint8_to_uint16<onerror<replace_with_fffd>,std::vector<char16_t>>(text);
// the same as
uint8_to_uint16<std::vector<char16_t>,onerror<replace_with_fffd>>(text);
```

#### **|```u16string utf8_to_utf16<result_type,policies...>(start_iterator,end_iterator)```**
#### **|```u16string utf8_to_utf16<result_type,policies...>(iteratble)```**
***policies:***

**```to<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```u16string utf8_to_utf32<result_type,policies...>(start_iterator,end_iterator)```**
#### **|```u16string utf8_to_utf32<result_type,policies...>(iteratble)```**
***policies:***

**```to<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```u8string utf16_to_utf8<result_type,policies...>(start_iterator,end_iterator)```**
#### **|```u8string utf16_to_utf8<result_type,policies...>(iteratble)```**

***policies:***


**```from<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---


#### **|```u16string utf16_to_utf32<result_type,policies...>(start_iterator,end_iterator)```**
#### **|```u16string utf16_to_utf32<result_type,policies...>(iteratble)```**

***policies:***

**```from<big_endian|little_endian>```**<br/>
**```to<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```u8string utf32_to_utf16<result_type,policies...>(start_iterator,end_iterator)```**
#### **|```u8string utf32_to_utf16<result_type,policies...>(iteratble)```**

***policies:***

**```from<big_endian|little_endian>```**<br/>
**```to<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```u8string utf32_to_utf8<result_type,policies...>(start_iterator,end_iterator)```**
#### **|```u8string utf32_to_utf8<result_type,policies...>(iteratble)```**

***policies:***

**```from<big_endian|little_endian>```**<br/>
**```onerror<replace_with<char32_t>|throw_exception>```**

---

#### **|```bom_type detect_bom(start_iterator,end_iterator)```**
Detects a bom from a byte sequence. reads upto 4 bytes to detect the bom.

This is nesseccary to distinguish between
utf32_little_endian and utf16_little_endian where the first two bytes are identical. 

*Note: there is potential for ambiguity when the first two bytes after the bom in a utf16_little_endian byte sequence are 0x0.*

returns one of,

```
none,
utf8,
utf16_little_endian,
utf16_big_endian,
utf32_little_endian,
utf32_big_endian,
```

## Constants

**```constexpr uint8_t utf8_bom[]```**<br/>
**```constexpr uint8_t utf16_little_endian_bom[]```**<br/>
**```constexpr uint8_t utf16_big_endian_bom[]```**<br/>
**```constexpr uint8_t utf32_little_endian_bom[]```**<br/>
**```constexpr uint8_t utf32_little_endian_bom[]```**<br/>
**```constexpr uint8_t utf32_big_endian_bom[]```**<br/>

## Cmake

This project can be used as an external project cmake's find_package(),

```cmake
# CMakeLists.txt
find_package(utf REQUIRED)
...
add_library(somelib ...)
...
target_link_libraries(somelib PRIVATE utf::utf)
```
or placed in a thirdparty folder and used through add_subdirectory,
```cmake
# Disable building tests
set(UTF_BUILD_TESTS OFF CACHE INTERNAL "")
..
add_subdirectory(thirdparty/utf)
...
add_library(somelib ...)
...
target_link_libraries(somelib PRIVATE utf::utf)
```

## Examples

* Convert existing utf16 string to utf8 with utility functions.

```c++
#include <ifstream>
#include <iterator>
#include <utf/utf.h>

int main()
{
	using namespace utf;
    
    u16string u16_text = U"ɦΈ˪˪ʘ";
    //use default platform endianness, default onerror policy.
    u8string u8_text = utf16_to_utf8(u16_text.begin(),u16_text.end());
}

```

* Convert from an existing utf32 string to utf8 with iterators.

```c++
#include <utf/utf.h>

int main()
{
    using namespace utf;

    u32string u32_text = U"ɦΈ˪˪ʘ";

    //use the bidirectional_iterator constructor, default platform endianness, default error policy.
    utf32_to_utf8_iterator<u32string::iterator> pos(u32_text.begin());
    utf32_to_utf8_iterator<u32string::iterator> end(u32_text.end());

    u8string u8_text(pos, end);
}

```

* Reading from a UTF-32 little endian file, checking for a byte order mark, and converting to UTF-8.

```c++
#include <fstream>
#include <iterator>
#include <cassert>
#include <utf/utf.h>

int main()
{
    using namespace utf;

    // open the file
    std::ifstream uc_file("unicode_file.txt", std::ios::binary);

    //The iterators to use for the stream.
    using base_iterator = std::istreambuf_iterator<char>;

    base_iterator base_pos(uc_file);
    base_iterator base_end{};

    //detect the if the file contains a byte order mark and that it is UTF-32 little endian encoded file.
    bom_type uc_type = detect_bom(base_pos, base_end);
    assert( uc_type == bom_type::utf32_little_endian );

    // detect_bom will read upto 4 bytes to detect the bom, even if the bom is two bytes (utf16) or three bytes (utf8). For input_iterators,
    // bytes may already have been read that are part of the unicode text and cannot be re-read. In this case the bom is 4 bytes. However when
    // detecting utf16 or utf8 boms, the stream should be reset to the begining and the iterator advanced by exactly the size of the bom.
    // Although this is not necessary here, as sizeof(utf32_little_endian_bom) == 4

    uc_file.clear();
    uc_file.seekg(0, std::ios::beg);
    std::advance(base_pos, sizeof(utf32_little_endian_bom));

    //read from the stream 4 bytes at a time for the utf32_to_utf8_iterator

    using btol_iterator = bytes_to_long_iterator<base_iterator>;

    // using the input iterator constructor
    btol_iterator b2l_pos(base_pos, base_end);
    btol_iterator b2l_end(base_end, base_end);

    //specify the source endianness. The default will be the platform endianness. using the default onerror<throw_exception> policy.
    using utf_iterator = utf32_to_utf8_iterator<btol_iterator, from<little_endian>>;


    utf_iterator utf_pos(b2l_pos);
    utf_iterator utf_end(b2l_end);

    // construct a new std::basic_string<char> from the iterators.
    u8string result_u8(utf_pos, utf_end);

    return 0;
}
```
## License

The files in this repository are licensed under the Boost Software License 1.0. A copy of the license is available in the root of the repository.
