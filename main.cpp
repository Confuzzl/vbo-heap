import debug;
import vbo_heap;

int main() {
  VBOHeap heap{};
  // heap.vbos[0].freeList.begin->str();
  VBOHeap::VBOHandle v = heap.get(1);
  // heap.print();
}
