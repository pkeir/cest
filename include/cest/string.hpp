#ifndef _CEST_STRING_HPP_
#define _CEST_STRING_HPP_

// $MYGCC/bin/g++ -std=c++2a -I .. -c ../../tests/string_tests.hpp

#include <string>   // std::char_traits
#include <memory>   // std::allocator
#include <iterator> // std::reverse_iterator

namespace cest {

template <
  class CharT,
  class Traits    = std::char_traits<CharT>,
  class Allocator = std::allocator<CharT>
> class basic_string {
public:

  using traits_type         = Traits;
  using value_type          = CharT;
  using allocator_type      = Allocator;
  using size_type           = std::allocator_traits<Allocator>::size_type;
  using difference_type     = std::allocator_traits<Allocator>::difference_type;
  using reference           = value_type&;
  using const_reference     = const value_type&;
  using pointer             = std::allocator_traits<Allocator>::pointer;
  using const_pointer       = std::allocator_traits<Allocator>::const_pointer;
  using iterator            = CharT*;
  using const_iterator      = const iterator;
  using reverse_iterator    = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr basic_string() : m_p(nullptr), m_size(0), m_capacity(0) {}
  constexpr ~basic_string() {
    std::destroy_n(m_p,m_size);
    if (0 != m_capacity) m_alloc.deallocate(m_p,m_capacity);
  }

  constexpr size_type        size() const { return m_size;       }
  constexpr size_type    capacity() const { return m_capacity;   }
  constexpr iterator        begin()       { return m_p;          }
  constexpr const_iterator  begin() const { return m_p;          }
  constexpr const_iterator cbegin() const { return m_p;          }
  constexpr iterator          end()       { return m_p + m_size; }
  constexpr bool            empty() const { return m_size == 0;  }
  constexpr void         pop_back()       { m_size--; }
  constexpr reference       operator[](size_type pos)       { return m_p[pos]; }
  constexpr const_reference operator[](size_type pos) const { return m_p[pos]; }

  constexpr void push_back(const value_type &value) {
    if (0 == m_capacity) {
      reserve(1);
    } else if (m_capacity == m_size) {
      reserve(m_capacity * 2);
    }
    std::construct_at(&m_p[m_size++],value);
  }

  constexpr void reserve(size_type new_cap)
  {
    if (new_cap > m_capacity)
    {
      value_type *p = m_alloc.allocate(new_cap);
      std::copy_n(m_p, m_size, p);
      if (0 != m_capacity) {
        std::destroy_n(m_p,m_size);
        m_alloc.deallocate(m_p,m_capacity);
      }
      m_p = p;
      m_capacity = new_cap;
    }
  }

  size_type m_size, m_capacity;
  allocator_type m_alloc;
  value_type *m_p;
};

using string    = basic_string<char>;
using wstring   = basic_string<wchar_t>;
using u8string  = basic_string<char8_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

} // namespace cest

#endif // _CEST_STRING_HPP_
