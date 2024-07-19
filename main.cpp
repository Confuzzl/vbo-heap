import debug;
import vbo_heap;
import stack;
import heap;

int main() {
  // VBOHeap heap{};
  // std::vector<VBOHeap::VBOHandle> handles{};
  // handles.reserve(100);
  // handles.emplace_back(heap.get(2));
  // handles.emplace_back(heap.get(3));
  //// handles.emplace_back(heap.get(2));
  // heap.print();
  //// println(handles[0].str());
  // println("ERASING");
  // handles.erase(handles.begin());
  // heap.print();
  //// const auto a = heap.get(15);
  //// heap.print();
  ////{
  ////   const auto b = heap.get(4);
  ////   heap.print();
  //// }
  //// heap.print();

  heap::allocator heap{};
  heap.init();
  //{
  std::vector<heap::handle> handles{};
  handles.reserve(10);
  handles.emplace_back(heap.get(3));
  handles.emplace_back(heap.get(2));
  heap.print();
  handles.erase(handles.cbegin());
  heap.print();
  handles.erase(handles.cbegin());
  heap.print();

  //}
  // heap.print();
}