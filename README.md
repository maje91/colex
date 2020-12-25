# Colex

This library provides a set of common transformations
and reductions over the C++ STL collections. We refer to
transformations and reductions as _expressions_. Expressions
can be arbitrarily combined in advance before being applied
to a collection. If the final expression is _not_ a reduction,
it can be _collected_ into any of the STL collections.
All expressions are lazily applied and compiles down to 
optimal for loops*. This is a work in progress, and I'll
add functionality as I need it in other projects.

&#8291;* That's the idea anyway, but take it with a grain of salt. 
I am no C++ expert, and have not verified this claim.

## Installation
The library is header-only (so far). Simply clone the
repo and include `colex.hpp` in your code. Everything
you need is found under the `colex` namespace.

## Usage
The library overloads the `|` operator
to chain expressions, apply them to collections,
and collect them into other collections. In the following
examples, we assume that `using namespace colex;` is
present.

### Square
In this example we square all elements of a vector
using `map` and then `collect` them to another vector.
```cpp
std::vector<int> xs { 1, 2, 3 };

auto ys = xs
        | map([](int x) { return x * x; }) 
        | collect<std::vector>();

// ys == std::vector<int> { 1, 4, 9 };
```

### Square and sum
In this example we square all elements of a vector using `map`
and sum them using `fold`. We also show that expressions can
be combined in advance before applying them to a collection.
```cpp
std::vector<int> xs { 1, 2, 3 };

auto square = map([](int x) { return x * x; });
auto sum = fold(0, [](int acc, int x) { return acc + x; });

auto sum_of_squares = square | sum;

auto y = xs | sum_of_squares;
// y == 1 + 4 + 9 (type: int)
```

### Avoiding copies
If the elements of our collections are large, we should
avoid copying. This can be achieved by either referencing
the elements in the first expression, or moving them
by passing the collection as an `rvalue`.

Using references:
```cpp
std::vector<BigThing> xs = some_big_things();

// The transformation takes BigThing by const reference.
auto transformation = map([](const BigThing &x) { return do_something(x); });

auto ys = xs | transformation | collect<std::vector>();
```

Using moves:
```cpp
std::vector<BigThing> xs = some_big_things();

// The transformation takes BigThing by value
auto transformation = map([](BigThing x) { return do_something_else(x); });

auto ys = std::move(xs) | transformation | collect<std::vector>();

// We can do the same by passing collection directly from the function
auto zs = some_big_things() | transformation | collect<std::vector>();
```

### Converting Between Collections
In this example we filter duplicates of a vector by first collecting
to a set, then back to another vector. 
While the example immediately collects to another type of container,
this can of course also be done after applying some expressions.
```cpp
std::vector<int> xs { 1, 1, 2, 2, 3, 3 };

// We use std::move to avoid copies.
std::set<int> set = std::move(xs) | collect<std::set>();
std::vector<int> uniques = std::move(set) | collect<std::vector>();

// Can also be done in a single operation
std::vector<int> uniques = std::move(xs) 
        | collect<std::set>() 
        | collect<std::vector>();

// In both cases: uniques == { 1, 2, 3 }
```

## Available Expressions
### `map(F func)`
Applies a function `F: T x -> U` to all incoming elements.

This example doubles all elements of `xs`.
```cpp
std::vector<int> xs = {1, 2, 3};
auto ys = xs | map([](int x) { 2 * x; }) | collect<std::vector>();

// ys == std::vector<int> {2, 4, 6}
```
  
###`fold(U initial, F func)` 
Reduces all incoming elements to a single value.
Applies a function `F: U acc, T x -> U` where `acc` is the accumulated
value so far, and `x` is the next incoming element. The returned value
is used as `acc` for the next element. For the first element,
`acc` is set to `initial`.

This example sums all elements of `xs`.
```cpp
std::vector<int> xs = {1, 2, 3};
auto y = xs | fold(0, [](int acc, int x) { return acc + x; });

// y == 6
```

## Supported Containers
* `std::vector`
* `std::set`
