# Colex

This C++ library provides a set of common transformations
and reductions over the STL collections. We refer to
transformations and reductions as _expressions_. Expressions
can be arbitrarily combined in advance before being applied
to a collection. To apply an expression to a collection,
we first create an iterator from the collection, and
then apply the expression to the iterator.
The collection is here referred to as an _input_.
If the final expression is _not_ a reduction,
it can be _collected_. By this, we mean that the expression is evaluated
and put into another collection, which we refer to as an _output_.
All expressions are lazily applied. This is a work in progress, and I'll
add functionality as I need it for other projects.

## Installation
Clone the repo and `cd` into it. Then
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target colex
cmake --install . --prefix /where/to/put/it
```
This puts a shared library under `/where/to/put/it/lib` and
the header files under `/where/to/put/it/include`.

## Usage
The library overloads the `|` operator
to chain expressions, apply them to inputs,
and collect them into outputs. In the following
examples, we assume that `using namespace colex;` is
present.

### Square
In this example we square all elements of a vector
using `map` and then `collect` them to another vector.
`iter` creates an iterator that borrows `xs` by const
reference. Hence, no copies are made.
```cpp
std::vector<int> xs { 1, 2, 3 };

auto ys = iter(xs)
        | map([](const int &x) { return x * x; }) 
        | collect<std::vector>();

// ys == std::vector<int> { 1, 4, 9 };
```

### Sum of squares
In this example we square all elements of a vector using `map`
and sum them using `fold`. We show that expressions can
be combined in advance before applying them to a collection.
Additionally, we show that iterators can be created from
`std::initializer_list`s. This is especially useful in
the `flat_map` expression.
```cpp
auto square = map([](const int& x) { return x * x; });
auto sum = fold(0, [](int acc, int x) { return acc + x; });

auto sum_of_squares = square | sum;

auto y = iter({1, 2, 3}) | sum_of_squares;
// y == 1 + 4 + 9 (type: int)
```

### Avoiding copies
If the elements of our collections are large, we should
avoid copying. This can be achieved by either referencing
the elements in the first expression, or moving the
collection by passing an rvalue to `iter`.

Using references:
```cpp
std::vector<BigThing> xs = some_big_things();

// The transformation takes BigThing by const reference.
auto transformation = map([](const BigThing &x) { return do_something(x); });

auto ys = iter(xs) | transformation | collect<std::vector>();
```

Using moves:
```cpp
std::vector<BigThing> xs = some_big_things();

// The transformation takes BigThing by value
auto transformation = map([](BigThing x) { return do_something_owned(x); });

auto ys = iter(std::move(xs)) | transformation | collect<std::vector>();

// We can do the same by passing the collection directly from the function
// std::move is then unecessarry since the result of
// some_big_things() is already an rvalue
auto ys = iter(some_big_things()) | transformation | collect<std::vector>();
```

### Converting between collections
In this example we filter duplicates of a vector by first collecting
to a set, then back to another vector. 
While the example immediately collects to another type of collection,
this can of course also be done after applying some expressions.
```cpp
std::vector<int> xs { 1, 1, 2, 2, 3, 3 };

// We use std::move to avoid copies.
std::set<int> set = iter(std::move(xs)) | collect<std::set>();
std::vector<int> uniques = iter(std::move(set)) | collect<std::vector>();

// uniques == { 1, 2, 3 }
```

## Available Expressions
### `map(F func)`
Applies a function `F: (T x) -> U` to all input elements.

This example doubles all elements of `xs`.
```cpp
std::vector<int> xs = {1, 2, 3};

auto ys = iter(xs) 
        | map([](int x) { 2 * x; }) 
        | collect<std::vector>();

// ys == std::vector<int> {2, 4, 6}
```
  
### `fold(U initial, F func)` 
Reduces all input elements to a single value.
Applies a function `F: (U acc, T x) -> U` where `acc` is the accumulated
value so far, and `x` is the next input element. The returned value
is used as `acc` for the next element. For the first element,
`acc` is set to `initial`.

This example sums all elements of `xs`.
```cpp
std::vector<int> xs = {1, 2, 3};

auto y = iter(xs) 
       | fold(0, [](int acc, int x) { return acc + x; });

// y == 6
```

### `fold1(F func)`
Reduces all input elements to a single value where the first element
is used as the initial value. Requires that the input iterator has at least
one element, or else the program will crash.

This example sums all elements of `xs`.
```cpp
std::vector<int> xs = {1, 2, 3};

auto y = iter(xs) 
       | fold1([](int acc, int x) { return acc + x; });

// y == 6
```

### `filter(F predicate)`
Removes all input elements that doesn't satisfy the
predicate `F: (T x) -> bool`.

This example only keeps numbers that are smaller than 2
```cpp
std::vector<int> xs {0, 1, 2, 3, 4};

auto ys = iter(xs)
        | filter([](int x) { return x < 2; })
        | collect<std::vector>();

// ys == std::vector<int> {0, 1}
```

### `flat_map(F func)`
Applies a function `F: (T x) -> Iterator<U>`. The returned
iterators are concatenated and their elements are iterated over.

This examples puts a space after letters of the original
iterator.
```cpp
std::vector<char> xs { 'a', 'b', 'c' };
auto ys = iter(xs)
        | flat_map([](char x) { return iter({x, ' '}); })
        | collect<std::vector>();

// ys == std::vector<char> { 'a', ' ', 'b', ' ', 'c', ' ' }
```

### `take(size_t count)`
Stops iterating after the first `count` input elements.

This example extracts the two first elements of the vector
```cpp
std::vector<int> xs {1, 2, 3, 4};
auto ys = iter(std::move(xs)) | take(2) | collect<std::vector>();

// ys == std::vector<int> {1, 2}
```

### `drop(size_t count)`
Skips iterating over the first `count` input elements.

This example extracts the two last elements of the vector
```cpp
std::vector<int> xs {1, 2, 3, 4};
auto ys = iter(std::move(xs)) | drop(2) | collect<std::vector>();

// ys == std::vector<int> {3, 4}
```

### `slice(size_t start, size_t count)`
Iterates over elements with indices in the range `[start, start+count)`.

This example extracts the middle two elements of the vector

```cpp
std::vector<int> xs {1, 2, 3, 4};
auto ys = iter(std::move(xs)) | slice(1, 2) | collect<std::vector>();

// ys == std::vector<int> { 2, 3 }
```

### `enumerate()`
Adds an index to the elements of the expression.

This example adds indices to the elements of the input
```cpp
auto y = iter({1, 1, 1}) | enumerate() | collect<std::vector>();

// y == std::vector<std::pair<size_t, int> {{0, 1}, {1, 1}, {2, 1}}
```

### `for_each(F func)`
Applies a funcion `F: T x -> void` to all elements.

This example prints the elements of the input
```cpp
iter({1, 2, 3}) | for_each([](int x) { std::cout << x << std::endl; });
```

### `pairwise()`
Iterates over adjacent pairs instead of single elements

This example collects pairs of elements into a vector
```cpp
auto ys = iter({1, 2, 3, 4})
    | pairwise()
    | collect<std::vector>();

// ys == std::vector<std::pair<int, int>> {{1, 2}, {2, 3}, {3, 4}}
```

### `chunk_map(size_t size, E expr)`
Splits the input iterator into an iterator of
inner iterators with `size` elements each.
Applies the expression `expr` to each inner
iterator

This example sums adjacent numbers
```cpp
auto ys = iter({1, 2, 3, 4, 5})
        | chunk_map(2, fold1(std::plus()))
        | collect<std::vector>();

// ys == std::vector<int> { 3, 7, 5 }
```

### `chunk(size_t size)`
Splits the input iterator into an iterator of
inner iterators with `size` elements each.

This example sums adjacent numbers
```cpp
auto ys = iter({1, 2, 3, 4, 5})
| chunk_map(2)
| map([](auto x) { return std::move(x) | fold1(std::plus()))
| collect<std::vector>();

// ys == std::vector<int> { 3, 7, 5 }
```

### `partition(std::vector<size_t> partition_sizes)`
Splits the input iterator into an iterator of
inner iterators where `partition_sizes` define
the number of elements of each inner iterator.
If there are not enough elements defined by
`partition_sizes` the remaining elements of
the underlying iterator is returned as the
last partition.

This examples sums the numbers of each partition
```cpp
std::vector<size_t> partition_sizes {2, 3};
std::vector<int> xs {1, 2, 3, 4, 5, 6, 7};

auto ys = iter(xs) | partition(partition_sizes)
| map([](auto x) { return std::move(x) | fold1(std::plus()); })
| collect<std::vector>();

// ys == std::vector<int> {3, 12, 13}
```

## Supported Collections
### `std::vector`
Can be used as both input and output.

### `std::set`
Can be used as both input and output.

### `std::unordered_set`
Can be used as both input and output.

### `std::map`
Can be used as both input and output. Elements must
be `std::pair`s.

### `std::unordered_map`
Can be used as both input and output. Elements must
be `std::pair`s.

### `std::array`
Can only be used as input.

### `std::initializer_list`
Can only be used as input. Converted to an `std::vector` internally

### `T*`
Can only be used as input. Must supply the number of elements.