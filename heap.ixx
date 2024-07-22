export module heap;

import gl;
import <vector>;
import <memory>;
import <list>;

import debug;

export namespace heap {
struct buffer_object;
struct raw_handle {
  buffer_object *parent = nullptr;
  GLuint offset = 0, size = 0;

  raw_handle() = default;
  raw_handle(buffer_object *parent, const GLuint offset, const GLuint size)
      : parent{parent}, offset{offset}, size{size} {}
  ~raw_handle();
  // raw_handle(const raw_handle &) = delete;
  // raw_handle(raw_handle &&) = default;
  // raw_handle &operator=(const raw_handle &) = delete;
  // raw_handle &operator=(raw_handle &&) = default;
};
struct raw_vbo_handle : raw_handle {
  GLuint count = 0, vertexSize = 0;

  raw_vbo_handle() = default;
  raw_vbo_handle(buffer_object *parent, const GLuint offset, const GLuint size,
                 const GLuint count, const GLuint vertexSize)
      : raw_handle(parent, offset, size), count{count}, vertexSize{vertexSize} {
  }
};
struct raw_ebo_handle : raw_handle {
  using raw_handle::raw_handle;

  void write(const std::initializer_list<GLuint> &indices) {}
};

using vbo_handle = std::unique_ptr<raw_vbo_handle>;
using ebo_handle = std::unique_ptr<raw_ebo_handle>;

struct buffer_object {
  static constexpr auto SIZE = 32u;

  GLuint ID;

  struct free_block {
    GLuint offset, size;
  };
  using free_list = std::list<free_block>;
  free_list freeList{{0, SIZE}};

  void free(const raw_handle *handle);
  void coalesce(const free_list::iterator &block);
  void coalesceRight(const free_list::iterator &block);

  void print();
};
template <typename T>
concept is_buffer_object = requires(T t) {
  { t.print() } -> std::same_as<void>;
};

struct vbo : buffer_object {
  template <typename T> vbo_handle allocate(const GLuint count) {
    const GLuint size = count * static_cast<GLuint>(sizeof(T));
    if (size > SIZE)
      return {};
    for (auto current = freeList.begin(); current != freeList.cend();
         current++) {
      if (size > current->size)
        continue;

      const auto newSize = current->size - size;

      auto out = std::make_unique<raw_vbo_handle>(
          this, current->offset, size, count, static_cast<GLuint>(sizeof(T)));
      if (newSize == 0) {
        freeList.erase(current);
      } else {
        current->offset += size;
        current->size = newSize;
      }
      println("[{}, {}]", out->offset, size);
      println("<{}, {}>", current->offset, current->size);
      return out;
    }
    return {};
  }
};

struct ebo : buffer_object {
  ebo_handle allocate(const std::initializer_list<GLuint> &indices);
};

template <is_buffer_object T> struct allocator {
  std::vector<T> buffers;

  allocator() { buffers.reserve(8); }

  void init() { buffers.emplace_back(); }

  void print() {
    for (T &buffer : buffers) {
      buffer.print();
    }
  }
};
struct vbo_allocator : allocator<vbo> {
  template <typename T> vbo_handle get(const GLuint count) {
    for (auto &buffer : buffers) {
      if (auto out = buffer.allocate<T>(count); out)
        return out;
    }
    return buffers.emplace_back().allocate<T>(count);
  }
};
struct ebo_allocator : allocator<ebo> {
  ebo_handle get(const std::initializer_list<GLuint> &indices);
};
} // namespace heap