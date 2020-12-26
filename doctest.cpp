#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"

#include "colex.hpp"

#include <array>

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

  auto ys = iter(v) | map([](const MoveInt &x) { return 2 * x.x; }) | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 4);
  CHECK(ys[3] == 6);
  CHECK(ys[4] == 8);
}

TEST_CASE("composition borrow borrow") {
  auto v = move_int_vec();

  auto sum = [](int acc, MoveInt x) { return acc + x.x; };

  auto folder = fold(10, sum);
  auto mapper = map(square);

  auto expr = mapper | folder;
  auto value = iter(v) | expr;
  CHECK(value == 10 + 1 + 4 + 9 + 16);
}

TEST_CASE("composition move borrow") {
  auto v = move_int_vec();

  auto sum = [](int acc, MoveInt x) { return acc + x.x; };

  auto folder = fold(10, sum);

  auto expr = map(square) | folder;
  auto value = iter(v) | expr;
  CHECK(value == 10 + 1 + 4 + 9 + 16);
}

TEST_CASE("composition borrow move") {
  auto v = move_int_vec();

  auto sum = [](int acc, MoveInt x) { return acc + x.x; };

  auto mapper = map(square);

  auto expr = mapper | fold(10, sum);
  auto value = iter(v) | expr;
  CHECK(value == 10 + 1 + 4 + 9 + 16);
}

TEST_CASE("composition move move") {
  auto v = move_int_vec();

  auto sum = [](int acc, MoveInt x) { return acc + x.x; };

  auto expr = map(square) | fold(10, sum);
  auto value = iter(v) | expr;
  CHECK(value == 10 + 1 + 4 + 9 + 16);
}

TEST_CASE("move map collect") {
  auto ys = iter(move_int_vec()) | map(square) | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 1);
  CHECK(ys[2] == 4);
  CHECK(ys[3] == 9);
  CHECK(ys[4] == 16);
}

TEST_CASE("filter") {
  auto xs = move_int_vec();
  auto ys = iter(xs) | filter([](const MoveInt &x) { return x < 2; }) | map([](const MoveInt &x) { return x.x; }) | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 1);
  CHECK(ys.size() == 2);
}

TEST_CASE("flat_map") {
  std::vector<char> xs{'a', 'b', 'c'};
  auto ys = iter(xs) | flat_map([](char x) { return iter(std::array<char, 2>{x, ' '}); }) | collect<std::vector>();

  CHECK(ys[0] == 'a');
  CHECK(ys[1] == ' ');
  CHECK(ys[2] == 'b');
  CHECK(ys[3] == ' ');
  CHECK(ys[4] == 'c');
  CHECK(ys[5] == ' ');
  CHECK(ys.size() == 6);
}

TEST_CASE("take") {
  auto xs = move_int_vec();
  auto ys = iter(std::move(xs)) | take(2) | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 1);
  CHECK(ys.size() == 2);
}

TEST_CASE("drop") {
  auto xs = move_int_vec();
  auto ys = iter(std::move(xs)) | drop(2) | collect<std::vector>();

  CHECK(ys[0] == 2);
  CHECK(ys[1] == 3);
  CHECK(ys[2] == 4);
  CHECK(ys.size() == 3);
}

TEST_CASE("slice") {
  auto xs = move_int_vec();
  auto ys = iter(std::move(xs)) | slice(1, 2) | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys.size() == 2);
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

  auto set = iter(std::move(xs)) | collect<std::set>();
  xs = iter(std::move(set)) | collect<std::vector>();

  CHECK(xs[0] == 0);
  CHECK(xs[1] == 1);
  CHECK(xs[2] == 2);
  CHECK(xs[3] == 3);
  CHECK(xs[4] == 4);
  CHECK(xs.size() == 5);
}

TEST_CASE("array input map") {
  std::array<MoveInt, 3> xs{1, 2, 3};

  auto ys = iter(xs) | map(square) | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 4);
  CHECK(ys[2] == 9);
}

TEST_CASE("array conversion") {
  std::array<MoveInt, 3> xs{1, 2, 3};
  auto ys = iter(std::move(xs)) | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
}