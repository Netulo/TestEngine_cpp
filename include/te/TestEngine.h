#pragma once
#include <cstddef>

#define CHECK(cond) te::check((cond), #cond, __FILE__, __LINE__, __func__)
#define TEST_CASE(f)                      \
    static void f();                      \
    te::Registrar registrar_##f({#f, f}); \
    static void f()

namespace te
{

    struct TestCase
    {
        const char *name;
        void (*fn)();
    };

    bool check(bool cond, const char *expr, const char *file, int line, const char *func);
    int runAll();
    void addTest(const TestCase &);
    void report(const bool ok, const char *file, int line, const char *func, const char *message);
    void runTest(size_t testIdx);

    struct Registrar
    {
        Registrar(TestCase t)
        {
            addTest(t);
        }
    };
}