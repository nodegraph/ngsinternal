
namespace ngs {

template<class T>
class InterfacePtr {
 public:
  COMPONENT_ID(CompShape, InvalidComponent);

  InterfacePtr(T& ptr):
    _ptr(ptr){}
  ~InterfacePtr(){} // Doesn't delete internal pointer
  T* operator->() const {
    return _ptr;
  }
  T& operator*() const {
    return *_ptr;
  }
 private:
  T* _ptr;
};

}
