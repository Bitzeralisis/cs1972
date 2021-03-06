#ifndef HEADERUTILS_H
#define HEADERUTILS_H

#define VALUE_ACCESSOR_DEEP(TYPE,NAME,MEMBER) TYPE NAME() const { return MEMBER; }
#define REFERENCE_ACCESSOR_DEEP(TYPE,NAME,MEMBER) TYPE& NAME() { return MEMBER; }
#define CONST_REFERENCE_ACCESSOR_DEEP(TYPE,NAME,MEMBER) const TYPE& NAME() const { return MEMBER; }
#define MUTATOR_DEEP(TYPE,NAME,MEMBER) void NAME(TYPE NAME) { MEMBER = NAME; }

#define VALUE_ACCESSOR(TYPE,NAME) inline VALUE_ACCESSOR_DEEP(TYPE,NAME,m_##NAME)
#define REFERENCE_ACCESSOR(TYPE,NAME) REFERENCE_ACCESSOR_DEEP(TYPE,NAME,m_##NAME)
#define CONST_REFERENCE_ACCESSOR(TYPE,NAME) CONST_REFERENCE_ACCESSOR_DEEP(TYPE,NAME,m_##NAME)
#define MUTATOR(TYPE,NAME) inline MUTATOR_DEEP(TYPE,NAME,m_##NAME)

#define VALACC_MUT(TYPE,NAME) VALUE_ACCESSOR(TYPE,NAME) MUTATOR(TYPE,NAME)

#endif // HEADERUTILS_H
