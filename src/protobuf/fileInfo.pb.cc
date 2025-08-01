// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: fileInfo.proto

#include "fileInfo.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

PROTOBUF_CONSTEXPR fileInfo::fileInfo(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.action_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.user_dir_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.file_name_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.size_)*/int64_t{0}
  , /*decltype(_impl_.port_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct fileInfoDefaultTypeInternal {
  PROTOBUF_CONSTEXPR fileInfoDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~fileInfoDefaultTypeInternal() {}
  union {
    fileInfo _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 fileInfoDefaultTypeInternal _fileInfo_default_instance_;
static ::_pb::Metadata file_level_metadata_fileInfo_2eproto[1];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_fileInfo_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_fileInfo_2eproto = nullptr;

const uint32_t TableStruct_fileInfo_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::fileInfo, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::fileInfo, _impl_.action_),
  PROTOBUF_FIELD_OFFSET(::fileInfo, _impl_.user_dir_),
  PROTOBUF_FIELD_OFFSET(::fileInfo, _impl_.file_name_),
  PROTOBUF_FIELD_OFFSET(::fileInfo, _impl_.port_),
  PROTOBUF_FIELD_OFFSET(::fileInfo, _impl_.size_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::fileInfo)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::_fileInfo_default_instance_._instance,
};

const char descriptor_table_protodef_fileInfo_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016fileInfo.proto\"[\n\010fileInfo\022\016\n\006action\030\001"
  " \001(\t\022\020\n\010user_dir\030\002 \001(\t\022\021\n\tfile_name\030\003 \001("
  "\t\022\014\n\004port\030\004 \001(\005\022\014\n\004size\030\005 \001(\003b\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_fileInfo_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_fileInfo_2eproto = {
    false, false, 117, descriptor_table_protodef_fileInfo_2eproto,
    "fileInfo.proto",
    &descriptor_table_fileInfo_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_fileInfo_2eproto::offsets,
    file_level_metadata_fileInfo_2eproto, file_level_enum_descriptors_fileInfo_2eproto,
    file_level_service_descriptors_fileInfo_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_fileInfo_2eproto_getter() {
  return &descriptor_table_fileInfo_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_fileInfo_2eproto(&descriptor_table_fileInfo_2eproto);

// ===================================================================

class fileInfo::_Internal {
 public:
};

fileInfo::fileInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:fileInfo)
}
fileInfo::fileInfo(const fileInfo& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  fileInfo* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.action_){}
    , decltype(_impl_.user_dir_){}
    , decltype(_impl_.file_name_){}
    , decltype(_impl_.size_){}
    , decltype(_impl_.port_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.action_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.action_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_action().empty()) {
    _this->_impl_.action_.Set(from._internal_action(), 
      _this->GetArenaForAllocation());
  }
  _impl_.user_dir_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.user_dir_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_user_dir().empty()) {
    _this->_impl_.user_dir_.Set(from._internal_user_dir(), 
      _this->GetArenaForAllocation());
  }
  _impl_.file_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.file_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_file_name().empty()) {
    _this->_impl_.file_name_.Set(from._internal_file_name(), 
      _this->GetArenaForAllocation());
  }
  ::memcpy(&_impl_.size_, &from._impl_.size_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.port_) -
    reinterpret_cast<char*>(&_impl_.size_)) + sizeof(_impl_.port_));
  // @@protoc_insertion_point(copy_constructor:fileInfo)
}

inline void fileInfo::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.action_){}
    , decltype(_impl_.user_dir_){}
    , decltype(_impl_.file_name_){}
    , decltype(_impl_.size_){int64_t{0}}
    , decltype(_impl_.port_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.action_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.action_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.user_dir_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.user_dir_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.file_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.file_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

fileInfo::~fileInfo() {
  // @@protoc_insertion_point(destructor:fileInfo)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void fileInfo::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.action_.Destroy();
  _impl_.user_dir_.Destroy();
  _impl_.file_name_.Destroy();
}

void fileInfo::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void fileInfo::Clear() {
// @@protoc_insertion_point(message_clear_start:fileInfo)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.action_.ClearToEmpty();
  _impl_.user_dir_.ClearToEmpty();
  _impl_.file_name_.ClearToEmpty();
  ::memset(&_impl_.size_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.port_) -
      reinterpret_cast<char*>(&_impl_.size_)) + sizeof(_impl_.port_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* fileInfo::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // string action = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_action();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "fileInfo.action"));
        } else
          goto handle_unusual;
        continue;
      // string user_dir = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 18)) {
          auto str = _internal_mutable_user_dir();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "fileInfo.user_dir"));
        } else
          goto handle_unusual;
        continue;
      // string file_name = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          auto str = _internal_mutable_file_name();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "fileInfo.file_name"));
        } else
          goto handle_unusual;
        continue;
      // int32 port = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 32)) {
          _impl_.port_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 size = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 40)) {
          _impl_.size_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* fileInfo::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:fileInfo)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // string action = 1;
  if (!this->_internal_action().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_action().data(), static_cast<int>(this->_internal_action().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "fileInfo.action");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_action(), target);
  }

  // string user_dir = 2;
  if (!this->_internal_user_dir().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_user_dir().data(), static_cast<int>(this->_internal_user_dir().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "fileInfo.user_dir");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_user_dir(), target);
  }

  // string file_name = 3;
  if (!this->_internal_file_name().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_file_name().data(), static_cast<int>(this->_internal_file_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "fileInfo.file_name");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_file_name(), target);
  }

  // int32 port = 4;
  if (this->_internal_port() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(4, this->_internal_port(), target);
  }

  // int64 size = 5;
  if (this->_internal_size() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt64ToArray(5, this->_internal_size(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:fileInfo)
  return target;
}

size_t fileInfo::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:fileInfo)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string action = 1;
  if (!this->_internal_action().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_action());
  }

  // string user_dir = 2;
  if (!this->_internal_user_dir().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_user_dir());
  }

  // string file_name = 3;
  if (!this->_internal_file_name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_file_name());
  }

  // int64 size = 5;
  if (this->_internal_size() != 0) {
    total_size += ::_pbi::WireFormatLite::Int64SizePlusOne(this->_internal_size());
  }

  // int32 port = 4;
  if (this->_internal_port() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_port());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData fileInfo::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    fileInfo::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*fileInfo::GetClassData() const { return &_class_data_; }


void fileInfo::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<fileInfo*>(&to_msg);
  auto& from = static_cast<const fileInfo&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:fileInfo)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_action().empty()) {
    _this->_internal_set_action(from._internal_action());
  }
  if (!from._internal_user_dir().empty()) {
    _this->_internal_set_user_dir(from._internal_user_dir());
  }
  if (!from._internal_file_name().empty()) {
    _this->_internal_set_file_name(from._internal_file_name());
  }
  if (from._internal_size() != 0) {
    _this->_internal_set_size(from._internal_size());
  }
  if (from._internal_port() != 0) {
    _this->_internal_set_port(from._internal_port());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void fileInfo::CopyFrom(const fileInfo& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:fileInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool fileInfo::IsInitialized() const {
  return true;
}

void fileInfo::InternalSwap(fileInfo* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.action_, lhs_arena,
      &other->_impl_.action_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.user_dir_, lhs_arena,
      &other->_impl_.user_dir_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.file_name_, lhs_arena,
      &other->_impl_.file_name_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(fileInfo, _impl_.port_)
      + sizeof(fileInfo::_impl_.port_)
      - PROTOBUF_FIELD_OFFSET(fileInfo, _impl_.size_)>(
          reinterpret_cast<char*>(&_impl_.size_),
          reinterpret_cast<char*>(&other->_impl_.size_));
}

::PROTOBUF_NAMESPACE_ID::Metadata fileInfo::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_fileInfo_2eproto_getter, &descriptor_table_fileInfo_2eproto_once,
      file_level_metadata_fileInfo_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::fileInfo*
Arena::CreateMaybeMessage< ::fileInfo >(Arena* arena) {
  return Arena::CreateMessageInternal< ::fileInfo >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
