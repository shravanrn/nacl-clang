// RUN: %clang_cc1 -emit-llvm -triple %itanium_abi_triple -g %s -o - -fno-standalone-debug | FileCheck %s

// Run again with -gline-tables-only and verify we don't crash.  We won't output
// type info at all.
// RUN: %clang_cc1 -emit-llvm -triple %itanium_abi_triple -g %s -o - -gline-tables-only | FileCheck %s -check-prefix LINES-ONLY

// LINES-ONLY-NOT: !MDCompositeType(tag: DW_TAG_structure_type

template <typename T>
struct a {
};
extern template class a<int>;
// CHECK-NOT: MDCompositeType(tag: DW_TAG_structure_type, name: "a<int>"

template <typename T>
struct b {
};
extern template class b<int>;
b<int> bi;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "b<int>"
// CHECK-NOT: DIFlagFwdDecl
// CHECK-SAME: ){{$}}

template <typename T>
struct c {
  void f() {}
};
extern template class c<int>;
c<int> ci;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "c<int>"
// CHECK-SAME: DIFlagFwdDecl

template <typename T>
struct d {
  void f();
};
extern template class d<int>;
d<int> di;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "d<int>"
// CHECK-NOT: DIFlagFwdDecl
// CHECK-SAME: ){{$}}

template <typename T>
struct e {
  void f();
};
template <typename T>
void e<T>::f() {
}
extern template class e<int>;
e<int> ei;
// There's no guarantee that the out of line definition will appear before the
// explicit template instantiation definition, so conservatively emit the type
// definition here.
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "e<int>"
// CHECK-NOT: DIFlagFwdDecl
// CHECK-SAME: ){{$}}

template <typename T>
struct f {
  void g();
};
extern template class f<int>;
template <typename T>
void f<T>::g() {
}
f<int> fi;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "f<int>"
// CHECK-NOT: DIFlagFwdDecl
// CHECK-SAME: ){{$}}

template <typename T>
struct g {
  void f();
};
template <>
void g<int>::f();
extern template class g<int>;
g<int> gi;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "g<int>"
// CHECK-NOT: DIFlagFwdDecl
// CHECK-SAME: ){{$}}

template <typename T>
struct h {
};
template class h<int>;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "h<int>"
// CHECK-NOT: DIFlagFwdDecl
// CHECK-SAME: ){{$}}

template <typename T>
struct i {
  void f() {}
};
template<> void i<int>::f();
extern template class i<int>;
i<int> ii;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "i<int>"
// CHECK-NOT: DIFlagFwdDecl
// CHECK-SAME: ){{$}}

template <typename T1, typename T2 = T1>
struct j {
};
extern template class j<int>;
j<int> jj;
// CHECK: MDCompositeType(tag: DW_TAG_structure_type, name: "j<int, int>"

template <typename T>
struct k {
};
template <>
struct k<int>;
template struct k<int>;
// CHECK-NOT: !MDCompositeType(tag: DW_TAG_structure_type, name: "k<int>"
