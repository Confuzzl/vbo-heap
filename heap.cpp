module heap;

using namespace heap;

import debug;

raw_handle::~raw_handle() {
  if (parent) {
    parent->free(this);
  }
}

handle buffer_object::allocate(const GLuint size) {
  if (size > SIZE)
    return {};
  for (auto current = freeList.begin(); current != freeList.cend(); current++) {
    if (size > current->size)
      continue;

    const auto newSize = current->size - size;

    handle out = std::make_unique<raw_handle>(this, current->offset, size);
    if (newSize == 0) {
      freeList.erase(current);
    } else {
      current->offset += size;
      current->size = newSize;
    }
    // println("[{}, {}]", out->offset, size);
    // println("<{}, {}>", current->offset, current->size);
    return out;
  }
  return {};
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

  for (auto i = 4; i < out.size(); i += 5) {
    out.insert(out.cbegin() + i, ' ');
  }

  println("[{}]", out);
}

handle allocator::get(const GLuint size) {
  for (auto &buffer : buffers) {
    if (handle out = buffer.allocate(size); out)
      return out;
  }
  return buffers.emplace_back().allocate(size);
}
void allocator::print() {
  for (auto &buffer : buffers) {
    buffer.print();
  }
}
