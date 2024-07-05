module vbo_heap;

VBOHeap::VBOHandle::~VBOHandle() { data->chunk->free(data); }

VBOHeap::VBO::free_list::free_list() {
  free_block *block = new free_block{nullptr, nullptr, 0, BLOCK_SIZE};
  begin = block;
  end = block;
}
VBOHeap::VBO::free_list::~free_list() {
  free_block *current = begin;
  while (current) {
    println(current->str());
    free_block *next = current->next;
    delete current;
    current = next;
  }
}

VBOHeap::VBO::free_list::free_block *
VBOHeap::VBO::free_list::setBegin(free_block *block) {
  block->prev = nullptr;
  block->next = begin;
  begin->prev = block;
  begin = block;
  return block;
}
VBOHeap::VBO::free_list::free_block *
VBOHeap::VBO::free_list::setEnd(free_block *block) {
  block->next = nullptr;
  block->prev = end;
  end->next = block;
  end = block;
  return block;
}

VBOHeap::VBO::free_list::free_block *
VBOHeap::VBO::free_list::prepend(const GLintptr offset, const GLsizeiptr size) {
  return setBegin(new free_block{nullptr, nullptr, offset, size});
}
VBOHeap::VBO::free_list::free_block *
VBOHeap::VBO::free_list::append(const GLintptr offset, const GLsizeiptr size) {
  return setEnd(new free_block{end, nullptr, offset, size});
}
VBOHeap::VBO::free_list::free_block *
VBOHeap::VBO::free_list::insertAfter(free_block *before, const GLintptr offset,
                                     const GLsizeiptr size) {
  if (before == end) {
    return setEnd(new free_block{nullptr, nullptr, offset, size});
  }

  free_block *block = new free_block{nullptr, nullptr, offset, size};

  before->next = block;
  block->prev = before;
  free_block *after = before->next;
  block->next = after;
  after->prev = block;

  return block;
}
VBOHeap::VBO::free_list::free_block *
VBOHeap::VBO::free_list::insertBefore(free_block *after, const GLintptr offset,
                                      const GLsizeiptr size) {
  free_block *block = new free_block{nullptr, nullptr, offset, size};
  if (after == begin) {
    setBegin(block);
  } else {
    after->prev = block;
    block->next = after;
    free_block *before = after->prev;
    block->prev = before;
    before->next = block;
  }
  return block;
}

VBOHeap::VBO::free_list::free_block *
VBOHeap::VBO::free_list::coalesce(free_block *block) {
  free_block *before = block->prev, *after = block->next;

  // coalesce left
  if (before && before->offset + before->size == block->offset) {
    block->offset = before->offset;
    block->size += before->size;
    if (before == begin) {
      block->prev = nullptr;
      begin = block;
    } else {
      before->prev->next = block;
    }
    delete before;
  }

  // coalesce right
  if (after && block->offset + block->size == after->offset) {
    block->size += after->size;
    if (after == end) {
      block->next = nullptr;
      end = block;
    } else {
      after->next->prev = block;
    }
    delete after;
  }
  return block;
}

void VBOHeap::VBO::free_list::popFront() { remove(begin); }
void VBOHeap::VBO::free_list::popBack() { remove(end); }
void VBOHeap::VBO::free_list::remove(free_block *block) {
  free_block *prev = block->prev, *next = block->next;
  if (block == begin) {
    setBegin(next);
  } else if (block == end) {
    setEnd(prev);
  } else {
    prev->next = next;
    next->prev = prev;
  }
  delete block;
}

VBOHeap::VBO::VBO() : freeList{} { println("VBO CREATED"); }

VBOHeap::raw_vbo_handle *VBOHeap::VBO::get(const GLsizeiptr size) {
  free_list::free_block *current = freeList.begin;
  while (current) {
    GLsizeiptr newSize = current->size - size;
    if (newSize < 0) {
      current = current->next;
      continue;
    }

    raw_vbo_handle *handle = new raw_vbo_handle{this, current->offset, size};
    if (newSize == 0) {
      freeList.remove(current);
    } else {
      current->offset += size;
      current->size = newSize;
    }
    return handle;
  }
  return nullptr; // no frees
}
void VBOHeap::VBO::free(raw_vbo_handle *handle) {
  println("freeing {}", handle->str());
  const auto [chunk, offset, size] = *handle;
  free_list::free_block *before = freeList.begin;
  // insert new free after free with the largest offset smaller than handle
  // offset
  while (before->next && before->next->offset < offset) {
    before = before->next;
  }
  free_list::free_block *block = freeList.insertBefore(before, offset, size);
  freeList.coalesce(block);
}

VBOHeap::VBOHeap() : vbos{1} {}

VBOHeap::VBOHandle VBOHeap::get(const GLsizeiptr size) {
  if (size <= 0 || size > BLOCK_SIZE)
    return {};
  for (VBO &vbo : vbos) {
    raw_vbo_handle *handle = vbo.get(size);
    if (handle)
      return {handle};
  }
  VBO &newVBO = vbos.emplace_back();
  return {newVBO.get(size)};
}