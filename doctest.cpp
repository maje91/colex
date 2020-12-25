#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"

#include "colex.hpp"

using namespace colex;

struct MoveInt {
  MoveInt(int x) : x(x) {}
  MoveInt(const MoveInt &) = delete;
  MoveInt(MoveInt &&) noexcept = default;

  int x;
};

bool operator==(const MoveInt &a, int b) {
  return a.x == b;
}

bool operator==(const MoveInt &a, const MoveInt &b) {
  return a.x == b.x;
}

bool operator<(const MoveInt &a, const MoveInt &b) {
  return a.x < b.x;
}


std::vector<MoveInt> move_int_vec() {
  std::vector<MoveInt> v;
  v.emplace_back(0);
  v.emplace_back(1);
  v.emplace_back(2);
  v.emplace_back(3);
  v.emplace_back(4);

  return v;
}

MoveInt square(const MoveInt &x) { return x.x * x.x; }

TEST_CASE("map collect") {
  auto v = move_int_vec();

  auto ys = v | map([](const MoveInt &x) { return 2 * x.x; }) | collect<std::vector>();

          CHECK(ys[0] == 0);
          CHECK(ys[1] == 2);
          CHECK(ys[2] == 4);
          CHECK(ys[3] == 6);
          CHECK(ys[4] == 8);
}

TEST_CASE("map fold") {
  auto v = move_int_vec();

  auto sum = fold(10, [](int acc, MoveInt x) { return acc + x.x; });

  auto value = v | map(square) | sum;
  CHECK(value == 10 + 1 + 4 + 9 + 16);
}

TEST_CASE("move map collect") {
  auto ys = move_int_vec() | map(square) | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 1);
  CHECK(ys[2] == 4);
  CHECK(ys[3] == 9);
  CHECK(ys[4] == 16);
}

TEST_CASE("conversion") {
  auto xs = move_int_vec();
  xs.emplace_back(3);

  CHECK(xs[0] == 0);
  CHECK(xs[1] == 1);
  CHECK(xs[2] == 2);
  CHECK(xs[3] == 3);
  CHECK(xs[4] == 4);
  CHECK(xs[5] == 3);
  CHECK(xs.size() == 6);

  xs = std::move(xs) | collect<std::set>() | collect<std::vector>();

  CHECK(xs[0] == 0);
  CHECK(xs[1] == 1);
  CHECK(xs[2] == 2);
  CHECK(xs[3] == 3);
  CHECK(xs[4] == 4);
  CHECK(xs.size() == 5);
}
