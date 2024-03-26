#pragma once

#include <functional>
#include <iostream>
#include <vector>

namespace tbdd
{
    struct Message
    {
        std::string testName;
        std::string given;
        std::string when;
        std::string then;
    };

    auto& operator<<(std::ostream& output, const Message& message)
    {
        output << "TEST " << message.testName << "\n";

        static constexpr auto indent = "  ";
        std::string currentIndent{ indent };

        for (const auto& [messagePart, title] : std::vector{
                 std::make_pair(message.given, "GIVEN"),
                 std::make_pair(message.when, "WHEN"),
                 std::make_pair(message.then, "THEN"),
             })
        {
            if (!std::empty(messagePart))
            {
                output << currentIndent << title << " " << messagePart << "\n";
                currentIndent += indent;
            }
        }

        return output;
    }

    template <typename... Inputs>
    class Test
    {
    public:
        using MakeInputs = std::function<std::tuple<Inputs...>()>;

        explicit Test(std::string name)
        {
            currentMessage.testName = name;
        }

        Test(Message message, MakeInputs makeIns)
            : currentMessage{ std::move(message) }
            , makeInputs{ makeIns }
        {
        }

        Test() = delete;
        Test(const Test&) = default;
        Test(Test&&) = default;
        Test& operator=(const Test&) = default;
        Test& operator=(Test&&) = default;
        ~Test() = default;

        template <typename... NewInputs>
        [[nodiscard]] auto given(std::string message, NewInputs... inputs)
        {
            currentMessage.given = message;
            return Test<NewInputs...>{
                currentMessage,
                [ins = std::make_tuple(inputs...)] {
                    return ins;
                },
            };
        }

        template <typename Creator, std::enable_if_t<std::is_invocable_v<Creator>, bool> = false>
        [[nodiscard]] auto given(std::string message, Creator create)
        {
            currentMessage.given = message;
            return Test<decltype(create())>{
                currentMessage,
                [create] {
                    return create();
                },
            };
        }

        template <typename Operation>
        [[nodiscard]] auto& when(std::string message, Operation operation)
        {
            currentMessage.when = message;

            alterInputs = [operation](std::tuple<Inputs...>& ins) {
                static_assert(std::is_same<decltype(std::apply(operation, ins)), void>());
                std::apply(operation, ins);
            };

            return *this;
        }

        template <typename Condition>
        auto& then(Condition condition)
        {
            return then("#" + std::to_string(++thenCount), condition);
        }

        template <typename Condition>
        auto& then(std::string message, Condition condition)
        {
            if constexpr (!std::is_invocable<Condition>() && std::is_convertible<Condition, bool>())
            {
                currentMessage.then = message;

                if (!static_cast<bool>(condition))
                {
                    failureCount++;
                    std::cerr << "[FAILED] " << currentMessage << std::endl;
                }

                return *this;
            }
            else
            {
                auto inputs = makeInputs();

                if (alterInputs != nullptr)
                    alterInputs(inputs);

                static_assert(std::is_convertible<decltype(std::apply(condition, inputs)), bool>());
                return then(message, std::apply(condition, inputs));
            }
        }

        [[nodiscard]] auto getNumFailures() const
        {
            return failureCount;
        }

        template <typename Integral>
        [[nodiscard]] operator Integral() const
        {
            static_assert(std::is_integral<Integral>());
            return static_cast<Integral>(failureCount);
        }

        template <typename... OtherInputs>
        [[nodiscard]] auto& operator+(const Test<OtherInputs...>& other)
        {
            failureCount += other.getNumFailures();
            return *this;
        }

    private:
        Message currentMessage;
        std::function<std::tuple<Inputs...>()> makeInputs = []() {
            return std::make_tuple();
        };
        std::function<void(std::tuple<Inputs...>&)> alterInputs;
        int failureCount = 0;
        int thenCount = 0;
    };

    [[nodiscard]] auto test(std::string message)
    {
        return Test<>{ message };
    }
} // namespace tbdd
