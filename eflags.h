#include <iostream>
#include <type_traits>

namespace {
    /**
     * @brief Overloads the bitwise OR operator for enum classes.
     *
     * @tparam Enum The enum class type.
     * @param lhs The left-hand side enum value.
     * @param rhs The right-hand side enum value.
     * @return The result of applying the bitwise OR operator to the underlying values of lhs and rhs.
     */
    template <typename Enum>
    constexpr auto operator|(Enum lhs, Enum rhs) noexcept
        -> std::enable_if_t<std::is_enum_v<Enum>, Enum>
    {
        using underlying = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    }

    /**
     * @brief Overloads the bitwise AND operator for enum classes.
     *
     * @tparam Enum The enum class type.
     * @param lhs The left-hand side enum value.
     * @param rhs The right-hand side enum value.
     * @return The result of applying the bitwise AND operator to the underlying values of lhs and rhs.
     */
    template <typename Enum>
    constexpr auto operator&(Enum lhs, Enum rhs) noexcept
        -> std::enable_if_t<std::is_enum_v<Enum>, Enum>
    {
        using underlying = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    }

    /**
     * @brief Utility class for managing flags of an enum class type.
     *
     * @tparam Enum The enum class type.
     */
    template <typename Enum>
    class EnumFlags {
        static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type.");
    public:
        /**
         * @brief Default constructor, initializes flags to 0.
         */
        constexpr EnumFlags() : flags(0) {
            static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type.");
        }

        /**
         * @brief Constructor that initializes flags with a single enum value.
         *
         * @param flag The enum value.
         */
        constexpr EnumFlags(Enum flag) noexcept : flags(static_cast<std::underlying_type_t<Enum>>(flag)) {
            static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type.");
        }
        /**
         * @brief Variadic constructor that initializes flags with multiple enum values.
         *
         * @param flags The enum values.
         */
        template <typename... Enums, typename = std::enable_if_t<(std::is_same_v<Enum, Enums> && ...)>>
        constexpr EnumFlags(Enums... flags) noexcept : flags((static_cast<std::underlying_type_t<Enum>>(flags) | ...)) {
            static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type.");
        }

        /**
         * @brief Sets a flag.
         *
         * @param flag The enum value to set.
         */
        template <typename E = Enum, typename = std::enable_if_t<std::is_same_v<E, Enum>>>
        constexpr void set(Enum flag) noexcept {
            static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type.");
            flags |= static_cast<std::underlying_type_t<Enum>>(flag);
        }

        /**
         * @brief Variadic version of set that sets multiple flags.
         *
         * @param flags The enum values to set.
         */
        template <typename... Enums, typename = std::enable_if_t<(std::is_same_v<Enum, Enums> && ...)>>
        constexpr void set(Enums... flags) noexcept {
            (set(flags), ...);
        }

        /**
         * @brief Resets (clears) a flag.
         *
         * @param flag The enum value to reset.
         */
        constexpr void reset(Enum flag) noexcept {
            static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type.");
            flags &= ~static_cast<std::underlying_type_t<Enum>>(flag);
        }

        /**
         * @brief Checks if a flag is set.
         *
         * @param flag The enum value to check.
         * @return True if the flag is set, false otherwise.
         */
        constexpr bool hasFlag(Enum flag) const noexcept {
            static_assert(std::is_enum_v<Enum>, "Template parameter must be an enum type.");
            return (flags & static_cast<std::underlying_type_t<Enum>>(flag)) != 0;
        }

        /**
         * @brief Returns true if any flags are set.
         *
         * @return True if any flags are set, false otherwise.
         */
        explicit operator bool() const noexcept {
            return flags != 0;
        }

    private:
        std::underlying_type_t<Enum> flags; /**< Internal storage for the flags. */
    };
}

#ifdef DEBUG_TEST
#include <cassert>
#define START_TEST() test::debug_test()

namespace test {

    /**
     * @brief Enum class for testing purposes with char as underlying type.
     */
    enum class TEST_V1 : char {
        one = 1 << 0, /**< Represents flag one. */
        two = 1 << 1, /**< Represents flag two. */
        three = 1 << 2 /**< Represents flag three. */
    };

    /**
     * @brief Enum class for testing purposes with int as underlying type.
     */
    enum class TEST_V2 : int {
        one = 1 << 0, /**< Represents flag one. */
        two = 1 << 1, /**< Represents flag two. */
        three = 1 << 2 /**< Represents flag three. */
    };

    /**
     * @brief Runs a series of tests to validate the functionality of the EnumFlags class.
     */
    static void debug_test() {
        // TEST_V1 Tests
        EnumFlags<TEST_V1> flags1;

        // Single flag test
        flags1.set(TEST_V1::one);
        assert(flags1.hasFlag(TEST_V1::one));
        assert(!flags1.hasFlag(TEST_V1::two));
        assert(!flags1.hasFlag(TEST_V1::three));

        // Multiple flag test
        flags1.set(TEST_V1::two, TEST_V1::three);
        assert(flags1.hasFlag(TEST_V1::one));
        assert(flags1.hasFlag(TEST_V1::two));
        assert(flags1.hasFlag(TEST_V1::three));

        // Flag reset test
        flags1.reset(TEST_V1::two);
        assert(flags1.hasFlag(TEST_V1::one));
        assert(!flags1.hasFlag(TEST_V1::two));
        assert(flags1.hasFlag(TEST_V1::three));

        // All flags reset test
        flags1.reset(TEST_V1::one | TEST_V1::two | TEST_V1::three);
        assert(!flags1.hasFlag(TEST_V1::one) && !flags1.hasFlag(TEST_V1::two) && !flags1.hasFlag(TEST_V1::three));

        // TEST_V2 Tests
        EnumFlags<TEST_V2> flags2;

        // Single flag test
        flags2.set(TEST_V2::one);
        assert(flags2.hasFlag(TEST_V2::one));
        assert(!flags2.hasFlag(TEST_V2::two));
        assert(!flags2.hasFlag(TEST_V2::three));

        // Multiple flag test
        flags2.set(TEST_V2::two, TEST_V2::three);
        assert(flags2.hasFlag(TEST_V2::one));
        assert(flags2.hasFlag(TEST_V2::two));
        assert(flags2.hasFlag(TEST_V2::three));

        // Flag reset test
        flags2.reset(TEST_V2::two);
        assert(flags2.hasFlag(TEST_V2::one));
        assert(!flags2.hasFlag(TEST_V2::two));
        assert(flags2.hasFlag(TEST_V2::three));

        // All flags reset test
        flags2.reset(TEST_V2::one | TEST_V2::two | TEST_V2::three);
        assert(!flags2.hasFlag(TEST_V2::one) && !flags2.hasFlag(TEST_V2::two) && !flags2.hasFlag(TEST_V2::three));

        //flags1.set(TEST_V2::one); // Ошибка компиляции: типы TEST_V1 и TEST_V2 несовместимы

        std::cout << "All tests passed successfully!" << std::endl;
    }
}
#else
#define START_TEST()  // !START_TEST
#endif // !DEBUG_TEST
