export module heap;

import gl;
import <vector>;

export namespace heap {
struct buffer_object;
struct handle {
  buffer_object *parent;
  GLuint offset, size;
};
struct smart_handle {
  handle h;
  ~smart_handle();
};
struct buffer_object {
  GLuint ID;
  static constexpr auto SIZE = 0xffffff;

  struct free_block {
    GLuint offset, size;
    free_block *prev, *next;
  };
  free_block *begin, *end;

  ~buffer_object();

  free_block *allocate(const GLuint size);
  void free(handle &block);
};
struct allocator {
  std::vector<buffer_object> buffers;
};
} // namespace heap