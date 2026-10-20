#pragma once
#include <cstdio>
#include <cstddef>
#include <vector>
#include <cstdlib>
#include <exception>
#include <sstream>
#include <string>
#include <ostream>

#define CHECK(cond) te::check((cond), #cond, __FILE__, __LINE__, __func__)
#define CHECK_EQ(a, exp) te::checkEq((a), #a, (exp), #exp, __FILE__, __LINE__, __func__)
#define REQUIRE(cond) do { if (!te::check((cond), #cond " - REQUIRED. Closing test...", __FILE__, __LINE__, __func__)) throw te::detail::RequireFailed();} while(0)
#define CHECK_THROWS(expr, Type) \
    te::checkThrows<Type>([&] { (void)(expr); }, #expr, #Type, __FILE__, __LINE__, __func__)

#define TEST_CASE(f)                      \
    static void f();                      \
    static te::Registrar registrar_##f({#f, f, __FILE__}); \
    static void f()

namespace te
{

    struct TestCase
    {
        const char *name;
        void (*fn)();
        const char *fileName;
    };

    namespace detail {

        inline size_t &checks()
        {
            static size_t count = 0;
            return count;
        }

        inline size_t &failures()
        {
            static size_t count = 0;
            return count;
        }

        inline std::vector<TestCase> &getTests()
        {
            static std::vector<TestCase> tests;
            return tests;
        }

        struct RequireFailed {

        };
    }


    inline void addTest(const TestCase &t)
    {
        detail::getTests().push_back(t);
    }


    inline void report(bool ok, const char *file, int line, const char *func, const char *message)
    {
        detail::checks()++;
        if (!ok)
        {
            detail::failures()++;
            fprintf(stdout, "      FAIL %s:%d in %s(): %s\n", file, line, func, message);
        }
    }


    // ------------
    //    CHECKS
    // ------------

    inline bool check(bool cond, const char *expr, const char *file, int line, const char *func)
    {
        report(cond, file, line, func, expr);
        return cond;
    }

    template <typename T1, typename T2>
    bool checkEq(const T1& got, const char *gotName, const T2& expected, const char *expectedName, const char *file, int line, const char *func) {
        bool cond = (got == expected);
        std::string txt;
        if (!cond){
            std::ostringstream out;
            out << gotName << " == " << expectedName << " (got " << got << ", expected " << expected <<")";
            txt = out.str();
        }
        report(cond, file, line, func, txt.c_str());
        return cond;
    }

    template <typename Ex, typename Fn>
        bool checkThrows(Fn fn, const char *exprText, const char *typeText,
                         const char *file, int line, const char *func)
        {
            std::string problem;

            try
            {
                fn();
                problem = "no exception thrown";
            }
            catch (const Ex &)
            {

            }
            catch (const detail::RequireFailed &)
            {
                throw;
            }
            catch (const std::exception &e)
            {
                problem = std::string("wrong exception type: ") + e.what();
            }
            catch (...)
            {
                problem = "wrong exception type (not derived from std::exception)";
            }

            bool ok = problem.empty();
            std::string msg;
            if (!ok)
            {
                std::ostringstream out;
                out << "CHECK_THROWS(" << exprText << ", " << typeText << "): " << problem;
                msg = out.str();
            }
            report(ok, file, line, func, msg.c_str());
            return ok;
        }



    // ----------------



    inline void runTest(size_t testIdx) 
    {
        size_t failuresBefore = detail::failures();
        try
        {
            detail::getTests()[testIdx].fn();
        }
        catch (const detail::RequireFailed) {

        }
        catch (const std::exception &e)
        {
            report(false, "unknown", -1, detail::getTests()[testIdx].name, e.what());
        }
        catch (...)
        {
            report(false, "unknown", -1, detail::getTests()[testIdx].name, "unknown exception");
        }
        fprintf(stdout, "[%s] %s: %s\n", failuresBefore == detail::failures() ? " OK " : "FAIL", detail::getTests()[testIdx].fileName ,detail::getTests()[testIdx].name);
    }

    inline int runAll()
    {
        size_t size = detail::getTests().size();
        for (size_t test = 0; test < size; test++)
        {
            runTest(test);
        }

        fprintf(stdout, "Total checks: %zu | Failures: %zu\n", detail::checks(), detail::failures());

        return detail::failures() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }


    struct Registrar
    {
        Registrar(TestCase t)
        {
            addTest(t);
        }
    };
}

#ifdef TE_MAIN
int main() {
    return te::runAll();
}
#endif