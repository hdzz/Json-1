/****************************************************************************************************
Json.h

Purpose:
	Json Reader & Writer.
	
Author:
	updated by Wookong

	created by tunnuz from https://github.com/tunnuz/json/blob/master/json_st.hh

Created Time:
	2014-01-06
****************************************************************************************************/

#ifndef _JSON_H__
#define _JSON_H__

#if AB_PRAGMA_ONCE
#pragma once
#endif

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
using namespace std;

#include "jsonerr.h"

#ifdef __WINDOWS__
	#define JSON_TSTRING(type)			basic_string<type, char_traits<type>, allocator<type> >
#else
	#define JSON_TSTRING(type)			basic_string<type>
#endif

#ifdef __WINDOWS__
	#define JSON_TSTRINGSTREAM(type)	basic_stringstream<type, char_traits<type>, allocator<type> >
#else
	#define JSON_TSTRINGSTREAM(type)	basic_stringstream<type>
#endif

#define JSON_CHECK_TYPE(type, except)	JSON_INTERNAL_ASSERT_CHECK_EX (type == except, JSON_TYPE_ERROR, _T("Type error: except(%s), actual(%s)"), get_type_name (except), get_type_name (type))

#ifdef __CXX11__
	#define JSON_MOVE(statement)		std::move (statement)
#else
	#define JSON_MOVE(statement)		(statement)
#endif

#define JSON_EPSILON					1E-6

namespace JSON
{
	namespace detail
	{
		template<bool, typename T = void>
		struct json_enable_if {};
		template<typename T>
		struct json_enable_if<true, T> {typedef T type;};

		template<class T>
		struct json_remove_const {typedef T type;};
		template<class T>
		struct json_remove_const<const T> {typedef T type;};
		template<class T>
		struct json_remove_volatile {typedef T type;};
		template<class T>
		struct json_remove_volatile<volatile T> {typedef T type;};
		template<class T>
		struct json_remove_cv {typedef typename json_remove_const<typename json_remove_volatile<T>::type>::type type;};

		struct json_true_type {enum {value = true};};
		struct json_false_type {enum {value = false};};

		template<class T1, class T2>
		struct json_is_same_ : json_false_type {};
		template<class T>
		struct json_is_same_<T, T> : json_true_type {};
		template<class T1, class T2>
		struct json_is_same : json_is_same_<typename json_remove_cv<T1>::type, T2> {};

		template<class T> struct json_is_integral_ : json_false_type {};
		template<> struct json_is_integral_<char> : json_true_type {};
		template<> struct json_is_integral_<unsigned char> : json_true_type {};
		template<> struct json_is_integral_<signed char> : json_true_type {};
#ifdef _NATIVE_WCHAR_T_DEFINED
		template<> struct json_is_integral_<wchar_t> : json_true_type {};
#endif /* _NATIVE_WCHAR_T_DEFINED */
		template<> struct json_is_integral_<unsigned short> : json_true_type {};
		template<> struct json_is_integral_<signed short> : json_true_type {};
		template<> struct json_is_integral_<unsigned int> : json_true_type {};
		template<> struct json_is_integral_<signed int> : json_true_type {};
		template<> struct json_is_integral_<unsigned long> : json_true_type {};
		template<> struct json_is_integral_<signed long> : json_true_type {};
		template<> struct json_is_integral_<uint64> : json_true_type {};
		template<> struct json_is_integral_<int64> : json_true_type {};

		template<class T> struct json_is_floating_point_ : json_false_type {};
		template<> struct json_is_floating_point_<float> : json_true_type {};
		template<> struct json_is_floating_point_<double> : json_true_type {};
		template<> struct json_is_floating_point_<long double> : json_true_type {};

		template<class T>
		struct json_is_integral
		{
			typedef typename json_remove_cv<T>::type type;
			enum {value = json_is_integral_<type>::value};
		};

		template<class T>
		struct json_is_floating_point
		{
			typedef typename json_remove_cv<T>::type type;
			enum {value = json_is_floating_point_<type>::value};
		};

		template<class T>
		struct json_is_arithmetic
		{
			typedef typename json_remove_cv<T>::type type;
			enum {value = json_is_integral_<type>::value || json_is_same<type, bool>::value || json_is_floating_point_<type>::value};
		};
	}

	/** Possible JSON type of a value (array, object, bool, ...). */
	enum Type
	{
		INTEGER,    // Integer
		FLOAT,      // Float 3.14 12e-10
		BOOLEAN,    // Boolean (true, false)
		STRING,     // String " ... " or (not really JSON) ' ... '
		OBJECT,     // Object { ... }
		ARRAY,      // Array [ ... ]
		NIL         // Null
	};

	inline const char* get_type_name (int type);

	// Forward declaration
	template<class char_t>
	class ValueT;

	/** A JSON object, i.e., a container whose keys are strings, this
	is roughly equivalent to a Python dictionary, a PHP's associative
	array, a Perl or a C++ map (depending on the implementation). */
	template<class char_t>
	class ObjectT : public std::map<JSON_TSTRING(char_t), ValueT<char_t> > {};

	typedef ObjectT<char> Object;
	typedef ObjectT<wchar_t> ObjectW;

	/** A JSON array, i.e., an indexed container of elements. It contains
	JSON values, that can have any of the types in ValueType. */
	template<class char_t>
	class ArrayT : public std::vector<ValueT<char_t> > {};

	typedef ArrayT<char> Array;
	typedef ArrayT<wchar_t> ArrayW;

	/** A JSON value. Can have either type in ValueTypes. */
	template<class char_t>
	class ValueT
	{
	public:
		typedef JSON_TSTRING(char_t) tstring;

		/** Default constructor (type = NIL). */
		ValueT (void) : _type (NIL) {}

		/** Constructor with type. */
		ValueT (Type type);

		/** Copy constructor. */
		ValueT (const ValueT<char_t>& v);

		/** Constructor from integer. */
#define JSON_INTEGER_CTOR(type)		ValueT (type i) : _type (INTEGER), _integer (i) {}

		JSON_INTEGER_CTOR (unsigned char)
		JSON_INTEGER_CTOR (signed char)
#ifdef _NATIVE_WCHAR_T_DEFINED
		JSON_INTEGER_CTOR (wchar_t)
#endif /* _NATIVE_WCHAR_T_DEFINED */
		JSON_INTEGER_CTOR (unsigned short)
		JSON_INTEGER_CTOR (signed short)
		JSON_INTEGER_CTOR (unsigned int)
		JSON_INTEGER_CTOR (signed int)
		JSON_INTEGER_CTOR (unsigned long)
		JSON_INTEGER_CTOR (signed long)
		JSON_INTEGER_CTOR (uint64)
		JSON_INTEGER_CTOR (int64)

		/** Constructor from float. */
#define JSON_FLOAT_CTOR(type)		ValueT (type f) : _type (FLOAT), _float (f) {}

		JSON_FLOAT_CTOR (float)
		JSON_FLOAT_CTOR (double)
		JSON_FLOAT_CTOR (long double)

		/** Constructor from bool. */
		ValueT (bool b) : _type (BOOLEAN), _boolean (b) {}

		/** Constructor from pointer to char (C-string).  */
		ValueT (const char_t* s, bool needConv = true) : _type (STRING), _needConv (needConv), _string (s) {}
		/** Constructor from pointer to char (C-string).  */
		ValueT (const char_t* s, size_t l, bool needConv = true) : _type (STRING), _needConv (needConv), _string (s, l) {}
		/** Constructor from STD string  */
		ValueT (const tstring& s, bool needConv = true) : _type (STRING), _needConv (needConv), _string (s) {}

		/** Constructor from pointer to Object. */
		ValueT (const ObjectT<char_t>& o) : _type (OBJECT), _object (o) {}

		/** Constructor from pointer to Array. */
		ValueT (const ArrayT<char_t>& a) : _type (ARRAY), _array (a) {}

#ifdef __CXX11__
		/** Move constructor. */
		ValueT (ValueT<char_t>&& v) : _type (v._type) {assign (JSON_MOVE (v));}

		/** Move constructor from STD string  */
		ValueT (tstring&& s, bool needConv = true) : _type (STRING), _string (JSON_MOVE (s)), _needConv (needConv) {}

		/** Move constructor from pointer to Object. */
		ValueT (ObjectT<char_t>&& o) : _type (OBJECT), _object (JSON_MOVE (o)) {}

		/** Move constructor from pointer to Array. */
		ValueT (ArrayT<char_t>&& a) : _type (ARRAY), _array (JSON_MOVE (a)) {}
#endif

		/** Assign function. */
		void assign (const ValueT<char_t>& v);

		/** Assign function from integer. */
		template<class T>
		inline typename detail::json_enable_if<detail::json_is_integral<T>::value>::type
		assign (T i)
		{
			clear ();
			_type = INTEGER;
			_integer = i;
		}

		/** Assign function from float. */
		template<class T>
		inline typename detail::json_enable_if<detail::json_is_floating_point<T>::value>::type
		assign (T f)
		{
			clear ();
			_type = FLOAT;
			_float = f;
		}

		/** Assign function from bool. */
		inline void assign (bool b)
		{
			clear ();
			_type = BOOLEAN;
			_boolean = b;
		}

		/** Assign function from pointer to char (C-string).  */
		inline void assign (const char_t* s, bool needConv = true)
		{
			clear ();
			_type = STRING;
			_string.assign (s);
			_needConv = needConv;
		}

		/** Assign function from pointer to char (C-string).  */
		inline void assign (const char_t* s, size_t l, bool needConv = true)
		{
			clear ();
			_type = STRING;
			_string.assign (s, l);
			_needConv = needConv;
		}

		/** Assign function from STD string  */
		inline void assign (const tstring& s, bool needConv = true)
		{
			clear ();
			_type = STRING;
			_string = s;
			_needConv = needConv;
		}

		/** Assign function from pointer to Object. */
		inline void assign (const ObjectT<char_t>& o)
		{
			clear ();
			_type = OBJECT;
			_object = o;
		}

		/** Assign function from pointer to Array. */
		inline void assign (const ArrayT<char_t>& a)
		{
			clear ();
			_type = ARRAY;
			_array = a;
		}

#ifdef __CXX11__
		/** Assign function. */
		void assign (ValueT<char_t>&& v);

		/** Assign function from STD string  */
		inline void assign (tstring&& s, bool needConv = true)
		{
			clear ();
			_type = STRING;
			_string = JSON_MOVE (s);
			_needConv = needConv;
		}

		/** Assign function from pointer to Object. */
		inline void assign (ObjectT<char_t>&& o)
		{
			clear ();
			_type = OBJECT;
			_object = JSON_MOVE (o);
		}

		/** Assign function from pointer to Array. */
		inline void assign (ArrayT<char_t>&& a)
		{
			clear ();
			_type = ARRAY;
			_array = JSON_MOVE (a);
		}
#endif

		/** Assignment operator. */
		inline ValueT<char_t>& operator= (const ValueT<char_t>& v)
		{
			if (this != &v) {
				assign (v);
			}
			return *this;
		}

#define JSON_ASSIGNMENT(arg)		{assign (arg);return *this;}

		/** Assignment operator from int/float/bool. */
		template<class T>
		inline typename detail::json_enable_if<detail::json_is_arithmetic<T>::value, ValueT<char_t>&>::type
		operator= (T a) JSON_ASSIGNMENT (a)

		/** Assignment operator from pointer to char (C-string).  */
		inline ValueT<char_t>& operator= (const char_t* s) JSON_ASSIGNMENT (s)

		/** Assignment operator from STD string  */
		inline ValueT<char_t>& operator= (const tstring& s) JSON_ASSIGNMENT (s)

		/** Assignment operator from pointer to Object. */
		inline ValueT<char_t>& operator= (const ObjectT<char_t>& o) JSON_ASSIGNMENT (o)

		/** Assignment operator from pointer to Array. */
		inline ValueT<char_t>& operator= (const ArrayT<char_t>& a) JSON_ASSIGNMENT (a)

#ifdef __CXX11__
		/** Assignment operator. */
		inline ValueT<char_t>& operator= (ValueT<char_t>&& v)
		{
			if (this != &v) {
				assign (JSON_MOVE (v));
			}
			return *this;
		}

		/** Assignment operator from STD string  */
		inline ValueT<char_t>& operator= (tstring&& s) JSON_ASSIGNMENT (JSON_MOVE (s))

		/** Assignment operator from pointer to Object. */
		inline ValueT<char_t>& operator= (ObjectT<char_t>&& o) JSON_ASSIGNMENT (JSON_MOVE (o))

		/** Assignment operator from pointer to Array. */
		inline ValueT<char_t>& operator= (ArrayT<char_t>&& a) JSON_ASSIGNMENT (JSON_MOVE (a))
#endif

		/** Type query. */
		inline Type type (void) const
		{
			return _type;
		}

		/** Cast operator for integer */
#define JSON_INTEGER_OPERATOR(type)		\
	inline operator type (void) const {JSON_CHECK_TYPE (_type, INTEGER);return _integer;}

		JSON_INTEGER_OPERATOR (unsigned char)
		JSON_INTEGER_OPERATOR (signed char)
#ifdef _NATIVE_WCHAR_T_DEFINED
		JSON_INTEGER_OPERATOR (wchar_t)
#endif /* _NATIVE_WCHAR_T_DEFINED */
		JSON_INTEGER_OPERATOR (unsigned short)
		JSON_INTEGER_OPERATOR (signed short)
		JSON_INTEGER_OPERATOR (unsigned int)
		JSON_INTEGER_OPERATOR (signed int)
		JSON_INTEGER_OPERATOR (unsigned long)
		JSON_INTEGER_OPERATOR (signed long)
		JSON_INTEGER_OPERATOR (uint64)
		JSON_INTEGER_OPERATOR (int64)

		/** Cast operator for float */
#define JSON_FLOAT_OPERATOR(type)		\
	inline operator type (void) const {JSON_CHECK_TYPE (_type, FLOAT);return _float;}

		JSON_FLOAT_OPERATOR (float)
		JSON_FLOAT_OPERATOR (double)
		JSON_FLOAT_OPERATOR (long double)

		/** Cast operator for bool */
		inline operator bool (void) const
		{
			JSON_CHECK_TYPE (_type, BOOLEAN);
			return _boolean;
		}

		/** Cast operator for STD string */
		inline operator tstring (void) const
		{
			JSON_CHECK_TYPE (_type, STRING);
			return _string;
		}

		/** Cast operator for Object */
		inline operator ObjectT<char_t> (void) const
		{
			JSON_CHECK_TYPE (_type, OBJECT);
			return _object;
		}

		/** Cast operator for Array */
		inline operator ArrayT<char_t> (void) const
		{
			JSON_CHECK_TYPE (_type, ARRAY);
			return _array;
		}

		/** Fetch integer reference*/
		inline int64& i (void)
		{
			if (_type == NIL) {
				_type = INTEGER;
				_integer = 0;
			}
			JSON_CHECK_TYPE (_type, INTEGER);
			return _integer;
		}

		/** Fetch integer value*/
		inline int64 i (void) const
		{
			JSON_CHECK_TYPE (_type, INTEGER);
			return _integer;
		}

		/** Fetch float reference */
		inline double& f (void)
		{
			if (_type == NIL) {
				_type = FLOAT;
				_float = 0;
			}
			JSON_CHECK_TYPE (_type, FLOAT);
			return _float;
		}

		/** Fetch float value */
		inline long double f (void) const
		{
			JSON_CHECK_TYPE (_type, FLOAT);
			return _float;
		}

		/** Fetch boolean reference */
		inline bool& b (void)
		{
			if (_type == NIL) {
				_type = BOOLEAN;
				_boolean = false;
			}
			JSON_CHECK_TYPE (_type, BOOLEAN);
			return _boolean;
		}

		/** Fetch boolean value */
		inline bool b (void) const
		{
			JSON_CHECK_TYPE (_type, BOOLEAN);
			return _boolean;
		}

		/** Fetch string reference */
		inline tstring& s (void)
		{
			if (_type == NIL) {
				_type = STRING;
				_needConv = true;
			}
			JSON_CHECK_TYPE (_type, STRING);
			return _string;
		}

		/** Fetch string const-reference */
		inline const tstring& s (void) const
		{
			JSON_CHECK_TYPE (_type, STRING);
			return _string;
		}

		/** Fetch object reference */
		inline ObjectT<char_t>& o (void)
		{
			if (_type == NIL) {
				_type = OBJECT;
			}
			JSON_CHECK_TYPE (_type, OBJECT);
			return _object;
		}

		/** Fetch object const-reference */
		inline const ObjectT<char_t>& o (void) const
		{
			JSON_CHECK_TYPE (_type, OBJECT);
			return _object;
		}

		/** Fetch array reference */
		inline ArrayT<char_t>& a (void)
		{
			if (_type == NIL) {
				_type = ARRAY;
			}
			JSON_CHECK_TYPE (_type, ARRAY);
			return _array;
		}

		/** Fetch array const-reference */
		inline const ArrayT<char_t>& a (void) const
		{
			JSON_CHECK_TYPE (_type, ARRAY);
			return _array;
		}

		/** Support [] operator for object. */
		inline ValueT<char_t>& operator[] (const char_t* key)
		{
			if (_type == NIL) {
				_type = OBJECT;
			}
			JSON_CHECK_TYPE (_type, OBJECT);
			return _object[key];
		}

		/** Support [] operator for object. */
		inline ValueT<char_t>& operator[] (const tstring& key)
		{
			if (_type == NIL) {
				_type = OBJECT;
			}
			JSON_CHECK_TYPE (_type, OBJECT);
			return _object[key];
		}

		/** Support [] operator for array. */
		inline ValueT<char_t>& operator[] (size_t pos)
		{
			JSON_CHECK_TYPE (_type, ARRAY);
			return _array[pos];
		}

		template<class T>
		T get (const tstring& key, const T& value) const;

		/** Clear current value. */
		void clear (void);

		/** Write value to stream. */
		void write (tstring& out) const;

		/**
			Read object/array from stream.
			Return char_t count(offset) parsed.
			If error occurred, throws a exception.
		*/
		size_t read (const char_t* in, size_t len);

		/**
			Read string from stream.
			Return char_t count(offset) parsed.
			NOTE: MUST with quotes.
			If error occurred, throws a exception.
		*/
		size_t read_string (const char_t* in, size_t len);

		/** Read number from stream.
			Return char_t count(offset) parsed.
			If error occurred, throws a exception.
		*/
		size_t read_number (const char_t* in, size_t len);

		/**
			Read boolean from stream.
			Return char_t count(offset) parsed.
			If error occurred, throws a exception.
		*/
		size_t read_boolean (const char_t* in, size_t len);

		/**
			Read null from stream.
			Return char_t count(offset) parsed.
			If error occurred, throws a exception.
		*/
		size_t read_nil (const char_t* in, size_t len);

	protected:

		/** Indicate current value type. */
		Type	_type;

		union {
			int64  _integer;
			double _float;
			bool   _boolean;
			bool   _needConv; /* Used for string. */
		};
		tstring			_string;
		ObjectT<char_t>	_object;
		ArrayT<char_t>	_array;
	};

	typedef ValueT<char> Value;
	typedef ValueT<wchar_t> ValueW;

	template<class char_t>
	struct WriterT
	{
		static void write (const ObjectT<char_t>& o, JSON_TSTRING(char_t)& out);
		static void write (const ArrayT<char_t>& a, JSON_TSTRING(char_t)& out);
	};

	typedef WriterT<char> Writer;
	typedef WriterT<wchar_t> WriterW;

	template<class char_t>
	struct ReaderT
	{
		static inline size_t read (ValueT<char_t>& v, const char_t* in, size_t len)
		{
			return v.read (in, len);
		}
	};

	typedef ReaderT<char> Reader;
	typedef ReaderT<wchar_t> ReaderW;

	/* Compare functions */
	template<class char_t>
	bool operator== (const ObjectT<char_t>& lhs, const ObjectT<char_t>& rhs);
	template<class char_t>
	bool operator== (const ArrayT<char_t>& lhs, const ArrayT<char_t>& rhs);
	template<class char_t>
	bool operator== (const ValueT<char_t>& lhs, const ValueT<char_t>& rhs);

	template<class char_t>
	inline bool operator!= (const ObjectT<char_t>& lhs, const ObjectT<char_t>& rhs) { return !operator== (lhs, rhs); }
	template<class char_t>
	inline bool operator!= (const ArrayT<char_t>& lhs, const ArrayT<char_t>& rhs) { return !operator== (lhs, rhs); }
	template<class char_t>
	inline bool operator!= (const ValueT<char_t>& lhs, const ValueT<char_t>& rhs) { return !operator== (lhs, rhs); }

	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_integral<T>::value, bool>::type
	operator== (const ValueT<char_t>& v, T i) { return v.type () == INTEGER && i == v.i (); }
	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_floating_point<T>::value, bool>::type
	operator== (const ValueT<char_t>& v, T f) { return v.type () == FLOAT && fabs (f - v.f ()) < JSON_EPSILON; }
	template<class char_t>
	inline bool operator== (const ValueT<char_t>& v, bool b) { return v.type () == BOOLEAN && b == v.b (); }
	template<class char_t>
	inline bool operator== (const ValueT<char_t>& v, const ObjectT<char_t>& o) { return v.type () == OBJECT && o == v.o (); }
	template<class char_t>
	inline bool operator== (const ValueT<char_t>& v, const ArrayT<char_t>& a) { return v.type () == ARRAY && a == v.a (); }
	template<class char_t>
	inline bool operator== (const ValueT<char_t>& v, const JSON_TSTRING(char_t)& s) { return v.type () == STRING && s == v.s (); }

	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_integral<T>::value, bool>::type
	operator== (T i, const ValueT<char_t>& v) { return operator== (v, i); }
	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_floating_point<T>::value, bool>::type
	operator== (T f, const ValueT<char_t>& v) { return operator== (v, f); }
	template<class char_t>
	inline bool operator== (bool b, const ValueT<char_t>& v) { return operator== (v, b); }
	template<class char_t>
	inline bool operator== (const ObjectT<char_t>& o, const ValueT<char_t>& v) { return operator== (v, o); }
	template<class char_t>
	inline bool operator== (const ArrayT<char_t>& a, const ValueT<char_t>& v) { return operator== (v, a); }
	template<class char_t>
	inline bool operator== (const JSON_TSTRING(char_t)& s, const ValueT<char_t>& v) { return operator== (v, s); }

	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_integral<T>::value, bool>::type
	operator!= (const ValueT<char_t>& v, T i) { return !operator== (v, i); }
	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_floating_point<T>::value, bool>::type
	operator!= (const ValueT<char_t>& v, T f) { return !operator== (v, f); }
	template<class char_t>
	inline bool operator!= (const ValueT<char_t>& v, bool b) { return !operator== (v, b); }
	template<class char_t>
	inline bool operator!= (const ValueT<char_t>& v, const ObjectT<char_t>& o) { return !operator== (v, o); }
	template<class char_t>
	inline bool operator!= (const ValueT<char_t>& v, const ArrayT<char_t>& a) { return !operator== (v, a); }
	template<class char_t>
	inline bool operator!= (const ValueT<char_t>& v, const JSON_TSTRING(char_t)& s) { return !operator== (v, s); }

	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_integral<T>::value, bool>::type
	operator!= (T i, const ValueT<char_t>& v) { return !operator== (v, i); }
	template<class char_t, class T>
	inline typename detail::json_enable_if<detail::json_is_floating_point<T>::value, bool>::type
	operator!= (T f, const ValueT<char_t>& v) { return !operator== (v, f); }
	template<class char_t>
	inline bool operator!= (bool b, const ValueT<char_t>& v) { return !operator== (v, b); }
	template<class char_t>
	inline bool operator!= (const ObjectT<char_t>& o, const ValueT<char_t>& v) { return !operator== (v, o); }
	template<class char_t>
	inline bool operator!= (const ArrayT<char_t>& a, const ValueT<char_t>& v) { return !operator== (v, a); }
	template<class char_t>
	inline bool operator!= (const JSON_TSTRING(char_t)& s, const ValueT<char_t>& v) { return !operator== (v, s); }
}

#include "Json.inl"

#endif // _JSON_H__
