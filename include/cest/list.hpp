#ifndef _CE_LIST_HPP_
#define _CE_LIST_HPP_

// $MYGCC/bin/g++ -std=c++2a -I .. -c ../../tests/list_tests.hpp

#include <memory>
#include <algorithm>

namespace cestd {

template <
  typename T,
  typename Allocator = std::allocator<T>
 >
struct list {
  struct iter;
  
  using value_type =      T;
  using allocator_type =  Allocator;
  using size_type =       std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference =       value_type&;
  using const_reference = const value_type&;
  using pointer =         std::allocator_traits<Allocator>::pointer;
  using const_pointer =   std::allocator_traits<Allocator>::const_pointer;
  using iterator =        iter;
  using const_iterator =  const iter;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  struct node {
    value_type value;
    node      *next_node;
    node      *prev_node;
  };

  struct iter {
    constexpr reference operator*()  { return curr_node->value;  }
    constexpr iter&     operator++()    {        // pre-increment
      curr_node = curr_node->next_node;
      return *this;
    }
    constexpr iter      operator++(int) {        // post-increment
      iter tmp(curr_node);
      ++(*this);
      return tmp; 
    }
    constexpr iter&     operator--()    {        // pre-decrement
      curr_node = curr_node->prev_node;
      return *this;
    }
    constexpr iter      operator--(int) {        // post-decrement
      iter tmp(curr_node);
      --(*this);
      return tmp; 
    }
    constexpr bool      operator==(const iter &other) {
      return this->curr_node == other.curr_node;
    }
    
    node *curr_node;
  };
  
  constexpr list() : m_alloc(allocator_type{}), m_front(nullptr), 
                     m_back(nullptr) {}

  ~list() {
    this.clear();
  }

  constexpr allocator_type get_allocator() const { return m_alloc; }
  
  constexpr       reference front()       { return m_front->value; }
  constexpr const_reference front() const { return m_front->value; }

  constexpr       reference back()       { return m_back->value; }
  constexpr const_reference back() const { return m_back->value; }
  
  /* Iterators */
  constexpr       iterator  begin()       noexcept { return iter(m_front); }
  constexpr const_iterator  begin() const noexcept { return iter(m_front); }
  constexpr const_iterator cbegin() const noexcept { return iter(m_front); }
  constexpr       iterator    end()       noexcept { return iter(nullptr); }
  constexpr const_iterator    end() const noexcept { return iter(nullptr); }
  constexpr const_iterator   cend() const noexcept { return iter(nullptr); }
  
  constexpr bool empty() const { return begin() == end(); }
  
  constexpr size_type size() const noexcept { 
      return std::distance(begin(), end());
  }
  
  // Borrowing a little from libcxx with the min between allocator max size and 
  // difference_type
  constexpr size_type max_size() const noexcept { 
        return std::min<size_type>(
                  std::allocator_traits<Allocator>::max_size(m_alloc),
                  std::numeric_limits<difference_type>::max());
  }
  
  constexpr void clear() noexcept {
    node *curr_node = m_front;
    while (curr_node) {
      node *next_node = curr_node->next_node;
      std::destroy_at(curr_node);
      m_node_alloc.deallocate(curr_node,1);
      curr_node = next_node;
    }
    
    m_back = m_front = nullptr;
  }

  constexpr iterator insert(const_iterator pos, const T& value) {
    node *new_node = m_node_alloc.allocate(1);
    node* prev = pos->curr_node->prev_node;
    node* next = pos->curr_node;
    std::construct_at(new_node, value, next, prev);
    next->prev_node = new_node;
    
    if (next == m_front)
      m_front = new_node;
    else 
      prev->next_node = new_node;
      
    return iter(new_node);
  }
  
  constexpr iterator insert(const_iterator pos, T&& value) {
    node *new_node = m_node_alloc.allocate(1);
    node* prev = pos->curr_node->prev_node;
    node* next = pos->curr_node;
    std::construct_at(new_node, std::move(value), next, prev);
    next->prev_node = new_node;
    
    if (next == m_front)
      m_front = new_node;
    else 
      prev->next_node = new_node;
      
    return iter(new_node);
  }
  
  constexpr iterator insert(const_iterator pos, size_type count, 
                            const T& value) { 
  }
  
  template <class InputIt>
  constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
  }
  
  constexpr iterator insert(const_iterator pos, 
                            std::initializer_list<T> ilist) {
  }

  // TODO: Deal with the edge case where the emplaced element is the first 
  // element in the list, or is it fine to assume that you cannot pass an empty
  // iterator..?
  template <class... Args>
  constexpr iterator emplace(const_iterator pos, Args&&... args) {
    node *new_node = m_node_alloc.allocate(1);
    node* prev = pos->curr_node->prev_node;
    node* next = pos->curr_node;
    std::construct_at(new_node, value_type(std::forward<Args>(args)...), next, 
                      prev);
    next->prev_node = new_node;
    
    if (next == m_front)
      m_front = new_node;
    else 
      prev->next_node = new_node;

    return iter(new_node);
  }

  // TODO/NOTE: There will likely be a lot of places where I need to check if
  // something is a nullptr... there's a lot of accessing of members without 
  // checking things, but perhaps its fine to assume a user is doing the correct
  // thing
  // It might be feasible to simplify this with some calls to pop_back/pop_front
  // if we know the element we're removing is in the back or front of the list
  // but that may just make the functon longer
  constexpr iterator erase(const_iterator pos) {
    node* tmp = pos->curr_node;
    node* next = tmp->next_node;
    node* prev = tmp->prev_node;
    node* ret = nullptr;
    
    if (next && prev) {
      next->prev_node = prev;
      prev->next_node = next;
      ret = next;
    } else if (!next && prev) { // end of list
      prev->next_node = nullptr;
      m_back = prev;
      ret = prev;
    } else if (next && !prev) { // front of list
      next->prev_node = nullptr;
      m_front = next;
      ret = next;
    } else { // empty list
      m_back = m_front = nullptr;
    }

    std::destroy_at(tmp);
    m_node_alloc.deallocate(tmp,1);
    return iter(ret);
  }

  // TODO
  constexpr iterator erase(const_iterator first, const_iterator last) {}
  
  constexpr void push_back(const T &value) {
    node *new_node = m_node_alloc.allocate(1);
    std::construct_at(new_node, value, nullptr, m_back);
    
    if (m_back) {
      m_back.next_node = new_node;
      m_back = new_node;
    } else
      m_front = m_back = new_node;
  }
  
  // The difference between push_back and emplace new seems to be that one 
  // forwards and the other does a move beforehand... you can see an example of 
  // this in the cppreference for lists emplace_back
  constexpr void push_back(T &&value)      {
    node *new_node = m_node_alloc.allocate(1);
    std::construct_at(new_node, std::move(value), nullptr, m_back);
    
    // TODO: Can this and its other equivelants be encapsulated in someway?
    if (m_back) {
      m_back.next_node = new_node;
      m_back = new_node;
    } else
      m_front = m_back = new_node;
  }
  
  template<class... Args>
  constexpr reference emplace_back(Args&&... args) {
    node *new_node = m_node_alloc.allocate(1);
    std::construct_at(new_node, value_type(std::forward<Args>(args)...), 
                      nullptr, m_back);

    if (m_back) {
      m_back.next_node = new_node;
      m_back = new_node;
    } else
      m_front = m_back = new_node;
  }

  // TODO: Handle case where we pop off the last or second last element..  
  constexpr void pop_back() {
    static_assert(!this.empty(), "empty list, cannot pop");
    node* tmp = m_back;
    
    if (m_back->prev_node) {
      m_back = m_back->prev_node;
      m_back->next_node = nullptr;
    } else { 
      // assumption that this is the last element in the list or somethings 
      // went wrong and we've lost a node
      m_front = m_back = nullptr;
    }
    
    std::destroy_at(tmp);
    m_node_alloc.deallocate(tmp,1);
  }
  
  constexpr void push_front(const T &value) {
    node *new_node = m_node_alloc.allocate(1);
    std::construct_at(new_node, value, m_front, nullptr);
    
    if (m_front) {
      m_front.prev_node = new_node;
      m_front = new_node;
    } else
      m_front = m_back = new_node;
  }
  
  constexpr void push_front(T &&value)      {
    node *new_node = m_node_alloc.allocate(1);
    std::construct_at(new_node, std::move(value), m_front, nullptr);
    
    if (m_front) {
      m_front.prev_node = new_node;
      m_front = new_node;
    } else
      m_front = m_back = new_node;
  }

  template<class... Args>
  constexpr reference emplace_front(Args&&... args) { 
    node *new_node = m_node_alloc.allocate(1);
    std::construct_at(new_node, value_type(std::forward<Args>(args)...), 
                      m_front, nullptr);
    m_front.prev_node = new_node;
    m_front = new_node;
    
    if (m_front) {
      m_front.prev_node = new_node;
      m_front = new_node;
    } else
      m_front = m_back = new_node;
  }
  
  constexpr void pop_front() {
    static_assert(!this.empty(), "empty list, cannot pop");
    node* tmp = m_front;
    
    if (m_front->next_node) {
      m_front = m_front->next_node;
      m_front->prev_node = nullptr;
    } else
      // assumption that this is the last element in the list or somethings 
      // went wrong and we've lost a node
      m_front = m_back = nullptr;
    

    std::destroy_at(tmp);
    m_node_alloc.deallocate(tmp,1);
  }
  
  // resize
  
  // swap 
  /*
  check this in list for an idea of how to implement it, although technically
  there is no need to implement everything...
  template <class _Tp, class _Alloc>
void
__list_imp<_Tp, _Alloc>::swap(__list_imp& __c)
  */
  constexpr void swap(list& other) noexcept {
  
  }
  
  node *m_front;
  node *m_back;
  allocator_type m_alloc;
  std::allocator_traits<allocator_type>::template 
      rebind_alloc<node> m_node_alloc;
};

} // namespace cestd

#endif // _CE_LIST_HPP_