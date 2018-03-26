#include <cstring>

namespace detail {
struct lite_str_allocator {
  static char* allocate(size_t size) {
    return new char[size];
  }
  static void deallocate(char* ptr) {
    delete [] ptr;
  }
};
}

template <typename char_type, typename allocator_t = detail::lite_str_allocator>
class basic_lite_str {
  char *ptr;
  typedef int length_t;
  length_t length;
  enum Type {
    ALLOCATED,
    LITERAL
  } type;
  typedef int alloc_info;

  public:
  alloc_info& get_alloc_info() const {
    return *((alloc_info*)(ptr + (length + 1) * sizeof(char_type)));
  }

  basic_lite_str() {
    type = LITERAL;
    length = 0;
    ptr = (char*)&length;
  }

  basic_lite_str(const char_type* s) {
    type = LITERAL;
    ptr = (char_type*)s;
    length = strlen(s);
  }

  basic_lite_str(const basic_lite_str& o) {
    copy_from(o);
  }

  basic_lite_str(basic_lite_str&& o) {
    move_from(std::move(o));
  }

  basic_lite_str& operator = (const basic_lite_str& o) {
    destruct();
    copy_from(o);
    return *this;
  }

  basic_lite_str& operator = (basic_lite_str&& o) {
    destruct();
    move_from(std::move(o));
    return *this;
  }

  void move_from(basic_lite_str&& o) {
    //std::cout << "Moving from\n";
    ptr = o.ptr;
    type = o.type;
    length = o.length;
    o.type = LITERAL;
  }

  void copy_from(const basic_lite_str& o) {
    ptr = o.ptr;
    type = o.type;
    length = o.length;
    switch (type) {
      case ALLOCATED:
        ++get_alloc_info();
        break;
      case LITERAL:
        break;
    }
  }

  void allocate(length_t l) {
    length = l;
    ptr = allocator_t::allocate((length + 1) * sizeof(char_type) + sizeof(alloc_info));
    get_alloc_info() = 1;
    type = ALLOCATED;
  }

  void destruct() {
    if (type == ALLOCATED && --get_alloc_info() == 0) {
      allocator_t::deallocate(ptr);
    }
  }

  ~basic_lite_str() {
    destruct();
  }

  char_type operator [](int ind) const {
    return ((char_type*) ptr) [ind];
  }

  length_t size() const {
    return length;
  }

  const char_type* data() const {
    return ptr;
  }

  char_type* writable_data() {
    return ptr;
  }
};

using lite_str = basic_lite_str<char>;


template <typename stream, typename alloc>
stream& operator << (stream& os, const basic_lite_str<char, alloc>& s) {
  os << s.data();
  return os;
}

template <typename char_type, typename alloc>
basic_lite_str<char_type, alloc> concatenate (const char_type* s1, const char_type* s2) {
  int l1 = strlen(s1);
  int l2 = strlen(s2);
  basic_lite_str<char_type, alloc> ret;
  ret.allocate(l1 + l2);
  strncpy(ret.writable_data(), s1, l1 * sizeof(char_type));
  strncpy(ret.writable_data() + l1 * sizeof(char_type), s2, (l2 + 1) * sizeof(char_type));
  return ret;
}

template <typename char_type, typename alloc>
basic_lite_str<char_type, alloc> operator + (const basic_lite_str<char_type, alloc>& s1, const basic_lite_str<char_type, alloc>& s2) {
  return concatenate<char_type, alloc>(s1.data(), s2.data());
}

template <typename char_type, typename alloc>
basic_lite_str<char_type, alloc> operator + (const basic_lite_str<char_type, alloc>& s1, const char* s2) {
  return concatenate<char_type, alloc>(s1.data(), s2);
}

template <typename char_type, typename alloc>
basic_lite_str<char_type, alloc> operator + (const char* s1, const basic_lite_str<char_type, alloc>& s2) {
  return concatenate<char_type, alloc>(s1, s2.data());
}

template <typename char_type, typename alloc>
basic_lite_str<char_type, alloc>& operator += (basic_lite_str<char_type, alloc>& s1, const char* s2) {
  s1 = concatenate<char_type, alloc>(s1.data(), s2);
  return s1;
}

template <typename char_type, typename alloc>
basic_lite_str<char_type, alloc>& operator += (basic_lite_str<char_type, alloc>& s1, const basic_lite_str<char_type, alloc>& s2) {
  s1 = concatenate<char_type, alloc>(s1.data(), s2.data());
  return s1;
}

template <typename char_type, typename alloc>
bool operator == (const basic_lite_str<char_type, alloc>& s1, const basic_lite_str<char_type, alloc>& s2) {
  return s1.data() == s2.data() || (s1.size() == s2.size() && memcmp(s1.data(), s2.data(), s1.size()) == 0);
}

template <typename char_type, typename alloc>
bool operator == (const basic_lite_str<char_type, alloc>& s1, const char* s2) {
  return s1.data() == s2 || (s1.size() == strlen(s2) && memcmp(s1.data(), s2, s1.size()) == 0);
}

template <typename char_type, typename alloc>
bool operator == (const char* s1, const basic_lite_str<char_type, alloc>& s2) {
  return s1 == s2.data() || (s2.size() == strlen(s1) && memcmp(s2.data(), s1, s2.size()) == 0);
}

template <typename char_type, typename alloc>
bool operator != (const basic_lite_str<char_type, alloc>& s1, const basic_lite_str<char_type, alloc>& s2) {
  return !(s1 == s2);
}

template <typename char_type, typename alloc>
bool operator != (const basic_lite_str<char_type, alloc>& s1, const char* s2) {
  return !(s1 == s2);
}

template <typename char_type, typename alloc>
bool operator != (const char* s1, const basic_lite_str<char_type, alloc>& s2) {
  return !(s1 == s2);
}

