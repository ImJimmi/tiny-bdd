#include <tiny-bdd.h>

int main()
{
    using namespace tbdd;

    return test("empty test")
         + test("single, undocumented THEN")
               .then(true)
         + test("multiple, undocumented THENs")
               .then(true)
               .then(true)
               .then(true)
         + test("documented THENs")
               .then("this message is printed if the condition is false", true)
         + test("functional THENs")
               .then([] {
                   return true;
               })
               .then("this message is printed if the function returns false",
                     [] {
                         return true;
                     })
         + test("simple GIVENs")
               .given("a single value", 123)
               .then("the value passed to the THEN matches the GIVEN value",
                     [](auto value) {
                         return value == 123;
                     })
               .given("multiple values", 456, "Hello, World!", 748.485)
               .then("the values passed to the THEN match the GIVEN values",
                     [](auto x, auto y, auto z) {
                         return std::tie(x, y, z) == std::make_tuple(456, "Hello, World!", 748.485);
                     })
               .given("nothing")
               .then("the THEN receives no values",
                     [] {
                         return true;
                     })
         + test("functional GIVENs")
               .given("a value returned by a function",
                      [] {
                          return std::make_pair(246, "135");
                      })
               .then("the THEN receives the value created by the GIVEN",
                     [](auto value) {
                         return value == std::make_pair(246, "135");
                     })
         + test("GIVEN > WHEN > THEN")
               .given("a single value", 100)
               .when("the value is altered",
                     [](auto& value) {
                         value += 444;
                     })
               .then("all following THENs receive the altered value",
                     [](auto value) {
                         return value == 100 + 444;
                     })
               .then([](auto value) {
                   return value == 100 + 444;
               })
               .when("the value is altered a different way",
                     [](auto& value) {
                         value += 999;
                     })
               .then("all following THENs receive the value with only the most recent alteration",
                     [](auto value) {
                         return value == 100 + 999;
                     })
               .given("multiple values", "Hello", "World")
               .when("the values are altered",
                     [](auto& a, auto& b) -> void {
                         a = "Goodbye";
                         b = "Nothingness";
                     })
               .then("the THEN received both altered values",
                     [](const auto& a, const auto& b) {
                         return std::tie(a, b) == std::make_tuple("Goodbye", "Nothingness");
                     })
               .given("nothing")
               .when("we alter nothing",
                     [] {
                         // nothing to do here!
                     })
               .then("the WHEN and THEN are both called with no arguments",
                     [] {
                         return true;
                     });
}
