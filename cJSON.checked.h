/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 3 define options:

CJSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
CJSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
CJSON_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the CJSON_API_VISIBILITY flag to "export" the same symbols the way CJSON_EXPORT_SYMBOLS does

*/

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type)   type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 15

#include <stddef.h>

/* cJSON Types: */
#define cJSON_Invalid (0)
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw    (1 << 7) /* raw json */

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct cJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} cJSON;

typedef struct cJSON_Hooks
{
      /* malloc/free are CDECL on Windows regardless of the default calling convention of the compiler, so ensure the hooks allow passing those functions directly. */
      _Ptr<void *(size_t sz)> malloc_fn;
      _Ptr<void (void *ptr)> free_fn;
} cJSON_Hooks;

typedef int cJSON_bool;

/* Limits how deeply nested arrays/objects can be before cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* returns the version of cJSON as a string */
const char *cJSON_Version(void) : itype(_Nt_array_ptr<const char>);

/* Supply malloc, realloc and free functions to cJSON */
CJSON_PUBLIC(void) cJSON_InitHooks(_Ptr<cJSON_Hooks> hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of cJSON_Parse (with cJSON_Delete) and cJSON_Print (with stdlib free, cJSON_Hooks.free_fn, or cJSON_free as appropriate). The exception is cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a cJSON object you can interrogate. */
_Ptr<cJSON> cJSON_Parse(_Nt_array_ptr<const char> value);
_Ptr<cJSON> cJSON_ParseWithLength(_Ptr<const char> value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match cJSON_GetErrorPtr(). */
_Ptr<cJSON> cJSON_ParseWithOpts(_Nt_array_ptr<const char> value, _Ptr<_Ptr<const char>> return_parse_end, cJSON_bool require_null_terminated);
_Ptr<cJSON> cJSON_ParseWithLengthOpts(const char *value : itype(_Ptr<const char>), size_t buffer_length, const char **return_parse_end : itype(_Ptr<_Ptr<const char>>), cJSON_bool require_null_terminated);

/* Render a cJSON entity to text for transfer/storage. */
char *cJSON_Print(_Ptr<const cJSON> item) : itype(_Nt_array_ptr<char>);
/* Render a cJSON entity to text for transfer/storage without any formatting. */
char *cJSON_PrintUnformatted(_Ptr<const cJSON> item) : itype(_Ptr<char>);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
char *cJSON_PrintBuffered(_Ptr<const cJSON> item, int prebuffer, cJSON_bool fmt) : itype(_Ptr<char>);
/* Render a cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(_Ptr<cJSON> item, char *buffer : itype(_Ptr<char>), const int length, const cJSON_bool format);
/* Delete a cJSON entity and all subentities. */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item : itype(_Ptr<cJSON>));

/* Returns the number of items in an array (or object). */
CJSON_PUBLIC(int) cJSON_GetArraySize(_Ptr<const cJSON> array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
_Ptr<cJSON> cJSON_GetArrayItem(_Ptr<const cJSON> array, int index);
/* Get item "string" from object. Case insensitive. */
_Ptr<cJSON> cJSON_GetObjectItem(const _Ptr<const cJSON> object, const _Nt_array_ptr<const char> string);
_Ptr<cJSON> cJSON_GetObjectItemCaseSensitive(const _Ptr<const cJSON> object, const _Nt_array_ptr<const char> string);
CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(_Ptr<const cJSON> object, _Nt_array_ptr<const char> string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
const char *cJSON_GetErrorPtr(void) : itype(_Ptr<const char>);

/* Check item type and return its value */
char *cJSON_GetStringValue(const _Ptr<const cJSON> item) : itype(_Ptr<char>);
CJSON_PUBLIC(double) cJSON_GetNumberValue(const _Ptr<const cJSON> item);

/* These functions check the type of an item */
CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const _Ptr<const cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const _Ptr<const cJSON> item);

/* These calls create a cJSON item of the appropriate type. */
_Ptr<cJSON>  cJSON_CreateNull(void);
_Ptr<cJSON>  cJSON_CreateTrue(void);
_Ptr<cJSON>  cJSON_CreateFalse(void);
_Ptr<cJSON>  cJSON_CreateBool(cJSON_bool boolean);
_Ptr<cJSON>  cJSON_CreateNumber(double num);
_Ptr<cJSON> cJSON_CreateString(const char *string : itype(_Ptr<const char>));
/* raw json */
_Ptr<cJSON> cJSON_CreateRaw(const char *raw : itype(_Ptr<const char>));
_Ptr<cJSON>  cJSON_CreateArray(void);
_Ptr<cJSON>  cJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by cJSON_Delete */
_Ptr<cJSON> cJSON_CreateStringReference(const char *string : itype(_Ptr<const char>));
/* Create an object/array that only references it's elements so
 * they will not be freed by cJSON_Delete */
_Ptr<cJSON> cJSON_CreateObjectReference(const cJSON *child : itype(_Ptr<const cJSON>));
_Ptr<cJSON> cJSON_CreateArrayReference(const cJSON *child : itype(_Ptr<const cJSON>));

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
_Ptr<cJSON> cJSON_CreateIntArray(_Array_ptr<const int> numbers : count(count), int count);
_Ptr<cJSON> cJSON_CreateFloatArray(_Array_ptr<const float> numbers : count(count), int count);
_Ptr<cJSON> cJSON_CreateDoubleArray(_Array_ptr<const double> numbers : count(count), int count);
_Ptr<cJSON> cJSON_CreateStringArray(_Array_ptr<const _Nt_array_ptr<const char>> strings : count(count), int count);

/* Append item to the specified array/object. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(_Ptr<cJSON> array, _Ptr<cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(_Ptr<cJSON> object, _Ptr<const char> string, _Ptr<cJSON> item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & cJSON_StringIsConst) is zero before
 * writing to `item->string` */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObjectCS(_Ptr<cJSON> object, _Ptr<const char> string, _Ptr<cJSON> item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToArray(_Ptr<cJSON> array, _Array_ptr<cJSON> item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToObject(_Ptr<cJSON> object, _Ptr<const char> string, _Array_ptr<cJSON> item);

/* Remove/Detach items from Arrays/Objects. */
_Ptr<cJSON> cJSON_DetachItemViaPointer(_Ptr<cJSON> parent, const _Ptr<cJSON> item);
_Ptr<cJSON> cJSON_DetachItemFromArray(_Ptr<cJSON> array, int which);
CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(_Ptr<cJSON> array, int which);
_Ptr<cJSON> cJSON_DetachItemFromObject(_Ptr<cJSON> object, _Nt_array_ptr<const char> string);
_Ptr<cJSON> cJSON_DetachItemFromObjectCaseSensitive(_Ptr<cJSON> object, _Nt_array_ptr<const char> string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(_Ptr<cJSON> object, _Nt_array_ptr<const char> string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(_Ptr<cJSON> object, _Nt_array_ptr<const char> string);

/* Update array items. */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInArray(_Ptr<cJSON> array, int which, cJSON *newitem : itype(_Ptr<cJSON>)); /* Shifts pre-existing items to the right. */
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(const _Ptr<cJSON> parent, const _Ptr<cJSON> item, cJSON *replacement : itype(_Ptr<cJSON>));
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInArray(_Ptr<cJSON> array, int which, _Ptr<cJSON> newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObject(_Ptr<cJSON> object, _Nt_array_ptr<const char> string, _Ptr<cJSON> newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObjectCaseSensitive(_Ptr<cJSON> object, _Nt_array_ptr<const char> string, _Ptr<cJSON> newitem);

/* Duplicate a cJSON item */
_Ptr<cJSON> cJSON_Duplicate(_Ptr<const cJSON> item, cJSON_bool recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const _Ptr<const cJSON> a, const _Ptr<const cJSON> b, const cJSON_bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant, 
 * but should point to a readable and writable address area. */
CJSON_PUBLIC(void) cJSON_Minify(char *json : itype(_Array_ptr<char>));

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
_Ptr<cJSON> cJSON_AddNullToObject(const _Ptr<cJSON> object, const _Ptr<const char> name);
_Ptr<cJSON> cJSON_AddTrueToObject(const _Ptr<cJSON> object, const _Ptr<const char> name);
_Ptr<cJSON> cJSON_AddFalseToObject(const _Ptr<cJSON> object, const _Ptr<const char> name);
_Ptr<cJSON> cJSON_AddBoolToObject(const _Ptr<cJSON> object, const _Ptr<const char> name, const cJSON_bool boolean);
_Ptr<cJSON> cJSON_AddNumberToObject(const _Ptr<cJSON> object, const _Ptr<const char> name, const double number);
_Ptr<cJSON> cJSON_AddStringToObject(const _Ptr<cJSON> object, const _Ptr<const char> name, const _Ptr<const char> string);
_Ptr<cJSON> cJSON_AddRawToObject(const _Ptr<cJSON> object, const _Ptr<const char> name, const _Ptr<const char> raw);
_Ptr<cJSON> cJSON_AddObjectToObject(const _Ptr<cJSON> object, const _Ptr<const char> name);
_Ptr<cJSON> cJSON_AddArrayToObject(const _Ptr<cJSON> object, const _Ptr<const char> name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the cJSON_SetNumberValue macro */
CJSON_PUBLIC(double) cJSON_SetNumberHelper(_Ptr<cJSON> object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a cJSON_String object, only takes effect when type of object is cJSON_String */
char *cJSON_SetValuestring(_Ptr<cJSON> object, const char *valuestring : itype(_Nt_array_ptr<const char>)) : itype(_Ptr<char>);

/* Macro for iterating over an array or object */
#define cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free objects using the malloc/free functions that have been set with cJSON_InitHooks */
CJSON_PUBLIC(void *) cJSON_malloc(size_t size);
CJSON_PUBLIC(void) cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif
