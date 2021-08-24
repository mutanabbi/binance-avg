#include <gtest/gtest.h>
#include "../book.hpp"
#include <chrono>

class BookTest : public ::testing::Test
{};

TEST_F(BookTest, Matching)
{
  Book book;
  struct Rec {
    Book::Bid bid;
    Book::Ask ask;
  };
  std::vector<Rec> rslt;
  book.reg_on_match([&rslt](const Book::Bid& bid, const Book::Ask& ask) {
    rslt.emplace_back(bid, ask);
  });

  using std::chrono::steady_clock;

  EXPECT_TRUE(empty(book.bids_by_level()));
  EXPECT_TRUE(empty(book.asks_by_level()));

  book.add(Book::Bid{steady_clock::now(), 10.0, 20.});
  book.add(Book::Bid{steady_clock::now(), 12.0, 20.});

  {
    auto bids = book.bids_by_level();
    auto asks = book.asks_by_level();
    EXPECT_TRUE(empty(asks));
    EXPECT_TRUE(empty(asks));

    auto it = begin(bids);

    // Bids sorted in descending order
    EXPECT_EQ((*it).level, 12.0);
    EXPECT_EQ((*it).quantity, 20.);
    ++it;
    EXPECT_EQ((*it).level, 10.0);
    EXPECT_EQ((*it).quantity, 20.);
    EXPECT_EQ(++it, end(bids));
  }

  book.add(Book::Ask{steady_clock::now(), 20.0, 20.});

  {
    auto bids = book.bids_by_level();
    auto asks = book.asks_by_level();
    EXPECT_FALSE(empty(asks));
    EXPECT_FALSE(empty(asks));
    {
      auto it = begin(bids);
      EXPECT_EQ((*it).level, 12.0);
      EXPECT_EQ((*it).quantity, 20.);
      ++it;
      EXPECT_EQ((*it).level, 10.0);
      EXPECT_EQ((*it).quantity, 20.);
      EXPECT_EQ(++it, end(bids));
    }
    {
      auto it = begin(asks);
      EXPECT_EQ((*it).level, 20.0);
      EXPECT_EQ((*it).quantity, 20.);
      EXPECT_EQ(++it, end(asks));
    }
  }

  book.add(Book::Ask{steady_clock::now(), 5.0, 30.});

  {
    auto bids = book.bids_by_level();
    auto asks = book.asks_by_level();
    EXPECT_FALSE(empty(asks));
    EXPECT_FALSE(empty(asks));
    {
      auto it = begin(bids);
      EXPECT_EQ((*it).level, 10.0);
      EXPECT_EQ((*it).quantity, 10.);
      EXPECT_EQ(++it, end(bids));
    }
    {
      auto it = begin(asks);
      EXPECT_EQ((*it).level, 20.0);
      EXPECT_EQ((*it).quantity, 20.);
      EXPECT_EQ(++it, end(asks));
    }
  }

  ASSERT_EQ(size(rslt), 2);
  EXPECT_EQ(rslt[0].bid.level, 12);
  EXPECT_EQ(rslt[0].bid.quantity, 20);
  EXPECT_EQ(rslt[0].ask.level, 5);
  EXPECT_EQ(rslt[0].ask.quantity, 30);

  EXPECT_EQ(rslt[1].bid.level, 10);
  EXPECT_EQ(rslt[1].bid.quantity, 20);
  EXPECT_EQ(rslt[1].ask.level, 5);
  EXPECT_EQ(rslt[1].ask.quantity, 10);
}

