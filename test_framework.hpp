#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace al {

class TestFramework final {
    int m_fails = 0;
    bool m_should_terminate;

public:
    explicit TestFramework(bool should_terminate = true)
    : m_should_terminate(should_terminate) {}

    template <class TestFunc>
    void run_test(TestFunc func, const std::string &test_name = {}) {
        try {
            func();
            std::cerr << test_name << " OK" << std::endl;
        } catch (std::exception &ex) {
            ++m_fails;
            std::cerr << test_name << " fail: " << ex.what() << std::endl;
        } catch (...) {
            ++m_fails;
            std::cerr << test_name << " fail: unknown exception caught" << std::endl;
        }
    }

    ~TestFramework() {
        if (std::cerr.flush(); m_fails > 0) {
            std::cerr << m_fails << " unit tests failed." << std::endl;

            if (m_should_terminate) {
                std::cerr << " Terminate." << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }

    inline static void assert(bool value, const std::string &hint = {}) {
        assert_equal(value, true, hint);
    }

    template <class T, class U>
    static void assert_equal(const T &t, const U &u, const std::string &hint = {});

private:
    template <class os, class, class = void>
    struct has_os_op : std::false_type {};

    template <class os, class T>
    struct has_os_op<os, T, std::void_t<decltype(std::declval<os &>() << std::declval<const T &>())>> : std::true_type {};

    template <class os, class T>
    inline static constexpr bool has_os_op_v = has_os_op<os, T>::value;

#define CHECK_COMPARE(name, op)                                                                                           \
    template <class T, class, class = void>                                                                               \
    struct name : std::false_type {};                                                                                     \
    template <class T, class U>                                                                                           \
    struct name<T, U, std::void_t<decltype(std::declval<const T &>() op std::declval<const U &>())>> : std::true_type {}; \
    template <class T, class U>                                                                                           \
    inline static constexpr bool name##_v = name<T, U>::value

    CHECK_COMPARE(has_eq_op, ==);
    CHECK_COMPARE(has_ne_op, !=);
    CHECK_COMPARE(has_lt_op, <);
    CHECK_COMPARE(has_gt_op, >);
    CHECK_COMPARE(has_le_op, <=);
    CHECK_COMPARE(has_ge_op, >=);
#undef CHECK_COMPARE

    template <class T, class U>
    static bool not_equal(const T &t, const U &u) {
        if constexpr (has_eq_op_v<T, U>) {
            return !(t == u);
        } else if constexpr (has_ne_op_v<T, U>) {
            return t != u;
        } else if constexpr (has_lt_op_v<T, U>) {
            return t < u || u < t;
        } else if constexpr (has_gt_op_v<T, U>) {
            return t > u || u > t;
        } else if constexpr (has_le_op_v<T, U>) {
            return !(t <= u) || !(u <= t);
        } else if constexpr (has_ge_op_v<T, U>) {
            return !(t >= u) || !(u >= t);
        } else {
            constexpr auto has_any_comparison_operator = has_eq_op_v<T, U> || has_ne_op_v<T, U>
            || has_lt_op_v<T, U> || has_gt_op_v<T, U> || has_le_op_v<T, U> || has_ge_op_v<T, U>;
            static_assert(has_any_comparison_operator, "No comparison operator for this operands!");
        }
        return false;
    }

    template <class Container>
    static std::ostream &print_container(std::ostream &os, const Container &cont) {
        os << '[';

        if (auto it = cont.begin(), end = cont.end(); it != end) {
            for (os << *it, ++it; it != end; ++it) {
                os << ", " << *it;
            }
        }
        return os << ']';
    }

    template <class K, class V, template <class, class> class Map>
    static std::ostream &print_map(std::ostream &os, const Map<K, V> &map) {
        os << '[';

        if (auto it = map.begin(), end = map.end(); it != end) {
            os << '{' << it->first << ':' << it->second << '}';

            for (++it; it != end; ++it) {
                os << ", {" << it->first << ':' << it->second << '}';
            }
        }
        return os << ']';
    }

#define OPEN_CONTAINER(cont)                                                     \
    template <class T>                                                           \
    static std::ostream &print_##cont(std::ostream &os, const std::cont<T> &q) { \
        struct open_##cont : std::cont<T> {                                      \
            auto get() const {                                                   \
                return std::cont<T>::c;                                          \
            }                                                                    \
        };                                                                       \
        return print_container(os, static_cast<const open_##cont &>(q).get());   \
    }

    OPEN_CONTAINER(queue)
    OPEN_CONTAINER(stack)
#undef OPEN_CONTAINER

    template <class T, std::size_t S>
    friend std::ostream &operator<<(std::ostream &os, const std::array<T, S> &array);

#define DECLARE_PRINT_CONTAINER(cont) \
    template <class T>                \
    friend std::ostream &operator<<(std::ostream &os, const std::cont<T> &cont);

    DECLARE_PRINT_CONTAINER(deque)
    DECLARE_PRINT_CONTAINER(vector)
    DECLARE_PRINT_CONTAINER(list)
    DECLARE_PRINT_CONTAINER(forward_list)
    DECLARE_PRINT_CONTAINER(set)
    DECLARE_PRINT_CONTAINER(multiset)
    DECLARE_PRINT_CONTAINER(unordered_set)
    DECLARE_PRINT_CONTAINER(unordered_multiset)
#undef DECLARE_PRINT_CONTAINER

#define DECLARE_PRINT_OPEN_CONTAINER(cont) \
    template <class T>                     \
    friend std::ostream &operator<<(std::ostream &os, const std::cont<T> &cont);

    DECLARE_PRINT_OPEN_CONTAINER(queue)
    DECLARE_PRINT_OPEN_CONTAINER(stack)
#undef DECLARE_PRINT_OPEN_CONTAINER

#define DECLARE_PRINT_MAP(map)  \
    template <class K, class V> \
    friend std::ostream &operator<<(std::ostream &os, const std::map<K, V> &map);

    DECLARE_PRINT_MAP(map)
    DECLARE_PRINT_MAP(multimap)
    DECLARE_PRINT_MAP(unordered_map)
    DECLARE_PRINT_MAP(unordered_multimap)
#undef DECLARE_PRINT_MAP
};

template <class T, std::size_t S>
std::ostream &operator<<(std::ostream &os, const std::array<T, S> &array) {
    return TestFramework::print_container(os, array);
}

#define PRINT_CONTAINER(cont)                                              \
    template <class T>                                                     \
    std::ostream &operator<<(std::ostream &os, const std::cont<T> &cont) { \
        return TestFramework::print_container(os, cont);                   \
    }

PRINT_CONTAINER(deque)
PRINT_CONTAINER(vector)
PRINT_CONTAINER(list)
PRINT_CONTAINER(forward_list)
PRINT_CONTAINER(set)
PRINT_CONTAINER(multiset)
PRINT_CONTAINER(unordered_set)
PRINT_CONTAINER(unordered_multiset)
#undef PRINT_CONTAINER

#define PRINT_OPEN_CONTAINER(cont)                                         \
    template <class T>                                                     \
    std::ostream &operator<<(std::ostream &os, const std::cont<T> &cont) { \
        return TestFramework::print_##cont(os, cont);                      \
    }

PRINT_OPEN_CONTAINER(queue)
PRINT_OPEN_CONTAINER(stack)
#undef PRINT_OPEN_CONTAINER

#define PRINT_MAP(map)                                                      \
    template <class K, class V>                                             \
    std::ostream &operator<<(std::ostream &os, const std::map<K, V> &map) { \
        return TestFramework::print_map(os, map);                           \
    }

PRINT_MAP(map)
PRINT_MAP(multimap)
PRINT_MAP(unordered_map)
PRINT_MAP(unordered_multimap)
#undef PRINT_MAP

template <class T, class U>
void TestFramework::assert_equal(const T &t, const U &u, const std::string &hint) {
    static_assert(has_os_op_v<std::ostream, T>, "No output operators for this operand!");
    static_assert(has_os_op_v<std::ostream, U>, "No output operators for this operand!");

    if (not_equal(t, u)) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;

        if (hint.empty() == false) {
            os << "\nHint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

} // namespace al

#if !defined(FILE_NAME)
#define FILE_NAME __FILE__
#endif

#define ASSERT_EQUAL(x, y)                                        \
    do {                                                          \
        std::ostringstream p_assert_os;                           \
        p_assert_os << #x << " != " << #y << '\n'                 \
                    << FILE_NAME << " : " << __LINE__;            \
        al::TestFramework::assert_equal(x, y, p_assert_os.str()); \
    } while (0)

#define ASSERT(x)                                        \
    do {                                                 \
        std::ostringstream p_assert_os;                  \
        p_assert_os << #x << " is false" << '\n'         \
                    << FILE_NAME << " : " << __LINE__;   \
        al::TestFramework::assert(x, p_assert_os.str()); \
    } while (0)

#define RUN_TEST(func) run_test(func, #func)
