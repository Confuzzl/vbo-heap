export module heap;

import gl;
import <vector>;
import <memory>;
import <list>;

export namespace heap {
struct buffer_object;
struct raw_handle {
  buffer_object *parent;
  GLuint offset, size;

  ~raw_handle();
};

using handle_ptr = std::unique_ptr<raw_handle>;
struct handle : handle_ptr {
  handle() = default;
  handle(handle_ptr &&ptr) : handle_ptr(std::move(ptr)) {}

  handle(const handle &) = delete;
  handle(handle &&) = default;
  handle &operator=(const handle &) = delete;
  handle &operator=(handle &&) = default;
};

struct buffer_object {
  static constexpr auto SIZE = 16u;

  GLuint ID;

  struct free_block {
    GLuint offset, size;
  };
  using free_list = std::list<free_block>;
  free_list freeList{{0, SIZE}};

  handle allocate(const GLuint size);
  void free(const raw_handle *handle);
  void coalesce(const free_list::iterator &block);
  void coalesceRight(const free_list::iterator &block);

  void print();
};
struct allocator {
  std::vector<buffer_object> buffers;

  allocator() { buffers.reserve(10); }

  void init() { buffers.emplace_back(); }

  handle get(const GLuint size);

  void print();
};
} // namespace heap