#include "test_framework.hpp"

#define TEST_ADAPTER(data_type)                       \
    std::data_type<int> data_type##_1, data_type##_2; \
    data_type##_1.push(1);                            \
    ASSERT_EQUAL(data_type##_1, data_type##_2)

#define TEST(data_type)                                  \
    std::data_type<int> data_type##_1{1}, data_type##_2; \
    ASSERT_EQUAL(data_type##_1, data_type##_2)

#define TEST_MAP(data_type)                                        \
    std::data_type<int, int> data_type##_1{{1, 1}}, data_type##_2; \
    ASSERT_EQUAL(data_type##_1, data_type##_2)

void test00() {
    TEST_ADAPTER(stack);
}
void test01() {
    TEST_ADAPTER(queue);
}

void test02() {
    TEST(vector);
}
void test03() {
    TEST(deque);
}
void test04() {
    TEST(list);
}
void test05() {
    TEST(forward_list);
}
void test06() {
    TEST(set);
}
void test07() {
    TEST(multiset);
}
void test08() {
    TEST(unordered_set);
}
void test09() {
    TEST(unordered_multiset);
}

void test10() {
    TEST_MAP(map);
}
void test11() {
    TEST_MAP(multimap);
}
void test12() {
    TEST_MAP(unordered_map);
}
void test13() {
    TEST_MAP(unordered_multimap);
}

int main() {
    al::TestFramework tf(false);

    {
        tf.RUN_TEST(test00), std::cerr << '\n';
        tf.RUN_TEST(test01), std::cerr << '\n';
        tf.RUN_TEST(test02), std::cerr << '\n';
        tf.RUN_TEST(test03), std::cerr << '\n';
        tf.RUN_TEST(test04), std::cerr << '\n';
        tf.RUN_TEST(test05), std::cerr << '\n';
        tf.RUN_TEST(test06), std::cerr << '\n';
        tf.RUN_TEST(test07), std::cerr << '\n';
        tf.RUN_TEST(test08), std::cerr << '\n';
        tf.RUN_TEST(test09), std::cerr << '\n';
        tf.RUN_TEST(test10), std::cerr << '\n';
        tf.RUN_TEST(test11), std::cerr << '\n';
        tf.RUN_TEST(test12), std::cerr << '\n';
        tf.RUN_TEST(test13), std::cerr << '\n';
    }
}
