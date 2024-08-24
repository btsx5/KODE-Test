#pragma once
#include "object_manager.h"

using namespace std::literals;

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const std::string& t_str, const std::string& u_str, const std::string& file,
    const std::string& func, unsigned line, const std::string& hint) {
    if (t != u) {
        std::cerr << std::boolalpha;
        std::cerr << file << "("s << line << "): "s << func << ": "s;
        std::cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        std::cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            std::cout << " Hint: "s << hint;
        }
        std::cerr << std::endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const std::string& expr_str, const std::string& func_name, const std::string& file_name, int line_number, const std::string& hint);

#define ASSERT(expr) AssertImpl(expr, #expr, __FUNCTION__, __FILE__, __LINE__, ""s)
#define ASSERT_HINT(expr, hint) AssertImpl(expr, #expr, __FUNCTION__, __FILE__, __LINE__, hint)

// -------- Начало модульных тестов  ----------

template <typename func_name>
void RunTestImpl(func_name test, const std::string& func_n) {
    test();
    std::cerr << func_n << " OK"s << std::endl;
}

#define RUN_TEST(func) RunTestImpl(func, #func)

void TestGroupByTime();
void TestGroupByType();
void TestGroupByName();
void TestGroupByDistance();
void TestObjectManager();
// --------- Окончание модульных тестов  -----------