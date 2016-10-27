//===- CodeObject.hpp - ELF object file implementation ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the HSA Code Object file class.
//
//===----------------------------------------------------------------------===//

#ifndef AMDGPU_DISASSEMBLER_HSA_CODE_OBJECT_HPP
#define AMDGPU_DISASSEMBLER_HSA_CODE_OBJECT_HPP

#include "llvm/ADT/StringRef.h"
#include "llvm/Object/ELFObjectFile.h"
#include "AMDKernelCodeT.h"

namespace llvm {

//===----------------------------------------------------------------------===//
// ELFNote
//===----------------------------------------------------------------------===//

struct amdgpu_hsa_code_object_version {
  uint32_t major_version;
  uint32_t minor_version;
};


struct amdgpu_hsa_isa {
  uint16_t vendor_name_size;
  uint16_t architecture_name_size;
  uint32_t major;
  uint32_t minor;
  uint32_t stepping;
  char *names;

  StringRef getVendorName() const {
    return StringRef(names, vendor_name_size);
  }

  StringRef getArchitectureName() const {
    return StringRef(names + vendor_name_size, architecture_name_size);
  }
};


enum AMDGPU_NOTES_TYPES {
  NT_AMDGPU_HSA_CODE_OBJECT_VERSION = 1,
  NT_AMDGPU_HSA_ISA = 3,
};


struct ELFNote {
  uint32_t namesz;
  uint32_t descsz;
  uint32_t type;

  enum {ALIGN = 4};

  ELFNote() = delete;
  ELFNote(const ELFNote&) = delete;
  ELFNote& operator =(const ELFNote&) = delete;

  StringRef getName() const {
    return StringRef(reinterpret_cast<const char*>(this) + sizeof(*this), namesz);
  }

  StringRef getDesc() const {
    return StringRef(getName().data() + alignTo(namesz, ALIGN), descsz);
  }

  size_t getSize() const {
    return sizeof(*this) + alignTo(namesz, ALIGN) + alignTo(descsz, ALIGN);
  }

  template <typename D> ErrorOr<const D*> as() const {
    if (descsz < sizeof(D))
      return object::object_error::parse_failed;

    return reinterpret_cast<const D*>(getDesc().data());
  }
};

const ELFNote* getNext(const ELFNote &N);


template <typename Item>
class const_varsize_item_iterator :
  std::iterator<std::forward_iterator_tag, const Item, void> {
  ArrayRef<uint8_t> Ref;

  const Item *item() const {
    return reinterpret_cast<const Item*>(Ref.data());
  }

  size_t getItemPadSize() const {
    assert(Ref.size() >= sizeof(Item));
    return (const uint8_t*)getNext(*item()) - (const uint8_t*)item();
  }

public:
  const_varsize_item_iterator() {}
  const_varsize_item_iterator(ArrayRef<uint8_t> Ref_) : Ref(Ref_) {}

  bool valid() const {
    return Ref.size() >= sizeof(Item) && Ref.size() >= getItemPadSize();
  }

  ErrorOr<const Item&> operator*() const {
    if (!valid())
      return object::object_error::parse_failed;

    return *item();
  }

  bool operator==(const const_varsize_item_iterator &Other) const {
    return (Ref.size() == Other.Ref.size()) &&
           (Ref.empty() || Ref.data() == Other.Ref.data());
  }

  bool operator!=(const const_varsize_item_iterator &Other) const {
    return !(*this == Other);
  }

  const_varsize_item_iterator &operator++() { // preincrement
    Ref = Ref.size() >= sizeof(Item) ?
      Ref.slice((std::min)(getItemPadSize(), Ref.size())) :
      decltype(Ref)();
    return *this;
  }
};

//===----------------------------------------------------------------------===//
// KernelSym
//===----------------------------------------------------------------------===//

class HSACodeObject;

class KernelSym : public object::ELF64LEObjectFile::Elf_Sym {
public:
  ErrorOr<const amd_kernel_code_t *> getAmdKernelCodeT(
    const HSACodeObject * CodeObject) const;

  static ErrorOr<const KernelSym *> asKernelSym(
    const object::ELF64LEObjectFile::Elf_Sym *Sym);
};


template <typename BaseIterator>
class conditional_iterator : public iterator_adaptor_base<
                                              conditional_iterator<BaseIterator>,
                                              BaseIterator,
                                              std::forward_iterator_tag> {
  
public:
  typedef std::function<bool(const value_type&)> PredicateTy;
  
protected:
  BaseIterator End;
  PredicateTy Predicate;

public:

  conditional_iterator(BaseIterator BI, BaseIterator E, PredicateTy P)
    : iterator_adaptor_base(BI), End(E), Predicate(P) {}

  conditional_iterator &operator++() {
    do {
      ++I;
    } while (I != End && !Predicate(*I));
    return *this;
  }
};

class kernel_sym_iterator : public conditional_iterator<object::elf_symbol_iterator> {
public:
  kernel_sym_iterator(object::elf_symbol_iterator It,
                      object::elf_symbol_iterator End,
                      PredicateTy P)
    : conditional_iterator<object::elf_symbol_iterator>(It, End, P) {}

  const object::ELFSymbolRef &operator*() const {
    return *I;
  }
};

//===----------------------------------------------------------------------===//
// HSACodeObject
//===----------------------------------------------------------------------===//

class HSACodeObject : public object::ELF64LEObjectFile {
private:
  mutable SmallVector<uint64_t, 8> KernelMarkers;

  void InitMarkers() const;

public:
  HSACodeObject(MemoryBufferRef Buffer, std::error_code &EC)
    : object::ELF64LEObjectFile(Buffer, EC) {
    InitMarkers();
  }

  typedef const_varsize_item_iterator<ELFNote> note_iterator;

  note_iterator notes_begin() const;
  note_iterator notes_end() const;
  iterator_range<note_iterator> notes() const;

  kernel_sym_iterator kernels_begin() const;
  kernel_sym_iterator kernels_end() const;
  iterator_range<kernel_sym_iterator> kernels() const;

  ErrorOr<ArrayRef<uint8_t>> getKernelCode(const KernelSym *Kernel) const;

  ErrorOr<const Elf_Shdr *> getSectionByName(StringRef Name) const;

  ErrorOr<uint32_t> getSectionIdxByName(StringRef) const;
  ErrorOr<uint32_t> getTextSectionIdx() const;
  ErrorOr<uint32_t> getNoteSectionIdx() const;
  ErrorOr<const Elf_Shdr *> getTextSection() const;
  ErrorOr<const Elf_Shdr *> getNoteSection() const;
};

} // namespace llvm

#endif