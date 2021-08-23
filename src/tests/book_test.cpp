#include <gtest/gtest.h>
#include "../book.hpp"
#include <chrono>
/// @todo Ilya: Debug only
#include <iostream>


class BookTest : public ::testing::Test
{};

TEST_F(BookTest, Matching)
{
  Book book;
  std::vector<std::pair<Book::Bid, Book::Ask>> rslt;
  book.reg_on_match([&rslt](const Book::Bid& bid, const Book::Ask& ask) {
    rslt.emplace_back(bid, ask);
    std::cout << "there is a match between " << bid << " and " << ask << std::endl;
  });

  book.add(Book::Bid{std::chrono::steady_clock::now(), 10.0, 20.});
  book.add(Book::Bid{std::chrono::steady_clock::now(), 12.0, 20.});
  book.add(Book::Ask{std::chrono::steady_clock::now(), 20.0, 20.});
  book.add(Book::Ask{std::chrono::steady_clock::now(), 5.0, 30.});

  ASSERT_EQ(rslt.size(), 2);
  EXPECT_EQ(rslt[0].first.level, 12);
  EXPECT_EQ(rslt[0].first.quantity, 20);
  EXPECT_EQ(rslt[0].second.level, 5);
  EXPECT_EQ(rslt[0].second.quantity, 30);

  EXPECT_EQ(rslt[1].first.level, 10);
  EXPECT_EQ(rslt[1].first.quantity, 20);
  EXPECT_EQ(rslt[1].second.level, 5);
  EXPECT_EQ(rslt[1].second.quantity, 10);
}

