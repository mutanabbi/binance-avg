#include <gtest/gtest.h>
#include "../average_monoid.hpp"
#include <chrono>


class AverageMonoidTest : public ::testing::Test
{};


TEST_F(AverageMonoidTest, ArithmeticProgression)
{
    auto fld = [] (std::size_t first, std::size_t last, std::size_t step) {
        decltype(first) sum = 0;
        std::size_t cnt = 0;
        for (auto i = first; i <= last; i += step, cnt++)
            sum += i;
        return static_cast<double>(sum) / cnt;
    };

    auto mfld = [] (std::size_t first, std::size_t last, std::size_t step) {
        auto avg = AverageMonoid<double>::mempty();
        for (auto i = first; i <= last; i += step)
            avg = mappend(avg, AverageMonoid{static_cast<double>(i)});
        return avg;
    };

    auto theory = [] (std::size_t first, std::size_t last, std::size_t step) {
        auto diff = last - first;
        return static_cast<double>(first + last - diff % step) / 2;
    };

    {
    std::size_t first = 1, last = 10000, step = 1;
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), fld(first, last, step));
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), theory(first, last, step));
    }
    {
    std::size_t first = 100500, last = 500100, step = 1234;
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), fld(first, last, step));
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), theory(first, last, step));
    }
    {
    std::size_t first = 25, last = 139, step = 12;
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), fld(first, last, step));
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), theory(first, last, step));
    }
    {
    std::size_t first = 42, last = 42, step = 1;
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), fld(first, last, step));
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), theory(first, last, step));
    }
    {
    std::size_t first = 1234, last = 4321, step = 13;
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), fld(first, last, step));
    EXPECT_DOUBLE_EQ(mfld(first, last, step).value(), theory(first, last, step));
    }
}

TEST_F(AverageMonoidTest, Duration)
{
    auto mfld = [] (std::size_t first, std::size_t last, std::size_t step) {
        using namespace std::chrono;
        auto avg = AverageMonoid<steady_clock::duration>::mempty();
        for (auto i = first; i <= last; i += step)
            avg = mappend(avg, decltype(avg){milliseconds{i}});
        return avg.value();
    };

    auto theory = [] (std::size_t first, std::size_t last, std::size_t step) {
        auto diff = last - first;
        using namespace std::chrono;
        return steady_clock::duration{milliseconds{first + last - diff % step}} / 2;
    };

    auto isAround = [] (auto val, auto expect, auto diff) {
        return val < expect + diff && expect - diff < val;
    };

    using namespace std::literals::chrono_literals;
    {
    std::size_t first = 1, last = 10000, step = 1;
    EXPECT_TRUE(isAround(mfld(first, last, step), theory(first, last, step), 1us));
    }
    {
    std::size_t first = 100500, last = 500100, step = 1234;
    EXPECT_TRUE(isAround(mfld(first, last, step), theory(first, last, step), 50us));
    }
    {
    std::size_t first = 25, last = 139, step = 12;
    EXPECT_TRUE(isAround(mfld(first, last, step), theory(first, last, step), 1us));
    }
    {
    std::size_t first = 42, last = 42, step = 1;
    EXPECT_EQ(mfld(first, last, step), theory(first, last, step));
    }
    {
    std::size_t first = 1234, last = 4321, step = 13;
    EXPECT_TRUE(isAround(mfld(first, last, step), theory(first, last, step), 10us));
    }
}
