#include "colex.hpp"

namespace colex {

expression::Take take(size_t count) {
  return expression::Take(count);
}

expression::Drop drop(size_t count) {
  return expression::Drop(count);
}

expression::Enumerate enumerate() {
  return expression::Enumerate();
}

expression::Composition<expression::Drop, expression::Take> slice(size_t start, size_t count) {
  return expression::Composition<expression::Drop, expression::Take>(drop(start), take(count));
}

expression::Chunk chunk(size_t size) {
  return expression::Chunk(size);
}

expression::Partition partition(std::vector<size_t> partition_sizes) {
  return expression::Partition(std::move(partition_sizes));
}

}