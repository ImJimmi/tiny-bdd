# Tiny-BDD

<p align="center">
    <img alt="GitHub License" src="https://img.shields.io/github/license/ImJimmi/tiny-bdd?style=for-the-badge">
</p>

<p align="center">
    <img src="https://img.shields.io/badge/c++17-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white"/>
    <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white"/>
</p>

<p align="center">
    <img alt="GitHub Actions Workflow Status" src="https://img.shields.io/github/actions/workflow/status/ImJimmi/tiny-bdd/test.yml?style=for-the-badge&logo=c%2B%2B&label=Tests">
</p>

<p align="center">
    <img alt="GitHub Repo stars" src="https://img.shields.io/github/stars/ImJimmi/tiny-bdd?style=for-the-badge&logo=github&color=gold">
    <img src="https://img.shields.io/github/contributors/ImJimmi/tiny-bdd?logo=github&style=for-the-badge"/>
    <img src="https://img.shields.io/github/last-commit/ImJimmi/tiny-bdd?logo=git&logoColor=white&style=for-the-badge"/>
</p>

A tiny single-header BDD-style testing framework for C++17.

Sometimes you just want a quick and easy way to test something. Maybe it's in a temporary project, or a demo project, somewhere where you don't want to faff around with large testing frameworks like [Catch2](https://github.com/catchorg/Catch2) or [GoogleTest](https://github.com/google/googletest). Tiny-BDD is designed to allow you to write good quality, well-documented tests with absolutely minimal effort.

## Examples

Describe a test using the `tbdd::test()` function and chain together some `then()`'s to define the conditions of the test. The test can be implicitly cast to `int`, giving the number of failures, which can be returned from `main()`, or `assert`'d to equal 0. If a test fails it will log to `stderr`:

```cpp
#include <tiny-bdd.h>

int main()
{
    return tbdd::test("adding numbers")
        .then("1+1 is 2", 1 + 1 == 2)
        .then(1 + 2 == 3)
        .then("2+3 is 5", 2 + 3 == 4);
}
```

```txt
[FAILED] TEST adding numbers
  THEN 2+3 is 5
```

Multiple tests can be chained together using `+`:

```cpp
return tbdd::test("adding numbers")
            .then(1 + 1 == 2)
            .then(1 + 2 == 3)
        + tbdd::test("subtracting numbers")
            .then(1 - 1 == 0)
            .then(1 - 2 == -1);
```

Tests can be given a set of inputs, which can be checked in functional conditions:

```cpp
return tbdd::test("inputs")
    .given("some inputs", 101, 202, "three-oh-three")
    .then("subsequent conditions are given those inputs",
          [](auto a, auto b, auto c) {
              return a == 101 && b == 202 && c == "three-oh-three";
          })
    .then("as is this one",
          [](auto a, auto b, auto c) {
              return a + b == 303 && c != "four-oh-four";
          })
    .given("some different inputs",
           [] {
               return std::make_tuple("lorum", '!');
           })
    .then("this one gets the new inputs",
          [](auto inputs) {
              return std::get<0>(inputs) == "lorum" && std::get<1>(inputs) == '!';
          });
```

A test's inputs can be altered before being checked by the conditions:

```cpp
return tbdd::test("transforming chars to ints")
    .given("some chars to be transformed and a container to transform them into",
           std::vector{ 'h', 'e', 'l', 'l', 'o' },
           std::vector<int>{})
    .when("the chars are converted to ints",
          [](auto& input, auto& output) {
              std::transform(std::begin(input),
                             std::end(input),
                             std::back_inserter(output),
                             [](auto c) {
                                 return static_cast<int>(c);
                             });
          })
    .then("the output container contains the integer values of the chars",
          [](auto&, auto& outputs) {
              return outputs == std::vector{ 104, 101, 108, 108, 111 };
          })
    .when("we resize the output to 3",
          [](auto&, auto& output) {
              output.resize(3);
          })
    .then("only the most recent alteration is applied",
          [](auto&, auto& outputs) {
              return outputs == std::vector{ 0, 0, 0 };
          });
```

When these tests fail, their output is very verbose making it easy to find the problem:

```txt
[FAILED] TEST transforming chars to ints
  GIVEN some chars to be transformed and a container to transform them into
    WHEN the chars are converted to ints
      THEN the output container contains the integer values of the chars

[FAILED] TEST transforming chars to ints
  GIVEN some chars to be transformed and a container to transform them into
    WHEN we resize the output to 3
      THEN only the most recent alteration is applied
```

See [the Tiny BDD tests](./test.cpp) for more examples.

## Integration

The easiest way to integrate Tiny-BDD is simply to copy the contents of [tiny-bdd.h](./tiny-bdd.h) to a header in your project. It has no dependencies, and requires no config - it just has a few classes and functions.

If you want to version-control Tiny-BDD in your project, you could add it as a submodule and add the repo to your includes path.

```bash
git clone https://github.com/ImJimmi/tiny-bdd.git
```

If your project uses CMake you can clone/submodule the repo then link against the `tiny-bdd` library:

```cmake
add_subdirectory(path/to/tiny-bdd)
target_link_libraries(my-project
PRIVATE
    tiny-bdd
)
```

Or, if you want to be really fancy, you could use [CPM](https://github.com/cpm-cmake/CPM.cmake):

```cmake
CPMAddPackage("gh:ImJimmi/tiny-bdd@main")
target_link_libraries(my-project
PRIVATE
    tiny-bdd
)
```
