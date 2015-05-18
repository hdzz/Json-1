/****************************************************************************************************
jsonerr.h

Purpose:
	Json Reader & Writer.

Author:
	Wookong

Created Time:
2014-01-06
****************************************************************************************************/

#ifndef __JSON_ERR_H__
#define __JSON_ERR_H__

#if AB_PRAGMA_ONCE
#pragma once
#endif

#define JSON_ERR_PROVIDER_NAME 						_T("JSON")

//
// expected type (%s) is different from input type(%s)¡£
//
#define JSON_TYPE_ERROR								0x00000001

//
// suffer error when parsing£¬string to parse£º%s£¬address£º%u¡£
//
#define JSON_PARSE_ERROR							0x00000002

//
// suffer error when decoding
//
#define JSON_DECODE_ERROR							0x00000003

//
// suffer error when type casting
//
#define JSON_TYPE_CASTING_ERROR						0x00000004

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Assert macros
//

#ifdef __WINDOWS__
	#define JSON_INTERNAL_ASSERT_CHECK_EX(expression, errId, ...)			\
		ASSERT_TRUE_E(expression, JSON_ERR_PROVIDER_NAME, errId, __VA_ARGS__)
#else
	#define JSON_INTERNAL_ASSERT_CHECK_EX(expression, errId, args...)		\
		ASSERT_TRUE_E(expression, JSON_ERR_PROVIDER_NAME, errId, args)
#endif

#ifdef __WINDOWS__
	#define JSON_INTERNAL_ASSERT_CHECK(expression, ...)						\
		JSON_INTERNAL_ASSERT_CHECK_EX(expression, JSON_ERR_INTERNAL_ERROR, __VA_ARGS__)
#else
	#define EOFS_INTERNAL_ASSERT_CHECK(expression, args...)					\
		JSON_INTERNAL_ASSERT_CHECK_EX(expression, JSON_ERR_INTERNAL_ERROR, args)
#endif

#ifdef __DEBUG__
	#ifdef __WINDOWS__
		#define JSON_ASSERT_EX(expression, errId, ...)						\
			JSON_INTERNAL_ASSERT_CHECK_EX (expression, errId, __VA_ARGS__)
		#define JSON_ASSERT(expression, ...)		JSON_INTERNAL_ASSERT_CHECK (expression, __VA_ARGS__)
	#else
		#define JSON_ASSERT_EX(expression, errId, args...)						\
			JSON_INTERNAL_ASSERT_CHECK_EX (expression, errId, args)
		#define JSON_ASSERT(expression, args...)	JSON_INTERNAL_ASSERT_CHECK (expression, args)
	#endif
#else
	#ifdef __WINDOWS__
		#define JSON_ASSERT_EX(expression, errId, ...)			((void)(0))
		#define JSON_ASSERT(expression, ...)					((void)(0))
	#else
		#define JSON_ASSERT_EX(expression, errId, args...)		((void)(0))
		#define JSON_ASSERT(expression, args...)				((void)(0))
	#endif
#endif

#endif // __JSON_ERR_H__

