#ifndef _IR_GENERATED_H_
#define _IR_GENERATED_H_

#include <map>
#include <functional>

class JSONLoader;
using NodeFactoryFn = IR::Node*(*)(JSONLoader&);

namespace IR {
extern std::map<cstring, NodeFactoryFn> unpacker_table;
}

#line 16 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
#include "frontends/common/constantParsing.h"
#line 17 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
namespace IR {
/// a value that can be evaluated at compile-time
class CompileTimeValue : public virtual INode {
 public:
#line 21 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    bool equiv(IR::CompileTimeValue const & other) const;
#line 24 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// Base class for P4 types
class Type : public Node {
 public:
#line 29 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    typedef Type_Unknown        Unknown;
    typedef Type_Boolean        Boolean;
    typedef Type_Bits           Bits;
    typedef Type_String         String;
    typedef Type_Varbits        Varbits;
    typedef Type_Void           Void;
#line 38 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// Well-defined only for types with fixed width
#line 37 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual int width_bits() const;
#line 42 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// When possible returns the corresponding type that can be inserted
/// in a P4 program; may return a Type_Name
#line 40 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual IR::Type const * getP4Type() const = 0;
#line 47 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type"; }
    static cstring static_type_name() { return "Type"; }
    void toJSON(JSONGenerator & json) const override;
    Type(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Type(Util::SourceInfo srcInfo);
    Type();
    IRNODE_ABSTRACT_SUBCLASS(Type)
};
}  // namespace IR
namespace IR {
/// Implemented by all types that may be generic:
/// Control, Extern, Method, Package, and Parser
class IMayBeGenericType : public virtual INode {
 public:
/// Allows the retrieval of type parameters
#line 47 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual IR::TypeParameters const * getTypeParameters() const = 0;
#line 69 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// Implemented by objects with an 'apply' method: Parser, Control, Table
class IApply : public virtual INode {
 public:
    static const cstring applyMethodName;
/// @returns the type signature of the apply method
#line 54 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual IR::Type_Method const * getApplyMethodType() const = 0;
#line 80 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 55 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual IR::ParameterList const * getApplyParameters() const = 0;
#line 83 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// base class for namespaces
class INamespace : public virtual INode {
 public:
#line 60 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const = 0;
#line 92 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// Does not allow two declarations with the same name
class ISimpleNamespace : public virtual INode, public virtual INamespace {
 public:
#line 65 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual const IR::IDeclaration *getDeclByName(cstring name) const = 0;
#line 101 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// A general namespace can have multiple declarations with the same name
/// E.g., an extern can have multiple methods with the same name.
class IGeneralNamespace : public virtual INode, public virtual INamespace {
 public:
#line 71 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual Util::Enumerator<const IR::IDeclaration *> * getDeclsByName(cstring name) const;
#line 111 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// prints an error if it finds duplicate names
#line 73 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void checkDuplicateDeclarations() const;
#line 115 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 74 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void validate() const override;
#line 118 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {

class INestedNamespace : public virtual INode, public virtual INamespace {
 public:
#line 80 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual std::vector<const IR::INamespace *> getNestedNamespaces() const = 0;
#line 127 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 81 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const;
#line 130 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// Interface implemented by something that can be called
/// like a function.
class IFunctional : public virtual INode {
 public:
/// The parameters of the functional object
#line 88 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual const IR::ParameterList *getParameters() const = 0;
#line 141 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// Returns true if the parameters can be matched with the
/// supplied arguments.
#line 91 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    bool callMatches(const IR::Vector<IR::Argument>* arguments) const;
#line 146 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// Implemented by things that look like type variables
class ITypeVar : public virtual INode {
 public:
#line 96 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual cstring getVarName() const = 0;
#line 155 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 97 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Type *asType() const { return to<Type>(); }
#line 158 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 98 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual int getDeclId() const = 0;
#line 161 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// Implemented by P4Parser, P4Control and Type_Package
class IContainer : public virtual INode, public virtual IMayBeGenericType, public virtual IDeclaration, public virtual IFunctional {
 public:
/// The type of the constructor as a method
#line 104 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual const IR::Type_Method *getConstructorMethodType() const = 0;
#line 171 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 105 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual const IR::ParameterList *getConstructorParameters() const = 0;
#line 174 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

#line 107 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::ParameterList *getParameters() const;
#line 178 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// This represents a primitive type
/// (called base type in the spec)
class Type_Base : public Type {
 public:
#line 113 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    IR::Type const * getP4Type() const override;
#line 188 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Base const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Base"; }
    static cstring static_type_name() { return "Type_Base"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Base(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Type_Base(Util::SourceInfo srcInfo);
    Type_Base();
    IRNODE_ABSTRACT_SUBCLASS(Type_Base)
};
}  // namespace IR
namespace IR {
/// This is needed by Expression
class Type_Unknown : public Type_Base {
 public:
#line 119 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    static const IR::Type_Unknown *get();
#line 209 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 120 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring toString() const override;
#line 212 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Unknown const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Unknown"; }
    static cstring static_type_name() { return "Type_Unknown"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Unknown(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Unknown(Util::SourceInfo srcInfo);
    Type_Unknown();
    IRNODE_SUBCLASS(Type_Unknown)
};
}  // namespace IR
namespace IR {
/// A statement or a declaration
class StatOrDecl : public Node {
 public:
    bool operator==(IR::StatOrDecl const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "StatOrDecl"; }
    static cstring static_type_name() { return "StatOrDecl"; }
    void toJSON(JSONGenerator & json) const override;
    StatOrDecl(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    StatOrDecl(Util::SourceInfo srcInfo);
    StatOrDecl();
    IRNODE_ABSTRACT_SUBCLASS(StatOrDecl)
};
}  // namespace IR
namespace IR {
/// Two declarations with the same name are not necessarily the same declaration.
/// That's why declid is used to distinguish them.
class Declaration : public StatOrDecl, public virtual IDeclaration {
 public:
    IR::ID name;
    int declid = nextId++;
#line 131 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    IR::ID getName() const override;
#line 254 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 132 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    bool equiv(IR::Node const & a_) const override;
#line 257 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
 private:
    static int nextId;
 public:
#line 136 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring toString() const override;
#line 263 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Declaration const & a) const override;
    cstring node_type_name() const override { return "Declaration"; }
    static cstring static_type_name() { return "Declaration"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Declaration(JSONLoader & json);
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Declaration(Util::SourceInfo srcInfo, IR::ID name);
    Declaration(IR::ID name);
    IRNODE_ABSTRACT_SUBCLASS(Declaration)
};
}  // namespace IR
namespace IR {
/// A declaration which introduces a type.
/// Two declarations with the same name are not the same declaration
/// That's why declid is used to distinguish them.
/// (We don't use multiple inheritance, so we can't
/// inherit both Type and Declaration.)
class Type_Declaration : public Type, public virtual IDeclaration {
 public:
    IR::ID name;
    int declid = nextId++;
#line 147 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    IR::ID getName() const override;
#line 290 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 148 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    bool equiv(IR::Node const & a_) const override;
#line 293 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
 private:
    static int nextId;
 public:
#line 152 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring toString() const override;
#line 299 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 153 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    IR::Type const * getP4Type() const override;
#line 302 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Declaration const & a) const override;
    cstring node_type_name() const override { return "Type_Declaration"; }
    static cstring static_type_name() { return "Type_Declaration"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Declaration(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Type_Declaration(Util::SourceInfo srcInfo, IR::ID name);
    Type_Declaration(IR::ID name);
    IRNODE_ABSTRACT_SUBCLASS(Type_Declaration)
};
}  // namespace IR
namespace IR {
/// base class for expressions
class Expression : public Node {
 public:
/// Note that the type field is not visited.
/// Most P4_16 passes don't use this field.
/// It is a used to hold the result of TypeInferencing for the expression.
/// It is used by the P4_14 front-end and by some back-ends.
/// It is not visited by the visitors by default (can be visited explicitly in preorder)
    const IR::Type* type = Type::Unknown::get();
#line 164 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 330 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    IRNODE_DECLARE_APPLY_OVERLOAD(Expression)
    bool operator==(IR::Expression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Expression"; }
    static cstring static_type_name() { return "Expression"; }
    void toJSON(JSONGenerator & json) const override;
    Expression(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Expression(Util::SourceInfo srcInfo, const IR::Type* type);
    Expression(const IR::Type* type);
    Expression(Util::SourceInfo srcInfo);
    Expression();
    IRNODE_ABSTRACT_SUBCLASS(Expression)
};
}  // namespace IR
namespace IR {
class Operation : public Expression {
 public:
#line 169 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual int getPrecedence() const = 0;
#line 353 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 170 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual cstring getStringOp() const = 0;
#line 356 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 172 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    typedef Operation_Unary Unary;
    typedef Operation_Binary Binary;
    typedef Operation_Relation Relation;
#line 361 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 176 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring toString() const override;
#line 364 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Operation const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Operation"; }
    static cstring static_type_name() { return "Operation"; }
    void toJSON(JSONGenerator & json) const override;
    Operation(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Operation(Util::SourceInfo srcInfo, const IR::Type* type);
    Operation(const IR::Type* type);
    Operation(Util::SourceInfo srcInfo);
    Operation();
    IRNODE_ABSTRACT_SUBCLASS(Operation)
};
}  // namespace IR
namespace IR {
/// Currently paths can be absolute (starting with a dot) or relative
/// (just an identifier).  In a previous design paths could have
/// multiple components.
class Path : public Node {
 public:
    IR::ID name;
    bool absolute = false;
#line 186 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    bool isDontCare() const;
#line 391 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 187 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring toString() const override;
#line 394 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 193 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring asString() const;
#line 397 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 199 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void dbprint(std::ostream & out) const override;
#line 400 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 200 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void validate() const override;
#line 403 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Path const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Path"; }
    static cstring static_type_name() { return "Path"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Path(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Path(Util::SourceInfo srcInfo, IR::ID name, bool absolute);
    Path(IR::ID name, bool absolute);
    Path(Util::SourceInfo srcInfo, IR::ID name);
    Path(IR::ID name);
    IRNODE_SUBCLASS(Path)
};
}  // namespace IR
namespace IR {
/// Handy class used in several NamedMaps
class NamedExpression : public Declaration {
 public:
    const IR::Expression* expression = nullptr;
    bool operator==(IR::NamedExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "NamedExpression"; }
    static cstring static_type_name() { return "NamedExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    NamedExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    NamedExpression(Util::SourceInfo srcInfo, IR::ID name, const IR::Expression* expression);
    NamedExpression(IR::ID name, const IR::Expression* expression);
    IRNODE_SUBCLASS(NamedExpression)
};
}  // namespace IR
namespace IR {
/// A token in an unparsed annotation.
/// This should really be P4::P4Parser::symbol_type, but p4parser.hpp depends
/// on the IR in a way that makes it difficult to #include in this file.
class AnnotationToken : public Node {
 public:
    int token_type;

    cstring text;
    UnparsedConstant * constInfo = nullptr;
#line 215 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void dbprint(std::ostream & out) const override;
#line 456 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::AnnotationToken const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "AnnotationToken"; }
    static cstring static_type_name() { return "AnnotationToken"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    AnnotationToken(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    AnnotationToken(Util::SourceInfo srcInfo, int token_type, cstring text, UnparsedConstant * constInfo);
    AnnotationToken(int token_type, cstring text, UnparsedConstant * constInfo);
    AnnotationToken(Util::SourceInfo srcInfo, int token_type, cstring text);
    AnnotationToken(int token_type, cstring text);
    IRNODE_SUBCLASS(AnnotationToken)
};
}  // namespace IR
namespace IR {
/// Annotations are used to provide additional information to the compiler
/// Most P4 entities can be optionally annotated
class Annotation : public Node {
 public:
    IR::ID name;
/// An unparsed annotation body
    IR::Vector<IR::AnnotationToken> body;
/// Whether the annotation body needs to be parsed.
/// Invariant: if this is true, then expr and kv must both be empty. If the
/// annotation is compiler-generated (e.g., derived from a P4₁₄ pragma),
/// then needsParsing will be false and the body will be empty, but expr or
/// kv may be populated.
    bool needsParsing;
/// Annotations that are simple expressions
    IR::Vector<IR::Expression> expr;
/// Annotations described as key-value pairs
    IR::IndexedVector<IR::NamedExpression> kv;
/// If this is true this is a structured annotation, and there are some
/// constraints on its contents.
    bool structured;
/// For annotations parsed from P4-16 source.
#line 246 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(Util::SourceInfo si, IR::ID n, IR::Vector<IR::AnnotationToken> const & a);
#line 497 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

#line 249 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(Util::SourceInfo si, IR::ID n, IR::Vector<IR::AnnotationToken> const & a, bool structured);
#line 501 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

#line 253 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(Util::SourceInfo si, IR::ID n, std::initializer_list<IR::Expression const *> const & a, bool structured = false);
#line 505 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 256 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(Util::SourceInfo si, IR::ID n, IR::Vector<IR::Expression> const & a, bool structured = false);
#line 508 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 258 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(Util::SourceInfo si, IR::ID n, IR::IndexedVector<IR::NamedExpression> const & kv, bool structured = false);
#line 511 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 261 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(IR::ID n, std::initializer_list<IR::Expression const *> const & a, bool structured = false);
#line 514 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 263 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(IR::ID n, IR::Vector<IR::Expression> const & a, bool structured = false);
#line 517 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 265 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(IR::ID n, intmax_t v, bool structured = false);
#line 520 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 268 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    Annotation(IR::ID n, cstring v, bool structured = false);
#line 523 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    static const cstring nameAnnotation;
/// Indicates the control-plane name.
    static const cstring tableOnlyAnnotation;
/// Action cannot be a default_action.
    static const cstring defaultOnlyAnnotation;
/// action can only be a default_action.
    static const cstring atomicAnnotation;
/// Code should be executed atomically.
    static const cstring hiddenAnnotation;
/// Object should not be exposed to the control-plane.
    static const cstring lengthAnnotation;
/// P4-14 annotation for varbit fields.
    static const cstring optionalAnnotation;
/// Optional parameter annotation
    static const cstring pkginfoAnnotation;
/// Package documentation annotation.
    static const cstring deprecatedAnnotation;
/// Deprecation annotation.
    static const cstring synchronousAnnotation;
/// Synchronous annotation.
    static const cstring pureAnnotation;
/// extern function/method annotation.
    static const cstring noSideEffectsAnnotation;
/// extern function/method annotation.
    static const cstring noWarnAnnotation;
/// noWarn annotation.
    static const cstring matchAnnotation;
/// Match annotation (for value sets).
#line 286 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring toString() const override;
#line 554 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 287 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void validate() const override;
#line 557 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// Extracts name value from a name annotation
#line 296 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring getName() const;
#line 561 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// Extracts a single string argument; error if the argument is not a string
#line 298 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring getSingleString() const;
#line 565 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 301 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    enum class Kind {
        Unstructured,
        StructuredEmpty,
        StructuredKVList,
        StructuredExpressionList
    };

    Kind annotationKind() const {
        if (!structured)
            return Kind::Unstructured;
        if (expr.size())
            return Kind::StructuredExpressionList;
        if (kv.size())
            return Kind::StructuredKVList;
        return Kind::StructuredEmpty;
    }
#line 583 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Annotation const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Annotation"; }
    static cstring static_type_name() { return "Annotation"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Annotation(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Annotation(Util::SourceInfo srcInfo, IR::ID name, IR::Vector<IR::AnnotationToken> body, bool needsParsing, IR::Vector<IR::Expression> expr, IR::IndexedVector<IR::NamedExpression> kv, bool structured);
    Annotation(IR::ID name, IR::Vector<IR::AnnotationToken> body, bool needsParsing, IR::Vector<IR::Expression> expr, IR::IndexedVector<IR::NamedExpression> kv, bool structured);
    IRNODE_SUBCLASS(Annotation)
};
}  // namespace IR
namespace IR {
/// There can be several annotations with the same "name", so this is a vector.
// FIXME -- get rid of this class -- classes with annotations should have an
// inline Vector<Annotation> instead (remove useless indirection)
class Annotations : public Node {
 public:
    IR::Vector<IR::Annotation> annotations;
    static IR::Annotations * empty;

#line 327 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    size_t size() const;
#line 612 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"


#line 330 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotation *getSingle(cstring name) const;
#line 617 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 331 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotations *add(const IR::Annotation* annot);
#line 620 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 335 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotations *add(const IR::Annotation* annot) const;
#line 623 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 336 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotations *addAnnotation(cstring name, const IR::Expression* expr, bool structured = false);
#line 626 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 338 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotations *addAnnotation(cstring name, const IR::Expression* expr, bool structured = false) const;
#line 629 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"


#line 342 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotations *addAnnotationIfNew(cstring name, const IR::Expression* expr, bool structured = false) const;
#line 634 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"


#line 346 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotations *addOrReplace(cstring name, const IR::Expression* expr, bool structured = false) const;
#line 639 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 352 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    typedef std::function<bool(const IR::Annotation*)> Filter;
#line 642 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 354 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotations *where(Filter func) const;
#line 645 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 360 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void validate() const override;
#line 648 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 361 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void dbprint(std::ostream & out) const override;
#line 651 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Annotations const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Annotations"; }
    static cstring static_type_name() { return "Annotations"; }
    void toJSON(JSONGenerator & json) const override;
    Annotations(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Annotations(Util::SourceInfo srcInfo, IR::Vector<IR::Annotation> annotations);
    Annotations(IR::Vector<IR::Annotation> annotations);
    Annotations(Util::SourceInfo srcInfo);
    Annotations();
    IRNODE_SUBCLASS(Annotations)
};
}  // namespace IR
namespace IR {
/// Implemented by all objects that can have annotations
class IAnnotated : public virtual INode {
 public:
#line 366 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual const IR::Annotations *getAnnotations() const = 0;
#line 675 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 367 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    const IR::Annotation *getAnnotation(cstring name) const override;
#line 678 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
class IInstance : public virtual INode {
 public:
#line 372 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual cstring Name() const = 0;
#line 686 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 373 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    virtual const IR::Type *getType() const = 0;
#line 689 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// An argument to a function call (or constructor call)
/// Arguments may have optional names
class Argument : public Node {
 public:
/// If an argument has no name the name.name is nullptr.
    IR::ID name;
    const IR::Expression* expression = nullptr;
#line 383 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void dbprint(std::ostream & out) const override;
#line 702 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 384 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    void validate() const override;
#line 705 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 385 "/home/aa/ppk_sw/ppk/p4c/ir/base.def"
    cstring toString() const override;
#line 708 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Argument const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Argument"; }
    static cstring static_type_name() { return "Argument"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Argument(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Argument(Util::SourceInfo srcInfo, IR::ID name, const IR::Expression* expression);
    Argument(IR::ID name, const IR::Expression* expression);
    Argument(Util::SourceInfo srcInfo, const IR::Expression* expression);
    Argument(const IR::Expression* expression);
    IRNODE_SUBCLASS(Argument)
};
}  // namespace IR
/** @} */
/* end group irdefs */
/* -*-C++-*- */
/** \addtogroup irdefs
  * @{
  */
/*
   This file contains IR related to representating the type hierarchy.
   Some of these classes never appear in the IR tree, they are just
   synthesized by the type-checker.
*/
#line 12 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
namespace IR {
enum class Direction {
    None,
    In,
    Out,
    InOut
};

inline cstring directionToString(IR::Direction direction) {
    switch (direction) {
        case IR::Direction::None:
            return "<none>";
        case IR::Direction::In:
            return "in";
        case IR::Direction::Out:
            return "out";
        case IR::Direction::InOut:
            return "inout";
        default:
            BUG("Unhandled case");
    }
}
}  // namespace IR

inline std::ostream& operator<<(std::ostream &out, IR::Direction d) {
    switch (d) {
        case IR::Direction::None:
            break;
        case IR::Direction::In:
            out << "in";
            break;
        case IR::Direction::Out:
            out << "out";
            break;
        case IR::Direction::InOut:
            out << "inout";
            break;
        default:
            BUG("Unhandled case");
    }
    return out;
}

inline bool operator>>(cstring s, IR::Direction &d) {
    if (!s || s == "") d = IR::Direction::None;
    else if (s == "in") d = IR::Direction::In;
    else if (s == "out") d = IR::Direction::Out;
    else if (s == "inout") d = IR::Direction::InOut;
    else return false;
    return true;
}
#line 790 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
namespace IR {
/// Represents the type of a type.
/// For example, in a declaration like
/// bit<32> x;
/// The type of x is Type_Bits(32);
/// The type of 'bit<32>' is Type_Type(Type_Bits(32))
/// TypeType should not appear in the program IR, just in the TypeMap produced
/// by type-checking.
class Type_Type : public Type {
 public:
    const IR::Type* type = nullptr;
#line 74 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 804 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 75 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 807 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 76 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type *getP4Type() const override;
#line 810 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 77 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 813 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Type const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_Type"; }
    static cstring static_type_name() { return "Type_Type"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Type(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Type(Util::SourceInfo srcInfo, const IR::Type* type);
    Type_Type(const IR::Type* type);
    IRNODE_SUBCLASS(Type_Type)
};
}  // namespace IR
namespace IR {
class Type_Boolean : public Type_Base {
 public:
#line 81 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_Boolean *get();
#line 835 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 82 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 838 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 83 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 841 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 84 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 844 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Boolean const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Boolean"; }
    static cstring static_type_name() { return "Type_Boolean"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Boolean(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Boolean(Util::SourceInfo srcInfo);
    Type_Boolean();
    IRNODE_SUBCLASS(Type_Boolean)
};
}  // namespace IR
namespace IR {
/// The type of a parser state
class Type_State : public Type_Base {
 public:
#line 89 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_State *get();
#line 866 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 90 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 869 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 91 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 872 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_State const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_State"; }
    static cstring static_type_name() { return "Type_State"; }
    void toJSON(JSONGenerator & json) const override;
    Type_State(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_State(Util::SourceInfo srcInfo);
    Type_State();
    IRNODE_SUBCLASS(Type_State)
};
}  // namespace IR
namespace IR {
/// Represents both bit<> and int<> types in P4-14 and P4-16
class Type_Bits : public Type_Base {
 public:
    int size = 0;

    const IR::Expression* expression = nullptr;

    bool isSigned;
#line 99 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_Bits *get(Util::SourceInfo si, int sz, bool isSigned = false);
#line 899 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 100 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_Bits *get(int sz, bool isSigned = false);
#line 902 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 101 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring baseName() const;
#line 905 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 102 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 908 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 104 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 911 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 105 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 914 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Bits const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_Bits"; }
    static cstring static_type_name() { return "Type_Bits"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Bits(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Bits(Util::SourceInfo srcInfo, int size, const IR::Expression* expression, bool isSigned);
    Type_Bits(int size, const IR::Expression* expression, bool isSigned);
    Type_Bits(Util::SourceInfo srcInfo, const IR::Expression* expression, bool isSigned);
    Type_Bits(const IR::Expression* expression, bool isSigned);
    Type_Bits(Util::SourceInfo srcInfo, int size, bool isSigned);
    Type_Bits(int size, bool isSigned);
    Type_Bits(Util::SourceInfo srcInfo, bool isSigned);
    Type_Bits(bool isSigned);
    IRNODE_SUBCLASS(Type_Bits)
};
}  // namespace IR
namespace IR {
class Type_Varbits : public Type_Base {
 public:
    int size = 0;

    const IR::Expression* expression = nullptr;

#line 111 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_Varbits *get(Util::SourceInfo si, int size = 0);
#line 948 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 112 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_Varbits *get();
#line 951 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 113 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 954 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 114 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 957 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Varbits const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_Varbits"; }
    static cstring static_type_name() { return "Type_Varbits"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Varbits(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Varbits(Util::SourceInfo srcInfo, int size, const IR::Expression* expression);
    Type_Varbits(int size, const IR::Expression* expression);
    Type_Varbits(Util::SourceInfo srcInfo, const IR::Expression* expression);
    Type_Varbits(const IR::Expression* expression);
    Type_Varbits(Util::SourceInfo srcInfo, int size);
    Type_Varbits(int size);
    Type_Varbits(Util::SourceInfo srcInfo);
    Type_Varbits();
    IRNODE_SUBCLASS(Type_Varbits)
};
}  // namespace IR
namespace IR {
class Parameter : public Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    Direction direction;
    const IR::Type* type = nullptr;
    const IR::Expression* defaultValue = nullptr;
#line 122 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 991 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 123 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool hasOut() const;
#line 994 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 125 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool isOptional() const;
#line 997 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 127 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1000 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Parameter const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Parameter"; }
    static cstring static_type_name() { return "Parameter"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Parameter(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Parameter(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, Direction direction, const IR::Type* type, const IR::Expression* defaultValue);
    Parameter(IR::ID name, const IR::Annotations* annotations, Direction direction, const IR::Type* type, const IR::Expression* defaultValue);
    Parameter(Util::SourceInfo srcInfo, IR::ID name, Direction direction, const IR::Type* type, const IR::Expression* defaultValue);
    Parameter(IR::ID name, Direction direction, const IR::Type* type, const IR::Expression* defaultValue);
    Parameter(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, Direction direction, const IR::Type* type);
    Parameter(IR::ID name, const IR::Annotations* annotations, Direction direction, const IR::Type* type);
    Parameter(Util::SourceInfo srcInfo, IR::ID name, Direction direction, const IR::Type* type);
    Parameter(IR::ID name, Direction direction, const IR::Type* type);
    IRNODE_SUBCLASS(Parameter)
};
}  // namespace IR
namespace IR {
class ParameterList : public Node, public virtual ISimpleNamespace {
 public:
    IR::IndexedVector<IR::Parameter> parameters;
#line 133 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 1032 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 134 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::Parameter *> * getEnumerator() const;
#line 1035 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 136 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 1038 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 138 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t size() const;
#line 1041 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 139 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool empty() const;
#line 1044 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 140 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Parameter *getParameter(cstring name) const;
#line 1047 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 142 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Parameter *getParameter(unsigned index) const;
#line 1050 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 146 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 1053 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 147 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void push_back(IR::Parameter const * p);
#line 1056 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 148 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1059 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 160 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    auto begin() const -> decltype(parameters.begin()) { return parameters.begin(); }
    auto end() const -> decltype(parameters.end()) { return parameters.end(); }
#line 1063 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ParameterList const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ParameterList"; }
    static cstring static_type_name() { return "ParameterList"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ParameterList(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ParameterList(Util::SourceInfo srcInfo, IR::IndexedVector<IR::Parameter> parameters);
    ParameterList(IR::IndexedVector<IR::Parameter> parameters);
    ParameterList(Util::SourceInfo srcInfo);
    ParameterList();
    IRNODE_SUBCLASS(ParameterList)
};
}  // namespace IR
namespace IR {
/// Represents a type variable written by the user
class Type_Var : public Type_Declaration, public virtual ITypeVar {
 public:
#line 167 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring getVarName() const override;
#line 1088 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 168 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int getDeclId() const override;
#line 1091 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 169 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1094 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 170 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1097 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Var const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Var"; }
    static cstring static_type_name() { return "Type_Var"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Var(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Var(Util::SourceInfo srcInfo, IR::ID name);
    Type_Var(IR::ID name);
    IRNODE_SUBCLASS(Type_Var)
};
}  // namespace IR
namespace IR {
/// Stands for the 'int' type: infinite precision constant
/// However, we represent it as a type variable, because we use
/// type unification to discover the correct type for the constants
/// in some contexts.
class Type_InfInt : public Type, public virtual ITypeVar {
 public:
    int declid = nextId++;
 private:
    static int nextId;
 public:
#line 182 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring getVarName() const override;
#line 1126 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 183 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int getDeclId() const override;
#line 1129 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 184 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1132 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 185 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1135 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 186 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool operator==(IR::Type_InfInt const & a) const override;
#line 1138 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 187 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool equiv(IR::Node const & a_) const override;
#line 1141 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 191 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1144 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    cstring node_type_name() const override { return "Type_InfInt"; }
    static cstring static_type_name() { return "Type_InfInt"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_InfInt(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_InfInt(Util::SourceInfo srcInfo);
    Type_InfInt();
    IRNODE_SUBCLASS(Type_InfInt)
};
}  // namespace IR
namespace IR {
class Type_Dontcare : public Type_Base {
 public:
#line 195 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1163 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 196 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_Dontcare *get();
#line 1166 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 197 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1169 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Dontcare const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Dontcare"; }
    static cstring static_type_name() { return "Type_Dontcare"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Dontcare(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Dontcare(Util::SourceInfo srcInfo);
    Type_Dontcare();
    IRNODE_SUBCLASS(Type_Dontcare)
};
}  // namespace IR
namespace IR {
class Type_Void : public Type_Base {
 public:
#line 201 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1190 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 202 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_Void *get();
#line 1193 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 203 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1196 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Void const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Void"; }
    static cstring static_type_name() { return "Type_Void"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Void(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Void(Util::SourceInfo srcInfo);
    Type_Void();
    IRNODE_SUBCLASS(Type_Void)
};
}  // namespace IR
namespace IR {
class Type_MatchKind : public Type_Base {
 public:
#line 207 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1217 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 208 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_MatchKind *get();
#line 1220 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 209 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1223 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_MatchKind const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_MatchKind"; }
    static cstring static_type_name() { return "Type_MatchKind"; }
    void toJSON(JSONGenerator & json) const override;
    Type_MatchKind(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_MatchKind(Util::SourceInfo srcInfo);
    Type_MatchKind();
    IRNODE_SUBCLASS(Type_MatchKind)
};
}  // namespace IR
namespace IR {
class TypeParameters : public Node, public virtual ISimpleNamespace {
 public:
    IR::IndexedVector<IR::Type_Var> parameters;
#line 214 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 1245 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 216 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool empty() const;
#line 1248 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 217 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t size() const;
#line 1251 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 218 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 1254 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 220 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void push_back(const IR::Type_Var* tv);
#line 1257 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 221 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 1260 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 222 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1263 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::TypeParameters const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "TypeParameters"; }
    static cstring static_type_name() { return "TypeParameters"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    TypeParameters(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    TypeParameters(Util::SourceInfo srcInfo, IR::IndexedVector<IR::Type_Var> parameters);
    TypeParameters(IR::IndexedVector<IR::Type_Var> parameters);
    TypeParameters(Util::SourceInfo srcInfo);
    TypeParameters();
    IRNODE_SUBCLASS(TypeParameters)
};
}  // namespace IR
namespace IR {
class StructField : public Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* type = nullptr;
#line 241 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 1289 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::StructField const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "StructField"; }
    static cstring static_type_name() { return "StructField"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    StructField(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    StructField(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    StructField(IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    StructField(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type);
    StructField(IR::ID name, const IR::Type* type);
    IRNODE_SUBCLASS(StructField)
};
}  // namespace IR
namespace IR {
class Type_StructLike : public Type_Declaration, public virtual INestedNamespace, public virtual ISimpleNamespace, public virtual IAnnotated, public virtual IMayBeGenericType {
 public:
    static const cstring minSizeInBits;
    static const cstring minSizeInBytes;
    const IR::Annotations* annotations = Annotations::empty;
    const IR::TypeParameters* typeParameters = new TypeParameters();
    IR::IndexedVector<IR::StructField> fields;
#line 250 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::TypeParameters *getTypeParameters() const override;
#line 1321 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 251 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    std::vector<const IR::INamespace *> getNestedNamespaces() const override;
#line 1324 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 252 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 1327 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 253 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 1330 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 255 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::StructField *getField(cstring name) const;
#line 1333 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 257 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int getFieldIndex(cstring name) const;
#line 1336 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// This function returns start offset of the given field name in bits.
/// If the given name is not a valid field name, -1 is returned.
/// The given offset may not be correct if varbit field(s) present in between.
/// Offset for all fields will be correct if:
///  - the type has only fixed width fields
///  - the type has fixed width fields with only one varbit field as a last member.
#line 272 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int getFieldBitOffset(cstring name) const;
#line 1345 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 282 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 1348 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 288 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 1351 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 290 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 1354 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 291 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1357 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    IRNODE_DECLARE_APPLY_OVERLOAD(Type_StructLike)
    bool operator==(IR::Type_StructLike const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_StructLike"; }
    static cstring static_type_name() { return "Type_StructLike"; }
    void toJSON(JSONGenerator & json) const override;
    Type_StructLike(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_StructLike(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    Type_StructLike(IR::ID name, const IR::TypeParameters* typeParameters);
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Type_StructLike(IR::ID name, const IR::Annotations* annotations);
    Type_StructLike(Util::SourceInfo srcInfo, IR::ID name);
    Type_StructLike(IR::ID name);
    IRNODE_ABSTRACT_SUBCLASS(Type_StructLike)
};
}  // namespace IR
namespace IR {
class Type_Struct : public Type_StructLike {
 public:
#line 297 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1395 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Struct const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Struct"; }
    static cstring static_type_name() { return "Type_Struct"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Struct(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_StructLike const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_Struct(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    Type_Struct(IR::ID name, const IR::TypeParameters* typeParameters);
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Type_Struct(IR::ID name, const IR::Annotations* annotations);
    Type_Struct(Util::SourceInfo srcInfo, IR::ID name);
    Type_Struct(IR::ID name);
    IRNODE_SUBCLASS(Type_Struct)
};
}  // namespace IR
namespace IR {
/// This is the type of a struct-valued expression whose
/// exact struct type is yet unknown; we only know the field names
/// and some information about their types.
class Type_UnknownStruct : public Type_StructLike {
 public:
    bool operator==(IR::Type_UnknownStruct const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_UnknownStruct"; }
    static cstring static_type_name() { return "Type_UnknownStruct"; }
    void toJSON(JSONGenerator & json) const override;
    Type_UnknownStruct(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_StructLike const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_UnknownStruct(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    Type_UnknownStruct(IR::ID name, const IR::TypeParameters* typeParameters);
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Type_UnknownStruct(IR::ID name, const IR::Annotations* annotations);
    Type_UnknownStruct(Util::SourceInfo srcInfo, IR::ID name);
    Type_UnknownStruct(IR::ID name);
    IRNODE_SUBCLASS(Type_UnknownStruct)
};
}  // namespace IR
namespace IR {
class Type_HeaderUnion : public Type_StructLike {
 public:
#line 309 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1467 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

#line 311 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 1471 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// start offset of any field in a union is 0
#line 317 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int getFieldBitOffset(cstring name) const;
#line 1475 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_HeaderUnion const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_HeaderUnion"; }
    static cstring static_type_name() { return "Type_HeaderUnion"; }
    void toJSON(JSONGenerator & json) const override;
    Type_HeaderUnion(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_StructLike const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_HeaderUnion(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    Type_HeaderUnion(IR::ID name, const IR::TypeParameters* typeParameters);
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Type_HeaderUnion(IR::ID name, const IR::Annotations* annotations);
    Type_HeaderUnion(Util::SourceInfo srcInfo, IR::ID name);
    Type_HeaderUnion(IR::ID name);
    IRNODE_SUBCLASS(Type_HeaderUnion)
};
}  // namespace IR
namespace IR {
class Type_Header : public Type_StructLike {
 public:
    static const cstring setValid;
    static const cstring setInvalid;
    static const cstring isValid;
#line 331 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1514 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Header const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Header"; }
    static cstring static_type_name() { return "Type_Header"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Header(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_StructLike const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Header(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Header(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Header(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Header(IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    Type_Header(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_Header(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    Type_Header(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_Header(IR::ID name, IR::IndexedVector<IR::StructField> fields);
    Type_Header(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_Header(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_Header(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    Type_Header(IR::ID name, const IR::TypeParameters* typeParameters);
    Type_Header(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Type_Header(IR::ID name, const IR::Annotations* annotations);
    Type_Header(Util::SourceInfo srcInfo, IR::ID name);
    Type_Header(IR::ID name);
    IRNODE_SUBCLASS(Type_Header)
};
}  // namespace IR
namespace IR {
class Type_Set : public Type {
 public:
    const IR::Type* elementType = nullptr;
#line 336 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1551 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 337 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1554 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 338 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1557 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 339 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 1560 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Set const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Set"; }
    static cstring static_type_name() { return "Type_Set"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Set(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Set(Util::SourceInfo srcInfo, const IR::Type* elementType);
    Type_Set(const IR::Type* elementType);
    IRNODE_SUBCLASS(Type_Set)
};
}  // namespace IR
namespace IR {
class Type_Indexed : public virtual INode {
 public:


#line 348 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    virtual size_t getSize() const = 0;
#line 1585 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 349 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type *at(size_t index) const;
#line 1588 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
/// Base class for Type_List and Type_Tuple
class Type_BaseList : public Type, public virtual Type_Indexed {
 public:
    IR::Vector<IR::Type> components;
#line 355 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 1598 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 356 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t getSize() const override;
#line 1601 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 357 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type *at(size_t index) const;
#line 1604 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 358 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 1607 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 365 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1610 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_BaseList const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_BaseList"; }
    static cstring static_type_name() { return "Type_BaseList"; }
    void toJSON(JSONGenerator & json) const override;
    Type_BaseList(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Type_BaseList(Util::SourceInfo srcInfo, IR::Vector<IR::Type> components);
    Type_BaseList(IR::Vector<IR::Type> components);
    Type_BaseList(Util::SourceInfo srcInfo);
    Type_BaseList();
    IRNODE_ABSTRACT_SUBCLASS(Type_BaseList)
};
}  // namespace IR
namespace IR {
/// The type of an expressionList; can be unified with both Type_Tuple and Type_Struct
class Type_List : public Type_BaseList {
 public:

#line 381 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1636 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_List const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_List"; }
    static cstring static_type_name() { return "Type_List"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_List(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_BaseList const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_List(Util::SourceInfo srcInfo, IR::Vector<IR::Type> components);
    Type_List(IR::Vector<IR::Type> components);
    Type_List(Util::SourceInfo srcInfo);
    Type_List();
    IRNODE_SUBCLASS(Type_List)
};
}  // namespace IR
namespace IR {
/// The type of a tuple.
class Type_Tuple : public Type_BaseList {
 public:
#line 386 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1661 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Tuple const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Tuple"; }
    static cstring static_type_name() { return "Type_Tuple"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Tuple(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_BaseList const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Tuple(Util::SourceInfo srcInfo, IR::Vector<IR::Type> components);
    Type_Tuple(IR::Vector<IR::Type> components);
    Type_Tuple(Util::SourceInfo srcInfo);
    Type_Tuple();
    IRNODE_SUBCLASS(Type_Tuple)
};
}  // namespace IR
namespace IR {
/// The type of an architectural block.
/// Abstract base for Type_Control, Type_Parser and Type_Package
class Type_ArchBlock : public Type_Declaration, public virtual IMayBeGenericType, public virtual IAnnotated, public virtual ISimpleNamespace {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::TypeParameters* typeParameters = new TypeParameters;
#line 394 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 1689 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 395 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::TypeParameters *getTypeParameters() const override;
#line 1692 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 396 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 1695 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 398 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 1698 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_ArchBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_ArchBlock"; }
    static cstring static_type_name() { return "Type_ArchBlock"; }
    void toJSON(JSONGenerator & json) const override;
    Type_ArchBlock(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Type_ArchBlock(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_ArchBlock(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    Type_ArchBlock(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    Type_ArchBlock(IR::ID name, const IR::TypeParameters* typeParameters);
    Type_ArchBlock(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Type_ArchBlock(IR::ID name, const IR::Annotations* annotations);
    Type_ArchBlock(Util::SourceInfo srcInfo, IR::ID name);
    Type_ArchBlock(IR::ID name);
    IRNODE_ABSTRACT_SUBCLASS(Type_ArchBlock)
};
}  // namespace IR
namespace IR {
class Type_Package : public Type_ArchBlock, public virtual IContainer, public virtual ISimpleNamespace {
 public:
    const IR::ParameterList* constructorParams = nullptr;
#line 404 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type_Method *getConstructorMethodType() const override;
#line 1729 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 405 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::ParameterList *getConstructorParameters() const override;
#line 1732 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 406 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1735 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 407 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 1738 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 409 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 1741 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Package const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Package"; }
    static cstring static_type_name() { return "Type_Package"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Package(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_ArchBlock const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Package(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, const IR::ParameterList* constructorParams);
    Type_Package(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, const IR::ParameterList* constructorParams);
    Type_Package(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, const IR::ParameterList* constructorParams);
    Type_Package(IR::ID name, const IR::TypeParameters* typeParameters, const IR::ParameterList* constructorParams);
    Type_Package(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* constructorParams);
    Type_Package(IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* constructorParams);
    Type_Package(Util::SourceInfo srcInfo, IR::ID name, const IR::ParameterList* constructorParams);
    Type_Package(IR::ID name, const IR::ParameterList* constructorParams);
    IRNODE_SUBCLASS(Type_Package)
};
}  // namespace IR
namespace IR {
class Type_Parser : public Type_ArchBlock, public virtual IApply {
 public:
    const IR::ParameterList* applyParams = nullptr;
#line 417 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type_Method *getApplyMethodType() const override;
#line 1774 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 418 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::ParameterList *getApplyParameters() const override;
#line 1777 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 419 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1780 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Parser const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Parser"; }
    static cstring static_type_name() { return "Type_Parser"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Parser(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_ArchBlock const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Parser(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Parser(IR::ID name, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* applyParams);
    Type_Parser(IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* applyParams);
    Type_Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::ParameterList* applyParams);
    Type_Parser(IR::ID name, const IR::ParameterList* applyParams);
    IRNODE_SUBCLASS(Type_Parser)
};
}  // namespace IR
namespace IR {
class Type_Control : public Type_ArchBlock, public virtual IApply {
 public:
    const IR::ParameterList* applyParams = nullptr;
#line 424 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type_Method *getApplyMethodType() const override;
#line 1813 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 425 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::ParameterList *getApplyParameters() const override;
#line 1816 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 426 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1819 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Control const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Control"; }
    static cstring static_type_name() { return "Type_Control"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Control(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_ArchBlock const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Control(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Control(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Control(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Control(IR::ID name, const IR::TypeParameters* typeParameters, const IR::ParameterList* applyParams);
    Type_Control(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* applyParams);
    Type_Control(IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* applyParams);
    Type_Control(Util::SourceInfo srcInfo, IR::ID name, const IR::ParameterList* applyParams);
    Type_Control(IR::ID name, const IR::ParameterList* applyParams);
    IRNODE_SUBCLASS(Type_Control)
};
}  // namespace IR
namespace IR {
/// A type referred by name
class Type_Name : public Type {
 public:
    const IR::Path* path = nullptr;
#line 432 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Type_Name(IR::ID id);
#line 1853 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 433 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1856 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 434 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1859 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 435 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1862 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 436 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 1865 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Name const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Name"; }
    static cstring static_type_name() { return "Type_Name"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Name(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Name(Util::SourceInfo srcInfo, const IR::Path* path);
    Type_Name(const IR::Path* path);
    IRNODE_SUBCLASS(Type_Name)
};
}  // namespace IR
namespace IR {
class Type_Stack : public virtual Type_Indexed, public Type {
 public:
    const IR::Type* elementType = nullptr;
    const IR::Expression* size = nullptr;
#line 445 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1890 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 447 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 1893 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 448 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool sizeKnown() const;
#line 1896 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 449 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t getSize() const override;
#line 1899 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 450 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type *at(size_t index) const;
#line 1902 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    static const cstring next;
    static const cstring last;
    static const cstring arraySize;
    static const cstring lastIndex;
    static const cstring push_front;
    static const cstring pop_front;
#line 457 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1911 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Stack const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Stack"; }
    static cstring static_type_name() { return "Type_Stack"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Stack(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Stack(Util::SourceInfo srcInfo, const IR::Type* elementType, const IR::Expression* size);
    Type_Stack(const IR::Type* elementType, const IR::Expression* size);
    IRNODE_SUBCLASS(Type_Stack)
};
}  // namespace IR
namespace IR {
/** Given a declaration
   extern E<T> { ... }
   Type_Specialized represents a type such
   E<bit<32>>
   baseType is Type_Extern E, arguments is a vector containing Type_Bits(32) */
class Type_Specialized : public Type {
 public:
    const IR::Type_Name* baseType = nullptr;
    const IR::Vector<IR::Type>* arguments = nullptr;
#line 469 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 1941 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 470 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 1944 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 471 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1947 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Specialized const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_Specialized"; }
    static cstring static_type_name() { return "Type_Specialized"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Specialized(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Specialized(Util::SourceInfo srcInfo, const IR::Type_Name* baseType, const IR::Vector<IR::Type>* arguments);
    Type_Specialized(const IR::Type_Name* baseType, const IR::Vector<IR::Type>* arguments);
    IRNODE_SUBCLASS(Type_Specialized)
};
}  // namespace IR
namespace IR {
/** Canonical representation of a Type_Specialized;
   only used by the type-checker, never in the IR tree. */
class Type_SpecializedCanonical : public Type {
 public:
    const IR::Type* baseType = nullptr;
/// canonical baseType; always IMayBeGenericType
    const IR::Vector<IR::Type>* arguments = nullptr;
/// canonical type arguments
/// 'substituted' is baseType with all type
/// variables substituted with the arguments.
    const IR::Type* substituted = nullptr;

#line 483 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 1980 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 488 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 1983 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_SpecializedCanonical const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_SpecializedCanonical"; }
    static cstring static_type_name() { return "Type_SpecializedCanonical"; }
    void toJSON(JSONGenerator & json) const override;
    Type_SpecializedCanonical(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_SpecializedCanonical(Util::SourceInfo srcInfo, const IR::Type* baseType, const IR::Vector<IR::Type>* arguments, const IR::Type* substituted);
    Type_SpecializedCanonical(const IR::Type* baseType, const IR::Vector<IR::Type>* arguments, const IR::Type* substituted);
    IRNODE_SUBCLASS(Type_SpecializedCanonical)
};
}  // namespace IR
namespace IR {
/// A declaration that consists of just an identifier, e.g., an enum member
class Declaration_ID : public Declaration, public virtual CompileTimeValue {
 public:
    bool operator==(IR::Declaration_ID const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Declaration_ID"; }
    static cstring static_type_name() { return "Declaration_ID"; }
    void toJSON(JSONGenerator & json) const override;
    Declaration_ID(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Declaration_ID(Util::SourceInfo srcInfo, IR::ID name);
    Declaration_ID(IR::ID name);
    IRNODE_SUBCLASS(Declaration_ID)
};
}  // namespace IR
namespace IR {
/// The type of a string literal
class Type_String : public Type_Base {
 public:
#line 499 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    static const IR::Type_String *get();
#line 2025 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 500 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 2028 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_String const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_String"; }
    static cstring static_type_name() { return "Type_String"; }
    void toJSON(JSONGenerator & json) const override;
    Type_String(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_String(Util::SourceInfo srcInfo);
    Type_String();
    IRNODE_SUBCLASS(Type_String)
};
}  // namespace IR
namespace IR {
class Type_Enum : public Type_Declaration, public virtual ISimpleNamespace, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    IR::IndexedVector<IR::Declaration_ID> members;
#line 506 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 2051 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 507 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 2054 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 509 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 2057 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 512 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 2060 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Enum const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_Enum"; }
    static cstring static_type_name() { return "Type_Enum"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Enum(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Enum(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::Declaration_ID> members);
    Type_Enum(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::Declaration_ID> members);
    Type_Enum(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::Declaration_ID> members);
    Type_Enum(IR::ID name, IR::IndexedVector<IR::Declaration_ID> members);
    IRNODE_SUBCLASS(Type_Enum)
};
}  // namespace IR
namespace IR {
/// A member of a serializable enum with a backing value
class SerEnumMember : public Declaration, public virtual CompileTimeValue {
 public:
    const IR::Expression* value = nullptr;
#line 518 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 2087 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::SerEnumMember const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "SerEnumMember"; }
    static cstring static_type_name() { return "SerEnumMember"; }
    void toJSON(JSONGenerator & json) const override;
    SerEnumMember(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    SerEnumMember(Util::SourceInfo srcInfo, IR::ID name, const IR::Expression* value);
    SerEnumMember(IR::ID name, const IR::Expression* value);
    IRNODE_SUBCLASS(SerEnumMember)
};
}  // namespace IR
namespace IR {
/** A serializable enumeration with a backing type */
class Type_SerEnum : public Type_Declaration, public virtual ISimpleNamespace, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* type = nullptr;
    IR::IndexedVector<IR::SerEnumMember> members;
#line 527 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 2114 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 528 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 2117 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 530 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 2120 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 533 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 2123 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_SerEnum const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_SerEnum"; }
    static cstring static_type_name() { return "Type_SerEnum"; }
    void toJSON(JSONGenerator & json) const override;
    Type_SerEnum(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_SerEnum(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type, IR::IndexedVector<IR::SerEnumMember> members);
    Type_SerEnum(IR::ID name, const IR::Annotations* annotations, const IR::Type* type, IR::IndexedVector<IR::SerEnumMember> members);
    Type_SerEnum(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type, IR::IndexedVector<IR::SerEnumMember> members);
    Type_SerEnum(IR::ID name, const IR::Type* type, IR::IndexedVector<IR::SerEnumMember> members);
    IRNODE_SUBCLASS(Type_SerEnum)
};
}  // namespace IR
namespace IR {
class Type_Table : public Type, public virtual IApply {
 public:
    const IR::P4Table* table = nullptr;
#line 538 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Type_Method *getApplyMethodType() const override;
#line 2149 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 539 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::ParameterList *getApplyParameters() const override;
#line 2152 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// names for the fields of the struct returned
/// by applying a table
    static const IR::ID hit;
    static const IR::ID miss;
    static const IR::ID action_run;
#line 545 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 2160 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 546 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 2163 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Table const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Table"; }
    static cstring static_type_name() { return "Type_Table"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Table(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Table(Util::SourceInfo srcInfo, const IR::P4Table* table);
    Type_Table(const IR::P4Table* table);
    IRNODE_SUBCLASS(Type_Table)
};
}  // namespace IR
namespace IR {
/// A special enum-like anonymous type that
/// represents all actions in a table's action list.
/// Used for 'switch' statements.
class Type_ActionEnum : public Type {
 public:
    const IR::ActionList* actionList = nullptr;
#line 554 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    bool contains(cstring name) const;
#line 2190 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 555 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 2193 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_ActionEnum const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_ActionEnum"; }
    static cstring static_type_name() { return "Type_ActionEnum"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_ActionEnum(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_ActionEnum(Util::SourceInfo srcInfo, const IR::ActionList* actionList);
    Type_ActionEnum(const IR::ActionList* actionList);
    IRNODE_SUBCLASS(Type_ActionEnum)
};
}  // namespace IR
namespace IR {
class Type_MethodBase : public Type, public virtual IMayBeGenericType, public virtual ISimpleNamespace {
 public:

    const IR::TypeParameters* typeParameters = new TypeParameters();
    const IR::Type* returnType = nullptr;

    const IR::ParameterList* parameters = nullptr;
#line 565 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t maxParameterCount() const;
#line 2222 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 566 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t minParameterCount() const;
#line 2225 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 567 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    virtual const IR::TypeParameters *getTypeParameters() const override;
#line 2228 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 568 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void dbprint(std::ostream & out) const override;
#line 2231 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 569 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 2234 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 570 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 2237 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 571 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 2240 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 573 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 2243 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_MethodBase const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_MethodBase"; }
    static cstring static_type_name() { return "Type_MethodBase"; }
    void toJSON(JSONGenerator & json) const override;
    Type_MethodBase(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Type_MethodBase(Util::SourceInfo srcInfo, const IR::TypeParameters* typeParameters, const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_MethodBase(const IR::TypeParameters* typeParameters, const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_MethodBase(Util::SourceInfo srcInfo, const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_MethodBase(const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_MethodBase(Util::SourceInfo srcInfo, const IR::TypeParameters* typeParameters, const IR::ParameterList* parameters);
    Type_MethodBase(const IR::TypeParameters* typeParameters, const IR::ParameterList* parameters);
    Type_MethodBase(Util::SourceInfo srcInfo, const IR::ParameterList* parameters);
    Type_MethodBase(const IR::ParameterList* parameters);
    IRNODE_ABSTRACT_SUBCLASS(Type_MethodBase)
};
}  // namespace IR
namespace IR {
/// Type for a method or function.
class Type_Method : public Type_MethodBase {
 public:

    cstring name;
#line 584 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 2275 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Method const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Method"; }
    static cstring static_type_name() { return "Type_Method"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Method(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_MethodBase const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Method(Util::SourceInfo srcInfo, const IR::TypeParameters* typeParameters, const IR::Type* returnType, const IR::ParameterList* parameters, cstring name);
    Type_Method(const IR::TypeParameters* typeParameters, const IR::Type* returnType, const IR::ParameterList* parameters, cstring name);
    Type_Method(Util::SourceInfo srcInfo, const IR::Type* returnType, const IR::ParameterList* parameters, cstring name);
    Type_Method(const IR::Type* returnType, const IR::ParameterList* parameters, cstring name);
    Type_Method(Util::SourceInfo srcInfo, const IR::TypeParameters* typeParameters, const IR::ParameterList* parameters, cstring name);
    Type_Method(const IR::TypeParameters* typeParameters, const IR::ParameterList* parameters, cstring name);
    Type_Method(Util::SourceInfo srcInfo, const IR::ParameterList* parameters, cstring name);
    Type_Method(const IR::ParameterList* parameters, cstring name);
    IRNODE_SUBCLASS(Type_Method)
};
}  // namespace IR
namespace IR {
/// Describes an argument of a MethodCall
/// Never used in the program IR; only used by typechecker.
class ArgumentInfo : public Node {
 public:
    bool leftValue;
    bool compileTimeConstant;
    const IR::Type* type = nullptr;
    const IR::Argument* argument = nullptr;
#line 594 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 2309 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ArgumentInfo const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ArgumentInfo"; }
    static cstring static_type_name() { return "ArgumentInfo"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ArgumentInfo(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ArgumentInfo(Util::SourceInfo srcInfo, bool leftValue, bool compileTimeConstant, const IR::Type* type, const IR::Argument* argument);
    ArgumentInfo(bool leftValue, bool compileTimeConstant, const IR::Type* type, const IR::Argument* argument);
    IRNODE_SUBCLASS(ArgumentInfo)
};
}  // namespace IR
namespace IR {
/// Used to represent the type of a MethodCallExpression
/// for unification.
/// Never appears in the program IR; only used by the typechecker.
class Type_MethodCall : public Type {
 public:
    const IR::Vector<IR::Type>* typeArguments = nullptr;
    const IR::Type_Var* returnType = nullptr;
    const IR::Vector<IR::ArgumentInfo>* arguments = nullptr;
#line 605 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 2338 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 606 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    IR::Type const * getP4Type() const override;
#line 2341 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 607 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    cstring toString() const override;
#line 2344 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_MethodCall const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_MethodCall"; }
    static cstring static_type_name() { return "Type_MethodCall"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_MethodCall(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_MethodCall(Util::SourceInfo srcInfo, const IR::Vector<IR::Type>* typeArguments, const IR::Type_Var* returnType, const IR::Vector<IR::ArgumentInfo>* arguments);
    Type_MethodCall(const IR::Vector<IR::Type>* typeArguments, const IR::Type_Var* returnType, const IR::Vector<IR::ArgumentInfo>* arguments);
    IRNODE_SUBCLASS(Type_MethodCall)
};
}  // namespace IR
namespace IR {
/// Actions look a lot like methods in many respects.
/// However, invoking an action returns another action
/// Having different IR nodes allows performing different transforms in visitors
class Type_Action : public Type_MethodBase {
 public:
    bool operator==(IR::Type_Action const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Action"; }
    static cstring static_type_name() { return "Type_Action"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Action(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_MethodBase const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Action(Util::SourceInfo srcInfo, const IR::TypeParameters* typeParameters, const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_Action(const IR::TypeParameters* typeParameters, const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_Action(Util::SourceInfo srcInfo, const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_Action(const IR::Type* returnType, const IR::ParameterList* parameters);
    Type_Action(Util::SourceInfo srcInfo, const IR::TypeParameters* typeParameters, const IR::ParameterList* parameters);
    Type_Action(const IR::TypeParameters* typeParameters, const IR::ParameterList* parameters);
    Type_Action(Util::SourceInfo srcInfo, const IR::ParameterList* parameters);
    Type_Action(const IR::ParameterList* parameters);
    IRNODE_SUBCLASS(Type_Action)
};
}  // namespace IR
namespace IR {
class Method : public Declaration, public virtual IAnnotated, public virtual IFunctional, public virtual ISimpleNamespace {
 public:
    const IR::Type_Method* type = nullptr;
    bool isAbstract = false;
    const IR::Annotations* annotations = Annotations::empty;
#line 621 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t maxParameterCount() const;
#line 2397 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 622 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    size_t minParameterCount() const;
#line 2400 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 623 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void setAbstract();
#line 2403 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 624 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 2406 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 625 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::ParameterList *getParameters() const override;
#line 2409 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

#line 627 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 2413 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 629 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 2416 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Method const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Method"; }
    static cstring static_type_name() { return "Method"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Method(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Method(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Method* type, bool isAbstract, const IR::Annotations* annotations);
    Method(IR::ID name, const IR::Type_Method* type, bool isAbstract, const IR::Annotations* annotations);
    Method(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Method* type, const IR::Annotations* annotations);
    Method(IR::ID name, const IR::Type_Method* type, const IR::Annotations* annotations);
    Method(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Method* type, bool isAbstract);
    Method(IR::ID name, const IR::Type_Method* type, bool isAbstract);
    Method(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Method* type);
    Method(IR::ID name, const IR::Type_Method* type);
    IRNODE_SUBCLASS(Method)
};
}  // namespace IR
namespace IR {
class Type_Typedef : public Type_Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* type = nullptr;
#line 636 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 2450 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 637 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 2453 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Typedef const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Typedef"; }
    static cstring static_type_name() { return "Type_Typedef"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Typedef(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Typedef(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    Type_Typedef(IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    Type_Typedef(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type);
    Type_Typedef(IR::ID name, const IR::Type* type);
    IRNODE_SUBCLASS(Type_Typedef)
};
}  // namespace IR
namespace IR {
/// A newtype is similar to typedef, but it introduces a new type; the
/// the new type does not inherit any
/// of the operations of the original type.  The new type has
/// assignment, equality, and casts to/from the original type.
/// The keyword for newtype is actually `type'.
class Type_Newtype : public Type_Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* type = nullptr;
#line 649 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    int width_bits() const override;
#line 2486 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 650 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 2489 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Newtype const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Type_Newtype"; }
    static cstring static_type_name() { return "Type_Newtype"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Newtype(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Newtype(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    Type_Newtype(IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    Type_Newtype(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type);
    Type_Newtype(IR::ID name, const IR::Type* type);
    IRNODE_SUBCLASS(Type_Newtype)
};
}  // namespace IR
namespace IR {
/// An 'extern' black-box (not a function)
class Type_Extern : public Type_Declaration, public virtual INestedNamespace, public virtual IGeneralNamespace, public virtual IMayBeGenericType, public virtual IAnnotated {
 public:
    const IR::TypeParameters* typeParameters = new TypeParameters;
    IR::Vector<IR::Method> methods;

    IR::NameMap<IR::Attribute, ordered_map> attributes;

    const IR::Annotations* annotations = Annotations::empty;
#line 662 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    std::vector<const IR::INamespace *> getNestedNamespaces() const override;
#line 2522 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 663 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 2525 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 666 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    virtual const IR::TypeParameters *getTypeParameters() const override;
#line 2528 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 667 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    void validate() const override;
#line 2531 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 668 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Annotations *getAnnotations() const override;
#line 2534 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// Returns the method that matches the specified arguments.
/// Returns nullptr if no method or more than one method match.
/// In the latter case it also reports an error.
#line 672 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Method *lookupMethod(IR::ID name, const IR::Vector<IR::Argument>* arguments) const;
#line 2540 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// Returns the constructor that matches the specified arguments.
/// Returns nullptr if no constructor or more than one constructor matches.
/// In the latter case it also reports an error.
#line 676 "/home/aa/ppk_sw/ppk/p4c/ir/type.def"
    const IR::Method *lookupConstructor(const IR::Vector<IR::Argument>* arguments) const;
#line 2546 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Extern const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_Extern"; }
    static cstring static_type_name() { return "Type_Extern"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Extern(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, IR::NameMap<IR::Attribute, ordered_map> attributes, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, IR::Vector<IR::Method> methods, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, IR::Vector<IR::Method> methods, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Type_Extern(IR::ID name, const IR::Annotations* annotations);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(IR::ID name, IR::Vector<IR::Method> methods, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(IR::ID name, IR::NameMap<IR::Attribute, ordered_map> attributes);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters, IR::Vector<IR::Method> methods);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, IR::Vector<IR::Method> methods);
    Type_Extern(IR::ID name, IR::Vector<IR::Method> methods);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    Type_Extern(IR::ID name, const IR::TypeParameters* typeParameters);
    Type_Extern(Util::SourceInfo srcInfo, IR::ID name);
    Type_Extern(IR::ID name);
    IRNODE_SUBCLASS(Type_Extern)
};
}  // namespace IR
namespace IR {
/** @} */
/* end group irdefs */
/* -*-C++-*- */
/* This file contains the IR classes for all expressions.
   The base classes are in base.def */
/** \addtogroup irdefs
  * @{
  */
class Operation_Unary : public Operation {
 public:
    const IR::Expression* expr = nullptr;
    int getPrecedence() const override { return DBPrint::Prec_Prefix; }
    bool operator==(IR::Operation_Unary const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Operation_Unary"; }
    static cstring static_type_name() { return "Operation_Unary"; }
    void toJSON(JSONGenerator & json) const override;
    Operation_Unary(JSONLoader & json);
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Operation_Unary(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* expr);
    Operation_Unary(const IR::Type* type, const IR::Expression* expr);
    Operation_Unary(Util::SourceInfo srcInfo, const IR::Expression* expr);
    Operation_Unary(const IR::Expression* expr);
    IRNODE_ABSTRACT_SUBCLASS(Operation_Unary)
};
}  // namespace IR
namespace IR {
class Neg : public Operation_Unary {
 public:
    cstring getStringOp() const override { return "-"; }
    bool operator==(IR::Neg const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Neg"; }
    static cstring static_type_name() { return "Neg"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Neg(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Unary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Neg(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* expr);
    Neg(const IR::Type* type, const IR::Expression* expr);
    Neg(Util::SourceInfo srcInfo, const IR::Expression* expr);
    Neg(const IR::Expression* expr);
    IRNODE_SUBCLASS(Neg)
};
}  // namespace IR
namespace IR {
class Cmpl : public Operation_Unary {
 public:
    cstring getStringOp() const override { return "~"; }
    bool operator==(IR::Cmpl const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Cmpl"; }
    static cstring static_type_name() { return "Cmpl"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Cmpl(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Unary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Cmpl(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* expr);
    Cmpl(const IR::Type* type, const IR::Expression* expr);
    Cmpl(Util::SourceInfo srcInfo, const IR::Expression* expr);
    Cmpl(const IR::Expression* expr);
    IRNODE_SUBCLASS(Cmpl)
};
}  // namespace IR
namespace IR {
class LNot : public Operation_Unary {
 public:
    cstring getStringOp() const override { return "!"; }
    bool operator==(IR::LNot const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "LNot"; }
    static cstring static_type_name() { return "LNot"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    LNot(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Unary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    LNot(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* expr);
    LNot(const IR::Type* type, const IR::Expression* expr);
    LNot(Util::SourceInfo srcInfo, const IR::Expression* expr);
    LNot(const IR::Expression* expr);
    IRNODE_SUBCLASS(LNot)
};
}  // namespace IR
namespace IR {
class Operation_Binary : public Operation {
 public:
    const IR::Expression* left = nullptr;
    const IR::Expression* right = nullptr;
    bool operator==(IR::Operation_Binary const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Operation_Binary"; }
    static cstring static_type_name() { return "Operation_Binary"; }
    void toJSON(JSONGenerator & json) const override;
    Operation_Binary(JSONLoader & json);
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Operation_Binary(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Operation_Binary(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Operation_Binary(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Operation_Binary(const IR::Expression* left, const IR::Expression* right);
    IRNODE_ABSTRACT_SUBCLASS(Operation_Binary)
};
}  // namespace IR
namespace IR {
class Operation_Ternary : public Operation {
 public:
    const IR::Expression* e0 = nullptr;
    const IR::Expression* e1 = nullptr;
    const IR::Expression* e2 = nullptr;
    bool operator==(IR::Operation_Ternary const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Operation_Ternary"; }
    static cstring static_type_name() { return "Operation_Ternary"; }
    void toJSON(JSONGenerator & json) const override;
    Operation_Ternary(JSONLoader & json);
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Operation_Ternary(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Operation_Ternary(const IR::Type* type, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Operation_Ternary(Util::SourceInfo srcInfo, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Operation_Ternary(const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    IRNODE_ABSTRACT_SUBCLASS(Operation_Ternary)
};
}  // namespace IR
namespace IR {
class Operation_Relation : public Operation_Binary {
 public:
    bool operator==(IR::Operation_Relation const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Operation_Relation"; }
    static cstring static_type_name() { return "Operation_Relation"; }
    void toJSON(JSONGenerator & json) const override;
    Operation_Relation(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Operation_Relation(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Operation_Relation(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Operation_Relation(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Operation_Relation(const IR::Expression* left, const IR::Expression* right);
    IRNODE_ABSTRACT_SUBCLASS(Operation_Relation)
};
}  // namespace IR
namespace IR {
class Mul : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "*"; }
    int getPrecedence() const override { return DBPrint::Prec_Mul; }
    bool operator==(IR::Mul const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Mul"; }
    static cstring static_type_name() { return "Mul"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Mul(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Mul(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Mul(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Mul(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Mul(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Mul)
};
}  // namespace IR
namespace IR {
class Div : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "/"; }
    int getPrecedence() const override { return DBPrint::Prec_Div; }
    bool operator==(IR::Div const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Div"; }
    static cstring static_type_name() { return "Div"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Div(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Div(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Div(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Div(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Div(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Div)
};
}  // namespace IR
namespace IR {
class Mod : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "%"; }
    int getPrecedence() const override { return DBPrint::Prec_Mod; }
    bool operator==(IR::Mod const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Mod"; }
    static cstring static_type_name() { return "Mod"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Mod(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Mod(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Mod(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Mod(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Mod(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Mod)
};
}  // namespace IR
namespace IR {
class Add : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "+"; }
    int getPrecedence() const override { return DBPrint::Prec_Add; }
    bool operator==(IR::Add const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Add"; }
    static cstring static_type_name() { return "Add"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Add(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Add(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Add(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Add(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Add(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Add)
};
}  // namespace IR
namespace IR {
class Sub : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "-"; }
    int getPrecedence() const override { return DBPrint::Prec_Sub; }
    bool operator==(IR::Sub const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Sub"; }
    static cstring static_type_name() { return "Sub"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Sub(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Sub(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Sub(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Sub(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Sub(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Sub)
};
}  // namespace IR
namespace IR {
class AddSat : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "|+|"; }
    int getPrecedence() const override { return DBPrint::Prec_AddSat; }
    bool operator==(IR::AddSat const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "AddSat"; }
    static cstring static_type_name() { return "AddSat"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    AddSat(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    AddSat(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    AddSat(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    AddSat(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    AddSat(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(AddSat)
};
}  // namespace IR
namespace IR {
class SubSat : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "|-|"; }
    int getPrecedence() const override { return DBPrint::Prec_SubSat; }
    bool operator==(IR::SubSat const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "SubSat"; }
    static cstring static_type_name() { return "SubSat"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    SubSat(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    SubSat(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    SubSat(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    SubSat(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    SubSat(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(SubSat)
};
}  // namespace IR
namespace IR {
class Shl : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "<<"; }
    int getPrecedence() const override { return DBPrint::Prec_Shl; }
    bool operator==(IR::Shl const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Shl"; }
    static cstring static_type_name() { return "Shl"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Shl(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Shl(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Shl(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Shl(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Shl(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Shl)
};
}  // namespace IR
namespace IR {
class Shr : public Operation_Binary {
 public:
    cstring getStringOp() const override { return ">>"; }
    int getPrecedence() const override { return DBPrint::Prec_Shr; }
    bool operator==(IR::Shr const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Shr"; }
    static cstring static_type_name() { return "Shr"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Shr(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Shr(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Shr(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Shr(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Shr(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Shr)
};
}  // namespace IR
namespace IR {
class Equ : public Operation_Relation {
 public:
    cstring getStringOp() const override { return "=="; }
    int getPrecedence() const override { return DBPrint::Prec_Equ; }
    bool operator==(IR::Equ const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Equ"; }
    static cstring static_type_name() { return "Equ"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Equ(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Relation const & a) const override { return a == *this; }
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Equ(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Equ(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Equ(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Equ(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Equ)
};
}  // namespace IR
namespace IR {
class Neq : public Operation_Relation {
 public:
    cstring getStringOp() const override { return "!="; }
    int getPrecedence() const override { return DBPrint::Prec_Neq; }
    bool operator==(IR::Neq const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Neq"; }
    static cstring static_type_name() { return "Neq"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Neq(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Relation const & a) const override { return a == *this; }
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Neq(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Neq(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Neq(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Neq(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Neq)
};
}  // namespace IR
namespace IR {
class Lss : public Operation_Relation {
 public:
    cstring getStringOp() const override { return "<"; }
    int getPrecedence() const override { return DBPrint::Prec_Lss; }
    bool operator==(IR::Lss const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Lss"; }
    static cstring static_type_name() { return "Lss"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Lss(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Relation const & a) const override { return a == *this; }
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Lss(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Lss(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Lss(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Lss(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Lss)
};
}  // namespace IR
namespace IR {
class Leq : public Operation_Relation {
 public:
    cstring getStringOp() const override { return "<="; }
    int getPrecedence() const override { return DBPrint::Prec_Leq; }
    bool operator==(IR::Leq const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Leq"; }
    static cstring static_type_name() { return "Leq"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Leq(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Relation const & a) const override { return a == *this; }
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Leq(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Leq(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Leq(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Leq(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Leq)
};
}  // namespace IR
namespace IR {
class Grt : public Operation_Relation {
 public:
    cstring getStringOp() const override { return ">"; }
    int getPrecedence() const override { return DBPrint::Prec_Grt; }
    bool operator==(IR::Grt const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Grt"; }
    static cstring static_type_name() { return "Grt"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Grt(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Relation const & a) const override { return a == *this; }
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Grt(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Grt(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Grt(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Grt(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Grt)
};
}  // namespace IR
namespace IR {
class Geq : public Operation_Relation {
 public:
    cstring getStringOp() const override { return ">="; }
    int getPrecedence() const override { return DBPrint::Prec_Geq; }
    bool operator==(IR::Geq const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Geq"; }
    static cstring static_type_name() { return "Geq"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Geq(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Relation const & a) const override { return a == *this; }
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Geq(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Geq(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Geq(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Geq(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Geq)
};
}  // namespace IR
namespace IR {
class BAnd : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "&"; }
    int getPrecedence() const override { return DBPrint::Prec_BAnd; }
    bool operator==(IR::BAnd const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "BAnd"; }
    static cstring static_type_name() { return "BAnd"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    BAnd(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    BAnd(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    BAnd(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    BAnd(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    BAnd(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(BAnd)
};
}  // namespace IR
namespace IR {
class BOr : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "|"; }
    int getPrecedence() const override { return DBPrint::Prec_BOr; }
    bool operator==(IR::BOr const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "BOr"; }
    static cstring static_type_name() { return "BOr"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    BOr(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    BOr(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    BOr(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    BOr(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    BOr(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(BOr)
};
}  // namespace IR
namespace IR {
class BXor : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "^"; }
    int getPrecedence() const override { return DBPrint::Prec_BXor; }
    bool operator==(IR::BXor const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "BXor"; }
    static cstring static_type_name() { return "BXor"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    BXor(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    BXor(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    BXor(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    BXor(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    BXor(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(BXor)
};
}  // namespace IR
namespace IR {
class LAnd : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "&&"; }
    int getPrecedence() const override { return DBPrint::Prec_LAnd; }
    bool operator==(IR::LAnd const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "LAnd"; }
    static cstring static_type_name() { return "LAnd"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    LAnd(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    LAnd(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    LAnd(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    LAnd(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    LAnd(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(LAnd)
};
}  // namespace IR
namespace IR {
class LOr : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "||"; }
    int getPrecedence() const override { return DBPrint::Prec_LOr; }
    bool operator==(IR::LOr const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "LOr"; }
    static cstring static_type_name() { return "LOr"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    LOr(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    LOr(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    LOr(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    LOr(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    LOr(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(LOr)
};
}  // namespace IR
namespace IR {
class Literal : public Expression, public virtual CompileTimeValue {
 public:
    bool operator==(IR::Literal const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Literal"; }
    static cstring static_type_name() { return "Literal"; }
    void toJSON(JSONGenerator & json) const override;
    Literal(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Literal(Util::SourceInfo srcInfo, const IR::Type* type);
    Literal(const IR::Type* type);
    Literal(Util::SourceInfo srcInfo);
    Literal();
    IRNODE_ABSTRACT_SUBCLASS(Literal)
};
}  // namespace IR
namespace IR {
/// This is an integer literal on arbitrary-precision.
class Constant : public Literal {
 public:
    big_int value;
    unsigned base;
/// base used when reading/writing
/// if noWarning is true, no warning is emitted
#line 143 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void handleOverflow(bool noWarning);
#line 3284 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"



#line 147 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant(int v, unsigned base = 10);
#line 3290 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 149 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant(unsigned v, unsigned base = 10);
#line 3293 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 152 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
#if __WORDSIZE == 64
    Constant(intmax_t v, unsigned base = 10) :
        Literal(new Type_InfInt()), value(v), base(base) {}
#else
    Constant(long v, unsigned base = 10) :
        Literal(new Type_InfInt()), value(v), base(base) {}
    Constant(unsigned long v, unsigned base = 10) :
        Literal(new Type_InfInt()), value(v), base(base) {}
    Constant(intmax_t v, unsigned base = 10) :
        Literal(new Type_InfInt()), value(v), base(base) {}
#endif
#line 3306 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 164 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant(uint64_t v, unsigned base = 10);
#line 3309 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 166 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant(big_int v, unsigned base = 10);
#line 3312 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 168 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant(Util::SourceInfo si, big_int v, unsigned base = 10);
#line 3315 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 170 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant(IR::Type const * t, big_int v, unsigned base = 10, bool noWarning = false);
#line 3318 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 172 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant(Util::SourceInfo si, IR::Type const * t, big_int v, unsigned base = 10, bool noWarning = false);
#line 3321 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 176 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    static Constant GetMask(unsigned width);
#line 3324 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 178 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    bool fitsInt() const;
#line 3327 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 179 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    bool fitsLong() const;
#line 3330 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 180 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    bool fitsUint() const;
#line 3333 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 181 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    bool fitsUint64() const;
#line 3336 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 182 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    bool fitsInt64() const;
#line 3339 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 183 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    long asLong() const;
#line 3342 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 187 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    int asInt() const;
#line 3345 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 191 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    unsigned asUnsigned() const;
#line 3348 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 196 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    uint64_t asUint64() const;
#line 3351 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 201 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    int64_t asInt64() const;
#line 3354 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"



#line 210 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Constant operator<<(const unsigned &shift) const;
    Constant operator>>(const unsigned &shift) const;
    Constant operator&(const Constant &c) const;
    Constant operator|(const Constant &c) const;
    Constant operator^(const Constant &c) const;
    Constant operator-(const Constant &c) const;
    Constant operator-() const;
#line 3366 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 218 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3369 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 231 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 3373 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Constant const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Constant"; }
    static cstring static_type_name() { return "Constant"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Constant(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Literal const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(Constant)
};
}  // namespace IR
namespace IR {
class BoolLiteral : public Literal {
 public:
    bool value;
#line 236 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3395 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::BoolLiteral const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "BoolLiteral"; }
    static cstring static_type_name() { return "BoolLiteral"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    BoolLiteral(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Literal const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    BoolLiteral(Util::SourceInfo srcInfo, const IR::Type* type, bool value);
    BoolLiteral(const IR::Type* type, bool value);
    BoolLiteral(Util::SourceInfo srcInfo, bool value);
    BoolLiteral(bool value);
    IRNODE_SUBCLASS(BoolLiteral)
};
}  // namespace IR
namespace IR {
class StringLiteral : public Literal {
 public:
    cstring value;
#line 241 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 3421 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 242 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3424 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 243 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    StringLiteral(IR::ID v);
#line 3427 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 245 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    operator IR::ID() const { return IR::ID(srcInfo, value); }
#line 3430 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::StringLiteral const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "StringLiteral"; }
    static cstring static_type_name() { return "StringLiteral"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    StringLiteral(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Literal const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    StringLiteral(Util::SourceInfo srcInfo, const IR::Type* type, cstring value);
    StringLiteral(const IR::Type* type, cstring value);
    StringLiteral(Util::SourceInfo srcInfo, cstring value);
    StringLiteral(cstring value);
    IRNODE_SUBCLASS(StringLiteral)
};
}  // namespace IR
namespace IR {
class PathExpression : public Expression {
 public:
    const IR::Path* path = nullptr;
#line 252 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    PathExpression(IR::ID id);
#line 3456 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 253 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3459 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::PathExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "PathExpression"; }
    static cstring static_type_name() { return "PathExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    PathExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    PathExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Path* path);
    PathExpression(const IR::Type* type, const IR::Path* path);
    PathExpression(Util::SourceInfo srcInfo, const IR::Path* path);
    PathExpression(const IR::Path* path);
    IRNODE_SUBCLASS(PathExpression)
};
}  // namespace IR
namespace IR {

class TypeNameExpression : public Expression {
 public:
    const IR::Type_Name* typeName = nullptr;
#line 262 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    TypeNameExpression(IR::ID id);
#line 3487 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 264 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void dbprint(std::ostream & out) const override;
#line 3490 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 265 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3493 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::TypeNameExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "TypeNameExpression"; }
    static cstring static_type_name() { return "TypeNameExpression"; }
    void toJSON(JSONGenerator & json) const override;
    TypeNameExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    TypeNameExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Type_Name* typeName);
    TypeNameExpression(const IR::Type* type, const IR::Type_Name* typeName);
    TypeNameExpression(Util::SourceInfo srcInfo, const IR::Type_Name* typeName);
    TypeNameExpression(const IR::Type_Name* typeName);
    IRNODE_SUBCLASS(TypeNameExpression)
};
}  // namespace IR
namespace IR {
class Slice : public Operation_Ternary {
 public:
    int getPrecedence() const override { return DBPrint::Prec_Postfix; }
    cstring getStringOp() const override { return "[:]"; }
#line 271 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3520 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

#line 273 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    unsigned getH() const;
#line 3524 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 274 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    unsigned getL() const;
#line 3527 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 275 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Slice(const IR::Expression* a, int hi, int lo);
#line 3530 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 277 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    Slice(Util::SourceInfo si, const IR::Expression* a, int hi, int lo);
#line 3533 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

#line 283 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    static const IR::Expression *make(const IR::Expression* a, unsigned hi, unsigned lo);
#line 3537 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Slice const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Slice"; }
    static cstring static_type_name() { return "Slice"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Slice(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Ternary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Slice(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Slice(const IR::Type* type, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Slice(Util::SourceInfo srcInfo, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Slice(const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    IRNODE_SUBCLASS(Slice)
};
}  // namespace IR
namespace IR {
class Member : public Operation_Unary {
 public:
    int getPrecedence() const override { return DBPrint::Prec_Postfix; }
    IR::ID member;
#line 289 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    virtual int offset_bits() const;
#line 3564 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 290 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    int lsb() const;
#line 3567 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 291 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    int msb() const;
#line 3570 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    cstring getStringOp() const override { return "."; }
#line 293 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3574 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Member const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Member"; }
    static cstring static_type_name() { return "Member"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Member(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Unary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Member(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* expr, IR::ID member);
    Member(const IR::Type* type, const IR::Expression* expr, IR::ID member);
    Member(Util::SourceInfo srcInfo, const IR::Expression* expr, IR::ID member);
    Member(const IR::Expression* expr, IR::ID member);
    IRNODE_SUBCLASS(Member)
};
}  // namespace IR
namespace IR {
class Concat : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "++"; }
    int getPrecedence() const override { return DBPrint::Prec_Add; }
    bool operator==(IR::Concat const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Concat"; }
    static cstring static_type_name() { return "Concat"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Concat(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Concat(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Concat(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Concat(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Concat(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Concat)
};
}  // namespace IR
namespace IR {
class ArrayIndex : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "[]"; }
    int getPrecedence() const override { return DBPrint::Prec_Postfix; }
#line 313 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3626 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ArrayIndex const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "ArrayIndex"; }
    static cstring static_type_name() { return "ArrayIndex"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ArrayIndex(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ArrayIndex(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    ArrayIndex(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    ArrayIndex(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    ArrayIndex(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(ArrayIndex)
};
}  // namespace IR
namespace IR {
class Range : public Operation_Binary {
 public:
    cstring getStringOp() const override { return ".."; }
    int getPrecedence() const override { return DBPrint::Prec_Low; }
    bool operator==(IR::Range const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Range"; }
    static cstring static_type_name() { return "Range"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Range(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Range(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Range(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Range(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Range(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Range)
};
}  // namespace IR
namespace IR {
class Mask : public Operation_Binary {
 public:
    cstring getStringOp() const override { return "&&&"; }
    int getPrecedence() const override { return DBPrint::Prec_Low; }
    bool operator==(IR::Mask const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Mask"; }
    static cstring static_type_name() { return "Mask"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Mask(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Binary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Mask(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Mask(const IR::Type* type, const IR::Expression* left, const IR::Expression* right);
    Mask(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    Mask(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(Mask)
};
}  // namespace IR
namespace IR {
class Mux : public Operation_Ternary {
 public:
    cstring getStringOp() const override { return "?:"; }
    int getPrecedence() const override { return DBPrint::Prec_Low; }
#line 333 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 3702 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Mux const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Mux"; }
    static cstring static_type_name() { return "Mux"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Mux(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Ternary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Mux(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Mux(const IR::Type* type, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Mux(Util::SourceInfo srcInfo, const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    Mux(const IR::Expression* e0, const IR::Expression* e1, const IR::Expression* e2);
    IRNODE_SUBCLASS(Mux)
};
}  // namespace IR
namespace IR {
class DefaultExpression : public Expression {
 public:
    bool operator==(IR::DefaultExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DefaultExpression"; }
    static cstring static_type_name() { return "DefaultExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DefaultExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DefaultExpression(Util::SourceInfo srcInfo, const IR::Type* type);
    DefaultExpression(const IR::Type* type);
    DefaultExpression(Util::SourceInfo srcInfo);
    DefaultExpression();
    IRNODE_SUBCLASS(DefaultExpression)
};
}  // namespace IR
namespace IR {

class This : public Expression {
 public:
    int id = nextId++;
 private:
    static int nextId;
 public:
    bool operator==(IR::This const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "This"; }
    static cstring static_type_name() { return "This"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    This(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    This(Util::SourceInfo srcInfo, const IR::Type* type);
    This(const IR::Type* type);
    This(Util::SourceInfo srcInfo);
    This();
    IRNODE_SUBCLASS(This)
};
}  // namespace IR
namespace IR {

class Cast : public Operation_Unary {
 public:
    const IR::Type* destType = type;
/// These will generally always be the same, except when a cast to a type argument of
/// a generic occurs.  Then at some point, the 'destType' will be specialized to a concrete
/// type, and 'type' will only be updated later when type inferencing occurs
    int getPrecedence() const override { return DBPrint::Prec_Prefix; }
    cstring getStringOp() const override { return "(cast)"; }
#line 359 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3780 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 360 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 3783 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Cast const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Cast"; }
    static cstring static_type_name() { return "Cast"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Cast(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Unary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Cast(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* expr);
    Cast(const IR::Type* type, const IR::Expression* expr);
    Cast(Util::SourceInfo srcInfo, const IR::Expression* expr);
    Cast(const IR::Expression* expr);
    IRNODE_SUBCLASS(Cast)
};
}  // namespace IR
namespace IR {
class SelectCase : public Node {
 public:
    const IR::Expression* keyset = nullptr;
    const IR::PathExpression* state = nullptr;
#line 366 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void dbprint(std::ostream & out) const override;
#line 3812 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::SelectCase const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "SelectCase"; }
    static cstring static_type_name() { return "SelectCase"; }
    void toJSON(JSONGenerator & json) const override;
    SelectCase(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    SelectCase(Util::SourceInfo srcInfo, const IR::Expression* keyset, const IR::PathExpression* state);
    SelectCase(const IR::Expression* keyset, const IR::PathExpression* state);
    IRNODE_SUBCLASS(SelectCase)
};
}  // namespace IR
namespace IR {
class SelectExpression : public Expression {
 public:
    const IR::ListExpression* select = nullptr;
    IR::Vector<IR::SelectCase> selectCases;
#line 372 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 3837 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::SelectExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "SelectExpression"; }
    static cstring static_type_name() { return "SelectExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    SelectExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    SelectExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::ListExpression* select, IR::Vector<IR::SelectCase> selectCases);
    SelectExpression(const IR::Type* type, const IR::ListExpression* select, IR::Vector<IR::SelectCase> selectCases);
    SelectExpression(Util::SourceInfo srcInfo, const IR::ListExpression* select, IR::Vector<IR::SelectCase> selectCases);
    SelectExpression(const IR::ListExpression* select, IR::Vector<IR::SelectCase> selectCases);
    IRNODE_SUBCLASS(SelectExpression)
};
}  // namespace IR
namespace IR {
class MethodCallExpression : public Expression {
 public:
    const IR::Expression* method = nullptr;
    const IR::Vector<IR::Type>* typeArguments = new Vector<Type>;
    const IR::Vector<IR::Argument>* arguments = new Vector<Argument>;
#line 381 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3864 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 382 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 3867 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 383 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    MethodCallExpression(Util::SourceInfo si, IR::ID m, std::initializer_list<const IR::Argument *> const & a);
#line 3870 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 385 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    MethodCallExpression(Util::SourceInfo si, const IR::Expression* m, std::initializer_list<const IR::Argument *> const & a);
#line 3873 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 388 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    MethodCallExpression(const IR::Expression* m, std::initializer_list<IR::Expression const *> const & a);
#line 3876 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::MethodCallExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "MethodCallExpression"; }
    static cstring static_type_name() { return "MethodCallExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    MethodCallExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    MethodCallExpression(const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    MethodCallExpression(const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method);
    MethodCallExpression(const IR::Type* type, const IR::Expression* method);
    MethodCallExpression(Util::SourceInfo srcInfo, const IR::Expression* method);
    MethodCallExpression(const IR::Expression* method);
    IRNODE_SUBCLASS(MethodCallExpression)
};
}  // namespace IR
namespace IR {
class ConstructorCallExpression : public Expression {
 public:
    const IR::Type* constructedType = type;

    const IR::Vector<IR::Argument>* arguments = nullptr;
#line 398 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    cstring toString() const override;
#line 3916 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 399 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 3919 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ConstructorCallExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ConstructorCallExpression"; }
    static cstring static_type_name() { return "ConstructorCallExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ConstructorCallExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ConstructorCallExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    ConstructorCallExpression(const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    ConstructorCallExpression(Util::SourceInfo srcInfo, const IR::Vector<IR::Argument>* arguments);
    ConstructorCallExpression(const IR::Vector<IR::Argument>* arguments);
    IRNODE_SUBCLASS(ConstructorCallExpression)
};
}  // namespace IR
namespace IR {
/// Represents a list of expressions separated by commas
class ListExpression : public Expression {
 public:
    IR::Vector<IR::Expression> components;
#line 415 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 3946 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 416 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    size_t size() const;
#line 3949 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 417 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void push_back(const IR::Expression* e);
#line 3952 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ListExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ListExpression"; }
    static cstring static_type_name() { return "ListExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ListExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ListExpression(Util::SourceInfo srcInfo, const IR::Type* type, IR::Vector<IR::Expression> components);
    ListExpression(const IR::Type* type, IR::Vector<IR::Expression> components);
    ListExpression(Util::SourceInfo srcInfo, IR::Vector<IR::Expression> components);
    ListExpression(IR::Vector<IR::Expression> components);
    IRNODE_SUBCLASS(ListExpression)
};
}  // namespace IR
namespace IR {
/// An expression that evaluates to a struct.
class StructExpression : public Expression {
 public:
/// The struct or header type that is being intialized.
/// May only be known after type checking; so it can be nullptr.
    const IR::Type* structType = nullptr;
    IR::IndexedVector<IR::NamedExpression> components;
#line 426 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 3982 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 432 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    size_t size() const;
#line 3985 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::StructExpression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "StructExpression"; }
    static cstring static_type_name() { return "StructExpression"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    StructExpression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    StructExpression(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Type* structType, IR::IndexedVector<IR::NamedExpression> components);
    StructExpression(const IR::Type* type, const IR::Type* structType, IR::IndexedVector<IR::NamedExpression> components);
    StructExpression(Util::SourceInfo srcInfo, const IR::Type* structType, IR::IndexedVector<IR::NamedExpression> components);
    StructExpression(const IR::Type* structType, IR::IndexedVector<IR::NamedExpression> components);
    IRNODE_SUBCLASS(StructExpression)
};
}  // namespace IR
namespace IR {
/// A ListExpression where all the components are compile-time values.
/// This is used by the evaluator pass.
class ListCompileTimeValue : public Node, public virtual CompileTimeValue {
 public:
    IR::Vector<IR::Node> components;
#line 439 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 4013 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ListCompileTimeValue const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ListCompileTimeValue"; }
    static cstring static_type_name() { return "ListCompileTimeValue"; }
    void toJSON(JSONGenerator & json) const override;
    ListCompileTimeValue(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ListCompileTimeValue(Util::SourceInfo srcInfo, IR::Vector<IR::Node> components);
    ListCompileTimeValue(IR::Vector<IR::Node> components);
    IRNODE_SUBCLASS(ListCompileTimeValue)
};
}  // namespace IR
namespace IR {
/// Experimental: an extern methond/function call with constant arguments to be
/// evaluated at compile time
class CompileTimeMethodCall : public MethodCallExpression, public virtual CompileTimeValue {
 public:
#line 448 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    CompileTimeMethodCall(const IR::MethodCallExpression* e);
#line 4036 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 449 "/home/aa/ppk_sw/ppk/p4c/ir/expression.def"
    void validate() const override;
#line 4039 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::CompileTimeMethodCall const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "CompileTimeMethodCall"; }
    static cstring static_type_name() { return "CompileTimeMethodCall"; }
    void toJSON(JSONGenerator & json) const override;
    CompileTimeMethodCall(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::MethodCallExpression const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(const IR::Expression* method, const IR::Vector<IR::Argument>* arguments);
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    CompileTimeMethodCall(const IR::Type* type, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    CompileTimeMethodCall(const IR::Expression* method, const IR::Vector<IR::Type>* typeArguments);
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* method);
    CompileTimeMethodCall(const IR::Type* type, const IR::Expression* method);
    CompileTimeMethodCall(Util::SourceInfo srcInfo, const IR::Expression* method);
    CompileTimeMethodCall(const IR::Expression* method);
    IRNODE_SUBCLASS(CompileTimeMethodCall)
};
}  // namespace IR
namespace IR {
/** @} */
/* end group irdefs */
/* -*-C++-*- */
/** \addtogroup irdefs
  * @{
  */
/**
  P4 compiler IR definition file.
  To understand this you really have to understand the shape of the generated IR code.

  The IR builder "knows" the following methods for a class T:

  cstring toString() const override;
  void dbprint(std::ostream &out) const override;
  bool operator==(const T &a) const;
  void validate() const;
  const char *node_type_name() const;
  void visit_children(Visitor &v);
  void dump_fields(std::ostream& out) const;

  C comments are ignored.
  C++ line comments can appear in some places and are emitted in the output.

  #emit/#end      -> copy text literally to output header file
  #emit_impl/#end -> copy text literally to output C++ file
  #noXXX          -> do not emit the specified implementation for the XXX method
                     e.g., #noconstructor, #nodbprint, #novisit_children
  #apply          -> generate apply overload for visitors
  method{ ... }   -> specifies an implementation for a default method
                     method can be 'operator=='

  Some assignments are turned into methods returning constant values:
  stringOp     -> cstring getStringOp() const
  precedence   -> int getPrecedence() const

  When defining fields and methods in an IR class, all fields and arguments of any IR
  class type are automatically converted into const IR::class * fields or arguments, unless
  they are identified as 'inline'.  It is not possible to create a non-const pointer to
  an IR class in any other IR class.

  There are some special keywords that can be applied (as decl modifiers) to fields that
  affect how they are created, checked, and initialized.

  inline      The field (of IR class type) should be defined directly in the object
              rather than being converted to a const T * as described above.
  NullOK      The field is a ppointer and may be null (verify will check that it is not otherwise)
  optional    The field may be proveded as an argument to the constructor but need not be.
              (overloaded constructors will be created as needed)

  Unless there is a '#noconstructor' tag in the class, a constructor
  will automatically be generated that takes as arguments values to
  initialize all fields of the IR class and its bases that do not have
  explicit initializers.  Fields marked 'optional' will create multiple
  constructors both with and without an argument for that field.
 */
class ParserState : public virtual ISimpleNamespace, public Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    IR::IndexedVector<IR::StatOrDecl> components;

    const IR::Expression* selectExpression = nullptr;
#line 64 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4133 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 65 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4136 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 67 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::IDeclaration const * getDeclByName(cstring name) const override;
#line 4139 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    static const cstring accept;
    static const cstring reject;
    static const cstring start;
    static const cstring verify;
#line 74 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    bool isBuiltin() const;
#line 4146 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 75 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4149 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ParserState const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ParserState"; }
    static cstring static_type_name() { return "ParserState"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ParserState(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ParserState(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StatOrDecl> components, const IR::Expression* selectExpression);
    ParserState(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StatOrDecl> components, const IR::Expression* selectExpression);
    ParserState(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StatOrDecl> components, const IR::Expression* selectExpression);
    ParserState(IR::ID name, IR::IndexedVector<IR::StatOrDecl> components, const IR::Expression* selectExpression);
    ParserState(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Expression* selectExpression);
    ParserState(IR::ID name, const IR::Annotations* annotations, const IR::Expression* selectExpression);
    ParserState(Util::SourceInfo srcInfo, IR::ID name, const IR::Expression* selectExpression);
    ParserState(IR::ID name, const IR::Expression* selectExpression);
    IRNODE_SUBCLASS(ParserState)
};
}  // namespace IR
namespace IR {

class P4Parser : public Type_Declaration, public virtual INestedNamespace, public virtual ISimpleNamespace, public virtual IApply, public virtual IContainer {
 public:
    const IR::Type_Parser* type = nullptr;
    const IR::ParameterList* constructorParams = new ParameterList;
    IR::IndexedVector<IR::Declaration> parserLocals;
    IR::IndexedVector<IR::ParserState> states;
#line 90 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::TypeParameters *getTypeParameters() const override;
#line 4184 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 91 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    std::vector<const IR::INamespace *> getNestedNamespaces() const override;
#line 4187 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 93 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4190 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 95 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 4193 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 99 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Type_Method *getApplyMethodType() const override;
#line 4196 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 100 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getApplyParameters() const override;
#line 4199 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 101 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Type_Method *getConstructorMethodType() const override;
#line 4202 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 102 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getConstructorParameters() const override;
#line 4205 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 103 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void checkDuplicates() const;
#line 4208 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    IRNODE_DECLARE_APPLY_OVERLOAD(P4Parser)
#line 105 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4212 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 112 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 4215 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::P4Parser const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "P4Parser"; }
    static cstring static_type_name() { return "P4Parser"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    P4Parser(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::Declaration> parserLocals, IR::IndexedVector<IR::ParserState> states);
    P4Parser(IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::Declaration> parserLocals, IR::IndexedVector<IR::ParserState> states);
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type, IR::IndexedVector<IR::Declaration> parserLocals, IR::IndexedVector<IR::ParserState> states);
    P4Parser(IR::ID name, const IR::Type_Parser* type, IR::IndexedVector<IR::Declaration> parserLocals, IR::IndexedVector<IR::ParserState> states);
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::ParserState> states);
    P4Parser(IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::ParserState> states);
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type, IR::IndexedVector<IR::ParserState> states);
    P4Parser(IR::ID name, const IR::Type_Parser* type, IR::IndexedVector<IR::ParserState> states);
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::Declaration> parserLocals);
    P4Parser(IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::Declaration> parserLocals);
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type, IR::IndexedVector<IR::Declaration> parserLocals);
    P4Parser(IR::ID name, const IR::Type_Parser* type, IR::IndexedVector<IR::Declaration> parserLocals);
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams);
    P4Parser(IR::ID name, const IR::Type_Parser* type, const IR::ParameterList* constructorParams);
    P4Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Parser* type);
    P4Parser(IR::ID name, const IR::Type_Parser* type);
    IRNODE_SUBCLASS(P4Parser)
};
}  // namespace IR
namespace IR {
class P4Control : public Type_Declaration, public virtual INestedNamespace, public virtual ISimpleNamespace, public virtual IApply, public virtual IContainer {
 public:
    const IR::Type_Control* type = nullptr;
    const IR::ParameterList* constructorParams = new ParameterList;
    IR::IndexedVector<IR::Declaration> controlLocals;
    const IR::BlockStatement* body = nullptr;
#line 121 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::TypeParameters *getTypeParameters() const override;
#line 4257 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 122 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    std::vector<const IR::INamespace *> getNestedNamespaces() const override;
#line 4260 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 124 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4263 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 126 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Type_Method *getApplyMethodType() const override;
#line 4266 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 127 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getApplyParameters() const override;
#line 4269 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 128 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Type_Method *getConstructorMethodType() const override;
#line 4272 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 129 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 4275 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 131 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getConstructorParameters() const override;
#line 4278 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    IRNODE_DECLARE_APPLY_OVERLOAD(P4Control)
#line 133 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4282 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 138 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 4285 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::P4Control const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "P4Control"; }
    static cstring static_type_name() { return "P4Control"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    P4Control(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    P4Control(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Control* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::Declaration> controlLocals, const IR::BlockStatement* body);
    P4Control(IR::ID name, const IR::Type_Control* type, const IR::ParameterList* constructorParams, IR::IndexedVector<IR::Declaration> controlLocals, const IR::BlockStatement* body);
    P4Control(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Control* type, IR::IndexedVector<IR::Declaration> controlLocals, const IR::BlockStatement* body);
    P4Control(IR::ID name, const IR::Type_Control* type, IR::IndexedVector<IR::Declaration> controlLocals, const IR::BlockStatement* body);
    P4Control(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Control* type, const IR::ParameterList* constructorParams, const IR::BlockStatement* body);
    P4Control(IR::ID name, const IR::Type_Control* type, const IR::ParameterList* constructorParams, const IR::BlockStatement* body);
    P4Control(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Control* type, const IR::BlockStatement* body);
    P4Control(IR::ID name, const IR::Type_Control* type, const IR::BlockStatement* body);
    IRNODE_SUBCLASS(P4Control)
};
}  // namespace IR
namespace IR {
/// A P4-16 action
class P4Action : public Declaration, public virtual ISimpleNamespace, public virtual IAnnotated, public virtual IFunctional {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::ParameterList* parameters = nullptr;
    const IR::BlockStatement* body = nullptr;
#line 146 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4319 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 148 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 4322 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 150 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4325 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 151 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getParameters() const override;
#line 4328 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::P4Action const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "P4Action"; }
    static cstring static_type_name() { return "P4Action"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    P4Action(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    P4Action(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* parameters, const IR::BlockStatement* body);
    P4Action(IR::ID name, const IR::Annotations* annotations, const IR::ParameterList* parameters, const IR::BlockStatement* body);
    P4Action(Util::SourceInfo srcInfo, IR::ID name, const IR::ParameterList* parameters, const IR::BlockStatement* body);
    P4Action(IR::ID name, const IR::ParameterList* parameters, const IR::BlockStatement* body);
    IRNODE_SUBCLASS(P4Action)
};
}  // namespace IR
namespace IR {
class Type_Error : public virtual ISimpleNamespace, public Type_Declaration {
 public:
    static const cstring error;
    IR::IndexedVector<IR::Declaration_ID> members;
#line 157 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4357 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 159 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 4360 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 161 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4363 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Error const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Type_Error"; }
    static cstring static_type_name() { return "Type_Error"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Type_Error(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Error(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::Declaration_ID> members);
    Type_Error(IR::ID name, IR::IndexedVector<IR::Declaration_ID> members);
    Type_Error(Util::SourceInfo srcInfo, IR::ID name);
    Type_Error(IR::ID name);
    IRNODE_SUBCLASS(Type_Error)
};
}  // namespace IR
namespace IR {

class Declaration_MatchKind : public Node, public virtual ISimpleNamespace {
 public:
    IR::IndexedVector<IR::Declaration_ID> members;
#line 167 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4391 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 169 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 4394 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 171 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4397 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Declaration_MatchKind const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Declaration_MatchKind"; }
    static cstring static_type_name() { return "Declaration_MatchKind"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Declaration_MatchKind(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Declaration_MatchKind(Util::SourceInfo srcInfo, IR::IndexedVector<IR::Declaration_ID> members);
    Declaration_MatchKind(IR::IndexedVector<IR::Declaration_ID> members);
    Declaration_MatchKind(Util::SourceInfo srcInfo);
    Declaration_MatchKind();
    IRNODE_SUBCLASS(Declaration_MatchKind)
};
}  // namespace IR
namespace IR {
/// Table property value abstract base class
class PropertyValue : public Node {
 public:
    bool operator==(IR::PropertyValue const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "PropertyValue"; }
    static cstring static_type_name() { return "PropertyValue"; }
    void toJSON(JSONGenerator & json) const override;
    PropertyValue(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    PropertyValue(Util::SourceInfo srcInfo);
    PropertyValue();
    IRNODE_ABSTRACT_SUBCLASS(PropertyValue)
};
}  // namespace IR
namespace IR {
/// A table property whose value is an expression
class ExpressionValue : public PropertyValue {
 public:
    const IR::Expression* expression = nullptr;
#line 180 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4440 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ExpressionValue const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ExpressionValue"; }
    static cstring static_type_name() { return "ExpressionValue"; }
    void toJSON(JSONGenerator & json) const override;
    ExpressionValue(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::PropertyValue const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ExpressionValue(Util::SourceInfo srcInfo, const IR::Expression* expression);
    ExpressionValue(const IR::Expression* expression);
    IRNODE_SUBCLASS(ExpressionValue)
};
}  // namespace IR
namespace IR {
class ExpressionListValue : public PropertyValue {
 public:
    IR::Vector<IR::Expression> expressions;
#line 185 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4464 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ExpressionListValue const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ExpressionListValue"; }
    static cstring static_type_name() { return "ExpressionListValue"; }
    void toJSON(JSONGenerator & json) const override;
    ExpressionListValue(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::PropertyValue const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ExpressionListValue(Util::SourceInfo srcInfo, IR::Vector<IR::Expression> expressions);
    ExpressionListValue(IR::Vector<IR::Expression> expressions);
    IRNODE_SUBCLASS(ExpressionListValue)
};
}  // namespace IR
namespace IR {

class ActionListElement : public Node, public virtual IAnnotated, public virtual IDeclaration {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Expression* expression = nullptr;

#line 192 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4491 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 193 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::ID getName() const override;
#line 4494 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 194 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Path *getPath() const;
#line 4497 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 195 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4500 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 196 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4503 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 201 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 4506 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ActionListElement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ActionListElement"; }
    static cstring static_type_name() { return "ActionListElement"; }
    void toJSON(JSONGenerator & json) const override;
    ActionListElement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ActionListElement(Util::SourceInfo srcInfo, const IR::Annotations* annotations, const IR::Expression* expression);
    ActionListElement(const IR::Annotations* annotations, const IR::Expression* expression);
    ActionListElement(Util::SourceInfo srcInfo, const IR::Expression* expression);
    ActionListElement(const IR::Expression* expression);
    IRNODE_SUBCLASS(ActionListElement)
};
}  // namespace IR
namespace IR {

class ActionList : public PropertyValue {
 public:
    IR::IndexedVector<IR::ActionListElement> actionList;
#line 207 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4531 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 208 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    size_t size() const;
#line 4534 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 209 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void push_back(const IR::ActionListElement* e);
#line 4537 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 210 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ActionListElement *getDeclaration(cstring n) const;
#line 4540 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ActionList const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ActionList"; }
    static cstring static_type_name() { return "ActionList"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ActionList(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::PropertyValue const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ActionList(Util::SourceInfo srcInfo, IR::IndexedVector<IR::ActionListElement> actionList);
    ActionList(IR::IndexedVector<IR::ActionListElement> actionList);
    IRNODE_SUBCLASS(ActionList)
};
}  // namespace IR
namespace IR {
class KeyElement : public Node, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Expression* expression = nullptr;
    const IR::PathExpression* matchType = nullptr;
#line 219 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4566 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 220 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::Node const * transform_visit(Transform & v);
#line 4569 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::KeyElement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "KeyElement"; }
    static cstring static_type_name() { return "KeyElement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    KeyElement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    KeyElement(Util::SourceInfo srcInfo, const IR::Annotations* annotations, const IR::Expression* expression, const IR::PathExpression* matchType);
    KeyElement(const IR::Annotations* annotations, const IR::Expression* expression, const IR::PathExpression* matchType);
    KeyElement(Util::SourceInfo srcInfo, const IR::Expression* expression, const IR::PathExpression* matchType);
    KeyElement(const IR::Expression* expression, const IR::PathExpression* matchType);
    IRNODE_SUBCLASS(KeyElement)
};
}  // namespace IR
namespace IR {

class Key : public PropertyValue {
 public:
    IR::Vector<IR::KeyElement> keyElements;
#line 243 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4596 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 244 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void push_back(const IR::KeyElement* ke);
#line 4599 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Key const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Key"; }
    static cstring static_type_name() { return "Key"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Key(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::PropertyValue const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Key(Util::SourceInfo srcInfo, IR::Vector<IR::KeyElement> keyElements);
    Key(IR::Vector<IR::KeyElement> keyElements);
    IRNODE_SUBCLASS(Key)
};
}  // namespace IR
namespace IR {
/// Pre-defined entry in a table
class Entry : public Node, public virtual IAnnotated {
 public:
/// annotations are optional (supported annotations: @priority(value))
    const IR::Annotations* annotations = Annotations::empty;
    const IR::ListExpression* keys = nullptr;
/// must be a tuple expression
    const IR::Expression* action = nullptr;
/// typically a MethodCallExpression.
/// The action must be defined in action list
#line 255 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4630 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 256 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ListExpression *getKeys() const;
#line 4633 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 257 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Expression *getAction() const;
#line 4636 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 258 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4639 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Entry const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Entry"; }
    static cstring static_type_name() { return "Entry"; }
    void toJSON(JSONGenerator & json) const override;
    Entry(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Entry(Util::SourceInfo srcInfo, const IR::Annotations* annotations, const IR::ListExpression* keys, const IR::Expression* action);
    Entry(const IR::Annotations* annotations, const IR::ListExpression* keys, const IR::Expression* action);
    Entry(Util::SourceInfo srcInfo, const IR::ListExpression* keys, const IR::Expression* action);
    Entry(const IR::ListExpression* keys, const IR::Expression* action);
    IRNODE_SUBCLASS(Entry)
};
}  // namespace IR
namespace IR {
/// List of predefined entries. Part of table properties
class EntriesList : public PropertyValue {
 public:
    IR::Vector<IR::Entry> entries;
#line 264 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    size_t size() const;
#line 4665 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 265 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4668 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::EntriesList const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "EntriesList"; }
    static cstring static_type_name() { return "EntriesList"; }
    void toJSON(JSONGenerator & json) const override;
    EntriesList(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::PropertyValue const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    EntriesList(Util::SourceInfo srcInfo, IR::Vector<IR::Entry> entries);
    EntriesList(IR::Vector<IR::Entry> entries);
    IRNODE_SUBCLASS(EntriesList)
};
}  // namespace IR
namespace IR {
class Property : public Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::PropertyValue* value = nullptr;
    bool isConstant;
#line 272 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4694 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 273 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4697 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Property const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Property"; }
    static cstring static_type_name() { return "Property"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Property(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Property(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::PropertyValue* value, bool isConstant);
    Property(IR::ID name, const IR::Annotations* annotations, const IR::PropertyValue* value, bool isConstant);
    Property(Util::SourceInfo srcInfo, IR::ID name, const IR::PropertyValue* value, bool isConstant);
    Property(IR::ID name, const IR::PropertyValue* value, bool isConstant);
    IRNODE_SUBCLASS(Property)
};
}  // namespace IR
namespace IR {
class TableProperties : public Node, public virtual ISimpleNamespace {
 public:
    IR::IndexedVector<IR::Property> properties;
#line 278 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 4725 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 279 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4728 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 281 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Property *getProperty(cstring name) const;
#line 4731 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 283 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 4734 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 285 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void push_back(const IR::Property* prop);
#line 4737 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    static const cstring actionsPropertyName;
    static const cstring keyPropertyName;
    static const cstring defaultActionPropertyName;
    static const cstring entriesPropertyName;
    static const cstring sizePropertyName;
#line 293 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4745 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::TableProperties const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "TableProperties"; }
    static cstring static_type_name() { return "TableProperties"; }
    void toJSON(JSONGenerator & json) const override;
    TableProperties(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    TableProperties(Util::SourceInfo srcInfo, IR::IndexedVector<IR::Property> properties);
    TableProperties(IR::IndexedVector<IR::Property> properties);
    TableProperties(Util::SourceInfo srcInfo);
    TableProperties();
    IRNODE_SUBCLASS(TableProperties)
};
}  // namespace IR
namespace IR {
class P4Table : public Declaration, public virtual IAnnotated, public virtual IApply {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::TableProperties* properties = nullptr;
#line 300 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4770 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 301 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Type_Method *getApplyMethodType() const override;
#line 4773 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 302 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getApplyParameters() const override;
#line 4776 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 303 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ActionList *getActionList() const;
#line 4779 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 311 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Key *getKey() const;
#line 4782 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 319 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Expression *getDefaultAction() const;
#line 4785 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 327 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Constant *getConstantProperty(cstring name) const;
#line 4788 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 334 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Constant *getSizeProperty() const;
#line 4791 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 337 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::EntriesList *getEntries() const;
#line 4794 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::P4Table const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "P4Table"; }
    static cstring static_type_name() { return "P4Table"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    P4Table(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    P4Table(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TableProperties* properties);
    P4Table(IR::ID name, const IR::Annotations* annotations, const IR::TableProperties* properties);
    P4Table(Util::SourceInfo srcInfo, IR::ID name, const IR::TableProperties* properties);
    P4Table(IR::ID name, const IR::TableProperties* properties);
    IRNODE_SUBCLASS(P4Table)
};
}  // namespace IR
namespace IR {
class P4ValueSet : public Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* elementType = nullptr;
    const IR::Expression* size = nullptr;

#line 353 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4825 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::P4ValueSet const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "P4ValueSet"; }
    static cstring static_type_name() { return "P4ValueSet"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    P4ValueSet(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    P4ValueSet(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* elementType, const IR::Expression* size);
    P4ValueSet(IR::ID name, const IR::Annotations* annotations, const IR::Type* elementType, const IR::Expression* size);
    P4ValueSet(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* elementType, const IR::Expression* size);
    P4ValueSet(IR::ID name, const IR::Type* elementType, const IR::Expression* size);
    IRNODE_SUBCLASS(P4ValueSet)
};
}  // namespace IR
namespace IR {
class Declaration_Variable : public Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* type = nullptr;
    const IR::Expression* initializer = nullptr;
#line 361 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4855 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 362 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4858 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Declaration_Variable const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Declaration_Variable"; }
    static cstring static_type_name() { return "Declaration_Variable"; }
    void toJSON(JSONGenerator & json) const override;
    Declaration_Variable(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Declaration_Variable(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Expression* initializer);
    Declaration_Variable(IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Expression* initializer);
    Declaration_Variable(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type, const IR::Expression* initializer);
    Declaration_Variable(IR::ID name, const IR::Type* type, const IR::Expression* initializer);
    Declaration_Variable(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    Declaration_Variable(IR::ID name, const IR::Annotations* annotations, const IR::Type* type);
    Declaration_Variable(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type);
    Declaration_Variable(IR::ID name, const IR::Type* type);
    IRNODE_SUBCLASS(Declaration_Variable)
};
}  // namespace IR
namespace IR {
class Declaration_Constant : public Declaration, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* type = nullptr;
    const IR::Expression* initializer = nullptr;
#line 372 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4891 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 373 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 4894 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 374 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 4897 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Declaration_Constant const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Declaration_Constant"; }
    static cstring static_type_name() { return "Declaration_Constant"; }
    void toJSON(JSONGenerator & json) const override;
    Declaration_Constant(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Declaration_Constant(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Expression* initializer);
    Declaration_Constant(IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Expression* initializer);
    Declaration_Constant(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type, const IR::Expression* initializer);
    Declaration_Constant(IR::ID name, const IR::Type* type, const IR::Expression* initializer);
    IRNODE_SUBCLASS(Declaration_Constant)
};
}  // namespace IR
namespace IR {
/// Like a variable, but for a statically allocated instance.
/// The syntax is Contructor(args) name = initializer;
/// Initializers are an experimental features, used for externs with
/// abstract methods.
class Declaration_Instance : public Declaration, public virtual IAnnotated, public virtual IInstance {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    const IR::Type* type = nullptr;

    const IR::Vector<IR::Argument>* arguments = nullptr;
    IR::NameMap<IR::Property> properties = {};

    const IR::BlockStatement* initializer = nullptr;

#line 389 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 4935 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 390 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Type *getType() const override;
#line 4938 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 391 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring Name() const override;
#line 4941 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 392 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4944 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Declaration_Instance const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Declaration_Instance"; }
    static cstring static_type_name() { return "Declaration_Instance"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Declaration_Instance(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Declaration_Instance(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    Declaration_Instance(IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    Declaration_Instance(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    Declaration_Instance(IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    Declaration_Instance(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    Declaration_Instance(IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    Declaration_Instance(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    Declaration_Instance(IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    IRNODE_SUBCLASS(Declaration_Instance)
};
}  // namespace IR
namespace IR {
/// Toplevel program representation
class P4Program : public Node, public virtual IGeneralNamespace {
 public:
/// Top-level program objects.
/// This is not an IndexedVector because:
/// - we allow overloaded function-like objects.
/// - not all objects in a P4Program are declarations (e.g., match_kind is not).
    IR::Vector<IR::Node> objects;
#line 406 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 4980 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 407 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 4983 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    static const cstring main;
    IRNODE_DECLARE_APPLY_OVERLOAD(P4Program)
    bool operator==(IR::P4Program const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "P4Program"; }
    static cstring static_type_name() { return "P4Program"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    P4Program(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    P4Program(Util::SourceInfo srcInfo, IR::Vector<IR::Node> objects);
    P4Program(IR::Vector<IR::Node> objects);
    P4Program(Util::SourceInfo srcInfo);
    P4Program();
    IRNODE_SUBCLASS(P4Program)
};
}  // namespace IR
namespace IR {
///////////////////////////// Statements //////////////////////////
class Statement : public StatOrDecl {
 public:
    bool operator==(IR::Statement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Statement"; }
    static cstring static_type_name() { return "Statement"; }
    void toJSON(JSONGenerator & json) const override;
    Statement(JSONLoader & json);
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Statement(Util::SourceInfo srcInfo);
    Statement();
    IRNODE_ABSTRACT_SUBCLASS(Statement)
};
}  // namespace IR
namespace IR {
class ExitStatement : public Statement {
 public:
#line 417 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 5027 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 418 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 5030 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ExitStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "ExitStatement"; }
    static cstring static_type_name() { return "ExitStatement"; }
    void toJSON(JSONGenerator & json) const override;
    ExitStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ExitStatement(Util::SourceInfo srcInfo);
    ExitStatement();
    IRNODE_SUBCLASS(ExitStatement)
};
}  // namespace IR
namespace IR {
class ReturnStatement : public Statement {
 public:
    const IR::Expression* expression = nullptr;
#line 423 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 5052 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ReturnStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ReturnStatement"; }
    static cstring static_type_name() { return "ReturnStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ReturnStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ReturnStatement(Util::SourceInfo srcInfo, const IR::Expression* expression);
    ReturnStatement(const IR::Expression* expression);
    IRNODE_SUBCLASS(ReturnStatement)
};
}  // namespace IR
namespace IR {
class EmptyStatement : public Statement {
 public:
#line 428 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void dbprint(std::ostream & out) const override;
#line 5076 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::EmptyStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "EmptyStatement"; }
    static cstring static_type_name() { return "EmptyStatement"; }
    void toJSON(JSONGenerator & json) const override;
    EmptyStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    EmptyStatement(Util::SourceInfo srcInfo);
    EmptyStatement();
    IRNODE_SUBCLASS(EmptyStatement)
};
}  // namespace IR
namespace IR {
class AssignmentStatement : public Statement {
 public:
    const IR::Expression* left = nullptr;
    const IR::Expression* right = nullptr;
    bool operator==(IR::AssignmentStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "AssignmentStatement"; }
    static cstring static_type_name() { return "AssignmentStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    AssignmentStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    AssignmentStatement(Util::SourceInfo srcInfo, const IR::Expression* left, const IR::Expression* right);
    AssignmentStatement(const IR::Expression* left, const IR::Expression* right);
    IRNODE_SUBCLASS(AssignmentStatement)
};
}  // namespace IR
namespace IR {
class IfStatement : public Statement {
 public:
    const IR::Expression* condition = nullptr;
    const IR::Statement* ifTrue = nullptr;
    const IR::Statement* ifFalse = nullptr;
#line 440 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 5125 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::IfStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "IfStatement"; }
    static cstring static_type_name() { return "IfStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    IfStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IfStatement(Util::SourceInfo srcInfo, const IR::Expression* condition, const IR::Statement* ifTrue, const IR::Statement* ifFalse);
    IfStatement(const IR::Expression* condition, const IR::Statement* ifTrue, const IR::Statement* ifFalse);
    IRNODE_SUBCLASS(IfStatement)
};
}  // namespace IR
namespace IR {
class BlockStatement : public Statement, public virtual ISimpleNamespace, public virtual IAnnotated {
 public:
    const IR::Annotations* annotations = Annotations::empty;
    IR::IndexedVector<IR::StatOrDecl> components;
#line 452 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 5150 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 454 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 5153 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 456 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void push_back(const IR::StatOrDecl* st);
#line 5156 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 457 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::Annotations *getAnnotations() const override;
#line 5159 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::BlockStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "BlockStatement"; }
    static cstring static_type_name() { return "BlockStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    BlockStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    BlockStatement(Util::SourceInfo srcInfo, const IR::Annotations* annotations, IR::IndexedVector<IR::StatOrDecl> components);
    BlockStatement(const IR::Annotations* annotations, IR::IndexedVector<IR::StatOrDecl> components);
    BlockStatement(Util::SourceInfo srcInfo, IR::IndexedVector<IR::StatOrDecl> components);
    BlockStatement(IR::IndexedVector<IR::StatOrDecl> components);
    BlockStatement(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    BlockStatement(const IR::Annotations* annotations);
    BlockStatement(Util::SourceInfo srcInfo);
    BlockStatement();
    IRNODE_SUBCLASS(BlockStatement)
};
}  // namespace IR
namespace IR {
class MethodCallStatement : public Statement {
 public:
    const IR::MethodCallExpression* methodCall = nullptr;
#line 463 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    MethodCallStatement(Util::SourceInfo si, IR::ID m, std::initializer_list<const IR::Argument *> const & a);
#line 5191 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 465 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    MethodCallStatement(Util::SourceInfo si, const IR::Expression* m, std::initializer_list<const IR::Argument *> const & a);
#line 5194 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 468 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 5197 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::MethodCallStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "MethodCallStatement"; }
    static cstring static_type_name() { return "MethodCallStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    MethodCallStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    MethodCallStatement(Util::SourceInfo srcInfo, const IR::MethodCallExpression* methodCall);
    MethodCallStatement(const IR::MethodCallExpression* methodCall);
    IRNODE_SUBCLASS(MethodCallStatement)
};
}  // namespace IR
namespace IR {
class SwitchCase : public Node {
 public:
    const IR::Expression* label = nullptr;
    const IR::Statement* statement = nullptr;

#line 475 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 5225 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::SwitchCase const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "SwitchCase"; }
    static cstring static_type_name() { return "SwitchCase"; }
    void toJSON(JSONGenerator & json) const override;
    SwitchCase(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    SwitchCase(Util::SourceInfo srcInfo, const IR::Expression* label, const IR::Statement* statement);
    SwitchCase(const IR::Expression* label, const IR::Statement* statement);
    IRNODE_SUBCLASS(SwitchCase)
};
}  // namespace IR
namespace IR {

class SwitchStatement : public Statement {
 public:
    const IR::Expression* expression = nullptr;
    IR::Vector<IR::SwitchCase> cases;
#line 487 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 5250 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::SwitchStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "SwitchStatement"; }
    static cstring static_type_name() { return "SwitchStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    SwitchStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Statement const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    SwitchStatement(Util::SourceInfo srcInfo, const IR::Expression* expression, IR::Vector<IR::SwitchCase> cases);
    SwitchStatement(const IR::Expression* expression, IR::Vector<IR::SwitchCase> cases);
    IRNODE_SUBCLASS(SwitchStatement)
};
}  // namespace IR
namespace IR {
class Function : public Declaration, public virtual IFunctional, public virtual ISimpleNamespace, public virtual INestedNamespace {
 public:
    const IR::Type_Method* type = nullptr;
    const IR::BlockStatement* body = nullptr;
#line 495 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getParameters() const override;
#line 5275 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 498 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 5278 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 500 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 5281 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 502 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    std::vector<const IR::INamespace *> getNestedNamespaces() const override;
#line 5284 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Function const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Function"; }
    static cstring static_type_name() { return "Function"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Function(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Function(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Method* type, const IR::BlockStatement* body);
    Function(IR::ID name, const IR::Type_Method* type, const IR::BlockStatement* body);
    IRNODE_SUBCLASS(Function)
};
}  // namespace IR
namespace IR {
/////////////////////////////////////////////////////////////
/**
 * Block is the base class for IR nodes produced by the evaluator.
 * A block represents a compile-time allocated resource.
 * Blocks are not visited using visitors, so the visit_children()
 * method is empty.  Users have to write custom visitors to
 * traverse the constantValue map.
 */
class Block : public Node, public virtual CompileTimeValue {
 public:
    const IR::Node* node = nullptr;
/// Node that evaluates to this block.
/// This is either a Declaration_Instance or a ConstructorCallExpression.
/// One value for each Node inside that evaluates to a compile-time constant.
/// This includes all constructor parameters, and all inner nested blocks.
    ordered_map<const IR::Node *, const IR::CompileTimeValue *> constantValue = {};
#line 523 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    virtual void dbprint(std::ostream & out) const override;
#line 5323 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 524 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    virtual void dbprint_recursive(std::ostream & out) const;
#line 5326 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// value can be null for parameters which are optional
#line 526 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void setValue(const IR::Node* node, const IR::CompileTimeValue* value);
#line 5330 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 527 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    bool hasValue(const IR::Node* node) const;
#line 5333 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 530 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::CompileTimeValue *getValue(const IR::Node* node) const;
#line 5336 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 535 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 5340 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 536 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    virtual IR::IDeclaration const * getContainer() const;
#line 5343 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Block const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Block"; }
    static cstring static_type_name() { return "Block"; }
    void toJSON(JSONGenerator & json) const override;
    Block(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Block(Util::SourceInfo srcInfo, const IR::Node* node);
    Block(const IR::Node* node);
    IRNODE_ABSTRACT_SUBCLASS(Block)
};
}  // namespace IR
namespace IR {
class TableBlock : public Block {
 public:
    const IR::P4Table* container = nullptr;
#line 541 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::IDeclaration const * getContainer() const override;
#line 5364 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::TableBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "TableBlock"; }
    static cstring static_type_name() { return "TableBlock"; }
    void toJSON(JSONGenerator & json) const override;
    TableBlock(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Block const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    TableBlock(Util::SourceInfo srcInfo, const IR::Node* node, const IR::P4Table* container);
    TableBlock(const IR::Node* node, const IR::P4Table* container);
    IRNODE_SUBCLASS(TableBlock)
};
}  // namespace IR
namespace IR {
/// An object that has been instantiated
class InstantiatedBlock : public Block, public virtual IDeclaration {
 public:
    const IR::Type* instanceType = nullptr;

#line 549 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    virtual const IR::ParameterList *getConstructorParameters() const = 0;
#line 5390 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 550 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void instantiate(std::vector<const IR::CompileTimeValue *> * args);
#line 5393 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// @return the argument that the given parameter was instantiated with.
/// It's a fatal error if no such parameter exists.
#line 554 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::CompileTimeValue *getParameterValue(cstring paramName) const;
#line 5398 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
/// @return the argument that the given parameter was instantiated with, or
/// null if no such parameter exists.
#line 558 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::CompileTimeValue *findParameterValue(cstring paramName) const;
#line 5403 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 560 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    virtual void dbprint(std::ostream & out) const override;
#line 5406 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::InstantiatedBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "InstantiatedBlock"; }
    static cstring static_type_name() { return "InstantiatedBlock"; }
    void toJSON(JSONGenerator & json) const override;
    InstantiatedBlock(JSONLoader & json);
    bool operator==(IR::Block const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    InstantiatedBlock(Util::SourceInfo srcInfo, const IR::Node* node, const IR::Type* instanceType);
    InstantiatedBlock(const IR::Node* node, const IR::Type* instanceType);
    IRNODE_ABSTRACT_SUBCLASS(InstantiatedBlock)
};
}  // namespace IR
namespace IR {
class ParserBlock : public InstantiatedBlock {
 public:
    const IR::P4Parser* container = nullptr;
#line 565 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getConstructorParameters() const override;
#line 5430 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 567 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 5433 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 568 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::ID getName() const override;
#line 5436 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 569 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::IDeclaration const * getContainer() const override;
#line 5439 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ParserBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ParserBlock"; }
    static cstring static_type_name() { return "ParserBlock"; }
    void toJSON(JSONGenerator & json) const override;
    ParserBlock(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::InstantiatedBlock const & a) const override { return a == *this; }
    bool operator==(IR::Block const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ParserBlock(Util::SourceInfo srcInfo, const IR::Node* node, const IR::Type* instanceType, const IR::P4Parser* container);
    ParserBlock(const IR::Node* node, const IR::Type* instanceType, const IR::P4Parser* container);
    IRNODE_SUBCLASS(ParserBlock)
};
}  // namespace IR
namespace IR {
class ControlBlock : public InstantiatedBlock {
 public:
    const IR::P4Control* container = nullptr;
#line 575 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getConstructorParameters() const override;
#line 5464 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 577 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 5467 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 578 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::ID getName() const override;
#line 5470 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 579 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::IDeclaration const * getContainer() const override;
#line 5473 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ControlBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ControlBlock"; }
    static cstring static_type_name() { return "ControlBlock"; }
    void toJSON(JSONGenerator & json) const override;
    ControlBlock(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::InstantiatedBlock const & a) const override { return a == *this; }
    bool operator==(IR::Block const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ControlBlock(Util::SourceInfo srcInfo, const IR::Node* node, const IR::Type* instanceType, const IR::P4Control* container);
    ControlBlock(const IR::Node* node, const IR::Type* instanceType, const IR::P4Control* container);
    IRNODE_SUBCLASS(ControlBlock)
};
}  // namespace IR
namespace IR {
class PackageBlock : public InstantiatedBlock {
 public:
    const IR::Type_Package* type = nullptr;
#line 585 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getConstructorParameters() const override;
#line 5498 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 586 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 5501 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 587 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::ID getName() const override;
#line 5504 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::PackageBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "PackageBlock"; }
    static cstring static_type_name() { return "PackageBlock"; }
    void toJSON(JSONGenerator & json) const override;
    PackageBlock(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::InstantiatedBlock const & a) const override { return a == *this; }
    bool operator==(IR::Block const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    PackageBlock(Util::SourceInfo srcInfo, const IR::Node* node, const IR::Type* instanceType, const IR::Type_Package* type);
    PackageBlock(const IR::Node* node, const IR::Type* instanceType, const IR::Type_Package* type);
    IRNODE_SUBCLASS(PackageBlock)
};
}  // namespace IR
namespace IR {
class ExternBlock : public InstantiatedBlock {
 public:
    const IR::Type_Extern* type = nullptr;
    const IR::Method* constructor = nullptr;

#line 594 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::ParameterList *getConstructorParameters() const override;
#line 5531 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 596 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    cstring toString() const override;
#line 5534 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 597 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::ID getName() const override;
#line 5537 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ExternBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ExternBlock"; }
    static cstring static_type_name() { return "ExternBlock"; }
    void toJSON(JSONGenerator & json) const override;
    ExternBlock(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::InstantiatedBlock const & a) const override { return a == *this; }
    bool operator==(IR::Block const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ExternBlock(Util::SourceInfo srcInfo, const IR::Node* node, const IR::Type* instanceType, const IR::Type_Extern* type, const IR::Method* constructor);
    ExternBlock(const IR::Node* node, const IR::Type* instanceType, const IR::Type_Extern* type, const IR::Method* constructor);
    IRNODE_SUBCLASS(ExternBlock)
};
}  // namespace IR
namespace IR {
/// Represents the program as a whole
class ToplevelBlock : public Block, public virtual IDeclaration {
 public:
#line 603 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::P4Program *getProgram() const;
#line 5562 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 604 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    const IR::PackageBlock *getMain() const;
#line 5565 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 605 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    IR::ID getName() const override;
#line 5568 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 607 "/home/aa/ppk_sw/ppk/p4c/ir/ir.def"
    void validate() const override;
#line 5571 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ToplevelBlock const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "ToplevelBlock"; }
    static cstring static_type_name() { return "ToplevelBlock"; }
    void toJSON(JSONGenerator & json) const override;
    ToplevelBlock(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Block const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ToplevelBlock(Util::SourceInfo srcInfo, const IR::Node* node);
    ToplevelBlock(const IR::Node* node);
    IRNODE_SUBCLASS(ToplevelBlock)
};
}  // namespace IR
/** @} */
/* end group irdefs */
/* -*-C++-*- */
/*
  This file contains IR classes needed just for the P4 v1.0/v1.1 front-end.
*/
/** \addtogroup irdefs
  * @{
  */
#line 11 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
namespace IR {
enum class CounterType { NONE, PACKETS, BYTES, BOTH };
}

inline std::ostream& operator<<(std::ostream &out, IR::CounterType d) {
    switch (d) {
        case IR::CounterType::NONE:
            out << "NONE";
            break;
        case IR::CounterType::PACKETS:
            out << "PACKETS";
            break;
        case IR::CounterType::BYTES:
            out << "BYTES";
            break;
        case IR::CounterType::BOTH:
            out << "BOTH";
            break;
        default:
            BUG("Unhandled case");
    }
    return out;
}

inline bool operator>>(cstring s, IR::CounterType &ctr) {
    if (!s || s == "" || s == "NONE") ctr = IR::CounterType::NONE;
    else if (s == "PACKETS") ctr = IR::CounterType::PACKETS;
    else if (s == "BYTES") ctr = IR::CounterType::BYTES;
    else if (s == "BOTH") ctr = IR::CounterType::BOTH;
    else return false;
    return true;
}
#line 5628 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
namespace IR {
class Type_Block : public Type_Base {
 public:
#line 46 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5634 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 47 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static const IR::Type_Block *get();
#line 5637 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 48 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5640 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Block const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Block"; }
    static cstring static_type_name() { return "Type_Block"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Block(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Block(Util::SourceInfo srcInfo);
    Type_Block();
    IRNODE_SUBCLASS(Type_Block)
};
}  // namespace IR
namespace IR {
class Type_Counter : public Type_Base {
 public:
#line 51 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5661 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 52 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static const IR::Type_Counter *get();
#line 5664 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 53 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5667 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Counter const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Counter"; }
    static cstring static_type_name() { return "Type_Counter"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Counter(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Counter(Util::SourceInfo srcInfo);
    Type_Counter();
    IRNODE_SUBCLASS(Type_Counter)
};
}  // namespace IR
namespace IR {
class Type_Expression : public Type_Base {
 public:
#line 56 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5688 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 57 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static const IR::Type_Expression *get();
#line 5691 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 58 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5694 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Expression const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Expression"; }
    static cstring static_type_name() { return "Type_Expression"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Expression(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Expression(Util::SourceInfo srcInfo);
    Type_Expression();
    IRNODE_SUBCLASS(Type_Expression)
};
}  // namespace IR
namespace IR {
class Type_FieldListCalculation : public Type_Base {
 public:
#line 61 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5715 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 62 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static const IR::Type_FieldListCalculation *get();
#line 5718 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 63 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5721 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_FieldListCalculation const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_FieldListCalculation"; }
    static cstring static_type_name() { return "Type_FieldListCalculation"; }
    void toJSON(JSONGenerator & json) const override;
    Type_FieldListCalculation(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_FieldListCalculation(Util::SourceInfo srcInfo);
    Type_FieldListCalculation();
    IRNODE_SUBCLASS(Type_FieldListCalculation)
};
}  // namespace IR
namespace IR {
class Type_Meter : public Type_Base {
 public:
#line 66 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5742 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 67 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static const IR::Type_Meter *get();
#line 5745 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 68 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5748 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Meter const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Meter"; }
    static cstring static_type_name() { return "Type_Meter"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Meter(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Meter(Util::SourceInfo srcInfo);
    Type_Meter();
    IRNODE_SUBCLASS(Type_Meter)
};
}  // namespace IR
namespace IR {
class Type_Register : public Type_Base {
 public:
#line 71 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5769 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 72 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static const IR::Type_Register *get();
#line 5772 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 73 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5775 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_Register const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_Register"; }
    static cstring static_type_name() { return "Type_Register"; }
    void toJSON(JSONGenerator & json) const override;
    Type_Register(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_Register(Util::SourceInfo srcInfo);
    Type_Register();
    IRNODE_SUBCLASS(Type_Register)
};
}  // namespace IR
namespace IR {
class Type_AnyTable : public Type_Base {
 public:
#line 76 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5796 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 77 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static const IR::Type_AnyTable *get();
#line 5799 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 78 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5802 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Type_AnyTable const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Type_AnyTable"; }
    static cstring static_type_name() { return "Type_AnyTable"; }
    void toJSON(JSONGenerator & json) const override;
    Type_AnyTable(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Base const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Type_AnyTable(Util::SourceInfo srcInfo);
    Type_AnyTable();
    IRNODE_SUBCLASS(Type_AnyTable)
};
}  // namespace IR
namespace IR {
class HeaderOrMetadata : public Node, public virtual IAnnotated {
 public:
    IR::ID type_name;
    IR::ID name;
    const IR::Annotations* annotations = nullptr;
    const IR::Type_StructLike* type = nullptr;
#line 87 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 5827 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 88 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    HeaderOrMetadata(IR::ID n, const IR::Type_StructLike* t);
#line 5830 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 90 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5833 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::HeaderOrMetadata const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "HeaderOrMetadata"; }
    static cstring static_type_name() { return "HeaderOrMetadata"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    HeaderOrMetadata(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    HeaderOrMetadata(Util::SourceInfo srcInfo, IR::ID type_name, IR::ID name, const IR::Annotations* annotations);
    HeaderOrMetadata(IR::ID type_name, IR::ID name, const IR::Annotations* annotations);
    IRNODE_ABSTRACT_SUBCLASS(HeaderOrMetadata)
};
}  // namespace IR
namespace IR {
class Header : public HeaderOrMetadata {
 public:
#line 94 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Header(IR::ID n, const IR::Type_Header* t);
#line 5856 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Header const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Header"; }
    static cstring static_type_name() { return "Header"; }
    void toJSON(JSONGenerator & json) const override;
    Header(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::HeaderOrMetadata const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Header(Util::SourceInfo srcInfo, IR::ID type_name, IR::ID name, const IR::Annotations* annotations);
    Header(IR::ID type_name, IR::ID name, const IR::Annotations* annotations);
    IRNODE_SUBCLASS(Header)
};
}  // namespace IR
namespace IR {
class HeaderStack : public HeaderOrMetadata {
 public:
    int size;
#line 100 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    HeaderStack(IR::ID n, const IR::Type_Header* t, int sz);
#line 5877 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::HeaderStack const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "HeaderStack"; }
    static cstring static_type_name() { return "HeaderStack"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    HeaderStack(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::HeaderOrMetadata const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    HeaderStack(Util::SourceInfo srcInfo, IR::ID type_name, IR::ID name, const IR::Annotations* annotations, int size);
    HeaderStack(IR::ID type_name, IR::ID name, const IR::Annotations* annotations, int size);
    IRNODE_SUBCLASS(HeaderStack)
};
}  // namespace IR
namespace IR {
class Metadata : public HeaderOrMetadata {
 public:
#line 105 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Metadata(IR::ID n, const IR::Type_StructLike* t);
#line 5898 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Metadata const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Metadata"; }
    static cstring static_type_name() { return "Metadata"; }
    void toJSON(JSONGenerator & json) const override;
    Metadata(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::HeaderOrMetadata const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Metadata(Util::SourceInfo srcInfo, IR::ID type_name, IR::ID name, const IR::Annotations* annotations);
    Metadata(IR::ID type_name, IR::ID name, const IR::Annotations* annotations);
    IRNODE_SUBCLASS(Metadata)
};
}  // namespace IR
namespace IR {
class HeaderRef : public Expression {
 public:
#line 110 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual const IR::HeaderOrMetadata *baseRef() const = 0;
#line 5918 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::HeaderRef const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "HeaderRef"; }
    static cstring static_type_name() { return "HeaderRef"; }
    void toJSON(JSONGenerator & json) const override;
    HeaderRef(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    HeaderRef(Util::SourceInfo srcInfo, const IR::Type* type);
    HeaderRef(const IR::Type* type);
    HeaderRef(Util::SourceInfo srcInfo);
    HeaderRef();
    IRNODE_ABSTRACT_SUBCLASS(HeaderRef)
};
}  // namespace IR
namespace IR {
class ConcreteHeaderRef : public HeaderRef {
 public:
    const IR::HeaderOrMetadata* ref = nullptr;
#line 116 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::HeaderOrMetadata *baseRef() const override;
#line 5941 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 117 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5944 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 118 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 5947 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ConcreteHeaderRef const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ConcreteHeaderRef"; }
    static cstring static_type_name() { return "ConcreteHeaderRef"; }
    void toJSON(JSONGenerator & json) const override;
    ConcreteHeaderRef(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::HeaderRef const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ConcreteHeaderRef(Util::SourceInfo srcInfo, const IR::Type* type, const IR::HeaderOrMetadata* ref);
    ConcreteHeaderRef(const IR::Type* type, const IR::HeaderOrMetadata* ref);
    ConcreteHeaderRef(Util::SourceInfo srcInfo, const IR::HeaderOrMetadata* ref);
    ConcreteHeaderRef(const IR::HeaderOrMetadata* ref);
    IRNODE_SUBCLASS(ConcreteHeaderRef)
};
}  // namespace IR
namespace IR {
class HeaderStackItemRef : public HeaderRef {
 public:
    const IR::Expression* base_ = nullptr;
    const IR::Expression* index_ = nullptr;
#line 128 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Expression *base() const;
#line 5975 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 129 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::HeaderOrMetadata *baseRef() const override;
#line 5978 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 130 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Expression *index() const;
#line 5981 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 131 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void set_base(const IR::Expression* b);
#line 5984 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 132 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 5987 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::HeaderStackItemRef const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "HeaderStackItemRef"; }
    static cstring static_type_name() { return "HeaderStackItemRef"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    HeaderStackItemRef(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::HeaderRef const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    HeaderStackItemRef(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* base_, const IR::Expression* index_);
    HeaderStackItemRef(const IR::Type* type, const IR::Expression* base_, const IR::Expression* index_);
    HeaderStackItemRef(Util::SourceInfo srcInfo, const IR::Expression* base_, const IR::Expression* index_);
    HeaderStackItemRef(const IR::Expression* base_, const IR::Expression* index_);
    IRNODE_SUBCLASS(HeaderStackItemRef)
};
}  // namespace IR
namespace IR {
class If : public Expression {
 public:
    const IR::Expression* pred = nullptr;
    const IR::Vector<IR::Expression>* ifTrue = nullptr;
    const IR::Vector<IR::Expression>* ifFalse = nullptr;
#line 139 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 6018 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::If const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "If"; }
    static cstring static_type_name() { return "If"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    If(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    If(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* pred, const IR::Vector<IR::Expression>* ifTrue, const IR::Vector<IR::Expression>* ifFalse);
    If(const IR::Type* type, const IR::Expression* pred, const IR::Vector<IR::Expression>* ifTrue, const IR::Vector<IR::Expression>* ifFalse);
    If(Util::SourceInfo srcInfo, const IR::Expression* pred, const IR::Vector<IR::Expression>* ifTrue, const IR::Vector<IR::Expression>* ifFalse);
    If(const IR::Expression* pred, const IR::Vector<IR::Expression>* ifTrue, const IR::Vector<IR::Expression>* ifFalse);
    IRNODE_SUBCLASS(If)
};
}  // namespace IR
namespace IR {

class NamedCond : public If {
 public:
    cstring name = unique_name();
#line 153 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    static cstring unique_name();
#line 6044 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 154 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    NamedCond(IR::If const & i);
#line 6047 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 155 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    bool operator==(IR::NamedCond const & a) const override;
#line 6050 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "NamedCond"; }
    static cstring static_type_name() { return "NamedCond"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    NamedCond(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::If const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(NamedCond)
};
}  // namespace IR
namespace IR {
class Apply : public Expression {
 public:
    IR::ID name;
    IR::NameMap<IR::Vector<IR::Expression>, ordered_map> actions = {};
#line 163 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 6072 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Apply const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Apply"; }
    static cstring static_type_name() { return "Apply"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Apply(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Apply(Util::SourceInfo srcInfo, const IR::Type* type, IR::ID name);
    Apply(const IR::Type* type, IR::ID name);
    Apply(Util::SourceInfo srcInfo, IR::ID name);
    Apply(IR::ID name);
    Apply(Util::SourceInfo srcInfo, const IR::Type* type);
    Apply(const IR::Type* type);
    Apply(Util::SourceInfo srcInfo);
    Apply();
    IRNODE_SUBCLASS(Apply)
};
}  // namespace IR
namespace IR {
class Primitive : public Operation {
 public:
    cstring name;
    IR::Vector<IR::Expression> operands = {};
#line 185 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(cstring n, const IR::Vector<IR::Expression>* l);
#line 6103 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 187 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(Util::SourceInfo si, cstring n, const IR::Vector<IR::Expression>* l);
#line 6106 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 189 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(cstring n, const IR::Expression* a1);
#line 6109 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 191 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(Util::SourceInfo si, cstring n, const IR::Expression* a1);
#line 6112 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 193 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(cstring n, const IR::Expression* a1, const IR::Expression* a2);
#line 6115 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 195 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(Util::SourceInfo si, cstring n, const IR::Expression* a1, const IR::Expression* a2);
#line 6118 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 198 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(cstring n, const IR::Expression* a1, const IR::Vector<IR::Expression>* a2);
#line 6121 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 201 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(Util::SourceInfo si, cstring n, const IR::Expression* a1, const IR::Vector<IR::Expression>* a2);
#line 6124 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 205 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(cstring n, const IR::Expression* a1, const IR::Expression* a2, const IR::Expression* a3);
#line 6127 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 207 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Primitive(Util::SourceInfo si, cstring n, const IR::Expression* a1, const IR::Expression* a2, const IR::Expression* a3);
#line 6130 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 210 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual bool isOutput(int operand_index) const;
#line 6133 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 211 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual unsigned inferOperandTypes() const;
#line 6136 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 212 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual const IR::Type *inferOperandType(int operand) const;
#line 6139 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 213 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual void typecheck() const;
#line 6142 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    IRNODE_DECLARE_APPLY_OVERLOAD(Primitive)
    cstring getStringOp() const override { return name; }
    int getPrecedence() const override { return DBPrint::Prec_Postfix; }
    bool operator==(IR::Primitive const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Primitive"; }
    static cstring static_type_name() { return "Primitive"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Primitive(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Primitive(Util::SourceInfo srcInfo, const IR::Type* type, cstring name);
    Primitive(const IR::Type* type, cstring name);
    Primitive(Util::SourceInfo srcInfo, cstring name);
    Primitive(cstring name);
    IRNODE_SUBCLASS(Primitive)
};
}  // namespace IR
namespace IR {
class FieldList : public Node, public virtual IAnnotated {
 public:
    IR::ID name;
    bool payload = false;
    const IR::Annotations* annotations = Annotations::empty;
    IR::Vector<IR::Expression> fields = {};
#line 224 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6177 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::FieldList const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "FieldList"; }
    static cstring static_type_name() { return "FieldList"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    FieldList(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    FieldList(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    FieldList(IR::ID name, const IR::Annotations* annotations);
    FieldList(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    FieldList(const IR::Annotations* annotations);
    FieldList(Util::SourceInfo srcInfo, IR::ID name);
    FieldList(IR::ID name);
    FieldList(Util::SourceInfo srcInfo);
    FieldList();
    IRNODE_SUBCLASS(FieldList)
};
}  // namespace IR
namespace IR {
class FieldListCalculation : public Node, public virtual IAnnotated {
 public:
    IR::ID name;
    const IR::NameList* input = nullptr;
    const IR::FieldList* input_fields = nullptr;
    const IR::NameList* algorithm = nullptr;
    int output_width = 0;
    const IR::Annotations* annotations = Annotations::empty;
#line 234 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6213 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::FieldListCalculation const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "FieldListCalculation"; }
    static cstring static_type_name() { return "FieldListCalculation"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    FieldListCalculation(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    FieldListCalculation(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    FieldListCalculation(IR::ID name, const IR::Annotations* annotations);
    FieldListCalculation(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    FieldListCalculation(const IR::Annotations* annotations);
    FieldListCalculation(Util::SourceInfo srcInfo, IR::ID name);
    FieldListCalculation(IR::ID name);
    FieldListCalculation(Util::SourceInfo srcInfo);
    FieldListCalculation();
    IRNODE_SUBCLASS(FieldListCalculation)
};
}  // namespace IR
namespace IR {
class CalculatedField : public Node, public virtual IAnnotated {
 public:
    const IR::Expression* field = nullptr;
class update_or_verify {
 public:
    Util::SourceInfo srcInfo;
    bool update;
    IR::ID name;
    const IR::Expression* cond = nullptr;
#line 244 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    update_or_verify();
#line 6250 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

    bool operator==(IR::CalculatedField::update_or_verify const & a) const;
    void toJSON(JSONGenerator & json) const;
    update_or_verify(JSONLoader & json);
    static IR::CalculatedField::update_or_verify * fromJSON(JSONLoader & json);
    update_or_verify(Util::SourceInfo srcInfo, bool update, IR::ID name, const IR::Expression* cond);
};
    safe_vector<update_or_verify> specs = {};
    const IR::Annotations* annotations = nullptr;
#line 248 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6262 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 249 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 6266 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::CalculatedField const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "CalculatedField"; }
    static cstring static_type_name() { return "CalculatedField"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    CalculatedField(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    CalculatedField(Util::SourceInfo srcInfo, const IR::Expression* field, const IR::Annotations* annotations);
    CalculatedField(const IR::Expression* field, const IR::Annotations* annotations);
    CalculatedField(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    CalculatedField(const IR::Annotations* annotations);
    IRNODE_SUBCLASS(CalculatedField)
};
}  // namespace IR
namespace IR {
class ParserValueSet : public Node, public virtual IAnnotated {
 public:
    IR::ID name;
    const IR::Annotations* annotations = Annotations::empty;
#line 258 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6291 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 259 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 6294 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 260 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6297 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ParserValueSet const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ParserValueSet"; }
    static cstring static_type_name() { return "ParserValueSet"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ParserValueSet(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ParserValueSet(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    ParserValueSet(IR::ID name, const IR::Annotations* annotations);
    ParserValueSet(Util::SourceInfo srcInfo, IR::ID name);
    ParserValueSet(IR::ID name);
    IRNODE_SUBCLASS(ParserValueSet)
};
}  // namespace IR
namespace IR {
class CaseEntry : public Node {
 public:
    safe_vector<std::pair<const IR::Expression *, const IR::Constant *>> values = {};
    IR::ID action;
    bool operator==(IR::CaseEntry const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "CaseEntry"; }
    static cstring static_type_name() { return "CaseEntry"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    CaseEntry(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    CaseEntry(Util::SourceInfo srcInfo, IR::ID action);
    CaseEntry(IR::ID action);
    CaseEntry(Util::SourceInfo srcInfo);
    CaseEntry();
    IRNODE_SUBCLASS(CaseEntry)
};
}  // namespace IR
namespace IR {
class V1Parser : public Node, public virtual IAnnotated {
 public:
    IR::ID name;
    IR::Vector<IR::Expression> stmts = {};
    const IR::Vector<IR::Expression>* select = nullptr;
    const IR::Vector<IR::CaseEntry>* cases = nullptr;
    IR::ID default_return = {};
    IR::ID parse_error = {};
    bool drop = false;
    const IR::Annotations* annotations = nullptr;
#line 277 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6352 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 278 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6355 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::V1Parser const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "V1Parser"; }
    static cstring static_type_name() { return "V1Parser"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    V1Parser(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    V1Parser(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    V1Parser(IR::ID name, const IR::Annotations* annotations);
    V1Parser(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    V1Parser(const IR::Annotations* annotations);
    IRNODE_SUBCLASS(V1Parser)
};
}  // namespace IR
namespace IR {
class ParserException : public Node {
 public:
    bool operator==(IR::ParserException const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "ParserException"; }
    static cstring static_type_name() { return "ParserException"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ParserException(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ParserException(Util::SourceInfo srcInfo);
    ParserException();
    IRNODE_SUBCLASS(ParserException)
};
}  // namespace IR
namespace IR {
class Attached : public Node, public virtual IInstance, public virtual IAnnotated {
 public:
    IR::ID name;
    const IR::Annotations* annotations = Annotations::empty;
#line 286 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring Name() const override;
#line 6400 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 287 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual char const * kind() const = 0;
#line 6403 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 288 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Type *getType() const override;
#line 6406 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 289 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6409 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 290 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual bool indexed() const;
#line 6412 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 291 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    IR::Attached * clone_rename(char const * ext) const;
#line 6415 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 295 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 6418 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 296 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6421 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Attached const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "Attached"; }
    static cstring static_type_name() { return "Attached"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Attached(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Attached(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Attached(IR::ID name, const IR::Annotations* annotations);
    Attached(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    Attached(const IR::Annotations* annotations);
    Attached(Util::SourceInfo srcInfo, IR::ID name);
    Attached(IR::ID name);
    Attached(Util::SourceInfo srcInfo);
    Attached();
    IRNODE_ABSTRACT_SUBCLASS(Attached)
};
}  // namespace IR
namespace IR {
class Stateful : public Attached {
 public:
    IR::ID table = {};
    bool direct = false;
    bool saturating = false;
    int instance_count = -1;
#line 304 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    virtual bool indexed() const override;
#line 6454 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 305 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    int index_width() const;
#line 6457 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

    bool operator==(IR::Stateful const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Stateful"; }
    static cstring static_type_name() { return "Stateful"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Stateful(JSONLoader & json);
    bool operator==(IR::Attached const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    Stateful(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Stateful(IR::ID name, const IR::Annotations* annotations);
    Stateful(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    Stateful(const IR::Annotations* annotations);
    Stateful(Util::SourceInfo srcInfo, IR::ID name);
    Stateful(IR::ID name);
    Stateful(Util::SourceInfo srcInfo);
    Stateful();
    IRNODE_ABSTRACT_SUBCLASS(Stateful)
};
}  // namespace IR
namespace IR {
class CounterOrMeter : public Stateful {
 public:
    CounterType type = CounterType::NONE;
#line 310 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void settype(cstring t);
#line 6486 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"

    bool operator==(IR::CounterOrMeter const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "CounterOrMeter"; }
    static cstring static_type_name() { return "CounterOrMeter"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    CounterOrMeter(JSONLoader & json);
    bool operator==(IR::Stateful const & a) const override { return a == *this; }
    bool operator==(IR::Attached const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    CounterOrMeter(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    CounterOrMeter(IR::ID name, const IR::Annotations* annotations);
    CounterOrMeter(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    CounterOrMeter(const IR::Annotations* annotations);
    CounterOrMeter(Util::SourceInfo srcInfo, IR::ID name);
    CounterOrMeter(IR::ID name);
    CounterOrMeter(Util::SourceInfo srcInfo);
    CounterOrMeter();
    IRNODE_ABSTRACT_SUBCLASS(CounterOrMeter)
};
}  // namespace IR
namespace IR {
class Counter : public CounterOrMeter {
 public:
    int max_width = -1;
    int min_width = -1;
#line 321 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    char const * kind() const override;
#line 6517 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 322 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    IR::Type const * getType() const override;
#line 6520 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Counter const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Counter"; }
    static cstring static_type_name() { return "Counter"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Counter(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::CounterOrMeter const & a) const override { return a == *this; }
    bool operator==(IR::Stateful const & a) const override { return a == *this; }
    bool operator==(IR::Attached const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Counter(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Counter(IR::ID name, const IR::Annotations* annotations);
    Counter(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    Counter(const IR::Annotations* annotations);
    Counter(Util::SourceInfo srcInfo, IR::ID name);
    Counter(IR::ID name);
    Counter(Util::SourceInfo srcInfo);
    Counter();
    IRNODE_SUBCLASS(Counter)
};
}  // namespace IR
namespace IR {
class Meter : public CounterOrMeter {
 public:
    const IR::Expression* result = nullptr;
    const IR::Expression* pre_color = nullptr;
    IR::ID implementation = {};
#line 329 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    char const * kind() const override;
#line 6553 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 330 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Type *getType() const override;
#line 6556 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Meter const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Meter"; }
    static cstring static_type_name() { return "Meter"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Meter(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::CounterOrMeter const & a) const override { return a == *this; }
    bool operator==(IR::Stateful const & a) const override { return a == *this; }
    bool operator==(IR::Attached const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Meter(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Meter(IR::ID name, const IR::Annotations* annotations);
    Meter(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    Meter(const IR::Annotations* annotations);
    Meter(Util::SourceInfo srcInfo, IR::ID name);
    Meter(IR::ID name);
    Meter(Util::SourceInfo srcInfo);
    Meter();
    IRNODE_SUBCLASS(Meter)
};
}  // namespace IR
namespace IR {
class Register : public Stateful {
 public:
    IR::ID layout = {};
    int width = -1;
    bool signed_ = false;

#line 338 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    char const * kind() const override;
#line 6592 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 339 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Type *getType() const override;
#line 6595 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Register const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "Register"; }
    static cstring static_type_name() { return "Register"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Register(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Stateful const & a) const override { return a == *this; }
    bool operator==(IR::Attached const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Register(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    Register(IR::ID name, const IR::Annotations* annotations);
    Register(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    Register(const IR::Annotations* annotations);
    Register(Util::SourceInfo srcInfo, IR::ID name);
    Register(IR::ID name);
    Register(Util::SourceInfo srcInfo);
    Register();
    IRNODE_SUBCLASS(Register)
};
}  // namespace IR
namespace IR {
class PrimitiveAction : public Node {
 public:
    bool operator==(IR::PrimitiveAction const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "PrimitiveAction"; }
    static cstring static_type_name() { return "PrimitiveAction"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    PrimitiveAction(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    PrimitiveAction(Util::SourceInfo srcInfo);
    PrimitiveAction();
    IRNODE_SUBCLASS(PrimitiveAction)
};
}  // namespace IR
namespace IR {
class NameList : public Node {
 public:
    safe_vector<IR::ID> names = {};
#line 346 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    NameList(Util::SourceInfo si, cstring n);
#line 6642 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 347 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    NameList(Util::SourceInfo si, IR::ID n);
#line 6645 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 348 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dump_fields(std::ostream & out) const override;
#line 6648 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::NameList const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "NameList"; }
    static cstring static_type_name() { return "NameList"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    NameList(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    NameList(Util::SourceInfo srcInfo);
    NameList();
    IRNODE_SUBCLASS(NameList)
};
}  // namespace IR
namespace IR {
class ActionArg : public Expression {
 public:
    cstring action_name;
    IR::ID name;
    bool read = false;
    bool write = false;
#line 357 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 6672 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 358 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6675 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ActionArg const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "ActionArg"; }
    static cstring static_type_name() { return "ActionArg"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ActionArg(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ActionArg(Util::SourceInfo srcInfo, const IR::Type* type, cstring action_name, IR::ID name);
    ActionArg(const IR::Type* type, cstring action_name, IR::ID name);
    ActionArg(Util::SourceInfo srcInfo, cstring action_name, IR::ID name);
    ActionArg(cstring action_name, IR::ID name);
    IRNODE_SUBCLASS(ActionArg)
};
}  // namespace IR
namespace IR {

class ActionFunction : public Node, public virtual IAnnotated {
 public:
    IR::ID name;
    IR::Vector<IR::Primitive> action = {};
    safe_vector<const IR::ActionArg *> args = {};
    const IR::Annotations* annotations = Annotations::empty;
#line 368 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6703 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 369 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::ActionArg *arg(cstring n) const;
#line 6706 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 374 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
#line 6710 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 381 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6713 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ActionFunction const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void validate() const override;
    cstring node_type_name() const override { return "ActionFunction"; }
    static cstring static_type_name() { return "ActionFunction"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ActionFunction(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ActionFunction(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    ActionFunction(IR::ID name, const IR::Annotations* annotations);
    ActionFunction(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    ActionFunction(const IR::Annotations* annotations);
    ActionFunction(Util::SourceInfo srcInfo, IR::ID name);
    ActionFunction(IR::ID name);
    ActionFunction(Util::SourceInfo srcInfo);
    ActionFunction();
    IRNODE_SUBCLASS(ActionFunction)
};
}  // namespace IR
namespace IR {
class ActionProfile : public Attached {
 public:
    IR::ID selector = {};
    safe_vector<IR::ID> actions = {};
    int size = 0;
#line 391 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    char const * kind() const override;
#line 6744 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 392 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    bool indexed() const override;
#line 6747 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ActionProfile const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "ActionProfile"; }
    static cstring static_type_name() { return "ActionProfile"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ActionProfile(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Attached const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ActionProfile(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    ActionProfile(IR::ID name, const IR::Annotations* annotations);
    ActionProfile(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    ActionProfile(const IR::Annotations* annotations);
    ActionProfile(Util::SourceInfo srcInfo, IR::ID name);
    ActionProfile(IR::ID name);
    ActionProfile(Util::SourceInfo srcInfo);
    ActionProfile();
    IRNODE_SUBCLASS(ActionProfile)
};
}  // namespace IR
namespace IR {
class ActionSelector : public Attached {
 public:
    IR::ID key = {};
    const IR::FieldListCalculation* key_fields = nullptr;
    IR::ID mode = {};
    IR::ID type = {};
#line 400 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    char const * kind() const override;
#line 6779 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::ActionSelector const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "ActionSelector"; }
    static cstring static_type_name() { return "ActionSelector"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    ActionSelector(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Attached const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    ActionSelector(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    ActionSelector(IR::ID name, const IR::Annotations* annotations);
    ActionSelector(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    ActionSelector(const IR::Annotations* annotations);
    ActionSelector(Util::SourceInfo srcInfo, IR::ID name);
    ActionSelector(IR::ID name);
    ActionSelector(Util::SourceInfo srcInfo);
    ActionSelector();
    IRNODE_SUBCLASS(ActionSelector)
};
}  // namespace IR
namespace IR {
class V1Table : public Node, public virtual IInstance, public virtual IAnnotated {
 public:
    IR::ID name;
    const IR::Vector<IR::Expression>* reads = 0;
    safe_vector<IR::ID> reads_types = {};
    int min_size = 0;
    int max_size = 0;
    int size = 0;
    IR::ID action_profile = {};
    safe_vector<IR::ID> actions = {};
    IR::ID default_action = {};
    bool default_action_is_const = false;
    const IR::Vector<IR::Expression>* default_action_args = 0;
    IR::TableProperties properties = {};

    const IR::Annotations* annotations = Annotations::empty;
#line 418 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void addProperty(const IR::Property* prop);
#line 6823 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 419 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6826 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 420 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6829 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 421 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring Name() const override;
#line 6832 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 422 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Type *getType() const override;
#line 6835 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::V1Table const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "V1Table"; }
    static cstring static_type_name() { return "V1Table"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    V1Table(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    V1Table(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    V1Table(IR::ID name, const IR::Annotations* annotations);
    V1Table(Util::SourceInfo srcInfo, const IR::Annotations* annotations);
    V1Table(const IR::Annotations* annotations);
    V1Table(Util::SourceInfo srcInfo, IR::ID name);
    V1Table(IR::ID name);
    V1Table(Util::SourceInfo srcInfo);
    V1Table();
    IRNODE_SUBCLASS(V1Table)
};
}  // namespace IR
namespace IR {
class V1Control : public Node, public virtual IAnnotated {
 public:
    IR::ID name;
    const IR::Vector<IR::Expression>* code = nullptr;
    const IR::Annotations* annotations = nullptr;
#line 430 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    V1Control(IR::ID n);
#line 6868 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 431 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    V1Control(Util::SourceInfo si, IR::ID n);
#line 6871 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    IRNODE_DECLARE_APPLY_OVERLOAD(V1Control)
#line 433 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::Annotations *getAnnotations() const override;
#line 6875 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 434 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6878 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::V1Control const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "V1Control"; }
    static cstring static_type_name() { return "V1Control"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    V1Control(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    V1Control(Util::SourceInfo srcInfo, IR::ID name, const IR::Vector<IR::Expression>* code, const IR::Annotations* annotations);
    V1Control(IR::ID name, const IR::Vector<IR::Expression>* code, const IR::Annotations* annotations);
    IRNODE_SUBCLASS(V1Control)
};
}  // namespace IR
namespace IR {
class AttribLocal : public Expression, public virtual IDeclaration {
 public:
    IR::ID name;
#line 439 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    IR::ID getName() const override;
#line 6903 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 440 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 6906 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::AttribLocal const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "AttribLocal"; }
    static cstring static_type_name() { return "AttribLocal"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    AttribLocal(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    AttribLocal(Util::SourceInfo srcInfo, const IR::Type* type, IR::ID name);
    AttribLocal(const IR::Type* type, IR::ID name);
    AttribLocal(Util::SourceInfo srcInfo, IR::ID name);
    AttribLocal(IR::ID name);
    IRNODE_SUBCLASS(AttribLocal)
};
}  // namespace IR
namespace IR {
class AttribLocals : public Node, public virtual ISimpleNamespace {
 public:
    IR::NameMap<IR::AttribLocal> locals = {};
#line 446 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    Util::Enumerator<const IR::IDeclaration *> * getDeclarations() const override;
#line 6930 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 448 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    const IR::IDeclaration *getDeclByName(cstring name) const override;
#line 6933 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::AttribLocals const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "AttribLocals"; }
    static cstring static_type_name() { return "AttribLocals"; }
    void toJSON(JSONGenerator & json) const override;
    AttribLocals(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    AttribLocals(Util::SourceInfo srcInfo);
    AttribLocals();
    IRNODE_SUBCLASS(AttribLocals)
};
}  // namespace IR
namespace IR {
class Attribute : public Declaration {
 public:
    const IR::Type* type = nullptr;
    const IR::AttribLocals* locals = nullptr;
    bool optional = false;
#line 455 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 6958 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::Attribute const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "Attribute"; }
    static cstring static_type_name() { return "Attribute"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    Attribute(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    Attribute(Util::SourceInfo srcInfo, IR::ID name);
    Attribute(IR::ID name);
    IRNODE_SUBCLASS(Attribute)
};
}  // namespace IR
namespace IR {
class GlobalRef : public Expression {
 public:
    const IR::Node* obj = nullptr;


#line 462 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void validate() const override;
#line 6985 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 463 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 6988 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 464 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 6991 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::GlobalRef const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "GlobalRef"; }
    static cstring static_type_name() { return "GlobalRef"; }
    void toJSON(JSONGenerator & json) const override;
    GlobalRef(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    GlobalRef(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Node* obj);
    GlobalRef(const IR::Type* type, const IR::Node* obj);
    GlobalRef(Util::SourceInfo srcInfo, const IR::Node* obj);
    GlobalRef(const IR::Node* obj);
    IRNODE_SUBCLASS(GlobalRef)
};
}  // namespace IR
namespace IR {
class AttributeRef : public Expression {
 public:
    cstring extern_name;
    const IR::Type_Extern* extern_type = nullptr;
    const IR::Attribute* attrib = nullptr;
#line 472 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    cstring toString() const override;
#line 7018 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 473 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void dbprint(std::ostream & out) const override;
#line 7021 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::AttributeRef const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "AttributeRef"; }
    static cstring static_type_name() { return "AttributeRef"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    AttributeRef(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    AttributeRef(Util::SourceInfo srcInfo, const IR::Type* type, cstring extern_name, const IR::Type_Extern* extern_type, const IR::Attribute* attrib);
    AttributeRef(const IR::Type* type, cstring extern_name, const IR::Type_Extern* extern_type, const IR::Attribute* attrib);
    AttributeRef(Util::SourceInfo srcInfo, cstring extern_name, const IR::Type_Extern* extern_type, const IR::Attribute* attrib);
    AttributeRef(cstring extern_name, const IR::Type_Extern* extern_type, const IR::Attribute* attrib);
    IRNODE_SUBCLASS(AttributeRef)
};
}  // namespace IR
namespace IR {
class V1Program : public Node {
 public:
    IR::NameMap<IR::Node, std::multimap> scope;
#line 480 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    explicit V1Program();
#line 7048 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 482 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    template<class T> const T *get(cstring name) const { return scope.get<T>(name); }
#line 7051 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 484 "/home/aa/ppk_sw/ppk/p4c/ir/v1.def"
    void add(cstring name, IR::Node const * n);
#line 7054 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    IRNODE_DECLARE_APPLY_OVERLOAD(V1Program)
    bool operator==(IR::V1Program const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "V1Program"; }
    static cstring static_type_name() { return "V1Program"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    V1Program(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(V1Program)
};
}  // namespace IR
namespace IR {
/** @} */
/* end group irdefs */
class IDPDKNode : public virtual INode {
 public:
#line 2 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    virtual std::ostream & toSpec(std::ostream & out) const = 0;
#line 7078 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
};
}  // namespace IR
namespace IR {
class DpdkDeclaration : public Node, public virtual IDPDKNode {
 public:
    const IR::Declaration* global = nullptr;
#line 7 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7087 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkDeclaration const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkDeclaration"; }
    static cstring static_type_name() { return "DpdkDeclaration"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkDeclaration(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkDeclaration(Util::SourceInfo srcInfo, const IR::Declaration* global);
    DpdkDeclaration(const IR::Declaration* global);
    IRNODE_SUBCLASS(DpdkDeclaration)
};
}  // namespace IR
namespace IR {
class DpdkExternDeclaration : public Declaration_Instance, public virtual IDPDKNode {
 public:
#line 12 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7109 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkExternDeclaration const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkExternDeclaration"; }
    static cstring static_type_name() { return "DpdkExternDeclaration"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkExternDeclaration(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Declaration_Instance const & a) const override { return a == *this; }
    bool operator==(IR::Declaration const & a) const override { return a == *this; }
    bool operator==(IR::StatOrDecl const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkExternDeclaration(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    DpdkExternDeclaration(IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    DpdkExternDeclaration(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    DpdkExternDeclaration(IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments, const IR::BlockStatement* initializer);
    DpdkExternDeclaration(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    DpdkExternDeclaration(IR::ID name, const IR::Annotations* annotations, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    DpdkExternDeclaration(Util::SourceInfo srcInfo, IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    DpdkExternDeclaration(IR::ID name, const IR::Type* type, const IR::Vector<IR::Argument>* arguments);
    IRNODE_SUBCLASS(DpdkExternDeclaration)
};
}  // namespace IR
namespace IR {
class DpdkHeaderType : public Type_Header, public virtual IDPDKNode {
 public:
#line 17 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7137 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkHeaderType const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkHeaderType"; }
    static cstring static_type_name() { return "DpdkHeaderType"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkHeaderType(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Header const & a) const override { return a == *this; }
    bool operator==(IR::Type_StructLike const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(IR::ID name, IR::IndexedVector<IR::StructField> fields);
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    DpdkHeaderType(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    DpdkHeaderType(IR::ID name, const IR::TypeParameters* typeParameters);
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    DpdkHeaderType(IR::ID name, const IR::Annotations* annotations);
    DpdkHeaderType(Util::SourceInfo srcInfo, IR::ID name);
    DpdkHeaderType(IR::ID name);
    IRNODE_SUBCLASS(DpdkHeaderType)
};
}  // namespace IR
namespace IR {
class DpdkStructType : public Type_Struct, public virtual IDPDKNode {
 public:
#line 22 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7174 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkStructType const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkStructType"; }
    static cstring static_type_name() { return "DpdkStructType"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkStructType(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Type_Struct const & a) const override { return a == *this; }
    bool operator==(IR::Type_StructLike const & a) const override { return a == *this; }
    bool operator==(IR::Type_Declaration const & a) const override { return a == *this; }
    bool operator==(IR::Type const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(IR::ID name, const IR::TypeParameters* typeParameters, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(IR::ID name, const IR::Annotations* annotations, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(IR::ID name, IR::IndexedVector<IR::StructField> fields);
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    DpdkStructType(IR::ID name, const IR::Annotations* annotations, const IR::TypeParameters* typeParameters);
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name, const IR::TypeParameters* typeParameters);
    DpdkStructType(IR::ID name, const IR::TypeParameters* typeParameters);
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name, const IR::Annotations* annotations);
    DpdkStructType(IR::ID name, const IR::Annotations* annotations);
    DpdkStructType(Util::SourceInfo srcInfo, IR::ID name);
    DpdkStructType(IR::ID name);
    IRNODE_SUBCLASS(DpdkStructType)
};
}  // namespace IR
namespace IR {
class DpdkAsmStatement : public Node, public virtual IDPDKNode {
 public:
#line 27 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7211 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkAsmStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkAsmStatement"; }
    static cstring static_type_name() { return "DpdkAsmStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkAsmStatement(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
 protected:
    DpdkAsmStatement(Util::SourceInfo srcInfo);
    DpdkAsmStatement();
    IRNODE_ABSTRACT_SUBCLASS(DpdkAsmStatement)
};
}  // namespace IR
namespace IR {
class DpdkAction : public Node {
 public:
    IR::IndexedVector<IR::DpdkAsmStatement> statements;
    IR::ID name;
    IR::ParameterList para;
#line 34 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const;
#line 7233 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkAction const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkAction"; }
    static cstring static_type_name() { return "DpdkAction"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkAction(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkAction(Util::SourceInfo srcInfo, IR::IndexedVector<IR::DpdkAsmStatement> statements, IR::ID name, IR::ParameterList para);
    DpdkAction(IR::IndexedVector<IR::DpdkAsmStatement> statements, IR::ID name, IR::ParameterList para);
    IRNODE_SUBCLASS(DpdkAction)
};
}  // namespace IR
namespace IR {
class DpdkTable : public Node {
 public:
    cstring name;
    const IR::Key* match_keys = nullptr;
    const IR::ActionList* actions = nullptr;
    const IR::Expression* default_action = nullptr;
    const IR::TableProperties* properties = nullptr;
#line 45 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const;
#line 7261 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkTable const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    cstring node_type_name() const override { return "DpdkTable"; }
    static cstring static_type_name() { return "DpdkTable"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkTable(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkTable(Util::SourceInfo srcInfo, cstring name, const IR::Key* match_keys, const IR::ActionList* actions, const IR::Expression* default_action, const IR::TableProperties* properties);
    DpdkTable(cstring name, const IR::Key* match_keys, const IR::ActionList* actions, const IR::Expression* default_action, const IR::TableProperties* properties);
    IRNODE_SUBCLASS(DpdkTable)
};
}  // namespace IR
namespace IR {

class DpdkAsmProgram : public Node {
 public:
    IR::IndexedVector<IR::DpdkHeaderType> headerType;
    IR::IndexedVector<IR::DpdkStructType> structType;
    IR::IndexedVector<IR::DpdkExternDeclaration> externDeclarations;
    IR::IndexedVector<IR::DpdkAction> actions;
    IR::IndexedVector<IR::DpdkTable> tables;
    IR::IndexedVector<IR::DpdkAsmStatement> statements;
    IR::IndexedVector<IR::DpdkDeclaration> globals;
#line 59 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const;
#line 7291 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkAsmProgram const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkAsmProgram"; }
    static cstring static_type_name() { return "DpdkAsmProgram"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkAsmProgram(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkAsmProgram(Util::SourceInfo srcInfo, IR::IndexedVector<IR::DpdkHeaderType> headerType, IR::IndexedVector<IR::DpdkStructType> structType, IR::IndexedVector<IR::DpdkExternDeclaration> externDeclarations, IR::IndexedVector<IR::DpdkAction> actions, IR::IndexedVector<IR::DpdkTable> tables, IR::IndexedVector<IR::DpdkAsmStatement> statements, IR::IndexedVector<IR::DpdkDeclaration> globals);
    DpdkAsmProgram(IR::IndexedVector<IR::DpdkHeaderType> headerType, IR::IndexedVector<IR::DpdkStructType> structType, IR::IndexedVector<IR::DpdkExternDeclaration> externDeclarations, IR::IndexedVector<IR::DpdkAction> actions, IR::IndexedVector<IR::DpdkTable> tables, IR::IndexedVector<IR::DpdkAsmStatement> statements, IR::IndexedVector<IR::DpdkDeclaration> globals);
    IRNODE_SUBCLASS(DpdkAsmProgram)
};
}  // namespace IR
namespace IR {
class DpdkListStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring name;
    IR::IndexedVector<IR::DpdkAsmStatement> statements;
#line 65 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7316 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkListStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkListStatement"; }
    static cstring static_type_name() { return "DpdkListStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkListStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkListStatement(Util::SourceInfo srcInfo, cstring name, IR::IndexedVector<IR::DpdkAsmStatement> statements);
    DpdkListStatement(cstring name, IR::IndexedVector<IR::DpdkAsmStatement> statements);
    DpdkListStatement(Util::SourceInfo srcInfo, IR::IndexedVector<IR::DpdkAsmStatement> statements);
    DpdkListStatement(IR::IndexedVector<IR::DpdkAsmStatement> statements);
    DpdkListStatement(Util::SourceInfo srcInfo, cstring name);
    DpdkListStatement(cstring name);
    DpdkListStatement(Util::SourceInfo srcInfo);
    DpdkListStatement();
    IRNODE_SUBCLASS(DpdkListStatement)
};
}  // namespace IR
namespace IR {
class DpdkApplyStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring table;
#line 71 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7347 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkApplyStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkApplyStatement"; }
    static cstring static_type_name() { return "DpdkApplyStatement"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkApplyStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkApplyStatement(Util::SourceInfo srcInfo, cstring table);
    DpdkApplyStatement(cstring table);
    IRNODE_SUBCLASS(DpdkApplyStatement)
};
}  // namespace IR
namespace IR {
class DpdkEmitStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* header = nullptr;
#line 75 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7370 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkEmitStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkEmitStatement"; }
    static cstring static_type_name() { return "DpdkEmitStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkEmitStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkEmitStatement(Util::SourceInfo srcInfo, const IR::Expression* header);
    DpdkEmitStatement(const IR::Expression* header);
    IRNODE_SUBCLASS(DpdkEmitStatement)
};
}  // namespace IR
namespace IR {
class DpdkExtractStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* header = nullptr;
#line 80 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7395 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkExtractStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkExtractStatement"; }
    static cstring static_type_name() { return "DpdkExtractStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkExtractStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkExtractStatement(Util::SourceInfo srcInfo, const IR::Expression* header);
    DpdkExtractStatement(const IR::Expression* header);
    IRNODE_SUBCLASS(DpdkExtractStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpStatement : public DpdkAsmStatement {
 public:
    cstring instruction;
    cstring label;
#line 87 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7420 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 89 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpStatement(cstring instruction, cstring l);
#line 7423 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpStatement"; }
    static cstring static_type_name() { return "DpdkJmpStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpStatement(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_ABSTRACT_SUBCLASS(DpdkJmpStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpLabelStatement : public DpdkJmpStatement {
 public:
#line 95 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpLabelStatement(cstring label);
#line 7441 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpLabelStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpLabelStatement"; }
    static cstring static_type_name() { return "DpdkJmpLabelStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpLabelStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpLabelStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpHitStatement : public DpdkJmpStatement {
 public:
#line 102 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpHitStatement(cstring label);
#line 7461 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpHitStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpHitStatement"; }
    static cstring static_type_name() { return "DpdkJmpHitStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpHitStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpHitStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpMissStatement : public DpdkJmpStatement {
 public:
#line 109 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpMissStatement(cstring label);
#line 7480 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpMissStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpMissStatement"; }
    static cstring static_type_name() { return "DpdkJmpMissStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpMissStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpMissStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpHeaderStatement : public DpdkJmpStatement {
 public:
    const IR::Expression* header = nullptr;
#line 115 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7500 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 117 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpHeaderStatement(cstring instruction, cstring label, const IR::Expression* hdr);
#line 7503 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpHeaderStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkJmpHeaderStatement"; }
    static cstring static_type_name() { return "DpdkJmpHeaderStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpHeaderStatement(JSONLoader & json);
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_ABSTRACT_SUBCLASS(DpdkJmpHeaderStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpIfInvalidStatement : public DpdkJmpHeaderStatement {
 public:
#line 123 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpIfInvalidStatement(cstring label, const IR::Expression* hdr);
#line 7524 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpIfInvalidStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpIfInvalidStatement"; }
    static cstring static_type_name() { return "DpdkJmpIfInvalidStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpIfInvalidStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpHeaderStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpIfInvalidStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpIfValidStatement : public DpdkJmpHeaderStatement {
 public:
#line 130 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpIfValidStatement(cstring label, const IR::Expression* hdr);
#line 7545 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpIfValidStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpIfValidStatement"; }
    static cstring static_type_name() { return "DpdkJmpIfValidStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpIfValidStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpHeaderStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpIfValidStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpCondStatement : public DpdkJmpStatement {
 public:
    const IR::Expression* src1 = nullptr;
    const IR::Expression* src2 = nullptr;
#line 137 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7567 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 139 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpCondStatement(cstring instruction, cstring label, const IR::Expression* src1, const IR::Expression* src2);
#line 7570 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpCondStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkJmpCondStatement"; }
    static cstring static_type_name() { return "DpdkJmpCondStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpCondStatement(JSONLoader & json);
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_ABSTRACT_SUBCLASS(DpdkJmpCondStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpEqualStatement : public DpdkJmpCondStatement, public virtual IDPDKNode {
 public:
#line 146 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpEqualStatement(cstring label, const IR::Expression* src1, const IR::Expression* src2);
#line 7591 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpEqualStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpEqualStatement"; }
    static cstring static_type_name() { return "DpdkJmpEqualStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpEqualStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpCondStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpEqualStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpNotEqualStatement : public DpdkJmpCondStatement, public virtual IDPDKNode {
 public:
#line 152 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpNotEqualStatement(cstring label, const IR::Expression* src1, const IR::Expression* src2);
#line 7611 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpNotEqualStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpNotEqualStatement"; }
    static cstring static_type_name() { return "DpdkJmpNotEqualStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpNotEqualStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpCondStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpNotEqualStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpGreaterEqualStatement : public DpdkJmpCondStatement, public virtual IDPDKNode {
 public:
#line 159 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpGreaterEqualStatement(cstring label, const IR::Expression* src1, const IR::Expression* src2);
#line 7632 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpGreaterEqualStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpGreaterEqualStatement"; }
    static cstring static_type_name() { return "DpdkJmpGreaterEqualStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpGreaterEqualStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpCondStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpGreaterEqualStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpGreaterStatement : public DpdkJmpCondStatement, public virtual IDPDKNode {
 public:
#line 166 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpGreaterStatement(cstring label, const IR::Expression* src1, const IR::Expression* src2);
#line 7652 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpGreaterStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpGreaterStatement"; }
    static cstring static_type_name() { return "DpdkJmpGreaterStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpGreaterStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpCondStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpGreaterStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpLessOrEqualStatement : public DpdkJmpCondStatement, public virtual IDPDKNode {
 public:
#line 173 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpLessOrEqualStatement(cstring label, const IR::Expression* src1, const IR::Expression* src2);
#line 7672 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpLessOrEqualStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpLessOrEqualStatement"; }
    static cstring static_type_name() { return "DpdkJmpLessOrEqualStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpLessOrEqualStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpCondStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpLessOrEqualStatement)
};
}  // namespace IR
namespace IR {
class DpdkJmpLessStatement : public DpdkJmpCondStatement, public virtual IDPDKNode {
 public:
#line 180 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkJmpLessStatement(cstring label, const IR::Expression* src1, const IR::Expression* src2);
#line 7692 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkJmpLessStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkJmpLessStatement"; }
    static cstring static_type_name() { return "DpdkJmpLessStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkJmpLessStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkJmpCondStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkJmpStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkJmpLessStatement)
};
}  // namespace IR
namespace IR {
class DpdkRxStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
#line 185 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7712 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkRxStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkRxStatement"; }
    static cstring static_type_name() { return "DpdkRxStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkRxStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkRxStatement(Util::SourceInfo srcInfo);
    DpdkRxStatement();
    IRNODE_SUBCLASS(DpdkRxStatement)
};
}  // namespace IR
namespace IR {
class DpdkTxStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
#line 190 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7732 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkTxStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkTxStatement"; }
    static cstring static_type_name() { return "DpdkTxStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkTxStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkTxStatement(Util::SourceInfo srcInfo);
    DpdkTxStatement();
    IRNODE_SUBCLASS(DpdkTxStatement)
};
}  // namespace IR
namespace IR {
class DpdkAssignmentStatement : public DpdkAsmStatement {
 public:
    cstring instruction;
    const IR::Expression* dst = nullptr;
#line 198 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkAssignmentStatement(cstring instruction, const IR::Expression* dst);
#line 7754 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkAssignmentStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkAssignmentStatement"; }
    static cstring static_type_name() { return "DpdkAssignmentStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkAssignmentStatement(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_ABSTRACT_SUBCLASS(DpdkAssignmentStatement)
};
}  // namespace IR
namespace IR {
class DpdkUnaryStatement : public DpdkAssignmentStatement {
 public:
    const IR::Expression* src = nullptr;
#line 204 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7776 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 206 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkUnaryStatement(cstring instruction, const IR::Expression* dst, const IR::Expression* src);
#line 7779 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkUnaryStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkUnaryStatement"; }
    static cstring static_type_name() { return "DpdkUnaryStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkUnaryStatement(JSONLoader & json);
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_ABSTRACT_SUBCLASS(DpdkUnaryStatement)
};
}  // namespace IR
namespace IR {
class DpdkMovStatement : public DpdkUnaryStatement {
 public:
#line 213 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkMovStatement(const IR::Expression* dst, const IR::Expression* src);
#line 7800 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkMovStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkMovStatement"; }
    static cstring static_type_name() { return "DpdkMovStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkMovStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkUnaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkMovStatement)
};
}  // namespace IR
namespace IR {
class DpdkNegStatement : public DpdkUnaryStatement {
 public:
#line 220 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkNegStatement(const IR::Expression* dst, const IR::Expression* src);
#line 7820 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkNegStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkNegStatement"; }
    static cstring static_type_name() { return "DpdkNegStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkNegStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkUnaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkNegStatement)
};
}  // namespace IR
namespace IR {
class DpdkCmplStatement : public DpdkUnaryStatement {
 public:
#line 227 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkCmplStatement(const IR::Expression* dst, const IR::Expression* src);
#line 7840 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkCmplStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkCmplStatement"; }
    static cstring static_type_name() { return "DpdkCmplStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkCmplStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkUnaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkCmplStatement)
};
}  // namespace IR
namespace IR {
class DpdkLNotStatement : public DpdkUnaryStatement {
 public:
#line 234 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkLNotStatement(const IR::Expression* dst, const IR::Expression* src);
#line 7860 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkLNotStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkLNotStatement"; }
    static cstring static_type_name() { return "DpdkLNotStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkLNotStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkUnaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkLNotStatement)
};
}  // namespace IR
namespace IR {
class DpdkBinaryStatement : public DpdkAssignmentStatement {
 public:
    const IR::Expression* src1 = nullptr;
    const IR::Expression* src2 = nullptr;
#line 241 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 7882 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 243 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkBinaryStatement(cstring instruction, const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 7885 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkBinaryStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkBinaryStatement"; }
    static cstring static_type_name() { return "DpdkBinaryStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkBinaryStatement(JSONLoader & json);
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_ABSTRACT_SUBCLASS(DpdkBinaryStatement)
};
}  // namespace IR
namespace IR {
class DpdkAddStatement : public DpdkBinaryStatement {
 public:
#line 250 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkAddStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 7906 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkAddStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkAddStatement"; }
    static cstring static_type_name() { return "DpdkAddStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkAddStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkAddStatement)
};
}  // namespace IR
namespace IR {
class DpdkAndStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 257 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkAndStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 7926 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkAndStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkAndStatement"; }
    static cstring static_type_name() { return "DpdkAndStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkAndStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkAndStatement)
};
}  // namespace IR
namespace IR {
class DpdkShlStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 264 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkShlStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 7946 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkShlStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkShlStatement"; }
    static cstring static_type_name() { return "DpdkShlStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkShlStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkShlStatement)
};
}  // namespace IR
namespace IR {
class DpdkShrStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 271 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkShrStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 7966 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkShrStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkShrStatement"; }
    static cstring static_type_name() { return "DpdkShrStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkShrStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkShrStatement)
};
}  // namespace IR
namespace IR {
class DpdkSubStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 278 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkSubStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 7986 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkSubStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkSubStatement"; }
    static cstring static_type_name() { return "DpdkSubStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkSubStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkSubStatement)
};
}  // namespace IR
namespace IR {
class DpdkOrStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 285 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkOrStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8006 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkOrStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkOrStatement"; }
    static cstring static_type_name() { return "DpdkOrStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkOrStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkOrStatement)
};
}  // namespace IR
namespace IR {
class DpdkEquStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 292 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkEquStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8026 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkEquStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkEquStatement"; }
    static cstring static_type_name() { return "DpdkEquStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkEquStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkEquStatement)
};
}  // namespace IR
namespace IR {
class DpdkXorStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 299 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkXorStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8046 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkXorStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkXorStatement"; }
    static cstring static_type_name() { return "DpdkXorStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkXorStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkXorStatement)
};
}  // namespace IR
namespace IR {
class DpdkCmpStatement : public DpdkBinaryStatement, public virtual IDPDKNode {
 public:
#line 306 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkCmpStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8066 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkCmpStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkCmpStatement"; }
    static cstring static_type_name() { return "DpdkCmpStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkCmpStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkCmpStatement)
};
}  // namespace IR
namespace IR {
class DpdkLAndStatement : public DpdkBinaryStatement {
 public:
#line 313 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkLAndStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8086 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkLAndStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkLAndStatement"; }
    static cstring static_type_name() { return "DpdkLAndStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkLAndStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkLAndStatement)
};
}  // namespace IR
namespace IR {
class DpdkLOrStatement : public DpdkBinaryStatement {
 public:
#line 320 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkLOrStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8106 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkLOrStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkLOrStatement"; }
    static cstring static_type_name() { return "DpdkLOrStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkLOrStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkLOrStatement)
};
}  // namespace IR
namespace IR {
class DpdkLeqStatement : public DpdkBinaryStatement {
 public:
#line 327 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkLeqStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8126 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkLeqStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkLeqStatement"; }
    static cstring static_type_name() { return "DpdkLeqStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkLeqStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkLeqStatement)
};
}  // namespace IR
namespace IR {
class DpdkLssStatement : public DpdkBinaryStatement {
 public:
#line 334 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkLssStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8146 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkLssStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkLssStatement"; }
    static cstring static_type_name() { return "DpdkLssStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkLssStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkLssStatement)
};
}  // namespace IR
namespace IR {
class DpdkGrtStatement : public DpdkBinaryStatement {
 public:
#line 341 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkGrtStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8166 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkGrtStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkGrtStatement"; }
    static cstring static_type_name() { return "DpdkGrtStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkGrtStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkGrtStatement)
};
}  // namespace IR
namespace IR {
class DpdkGeqStatement : public DpdkBinaryStatement {
 public:
#line 348 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkGeqStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8186 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkGeqStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkGeqStatement"; }
    static cstring static_type_name() { return "DpdkGeqStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkGeqStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkGeqStatement)
};
}  // namespace IR
namespace IR {
class DpdkNeqStatement : public DpdkBinaryStatement {
 public:
#line 355 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkNeqStatement(const IR::Expression* dst, const IR::Expression* src1, const IR::Expression* src2);
#line 8206 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkNeqStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkNeqStatement"; }
    static cstring static_type_name() { return "DpdkNeqStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkNeqStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkBinaryStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkNeqStatement)
};
}  // namespace IR
namespace IR {
class DpdkExternObjStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* methodCall = nullptr;
#line 361 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8227 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkExternObjStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkExternObjStatement"; }
    static cstring static_type_name() { return "DpdkExternObjStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkExternObjStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkExternObjStatement(Util::SourceInfo srcInfo, const IR::Expression* methodCall);
    DpdkExternObjStatement(const IR::Expression* methodCall);
    IRNODE_SUBCLASS(DpdkExternObjStatement)
};
}  // namespace IR
namespace IR {
class DpdkExternFuncStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
#line 366 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8250 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkExternFuncStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkExternFuncStatement"; }
    static cstring static_type_name() { return "DpdkExternFuncStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkExternFuncStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkExternFuncStatement(Util::SourceInfo srcInfo);
    DpdkExternFuncStatement();
    IRNODE_SUBCLASS(DpdkExternFuncStatement)
};
}  // namespace IR
namespace IR {
class DpdkReturnStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
#line 371 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8270 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkReturnStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkReturnStatement"; }
    static cstring static_type_name() { return "DpdkReturnStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkReturnStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkReturnStatement(Util::SourceInfo srcInfo);
    DpdkReturnStatement();
    IRNODE_SUBCLASS(DpdkReturnStatement)
};
}  // namespace IR
namespace IR {
class DpdkLabelStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring label;
#line 377 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8291 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 379 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkLabelStatement(cstring l);
#line 8294 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkLabelStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkLabelStatement"; }
    static cstring static_type_name() { return "DpdkLabelStatement"; }
    void dbprint(std::ostream & out) const override;
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkLabelStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkLabelStatement)
};
}  // namespace IR
namespace IR {
class DpdkChecksumAddStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring csum;
    cstring intermediate_value;
    const IR::Expression* field = nullptr;
#line 386 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8317 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkChecksumAddStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkChecksumAddStatement"; }
    static cstring static_type_name() { return "DpdkChecksumAddStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkChecksumAddStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkChecksumAddStatement(Util::SourceInfo srcInfo, cstring csum, cstring intermediate_value, const IR::Expression* field);
    DpdkChecksumAddStatement(cstring csum, cstring intermediate_value, const IR::Expression* field);
    IRNODE_SUBCLASS(DpdkChecksumAddStatement)
};
}  // namespace IR
namespace IR {
class DpdkChecksumSubStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring csum;
    cstring intermediate_value;
    const IR::Expression* field = nullptr;
#line 394 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8344 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkChecksumSubStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkChecksumSubStatement"; }
    static cstring static_type_name() { return "DpdkChecksumSubStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkChecksumSubStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkChecksumSubStatement(Util::SourceInfo srcInfo, cstring csum, cstring intermediate_value, const IR::Expression* field);
    DpdkChecksumSubStatement(cstring csum, cstring intermediate_value, const IR::Expression* field);
    IRNODE_SUBCLASS(DpdkChecksumSubStatement)
};
}  // namespace IR
namespace IR {
class DpdkChecksumClearStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring csum;
    cstring intermediate_value;
#line 401 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8370 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkChecksumClearStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkChecksumClearStatement"; }
    static cstring static_type_name() { return "DpdkChecksumClearStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkChecksumClearStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkChecksumClearStatement(Util::SourceInfo srcInfo, cstring csum, cstring intermediate_value);
    DpdkChecksumClearStatement(cstring csum, cstring intermediate_value);
    IRNODE_SUBCLASS(DpdkChecksumClearStatement)
};
}  // namespace IR
namespace IR {
class DpdkGetHashStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring hash;
    const IR::Expression* fields = nullptr;
    const IR::Expression* dst = nullptr;
#line 409 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8394 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkGetHashStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkGetHashStatement"; }
    static cstring static_type_name() { return "DpdkGetHashStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkGetHashStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkGetHashStatement(Util::SourceInfo srcInfo, cstring hash, const IR::Expression* fields, const IR::Expression* dst);
    DpdkGetHashStatement(cstring hash, const IR::Expression* fields, const IR::Expression* dst);
    IRNODE_SUBCLASS(DpdkGetHashStatement)
};
}  // namespace IR
namespace IR {
class DpdkGetChecksumStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* dst = nullptr;
    cstring checksum;
    cstring intermediate_value;
#line 417 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8421 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkGetChecksumStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkGetChecksumStatement"; }
    static cstring static_type_name() { return "DpdkGetChecksumStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkGetChecksumStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkGetChecksumStatement(Util::SourceInfo srcInfo, const IR::Expression* dst, cstring checksum, cstring intermediate_value);
    DpdkGetChecksumStatement(const IR::Expression* dst, cstring checksum, cstring intermediate_value);
    IRNODE_SUBCLASS(DpdkGetChecksumStatement)
};
}  // namespace IR
namespace IR {
class DpdkCastStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* src = nullptr;
    const IR::Expression* dst = nullptr;
    const IR::Type* type = nullptr;
#line 425 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8448 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkCastStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkCastStatement"; }
    static cstring static_type_name() { return "DpdkCastStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkCastStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkCastStatement(Util::SourceInfo srcInfo, const IR::Expression* src, const IR::Expression* dst, const IR::Type* type);
    DpdkCastStatement(const IR::Expression* src, const IR::Expression* dst, const IR::Type* type);
    IRNODE_SUBCLASS(DpdkCastStatement)
};
}  // namespace IR
namespace IR {
class DpdkVerifyStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* condition = nullptr;
    const IR::Expression* error = nullptr;
#line 432 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8473 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkVerifyStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkVerifyStatement"; }
    static cstring static_type_name() { return "DpdkVerifyStatement"; }
    void toJSON(JSONGenerator & json) const override;
    DpdkVerifyStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkVerifyStatement(Util::SourceInfo srcInfo, const IR::Expression* condition, const IR::Expression* error);
    DpdkVerifyStatement(const IR::Expression* condition, const IR::Expression* error);
    IRNODE_SUBCLASS(DpdkVerifyStatement)
};
}  // namespace IR
namespace IR {
class DpdkMeterExecuteStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring meter;
    const IR::Expression* index = nullptr;
    const IR::Expression* color = nullptr;
#line 440 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8499 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkMeterExecuteStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkMeterExecuteStatement"; }
    static cstring static_type_name() { return "DpdkMeterExecuteStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkMeterExecuteStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkMeterExecuteStatement(Util::SourceInfo srcInfo, cstring meter, const IR::Expression* index, const IR::Expression* color);
    DpdkMeterExecuteStatement(cstring meter, const IR::Expression* index, const IR::Expression* color);
    IRNODE_SUBCLASS(DpdkMeterExecuteStatement)
};
}  // namespace IR
namespace IR {
class DpdkCounterCountStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring counter;
    const IR::Expression* index = nullptr;
#line 447 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8525 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkCounterCountStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkCounterCountStatement"; }
    static cstring static_type_name() { return "DpdkCounterCountStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkCounterCountStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkCounterCountStatement(Util::SourceInfo srcInfo, cstring counter, const IR::Expression* index);
    DpdkCounterCountStatement(cstring counter, const IR::Expression* index);
    IRNODE_SUBCLASS(DpdkCounterCountStatement)
};
}  // namespace IR
namespace IR {
class DpdkRegisterDeclStatement : public DpdkAsmStatement {
 public:
    cstring reg;
    const IR::Expression* size = nullptr;
    const IR::Expression* init_val = nullptr;
#line 455 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8552 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkRegisterDeclStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkRegisterDeclStatement"; }
    static cstring static_type_name() { return "DpdkRegisterDeclStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkRegisterDeclStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkRegisterDeclStatement(Util::SourceInfo srcInfo, cstring reg, const IR::Expression* size, const IR::Expression* init_val);
    DpdkRegisterDeclStatement(cstring reg, const IR::Expression* size, const IR::Expression* init_val);
    DpdkRegisterDeclStatement(Util::SourceInfo srcInfo, cstring reg, const IR::Expression* size);
    DpdkRegisterDeclStatement(cstring reg, const IR::Expression* size);
    IRNODE_SUBCLASS(DpdkRegisterDeclStatement)
};
}  // namespace IR
namespace IR {
class DpdkRegisterReadStatement : public DpdkAssignmentStatement {
 public:
    cstring reg;
    const IR::Expression* index = nullptr;
#line 462 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8580 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 465 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    DpdkRegisterReadStatement(const IR::Expression* dst, cstring reg, const IR::Expression* index);
#line 8583 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkRegisterReadStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkRegisterReadStatement"; }
    static cstring static_type_name() { return "DpdkRegisterReadStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkRegisterReadStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAssignmentStatement const & a) const override { return a == *this; }
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IRNODE_SUBCLASS(DpdkRegisterReadStatement)
};
}  // namespace IR
namespace IR {
class DpdkRegisterWriteStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    cstring reg;
    const IR::Expression* index = nullptr;
    const IR::Expression* src = nullptr;
#line 473 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8609 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkRegisterWriteStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkRegisterWriteStatement"; }
    static cstring static_type_name() { return "DpdkRegisterWriteStatement"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkRegisterWriteStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkRegisterWriteStatement(Util::SourceInfo srcInfo, cstring reg, const IR::Expression* index, const IR::Expression* src);
    DpdkRegisterWriteStatement(cstring reg, const IR::Expression* index, const IR::Expression* src);
    IRNODE_SUBCLASS(DpdkRegisterWriteStatement)
};
}  // namespace IR
namespace IR {
class DpdkValidateStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* header = nullptr;
#line 479 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8634 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkValidateStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkValidateStatement"; }
    static cstring static_type_name() { return "DpdkValidateStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkValidateStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkValidateStatement(Util::SourceInfo srcInfo, const IR::Expression* header);
    DpdkValidateStatement(const IR::Expression* header);
    IRNODE_SUBCLASS(DpdkValidateStatement)
};
}  // namespace IR
namespace IR {
class DpdkInvalidateStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
    const IR::Expression* header = nullptr;
#line 484 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8659 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkInvalidateStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "DpdkInvalidateStatement"; }
    static cstring static_type_name() { return "DpdkInvalidateStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkInvalidateStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkInvalidateStatement(Util::SourceInfo srcInfo, const IR::Expression* header);
    DpdkInvalidateStatement(const IR::Expression* header);
    IRNODE_SUBCLASS(DpdkInvalidateStatement)
};
}  // namespace IR
namespace IR {
class DpdkDropStatement : public DpdkAsmStatement, public virtual IDPDKNode {
 public:
#line 488 "/home/aa/ppk_sw/ppk/p4c/backends/dpdk/dpdk.def"
    std::ostream & toSpec(std::ostream & out) const override;
#line 8683 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::DpdkDropStatement const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "DpdkDropStatement"; }
    static cstring static_type_name() { return "DpdkDropStatement"; }
    void dbprint(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    DpdkDropStatement(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::DpdkAsmStatement const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    DpdkDropStatement(Util::SourceInfo srcInfo);
    DpdkDropStatement();
    IRNODE_SUBCLASS(DpdkDropStatement)
};
}  // namespace IR
namespace IR {
class v1HeaderType : public Node {
 public:
    IR::ID name;
    const IR::Type_Struct* as_metadata = nullptr;
    const IR::Type_Header* as_header = nullptr;
#line 7 "/home/aa/ppk_sw/ppk/p4c/frontends/p4-14/ir-v1.def"
    v1HeaderType(IR::Type_Struct const * m, IR::Type_Header const * h = nullptr);
#line 8707 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
#line 9 "/home/aa/ppk_sw/ppk/p4c/frontends/p4-14/ir-v1.def"
    void dbprint(std::ostream & out) const override;
#line 8710 "/home/aa/ppk_sw/ppk/p4c/build/ir/ir-generated.h"
    bool operator==(IR::v1HeaderType const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    void visit_children(Visitor & v) override;
    void visit_children(Visitor & v) const override;
    void validate() const override;
    cstring node_type_name() const override { return "v1HeaderType"; }
    static cstring static_type_name() { return "v1HeaderType"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    v1HeaderType(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Node const & a) const override { return a == *this; }
    v1HeaderType(Util::SourceInfo srcInfo, IR::ID name, const IR::Type_Struct* as_metadata, const IR::Type_Header* as_header);
    v1HeaderType(IR::ID name, const IR::Type_Struct* as_metadata, const IR::Type_Header* as_header);
    IRNODE_SUBCLASS(v1HeaderType)
};
}  // namespace IR
namespace IR {

class IntMod : public Operation_Unary {
 public:
    unsigned width;
    cstring getStringOp() const override { return "><"; }
    int getPrecedence() const override { return DBPrint::Prec_Low; }
    bool operator==(IR::IntMod const & a) const override;
    bool equiv(IR::Node const & a_) const override;
    cstring node_type_name() const override { return "IntMod"; }
    static cstring static_type_name() { return "IntMod"; }
    void dump_fields(std::ostream & out) const override;
    void toJSON(JSONGenerator & json) const override;
    IntMod(JSONLoader & json);
    static IR::Node * fromJSON(JSONLoader & json);
    bool operator==(IR::Operation_Unary const & a) const override { return a == *this; }
    bool operator==(IR::Operation const & a) const override { return a == *this; }
    bool operator==(IR::Expression const & a) const override { return a == *this; }
    bool operator==(IR::Node const & a) const override { return a == *this; }
    IntMod(Util::SourceInfo srcInfo, const IR::Type* type, const IR::Expression* expr, unsigned width);
    IntMod(const IR::Type* type, const IR::Expression* expr, unsigned width);
    IntMod(Util::SourceInfo srcInfo, const IR::Expression* expr, unsigned width);
    IntMod(const IR::Expression* expr, unsigned width);
    IRNODE_SUBCLASS(IntMod)
};
}  // namespace IR
#endif /* _IR_GENERATED_H_ */
