#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"

#include "colex.hpp"

using namespace colex;

struct MoveInt {
  MoveInt(int x) : x(x) {}
  MoveInt(const MoveInt &) = delete;
  MoveInt(MoveInt &&) noexcept = default;
  MoveInt &operator=(MoveInt &&) = default;

  int x;
};

bool operator==(const MoveInt &a, int b) { return a.x == b; }

bool operator==(const MoveInt &a, const MoveInt &b) { return a.x == b.x; }

bool operator<(const MoveInt &a, const MoveInt &b) { return a.x < b.x; }

MoveInt operator+(MoveInt a, MoveInt b) { return MoveInt(a.x + b.x); }


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

  auto ys = iter(v) | map([](const MoveInt &x) { return 2 * x.x; })
          | collect<std::vector>();

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

TEST_CASE("rvalue map collect") {
  auto ys = iter(move_int_vec()) | map(square) | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 1);
  CHECK(ys[2] == 4);
  CHECK(ys[3] == 9);
  CHECK(ys[4] == 16);
}

TEST_CASE("filter") {
  auto xs = move_int_vec();
  auto ys = iter(std::move(xs)) | filter([](const MoveInt &x) { return x < 2; })
          | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 1);
  CHECK(ys.size() == 2);
}

TEST_CASE("flat_map") {
  auto xs = move_int_vec();
  auto ys = iter(move_int_vec()) | flat_map([](MoveInt x) {
              return iter(std::array<MoveInt, 2>{std::move(x), MoveInt(0)});
            })
          | collect<std::vector>();

  CHECK(ys[0] == 0);
  CHECK(ys[1] == 0);
  CHECK(ys[2] == 1);
  CHECK(ys[3] == 0);
  CHECK(ys[4] == 2);
  CHECK(ys[5] == 0);
  CHECK(ys[6] == 3);
  CHECK(ys[7] == 0);
  CHECK(ys[8] == 4);
  CHECK(ys[9] == 0);
  CHECK(ys.size() == 10);
}

TEST_CASE("enumerate") {
  auto ys = iter(move_int_vec()) | enumerate() | collect<std::vector>();

  CHECK(ys[0].first == 0);
  CHECK(ys[0].second == 0);
  CHECK(ys[1].first == 1);
  CHECK(ys[1].second == 1);
  CHECK(ys[2].first == 2);
  CHECK(ys[2].second == 2);
  CHECK(ys[3].first == 3);
  CHECK(ys[3].second == 3);
  CHECK(ys[4].first == 4);
  CHECK(ys[4].second == 4);
}

TEST_CASE("for each") {
  std::array<MoveInt, 3> xs{2, 4, 6};
  iter(xs) | enumerate()
          | for_each([](std::pair<size_t, const MoveInt &> pair) {
              if (pair.first == 0) { CHECK(pair.second == 2); }
              if (pair.first == 1) { CHECK(pair.second == 4); }
              if (pair.first == 2) { CHECK(pair.second == 6); }
            });
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

TEST_CASE("fold1") {
  auto y = iter(move_int_vec()) | fold1(std::plus());

  CHECK(y == 1 + 2 + 3 + 4);
}

TEST_CASE("pairwise borrow") {
  auto xs = move_int_vec();

  auto ys = iter(xs) | pairwise() | collect<std::vector>();

  CHECK(ys[0].first == 0);
  CHECK(ys[0].second == 1);
  CHECK(ys[1].first == 1);
  CHECK(ys[1].second == 2);
  CHECK(ys[2].first == 2);
  CHECK(ys[2].second == 3);
  CHECK(ys[3].first == 3);
  CHECK(ys[3].second == 4);
}

TEST_CASE("pairwise move") {
  auto ys = iter({0, 1, 2, 3, 4}) | pairwise() | collect<std::vector>();

  CHECK(ys[0].first == 0);
  CHECK(ys[0].second == 1);
  CHECK(ys[1].first == 1);
  CHECK(ys[1].second == 2);
  CHECK(ys[2].first == 2);
  CHECK(ys[2].second == 3);
  CHECK(ys[3].first == 3);
  CHECK(ys[3].second == 4);
}

TEST_CASE("initializer list") {
  auto ys = iter({1, 2, 3}) | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
}

TEST_CASE("zip") {
  std::array<MoveInt, 3> left{1, 2, 3};
  std::array<MoveInt, 3> right{4, 5, 6};

  auto ys = zip(iter(std::move(left)), iter(std::move(right)))
          | collect<std::vector>();

  CHECK(ys[0].first == 1);
  CHECK(ys[0].second == 4);
  CHECK(ys[1].first == 2);
  CHECK(ys[1].second == 5);
  CHECK(ys[2].first == 3);
  CHECK(ys[2].second == 6);
  CHECK(ys.size() == 3);
}

TEST_CASE("concat") {
  std::array<MoveInt, 3> left{1, 2, 3};
  std::array<MoveInt, 3> right{4, 5, 6};

  auto ys = concat(iter(std::move(left)), iter(std::move(right)))
          | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
  CHECK(ys[3] == 4);
  CHECK(ys[4] == 5);
  CHECK(ys[5] == 6);
  CHECK(ys.size() == 6);
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

TEST_CASE("unordered_set borrow") {
  std::unordered_set<int> xs{1, 2, 3};

  std::unordered_set<int> unordered_set = iter(xs)
                                        | map([](int x) { return 2 * x; })
                                        | collect<std::unordered_set>();
  std::set<int> set = iter(std::move(unordered_set)) | collect<std::set>();
  std::vector<int> ys = iter(std::move(set)) | collect<std::vector>();

  CHECK(ys[0] == 2);
  CHECK(ys[1] == 4);
  CHECK(ys[2] == 6);
  CHECK(ys.size() == 3);
}

TEST_CASE("unordered_set move") {
  std::unordered_set<int> xs{1, 2, 3};

  std::unordered_set<int> unordered_set = iter(std::move(xs))
                                        | map([](int x) { return 2 * x; })
                                        | collect<std::unordered_set>();
  std::set<int> set = iter(std::move(unordered_set)) | collect<std::set>();
  std::vector<int> ys = iter(std::move(set)) | collect<std::vector>();

  CHECK(ys[0] == 2);
  CHECK(ys[1] == 4);
  CHECK(ys[2] == 6);
  CHECK(ys.size() == 3);
}

TEST_CASE("map borrow") {
  std::map<int, int> xs{{1, 2}, {2, 4}, {3, 6}};

  std::map<int, int> _map = iter(xs) | map([](std::pair<int, int> x) {
                              return std::make_pair(x.first, 2 * x.second);
                            })
                          | collect<std::map>();
  std::vector<int> ys = iter(_map)
                      | map([](std::pair<int, int> x) { return x.first; })
                      | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
  CHECK(ys.size() == 3);
}

TEST_CASE("map move") {
  std::map<int, int> xs{{1, 2}, {2, 4}, {3, 6}};

  std::map<int, int> _map = iter(std::move(xs))
                          | map([](std::pair<int, int> x) {
                              return std::make_pair(x.first, 2 * x.second);
                            })
                          | collect<std::map>();
  std::vector<int> ys = iter(_map)
                      | map([](std::pair<int, int> x) { return x.first; })
                      | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
  CHECK(ys.size() == 3);
}

TEST_CASE("unordered_map borrow") {
  std::unordered_map<int, int> xs{{1, 2}, {2, 4}, {3, 6}};

  std::unordered_map<int, int> unordered_map =
          iter(xs) | map([](std::pair<int, int> x) {
            return std::make_pair(x.first, 2 * x.second);
          })
          | collect<std::unordered_map>();
  std::map<int, int> _map =
          iter(std::move(unordered_map)) | collect<std::map>();
  std::vector<int> ys = iter(_map)
                      | map([](std::pair<int, int> x) { return x.first; })
                      | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
  CHECK(ys.size() == 3);
}

TEST_CASE("unordered_map move") {
  std::unordered_map<int, int> xs{{1, 2}, {2, 4}, {3, 6}};

  std::unordered_map<int, int> unordered_map =
          iter(std::move(xs)) | map([](std::pair<int, int> x) {
            return std::make_pair(x.first, 2 * x.second);
          })
          | collect<std::unordered_map>();
  std::map<int, int> _map =
          iter(std::move(unordered_map)) | collect<std::map>();
  std::vector<int> ys = iter(_map)
                      | map([](std::pair<int, int> x) { return x.first; })
                      | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
  CHECK(ys.size() == 3);
}

TEST_CASE("range") {
  auto ys = range(1, 4) | collect<std::vector>();

  CHECK(ys[0] == 1);
  CHECK(ys[1] == 2);
  CHECK(ys[2] == 3);
  CHECK(ys.size() == 3);
}

TEST_CASE("chunk map") {
  auto ys = iter({1, 2, 3, 4, 5}) | chunk_map(2, fold(0, std::plus()))
          | collect<std::vector>();

  CHECK(ys[0] == 3);
  CHECK(ys[1] == 7);
  CHECK(ys[2] == 5);
  CHECK(ys.size() == 3);
}