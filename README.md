# lite_str
Simple reference counted, immutable c++ string class.

Features:
* small, cheap to copy
* wraps string literals without allocating

Example:
``` C++
#include "lite_str.h"

void print(lite_str s) {
    std::cout << s << "\n";
}

int main() {
    lite_str s = "lalala"; // so far no allocation
    print("foobar"); // nor here
    auto s2 = s + "pokpok"; // now we allocate a ref-counted buffer
    print(s2); // no allocation here
}

```
