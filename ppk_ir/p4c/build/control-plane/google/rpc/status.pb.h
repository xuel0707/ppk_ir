// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/rpc/status.proto

#ifndef PROTOBUF_INCLUDED_google_2frpc_2fstatus_2eproto
#define PROTOBUF_INCLUDED_google_2frpc_2fstatus_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include <google/protobuf/any.pb.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_google_2frpc_2fstatus_2eproto 

namespace protobuf_google_2frpc_2fstatus_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_google_2frpc_2fstatus_2eproto
namespace google {
namespace rpc {
class Status;
class StatusDefaultTypeInternal;
extern StatusDefaultTypeInternal _Status_default_instance_;
}  // namespace rpc
}  // namespace google
namespace google {
namespace protobuf {
template<> ::google::rpc::Status* Arena::CreateMaybeMessage<::google::rpc::Status>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace google {
namespace rpc {

// ===================================================================

class Status : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:google.rpc.Status) */ {
 public:
  Status();
  virtual ~Status();

  Status(const Status& from);

  inline Status& operator=(const Status& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  Status(Status&& from) noexcept
    : Status() {
    *this = ::std::move(from);
  }

  inline Status& operator=(Status&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const Status& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Status* internal_default_instance() {
    return reinterpret_cast<const Status*>(
               &_Status_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(Status* other);
  friend void swap(Status& a, Status& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline Status* New() const final {
    return CreateMaybeMessage<Status>(NULL);
  }

  Status* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<Status>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const Status& from);
  void MergeFrom(const Status& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Status* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .google.protobuf.Any details = 3;
  int details_size() const;
  void clear_details();
  static const int kDetailsFieldNumber = 3;
  ::google::protobuf::Any* mutable_details(int index);
  ::google::protobuf::RepeatedPtrField< ::google::protobuf::Any >*
      mutable_details();
  const ::google::protobuf::Any& details(int index) const;
  ::google::protobuf::Any* add_details();
  const ::google::protobuf::RepeatedPtrField< ::google::protobuf::Any >&
      details() const;

  // string message = 2;
  void clear_message();
  static const int kMessageFieldNumber = 2;
  const ::std::string& message() const;
  void set_message(const ::std::string& value);
  #if LANG_CXX11
  void set_message(::std::string&& value);
  #endif
  void set_message(const char* value);
  void set_message(const char* value, size_t size);
  ::std::string* mutable_message();
  ::std::string* release_message();
  void set_allocated_message(::std::string* message);

  // int32 code = 1;
  void clear_code();
  static const int kCodeFieldNumber = 1;
  ::google::protobuf::int32 code() const;
  void set_code(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:google.rpc.Status)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::google::protobuf::Any > details_;
  ::google::protobuf::internal::ArenaStringPtr message_;
  ::google::protobuf::int32 code_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_google_2frpc_2fstatus_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Status

// int32 code = 1;
inline void Status::clear_code() {
  code_ = 0;
}
inline ::google::protobuf::int32 Status::code() const {
  // @@protoc_insertion_point(field_get:google.rpc.Status.code)
  return code_;
}
inline void Status::set_code(::google::protobuf::int32 value) {
  
  code_ = value;
  // @@protoc_insertion_point(field_set:google.rpc.Status.code)
}

// string message = 2;
inline void Status::clear_message() {
  message_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Status::message() const {
  // @@protoc_insertion_point(field_get:google.rpc.Status.message)
  return message_.GetNoArena();
}
inline void Status::set_message(const ::std::string& value) {
  
  message_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:google.rpc.Status.message)
}
#if LANG_CXX11
inline void Status::set_message(::std::string&& value) {
  
  message_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:google.rpc.Status.message)
}
#endif
inline void Status::set_message(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  message_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:google.rpc.Status.message)
}
inline void Status::set_message(const char* value, size_t size) {
  
  message_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:google.rpc.Status.message)
}
inline ::std::string* Status::mutable_message() {
  
  // @@protoc_insertion_point(field_mutable:google.rpc.Status.message)
  return message_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Status::release_message() {
  // @@protoc_insertion_point(field_release:google.rpc.Status.message)
  
  return message_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Status::set_allocated_message(::std::string* message) {
  if (message != NULL) {
    
  } else {
    
  }
  message_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), message);
  // @@protoc_insertion_point(field_set_allocated:google.rpc.Status.message)
}

// repeated .google.protobuf.Any details = 3;
inline int Status::details_size() const {
  return details_.size();
}
inline ::google::protobuf::Any* Status::mutable_details(int index) {
  // @@protoc_insertion_point(field_mutable:google.rpc.Status.details)
  return details_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::google::protobuf::Any >*
Status::mutable_details() {
  // @@protoc_insertion_point(field_mutable_list:google.rpc.Status.details)
  return &details_;
}
inline const ::google::protobuf::Any& Status::details(int index) const {
  // @@protoc_insertion_point(field_get:google.rpc.Status.details)
  return details_.Get(index);
}
inline ::google::protobuf::Any* Status::add_details() {
  // @@protoc_insertion_point(field_add:google.rpc.Status.details)
  return details_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::google::protobuf::Any >&
Status::details() const {
  // @@protoc_insertion_point(field_list:google.rpc.Status.details)
  return details_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace rpc
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_google_2frpc_2fstatus_2eproto
