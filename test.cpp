#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <set>
#include "lite_str.h"

using namespace std;

struct my_str_allocator {
  static set<char*> allocated;
  static char* allocate(size_t size) {
    auto ret = new char[size];
    allocated.insert(ret);
    return ret;
  }

  static void deallocate(char* ptr) {
    allocated.erase(ptr);
    delete [] ptr;
  }

  static void check() {
    assert(allocated.empty());
  }
};

set<char*> my_str_allocator::allocated;

using test_str = basic_lite_str<char, my_str_allocator>;

void print(test_str s) {
  cout << s << endl;
}

#define EQ(a, b) do { if (!(a == b) || a != b) { std::cerr << a << "!=" << b << std::endl; abort(); }} while (0);

void test1() {
  test_str s1 = "foo";
  test_str s2 = "bar";
  s1 = s2 + " i baz";
  s1 = s1 + " i qux";
  EQ(s1, "bar i baz i qux");
  EQ("bar i baz i qux", s1);
  print(s1);
  print(s2);
}

void test2() {
  test_str s;
  EQ(s + "foo", "foo");
}

void test3() {
  test_str s;
  {
    test_str s2 = "foo";
    s = s2;
    EQ(s, s2);
  }
  EQ(s + "foo", "foofoo");
}

void test4() {
  test_str p[] = {"ab", "cd", "ef" };
  test_str s;
  for (int i = 0; i < 10; ++i)
    s += p[i % 3];
  EQ(s, "abcdefabcdefabcdefab");
}

int main() {
  test1();
  test2();
  test3();
  test4();
  my_str_allocator::check();
}
