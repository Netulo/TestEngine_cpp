#include "TestEngine.h"

#include <cstdio>
#include <cstddef>
#include <vector>
#include <cstdlib>
#include <exception>

namespace
{
    size_t g_checks = 0;
    size_t g_failures = 0;

    std::vector<te::TestCase> &getTests()
    {
        static std::vector<te::TestCase> tests;
        return tests;
    }
}

void te::addTest(const TestCase &t)
{
    getTests().push_back(t);
}

bool te::check(bool cond, const char *expr, const char *file, int line, const char *func)
{
    report(cond, file, line, func, expr);
    return cond;
}

void te::report(bool ok, const char *file, int line, const char *func, const char *message)
{
    g_checks++;
    if (!ok)
    {
        g_failures++;
        fprintf(stdout, "      FAIL %s:%d in %s(): %s\n", file, line, func, message);
    }
}

void te::runTest(size_t testIdx) 
{
    size_t failuresBefore = g_failures;
    try
    {
        getTests()[testIdx].fn();
    }
    catch (const std::exception &e)
    {
        report(false, "unknown", -1, getTests()[testIdx].name, e.what());
    }
    catch (...)
    {
        report(false, "unknown", -1, getTests()[testIdx].name, "unknown exception");
    }
    fprintf(stdout, "[%s] %s\n", failuresBefore == g_failures ? "OK" : "FAIL", getTests()[testIdx].name);
}

int te::runAll()
{
    size_t size = getTests().size();
    for (size_t test = 0; test < size; test++)
    {
        runTest(test);
    }

    fprintf(stdout, "Total checks: %zu | Failures: %zu\n", g_checks, g_failures);

    return g_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
