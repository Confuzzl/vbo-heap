export module vbo_heap;

import gl;
import debug;
import <vector>;
import <memory>;
import <forward_list>;
import <optional>;

export struct VBOHeap {
  static constexpr GLsizeiptr BLOCK_SIZE = 64;

  struct VBO;

  struct raw_vbo_handle {
    VBO *chunk = nullptr;
    GLintptr offset = 0;
    GLsizeiptr size = 0;

    // raw_vbo_handle() { println("empty handle"); }
    // raw_vbo_handle(VBO *chunk, const GLintptr offset, const GLsizeiptr size)
    //     : chunk{chunk}, offset{offset}, size{size} {
    //   println("real handle");
    // }
    //~raw_vbo_handle();

    std::string str() const {
      return std::format("{:x} {{\n"
                         "\tchunk= {:x}\n"
                         "\toffset = {}, size = {}\n"
                         "}}\n",
                         reinterpret_cast<intptr_t>(this),
                         reinterpret_cast<intptr_t>(chunk), offset, size);
    }
  };
  struct VBOHandle {
    raw_vbo_handle *data;
    ~VBOHandle();
  };

  struct VBO {
    GLuint ID;

    struct free_list {
      struct free_block {
        free_block *prev = nullptr, *next = nullptr;
        GLintptr offset;
        GLsizeiptr size;

        std::string str() const {
          return std::format("^ {}\n"
                             "[{}]\n"
                             "|offset: {}|\n"
                             "|size:   {}|\n"
                             "v {}",
                             reinterpret_cast<intptr_t>(prev),
                             reinterpret_cast<intptr_t>(this), offset, size,
                             reinterpret_cast<intptr_t>(next));
          // return std::format("{:x} {{\n"
          //                    "\tprev = {:x}, next = {:x}\n"
          //                    "\toffset = {}, size = {}\n"
          //                    "}}\n",
          //                    reinterpret_cast<intptr_t>(this),
          //                    reinterpret_cast<intptr_t>(prev),
          //                    reinterpret_cast<intptr_t>(next), offset, size);
        }
      };

      free_block *begin = nullptr, *end = nullptr;

      free_list();
      ~free_list();

      void setBegin(free_block *block);
      void setEnd(free_block *block);

      free_block *prepend(const GLintptr offset, const GLsizeiptr size);
      free_block *append(const GLintptr offset, const GLsizeiptr size);
      free_block *insertAfter(free_block *before, const GLintptr offset,
                              const GLsizeiptr size);
      free_block *insertBefore(free_block *after, const GLintptr offset,
                               const GLsizeiptr size);

      free_block *coalesce(free_block *block);

      void popFront();
      void popBack();
      void remove(free_block *block);

      void print() const {
        free_block *current = begin;
        while (current) {
          println(current->str());
          current = current->next;
        }
      }
    } freeList;

    VBO();

    raw_vbo_handle *get(const GLsizeiptr size);
    void free(raw_vbo_handle *handle);

    void print() const { freeList.print(); }
  };

  std::vector<VBO> vbos;

  VBOHeap();
  void print() {
    for (const VBO &vbo : vbos)
      vbo.print();
  }

  VBOHandle get(const GLsizeiptr size);
};