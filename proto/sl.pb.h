// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sl.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_sl_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_sl_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_sl_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_sl_2eproto {
  static const uint32_t offsets[];
};
namespace proto {
class Node;
struct NodeDefaultTypeInternal;
extern NodeDefaultTypeInternal _Node_default_instance_;
class Skiplist;
struct SkiplistDefaultTypeInternal;
extern SkiplistDefaultTypeInternal _Skiplist_default_instance_;
}  // namespace proto
PROTOBUF_NAMESPACE_OPEN
template<> ::proto::Node* Arena::CreateMaybeMessage<::proto::Node>(Arena*);
template<> ::proto::Skiplist* Arena::CreateMaybeMessage<::proto::Skiplist>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace proto {

// ===================================================================

class Node final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:proto.Node) */ {
 public:
  inline Node() : Node(nullptr) {}
  ~Node() override;
  explicit PROTOBUF_CONSTEXPR Node(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Node(const Node& from);
  Node(Node&& from) noexcept
    : Node() {
    *this = ::std::move(from);
  }

  inline Node& operator=(const Node& from) {
    CopyFrom(from);
    return *this;
  }
  inline Node& operator=(Node&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const Node& default_instance() {
    return *internal_default_instance();
  }
  static inline const Node* internal_default_instance() {
    return reinterpret_cast<const Node*>(
               &_Node_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Node& a, Node& b) {
    a.Swap(&b);
  }
  inline void Swap(Node* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Node* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Node* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Node>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const Node& from);
  void MergeFrom(const Node& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Node* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "proto.Node";
  }
  protected:
  explicit Node(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNextFieldNumber = 3,
    kValueFieldNumber = 2,
    kKeyFieldNumber = 1,
  };
  // repeated int32 next = 3;
  int next_size() const;
  private:
  int _internal_next_size() const;
  public:
  void clear_next();
  private:
  int32_t _internal_next(int index) const;
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >&
      _internal_next() const;
  void _internal_add_next(int32_t value);
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >*
      _internal_mutable_next();
  public:
  int32_t next(int index) const;
  void set_next(int index, int32_t value);
  void add_next(int32_t value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >&
      next() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >*
      mutable_next();

  // string value = 2;
  void clear_value();
  const std::string& value() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_value(ArgT0&& arg0, ArgT... args);
  std::string* mutable_value();
  PROTOBUF_NODISCARD std::string* release_value();
  void set_allocated_value(std::string* value);
  private:
  const std::string& _internal_value() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_value(const std::string& value);
  std::string* _internal_mutable_value();
  public:

  // int32 key = 1;
  void clear_key();
  int32_t key() const;
  void set_key(int32_t value);
  private:
  int32_t _internal_key() const;
  void _internal_set_key(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:proto.Node)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t > next_;
    mutable std::atomic<int> _next_cached_byte_size_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr value_;
    int32_t key_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_sl_2eproto;
};
// -------------------------------------------------------------------

class Skiplist final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:proto.Skiplist) */ {
 public:
  inline Skiplist() : Skiplist(nullptr) {}
  ~Skiplist() override;
  explicit PROTOBUF_CONSTEXPR Skiplist(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Skiplist(const Skiplist& from);
  Skiplist(Skiplist&& from) noexcept
    : Skiplist() {
    *this = ::std::move(from);
  }

  inline Skiplist& operator=(const Skiplist& from) {
    CopyFrom(from);
    return *this;
  }
  inline Skiplist& operator=(Skiplist&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const Skiplist& default_instance() {
    return *internal_default_instance();
  }
  static inline const Skiplist* internal_default_instance() {
    return reinterpret_cast<const Skiplist*>(
               &_Skiplist_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Skiplist& a, Skiplist& b) {
    a.Swap(&b);
  }
  inline void Swap(Skiplist* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Skiplist* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Skiplist* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Skiplist>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const Skiplist& from);
  void MergeFrom(const Skiplist& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Skiplist* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "proto.Skiplist";
  }
  protected:
  explicit Skiplist(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNodesFieldNumber = 1,
  };
  // repeated .proto.Node nodes = 1;
  int nodes_size() const;
  private:
  int _internal_nodes_size() const;
  public:
  void clear_nodes();
  ::proto::Node* mutable_nodes(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::proto::Node >*
      mutable_nodes();
  private:
  const ::proto::Node& _internal_nodes(int index) const;
  ::proto::Node* _internal_add_nodes();
  public:
  const ::proto::Node& nodes(int index) const;
  ::proto::Node* add_nodes();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::proto::Node >&
      nodes() const;

  // @@protoc_insertion_point(class_scope:proto.Skiplist)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::proto::Node > nodes_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_sl_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Node

// int32 key = 1;
inline void Node::clear_key() {
  _impl_.key_ = 0;
}
inline int32_t Node::_internal_key() const {
  return _impl_.key_;
}
inline int32_t Node::key() const {
  // @@protoc_insertion_point(field_get:proto.Node.key)
  return _internal_key();
}
inline void Node::_internal_set_key(int32_t value) {
  
  _impl_.key_ = value;
}
inline void Node::set_key(int32_t value) {
  _internal_set_key(value);
  // @@protoc_insertion_point(field_set:proto.Node.key)
}

// string value = 2;
inline void Node::clear_value() {
  _impl_.value_.ClearToEmpty();
}
inline const std::string& Node::value() const {
  // @@protoc_insertion_point(field_get:proto.Node.value)
  return _internal_value();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Node::set_value(ArgT0&& arg0, ArgT... args) {
 
 _impl_.value_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:proto.Node.value)
}
inline std::string* Node::mutable_value() {
  std::string* _s = _internal_mutable_value();
  // @@protoc_insertion_point(field_mutable:proto.Node.value)
  return _s;
}
inline const std::string& Node::_internal_value() const {
  return _impl_.value_.Get();
}
inline void Node::_internal_set_value(const std::string& value) {
  
  _impl_.value_.Set(value, GetArenaForAllocation());
}
inline std::string* Node::_internal_mutable_value() {
  
  return _impl_.value_.Mutable(GetArenaForAllocation());
}
inline std::string* Node::release_value() {
  // @@protoc_insertion_point(field_release:proto.Node.value)
  return _impl_.value_.Release();
}
inline void Node::set_allocated_value(std::string* value) {
  if (value != nullptr) {
    
  } else {
    
  }
  _impl_.value_.SetAllocated(value, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.value_.IsDefault()) {
    _impl_.value_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:proto.Node.value)
}

// repeated int32 next = 3;
inline int Node::_internal_next_size() const {
  return _impl_.next_.size();
}
inline int Node::next_size() const {
  return _internal_next_size();
}
inline void Node::clear_next() {
  _impl_.next_.Clear();
}
inline int32_t Node::_internal_next(int index) const {
  return _impl_.next_.Get(index);
}
inline int32_t Node::next(int index) const {
  // @@protoc_insertion_point(field_get:proto.Node.next)
  return _internal_next(index);
}
inline void Node::set_next(int index, int32_t value) {
  _impl_.next_.Set(index, value);
  // @@protoc_insertion_point(field_set:proto.Node.next)
}
inline void Node::_internal_add_next(int32_t value) {
  _impl_.next_.Add(value);
}
inline void Node::add_next(int32_t value) {
  _internal_add_next(value);
  // @@protoc_insertion_point(field_add:proto.Node.next)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >&
Node::_internal_next() const {
  return _impl_.next_;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >&
Node::next() const {
  // @@protoc_insertion_point(field_list:proto.Node.next)
  return _internal_next();
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >*
Node::_internal_mutable_next() {
  return &_impl_.next_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< int32_t >*
Node::mutable_next() {
  // @@protoc_insertion_point(field_mutable_list:proto.Node.next)
  return _internal_mutable_next();
}

// -------------------------------------------------------------------

// Skiplist

// repeated .proto.Node nodes = 1;
inline int Skiplist::_internal_nodes_size() const {
  return _impl_.nodes_.size();
}
inline int Skiplist::nodes_size() const {
  return _internal_nodes_size();
}
inline void Skiplist::clear_nodes() {
  _impl_.nodes_.Clear();
}
inline ::proto::Node* Skiplist::mutable_nodes(int index) {
  // @@protoc_insertion_point(field_mutable:proto.Skiplist.nodes)
  return _impl_.nodes_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::proto::Node >*
Skiplist::mutable_nodes() {
  // @@protoc_insertion_point(field_mutable_list:proto.Skiplist.nodes)
  return &_impl_.nodes_;
}
inline const ::proto::Node& Skiplist::_internal_nodes(int index) const {
  return _impl_.nodes_.Get(index);
}
inline const ::proto::Node& Skiplist::nodes(int index) const {
  // @@protoc_insertion_point(field_get:proto.Skiplist.nodes)
  return _internal_nodes(index);
}
inline ::proto::Node* Skiplist::_internal_add_nodes() {
  return _impl_.nodes_.Add();
}
inline ::proto::Node* Skiplist::add_nodes() {
  ::proto::Node* _add = _internal_add_nodes();
  // @@protoc_insertion_point(field_add:proto.Skiplist.nodes)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::proto::Node >&
Skiplist::nodes() const {
  // @@protoc_insertion_point(field_list:proto.Skiplist.nodes)
  return _impl_.nodes_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_sl_2eproto
