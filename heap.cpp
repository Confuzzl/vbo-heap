module heap;

using namespace heap;

import debug;

raw_handle::~raw_handle() {
  if (parent) {
    println("deleting handle");
    parent->free(this);
  }
}

void buffer_object::free(const raw_handle *handle) {
  auto after = freeList.cbegin();
  while (after != freeList.cend() &&
         handle->offset + handle->size > after->offset) {
    after++;
  }
  coalesce(freeList.insert(after, {handle->offset, handle->size}));
}
void buffer_object::coalesce(const free_list::iterator &block) {
  coalesceRight(block);
  if (block != freeList.cbegin())
    coalesceRight(std::prev(block));
}
void buffer_object::coalesceRight(const free_list::iterator &block) {
  auto next = std::next(block);
  if (block->offset + block->size == next->offset) {
    block->size += next->size;
    freeList.erase(next);
  }
}
void buffer_object::print() {
  std::string list = "";
  for (const auto &free : freeList)
    list += std::format("<{}, {}>", free.offset, free.size);
  println(list);

  std::string out = "";
  out += std::string(freeList.front().offset, 'X');
  for (auto free = freeList.cbegin(); free != freeList.cend(); free++) {
    out += std::string(free->size, '.');
    const auto next = std::next(free);
    const auto nextOffset = next == freeList.cend() ? SIZE : next->offset;
    out += std::string(nextOffset - (free->offset + free->size), 'X');
  }

  static constexpr auto GROUP = 8u;
  for (auto i = GROUP; i < out.size(); i += GROUP + 1) {
    out.insert(out.cbegin() + i, ' ');
  }

  println("[{}]", out);
}

ebo_handle ebo::allocate(const std::initializer_list<GLuint> &indices) {
  const GLuint size = static_cast<GLuint>(indices.size() * sizeof(GLuint));
  if (size > SIZE)
    return {};
  for (auto current = freeList.begin(); current != freeList.cend(); current++) {
    if (size > current->size)
      continue;

    const auto newSize = current->size - size;

    ebo_handle out =
        std::make_unique<raw_ebo_handle>(this, current->offset, size);
    out->write(indices);
    if (newSize == 0) {
      freeList.erase(current);
    } else {
      current->offset += size;
      current->size = newSize;
    }
    return out;
  }
  return {};
}

ebo_handle ebo_allocator::get(const std::initializer_list<GLuint> &indices) {
  for (auto &buffer : buffers) {
    if (auto out = buffer.allocate(indices); out)
      return out;
  }
  return buffers.emplace_back().allocate(indices);
}