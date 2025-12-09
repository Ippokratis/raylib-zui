#ifndef ZUI_H
#define ZUI_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <dbghelp.h>
#include <windows.h>
#else
#include <dlfcn.h>
#include <execinfo.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// --- ZUI HEADERS ---
#include "raylib.h"

    /*@
      logic boolean ZuiIsPowerOfTwo_logic(integer n) =
        n > 0 && (n & (n - 1)) == 0;
    */

    ///-----------------------------------------------------------------ZUI_ASSERT

#ifdef ZUI_DEBUG
#if defined(__APPLE__)

#define ZUI_MAX_CALLSTACK_DEPTH 64
#define ZUI_SYMBOL_NAME_BUFFER_SIZE 256

#define ZUI_ASSERT(cond, msg)                                             \
    do                                                                    \
    {                                                                     \
        if (!(cond))                                                      \
        {                                                                 \
            void *callstack[ZUI_MAX_CALLSTACK_DEPTH];                     \
            int frames = backtrace(callstack, ZUI_MAX_CALLSTACK_DEPTH);   \
            char **strs = backtrace_symbols(callstack, frames);           \
            TraceLog(LOG_ERROR, "[ZUI ASSERT FAILED]");                   \
            TraceLog(LOG_ERROR, "  Condition: %s", #cond);                \
            TraceLog(LOG_ERROR, "  Message:   %s", msg);                  \
            TraceLog(LOG_ERROR, "  Location:  %s:%d in %s()",             \
                     __FILE__, __LINE__, __func__);                       \
            TraceLog(LOG_ERROR, "  Call array:");                         \
            for (int _zui_i = 0; _zui_i < frames; _zui_i++)               \
            {                                                             \
                TraceLog(LOG_ERROR, "    [%d] %s", _zui_i, strs[_zui_i]); \
            }                                                             \
            free(strs);                                                   \
            abort();                                                      \
        }                                                                 \
    } while (0)
#elif defined(__linux__)
#define ZUI_ASSERT(cond, msg)                                                             \
    do                                                                                    \
    {                                                                                     \
        if (!(cond))                                                                      \
        {                                                                                 \
            void *callstack[ZUI_MAX_CALLSTACK_DEPTH];                                     \
            int frames = backtrace(callstack, ZUI_MAX_CALLSTACK_DEPTH);                   \
            char **strs = backtrace_symbols(callstack, frames);                           \
            TraceLog(LOG_ERROR, "[ZUI ASSERT FAILED] %s", msg);                           \
            TraceLog(LOG_ERROR, "Condition: %s", #cond);                                  \
            TraceLog(LOG_ERROR, "Location: %s:%d in %s()", __FILE__, __LINE__, __func__); \
            TraceLog(LOG_ERROR, "Call array:");                                           \
            for (int _zui_i = 0; _zui_i < frames; ++_zui_i)                               \
                TraceLog(LOG_ERROR, "  [%d] %s", _zui_i, strs[_zui_i]);                   \
            free(strs);                                                                   \
            abort();                                                                      \
        }                                                                                 \
    } while (0)
#elif defined(_WIN32)
#pragma comment(lib, "dbghelp.lib")
#define ZUI_ASSERT(cond, msg)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            TraceLog(LOG_ERROR, "[ZUI ASSERT FAILED] %s", msg);                                                        \
            TraceLog(LOG_ERROR, "Condition: %s", #cond);                                                               \
            TraceLog(LOG_ERROR, "Location: %s:%d in %s()", __FILE__, __LINE__, __func__);                              \
            HANDLE process = GetCurrentProcess();                                                                      \
            SymInitialize(process, NULL, TRUE);                                                                        \
            void *array[ZUI_MAX_CALLSTACK_DEPTH];                                                                      \
            unsigned short frames = CaptureStackBackTrace(0, ZUI_MAX_CALLSTACK_DEPTH, array, NULL);                    \
            SYMBOL_INFO *symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + ZUI_SYMBOL_NAME_BUFFER_SIZE, 1);         \
            if (symbol != NULL)                                                                                        \
            {                                                                                                          \
                symbol->MaxNameLen = ZUI_SYMBOL_NAME_BUFFER_SIZE - 1;                                                  \
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);                                                            \
                for (int i = 0; i < frames; ++i)                                                                       \
                {                                                                                                      \
                    SymFromAddr(process, (DWORD64)(array[i]), 0, symbol);                                              \
                    TraceLog(LOG_ERROR, "  [%d] %s + 0x%llx", i, symbol->Name, (DWORD64)(array[i]) - symbol->Address); \
                }                                                                                                      \
                free(symbol);                                                                                          \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                TraceLog(LOG_ERROR, "  (Unable to allocate memory for stack trace symbols)");                          \
            }                                                                                                          \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)
#else
#define ZUI_ASSERT(cond, msg)                                   \
    do                                                          \
    {                                                           \
        if (!(cond))                                            \
        {                                                       \
            TraceLog(LOG_ERROR, "[ZUI ASSERT FAILED] %s", msg); \
            TraceLog(LOG_ERROR, "Condition: %s", #cond);        \
            TraceLog(LOG_ERROR, "Location: %s:%d in %s()",      \
                     __FILE__, __LINE__, __func__);             \
            if (IsWindowReady())                                \
                CloseWindow();                                  \
            abort();                                            \
        }                                                       \
    } while (0)
#endif /* __APPLE__ */
#else  /* ZUI_DEBUG not defined */
#define ZUI_ASSERT(cond, msg) ((void)0)
#endif /* ZUI_DEBUG */

    ///--------------------------------------------------ZUI_DEFAULT_ALIGNMENT, ZUI_ALIGNOF

#ifndef ZUI_DEFAULT_ALIGNMENT
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define ZUI_DEFAULT_ALIGNMENT (_Alignof(max_align_t))
#else
#define ZUI_DEFAULT_ALIGNMENT (sizeof(void *))
#endif
#endif

#ifndef ZUI_ALIGNOF
#if defined(__cplusplus) && __cplusplus >= 201103L
#define ZUI_ALIGNOF(t) alignof(t)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define ZUI_ALIGNOF(t) _Alignof(t)
#elif defined(__GNUC__) || defined(__clang__)
#define ZUI_ALIGNOF(t) __alignof__(t)
#elif defined(_MSC_VER)
#define ZUI_ALIGNOF(t) __alignof(t)
#else
#define ZUI_ALIGNOF(t) ((size_t)&((struct { char c; t v; } *)0)->v)
#endif
#endif

#ifndef ZUI_MAX_DYNARRAY_CAPACITY
#define ZUI_MAX_DYNARRAY_CAPACITY 1048576u /* 1M items max */
#endif

    ///-----------------------------------------------------------ZUI_REPORT_ERROR

#ifdef ZUI_DEBUG
#define ZUI_REPORT_ERROR(result, ...)                     \
    do                                                    \
    {                                                     \
        TraceLog(LOG_ERROR, "[ZUI ERROR]");               \
        TraceLog(LOG_ERROR, "  Location:  %s:%d in %s()", \
                 __FILE__, __LINE__, __func__);           \
        TraceLog(LOG_ERROR, "  Code:      %s (%d)",       \
                 ZuiResultToString(result), result);      \
        TraceLog(LOG_ERROR, "  Message:   " __VA_ARGS__); \
    } while (0)
#else
#define ZUI_REPORT_ERROR(result, ...) ((void)0)
#endif
    ///-----------------------------------------------------------Enums
    typedef enum ZuiResult
    {
        ZUI_OK = 0, // Success

        // Pointer errors
        ZUI_ERROR_NULL_POINTER = 1, // NULL pointer argument
        ZUI_ERROR_NULL_CONTEXT = 2, // Global context is NULL
        ZUI_ERROR_NULL_ARENA = 3,   // Arena pointer is NULL

        // Memory errors
        ZUI_ERROR_OUT_OF_MEMORY = 10,     // Arena/allocation failed
        ZUI_ERROR_ARENA_EXHAUSTED = 11,   // Arena full
        ZUI_ERROR_ALLOCATION_FAILED = 12, // General allocation failure

        // Validation errors
        ZUI_ERROR_INVALID_CAPACITY = 20,     // Capacity out of range
        ZUI_ERROR_INVALID_ALIGNMENT = 21,    // Alignment not power of 2
        ZUI_ERROR_INVALID_ID = 22,           // Frame/Label ID out of bounds
        ZUI_ERROR_INVALID_BOUNDS = 23,       // Negative width/height
        ZUI_ERROR_INVALID_VALUE = 24,        // Invalid parameter value
        ZUI_ERROR_INVALID_ENUM = 25,         // Invalid enum value
        ZUI_ERROR_INVALID_OBJECT_STATE = 26, // Invalid object state

        // Overflow errors
        ZUI_ERROR_OVERFLOW = 30,      // Integer/size overflow
        ZUI_ERROR_OUT_OF_BOUNDS = 31, // Array index out of bounds

        // State errors
        ZUI_ERROR_NOT_INITIALIZED = 40,        // Context not initialized
        ZUI_ERROR_ALREADY_INITIALIZED = 41,    // Context already initialized
        ZUI_ERROR_INVALID_INTERNAL_STATE = 42, // Invalid internal state

        // Resource errors
        ZUI_ERROR_FONT_LOAD_FAILED = 50,    // Failed to load font
        ZUI_ERROR_TEXTURE_LOAD_FAILED = 51, // Failed to load texture
        ZUI_ERROR_RESOURCE_NOT_FOUND = 52,  // Resource not found

        // Operation errors
        ZUI_ERROR_OPERATION_FAILED = 60,   // Generic operation failure
        ZUI_ERROR_CIRCULAR_REFERENCE = 61, // Circular dependency detected
        ZUI_ERROR_ITEM_NOT_FOUND = 62,     // Item lookup failed
        ZUI_ERROR_BUFFER_TOO_SMALL = 63,   // Insufficient buffer size

        // Internal errors
        ZUI_ERROR_INTERNAL_ERROR = 70,  // Unexpected internal state
        ZUI_ERROR_NOT_IMPLEMENTED = 71, // Feature not implemented

    } ZuiResult;

    typedef enum ZuiConstants
    {
        ZUI_ALIGNMENT_MAX_ITEMS = 128,
        ZUI_MAX_TEXT_LENGTH = 128,
        ZUI_MAX_WINDOWS = 64,
        ZUI_BASE_FONT_SIZE = 18,
        ZUI_TITLEBAR_HEIGHT = 28,
        ZUI_WINDOWS_CAPACITY = 16,
        ZUI_MAX_INPUT_CHARS = 32,
        ZUI_FRAMES_CAPACITY = 16,
        ZUI_FRAME_ITEMS_CAPACITY = 32,
        ZUI_LABELS_CAPACITY = 32,
        ZUI_TEXTURES_CAPACITY = 32,
        ZUI_BUTTONS_CAPACITY = 32,
        ZUI_SLIDERS_CAPACITY = 16,
        ZUI_KNOBS_CAPACITY = 16,
        ZUI_NUMERIC_INPUTS_CAPACITY = 16,
        ZUI_MAX_FOCUSABLE_WIDGETS = 64,
        ZUI_CORNER_RADIUS = 8,
        ZUI_ROUNDNESS_SEGMENTS = 16,
        ZUI_DEFAULT_ARENA_SIZE = 1048576,
        ZUI_DEFAULT_FRAME_PADDING = 8,
        ZUI_DEFAULT_FRAME_GAP = 4,
        ZUI_DEFAULT_SCREEN_WIDTH = 800,
        ZUI_DEFAULT_SCREEN_HEIGHT = 600,
        ZUI_DEFAULT_DPI_SCALE = 1,
        ZUI_ROOT_FRAME_ID = 0,
        ZUI_MAX_ANCESTOR_DEPTH = 256,
    } ZuiConstants;

    static const float ZUI_DEFAULT_SPACING = 0.2F;

#ifndef FRAMA_C
    static const unsigned int ZUI_ID_INVALID = UINT_MAX - 1;
#endif

    typedef enum ZuiItemType
    {
        ZUI_FRAME,
        ZUI_LABEL,
        ZUI_ITEM_TYPE_TEXTURE,
        ZUI_ITEM_TYPE_9SLICE_TEXTURE,
        ZUI_ITEM_TYPE_BUTTON,
        ZUI_ITEM_TYPE_HORIZONTAL_3PATCH_TEXTURE,
        ZUI_ITEM_TYPE_VERTICAL_3PATCH_TEXTURE,
        ZUI_ITEM_TYPE_NUMERIC_INPUT,
        ZUI_ITEM_TYPE_SLIDER,
        ZUI_ITEM_TYPE_KNOB,
    } ZuiItemType;

    typedef enum ZuiAlignment
    {
        ZUI_ALIGN_TOP_LEFT,
        ZUI_ALIGN_TOP_CENTER,
        ZUI_ALIGN_TOP_RIGHT,
        ZUI_ALIGN_MIDDLE_LEFT,
        ZUI_ALIGN_CENTER,
        ZUI_ALIGN_MIDDLE_RIGHT,
        ZUI_ALIGN_BOTTOM_LEFT,
        ZUI_ALIGN_BOTTOM_CENTER,
        ZUI_ALIGN_BOTTOM_RIGHT,
    } ZuiAlignment;

    typedef enum ZuiAlignmentX
    {
        ZUI_ALIGN_X_LEFT,
        ZUI_ALIGN_X_CENTER,
        ZUI_ALIGN_X_RIGHT,
    } ZuiAlignmentX;

    typedef enum ZuiAlignmentY
    {
        ZUI_ALIGN_Y_TOP,
        ZUI_ALIGN_Y_CENTER,
        ZUI_ALIGN_Y_BOTTOM,
    } ZuiAlignmentY;

    typedef enum ZuiPlacement
    {
        ZUI_PLACE_RELATIVE,
        ZUI_PLACE_ABSOLUTE,
        ZUI_PLACE_ALIGN,
        ZUI_PLACE_CURSOR,
    } ZuiPlacement;

    typedef enum ZuiLabelPlacement
    {
        ZUI_LABEL_LEFT,
        ZUI_LABEL_RIGHT,
        ZUI_LABEL_ABOVE,
        ZUI_LABEL_BELOW,
    } ZuiLabelPlacement;

    ///-----------------------------------------------------------Structs

    typedef struct ZuiItemId
    {
        unsigned int value;
    } ZuiItemId;

    typedef void (*ZuiRenderFunction)(void *data);
    typedef void (*ZuiUpdateFunction)(void *data);

    typedef struct ZuiRenderItem
    {
        ZuiRenderFunction function;
        void *data;
    } ZuiRenderItem;

    typedef struct ZuiUpdateItem
    {
        ZuiUpdateFunction function;
        void *data;
    } ZuiUpdateItem;

    typedef struct ZuiItem
    {
        void *data;
        ZuiRenderFunction renderFunction;
        ZuiUpdateFunction updateFunction;
        Vector2 *parentPosition;
        unsigned int parentId;
        ZuiItemType type;
        unsigned int id;
    } ZuiItem;

    ///----------------------------------------------------------- ZuiOption

    typedef struct ZuiOptionInt
    {
        bool has;
        int value;
    } ZuiOptionInt;

    typedef struct ZuiOptionColor
    {
        bool has;
        Color value;
    } ZuiOptionColor;

    typedef struct ZuiOptionString
    {
        bool has;
        const char *value;
    } ZuiOptionString;

    typedef struct ZuiOptionVector2
    {
        bool has;
        Vector2 value;
    } ZuiOptionVector2;

    typedef struct ZuiOptionNPatchInfo
    {
        bool has;
        NPatchInfo value;
    } ZuiOptionNPatchInfo;

    typedef struct ZuiOptionTexture2D
    {
        bool has;
        Texture2D value;
    } ZuiOptionTexture2D;

    typedef struct ZuiOptionRectangle
    {
        bool has;
        Rectangle value;
    } ZuiOptionRectangle;

    typedef struct ZuiOptionFloat
    {
        bool has;
        float value;
    } ZuiOptionFloat;

    typedef struct ZuiOptionFont
    {
        bool has;
        Font value;
    } ZuiOptionFont;

    typedef ZuiOptionInt ZuiOptionAlignment;
    typedef ZuiOptionInt ZuiOptionPlacement;
    typedef ZuiOptionInt ZuiOptionBool;

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value == v;
    */
    ZuiOptionInt ZuiSomeInt(int v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasInt(ZuiOptionInt o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value.r == v.r && \result.value.g == v.g &&
              \result.value.b == v.b && \result.value.a == v.a;
    */
    ZuiOptionColor ZuiSomeColor(Color v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasColor(ZuiOptionColor o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value == v;
    */
    ZuiOptionString ZuiSomeString(const char *v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasString(ZuiOptionString o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value.x == v.x && \result.value.y == v.y;
    */
    ZuiOptionVector2 ZuiSomeVector2(Vector2 v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasVector2(ZuiOptionVector2 o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value.source == v.source && \result.value.left == v.left &&
              \result.value.top == v.top && \result.value.right == v.right &&
              \result.value.bottom == v.bottom && \result.value.layout == v.layout;
    */
    ZuiOptionNPatchInfo ZuiSomeNPatchInfo(NPatchInfo v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasNPatchInfo(ZuiOptionNPatchInfo o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value.id == v.id && \result.value.width == v.width &&
              \result.value.height == v.height && \result.value.mipmaps == v.mipmaps &&
              \result.value.format == v.format;
    */
    ZuiOptionTexture2D ZuiSomeTexture2D(Texture2D v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasTexture2D(ZuiOptionTexture2D o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value.x == v.x && \result.value.y == v.y &&
              \result.value.width == v.width && \result.value.height == v.height;
    */
    ZuiOptionRectangle ZuiSomeRectangle(Rectangle v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasRectangle(ZuiOptionRectangle o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value == v;
    */
    ZuiOptionFloat ZuiSomeFloat(float v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasFloat(ZuiOptionFloat o);

    /*@
      assigns \result;
      ensures \result.has == \true;
      ensures \result.value.baseSize == v.baseSize && \result.value.charsCount == v.charsCount &&
              \result.value.charsPadding == v.charsPadding && \result.value.texture.id == v.texture.id &&
              \result.value.recs == v.recs && \result.value.glyphs == v.glyphs;
    */
    ZuiOptionFont ZuiSomeFont(Font v);

    /*@
      assigns \result;
      ensures \result == o.has;
    */
    bool ZuiHasFont(ZuiOptionFont o);

#define ZUI_SOME_EVAL(x) _Generic((x), \
    int: ZuiSomeInt,                   \
    float: ZuiSomeFloat,               \
    char *: ZuiSomeString,             \
    const char *: ZuiSomeString,       \
    Color: ZuiSomeColor,               \
    Vector2: ZuiSomeVector2,           \
    Rectangle: ZuiSomeRectangle,       \
    Texture2D: ZuiSomeTexture2D,       \
    NPatchInfo: ZuiSomeNPatchInfo,     \
    Font: ZuiSomeFont)(x)

#define zui_some(...) ZUI_SOME_EVAL((__VA_ARGS__))

#define zui_has(...) _Generic(__VA_ARGS__, \
    ZuiOptionInt: ZuiHasInt,               \
    ZuiOptionFloat: ZuiHasFloat,           \
    ZuiOptionString: ZuiHasString,         \
    ZuiOptionColor: ZuiHasColor,           \
    ZuiOptionVector2: ZuiHasVector2,       \
    ZuiOptionRectangle: ZuiHasRectangle,   \
    ZuiOptionTexture2D: ZuiHasTexture2D,   \
    ZuiOptionNPatchInfo: ZuiHasNPatchInfo, \
    ZuiOptionFont: ZuiHasFont)(__VA_ARGS__)

    ///--------------------------------------------------------Utility Functions
    /*@
      requires n >= 0;
      assigns \result \from n;
      ensures \result == \false || \result == \true;
      ensures \result == \true <==> (n > 0 && (n & (n-1)) == 0);
      ensures n == 0 ==> \result == \false;
    */
    bool ZuiIsPowerOfTwo(size_t n);

    /*@
      assigns \result \from type;
      ensures \valid_read(\result);
    */
    const char *ZuiGetWidgetType(ZuiItemType type);

    /*@
      assigns \result \from result;
      ensures \valid_read(\result);
    */
    const char *ZuiResultToString(ZuiResult result);

    float ZuiPixelsToRoundness(Rectangle rect, float radius_px);

    // -----------------------------------------------------------------------------

    ///-----------------------------------------------------------ZuiArena

    typedef struct ZuiArena
    {
        unsigned char *buffer;
        size_t offset;
        size_t capacity;
    } ZuiArena;

    typedef struct ZuiArenaStats
    {
        size_t totalCapacity;
        size_t usedBytes;
        size_t availableBytes;
        float usagePercent;
    } ZuiArenaStats;

/// Allocate raw bytes with default alignment (preferred over function)
#define ZUI_ARENA_ALLOC(arena, size) \
    ZuiAllocArenaDefault(arena, size)

/// Allocate a single instance of a type from the arena
#define ZUI_ARENA_ALLOC_TYPE(arena, type) \
    ((type *)ZuiAllocArena(arena, sizeof(type), ZUI_ALIGNOF(type)))

/// Allocate an array of a type from the arena
#define ZUI_ARENA_ALLOC_ARRAY(arena, type, count) \
    ((type *)ZuiAllocArena(arena, sizeof(type) * (count), ZUI_ALIGNOF(type)))

    ///---------------------------------------------------------Arena

    /*@
      requires \valid(arena);
      requires capacity > 0 && capacity <= SIZE_MAX / 2;
      assigns *arena;
      ensures \result == ZUI_OK ==> \valid(arena->buffer + (0 .. capacity-1));
      ensures \result == ZUI_OK ==> arena->capacity == capacity;
      ensures \result == ZUI_OK ==> arena->offset == 0;
    */
    ZuiResult ZuiInitArena(ZuiArena *arena, size_t capacity);

    /*@
      assigns arena->buffer, arena->offset, arena->capacity;
      ensures arena->buffer == \null && arena->offset == 0 && arena->capacity == 0;
    */
    void ZuiUnloadArena(ZuiArena *arena);

    /*@
      requires \valid(arena);
      assigns arena->offset;
      ensures arena->offset == 0;
    */
    void ZuiResetArena(ZuiArena *arena);

    /*@
      lemma pointer_alignment_from_offset:
        \forall unsigned char* base; \forall integer offset, align;
          align > 0 && (align & (align-1)) == 0 ==>
          offset % align == 0 ==>
          ((uintptr_t)(base + offset) % align) == 0;
    */
    /*@
      requires \valid(arena);
      requires arena->buffer != \null && arena->offset <= arena->capacity;
      requires size > 0;
      requires alignment > 0 && (alignment & (alignment - 1)) == 0;
      requires alignment <= arena->capacity;
      assigns arena->offset, \result;
      behavior success:
        assumes arena->offset + size <= arena->capacity;
        ensures \valid(((char*)\result)+(0 .. size-1));
        ensures \result == \null || ((uintptr_t)\result % alignment) == 0;
        ensures arena->offset == \old(arena->offset) + size;
      behavior failure:
        assumes arena->offset + size > arena->capacity;
        ensures \result == \null;
      complete behaviors;
      disjoint behaviors;
    */
    void *ZuiAllocArena(ZuiArena *arena, size_t size, size_t alignment);

    /*@
      requires \valid(arena);
      requires arena->buffer != \null && arena->offset <= arena->capacity;
      requires size > 0;
      assigns \result;
      ensures \valid(((char*)\result)+(0 .. size-1)) || \result == \null;
    */
    void *ZuiAllocArenaDefault(ZuiArena *arena, size_t size);

    /*@
      requires \valid(arena);
      assigns \result \from *arena;
      ensures \result.totalCapacity == arena->capacity;
      ensures \result.usedBytes == arena->offset;
      ensures \result.availableBytes ==
              (arena->capacity >= arena->offset)
                ? (arena->capacity - arena->offset)
                : 0;
    */
    ZuiArenaStats ZuiGetArenaStats(const ZuiArena *arena);

    /*@
      requires \valid(arena);
      assigns \nothing;
    */
    void ZuiPrintArenaStats(const ZuiArena *arena);

    /*@
      assigns \result \from arena;
    */
    bool ZuiIsArenaValid(const ZuiArena *arena);

    // -----------------------------------------------------------------------------

    ///-----------------------------------------------------------ZuiDynArray Dynamic array

    typedef struct ZuiDynArray
    {
        void *items;
        unsigned int count;
        unsigned int capacity;
        size_t itemSize;
        size_t itemAlignment;
        const char *typeName;
    } ZuiDynArray;

    typedef struct ZuiDynArrayStats
    {
        unsigned int count;
        unsigned int capacity;
        float usagePercent;
        size_t memoryUsed;
    } ZuiDynArrayStats;

    ///-----------------------------------------------------------ACSL Logic Predicates

    /*@
      predicate ZuiIsDynArrayValid_logic(ZuiDynArray *array) =
        array != \null &&
        array->items != \null &&
        array->count <= array->capacity &&
        array->itemSize > 0 &&
        array->capacity > 0;
    */

#define ZUI_DYNARRAY_PUSH_TYPE(array, arena, type) \
    ((type *)ZuiPushDynArray(array, arena))

#define ZUI_DYNARRAY_GET_TYPE(array, type, index) \
    ((type *)ZuiGetDynArray(array, index))

    ///----------------------------------------------------Dynamic Array

    /*@
      requires \valid(array);
      requires \valid(arena);
      requires initialCapacity > 0 && initialCapacity <= ZUI_MAX_DYNARRAY_CAPACITY;
      requires itemSize > 0;
      requires ZuiIsPowerOfTwo_logic(itemAlignment);
      requires typeName != \null;

      assigns array->items, array->count, array->capacity, array->itemSize,
              array->itemAlignment, array->typeName;

      ensures \result == ZUI_OK <==>
              array->items != \null &&
              array->count == 0 &&
              array->capacity == initialCapacity &&
              array->itemSize == itemSize &&
              array->itemAlignment == itemAlignment &&
              array->typeName == typeName &&
              \valid((char*)array->items + (0..itemSize*initialCapacity-1));
    */
    ZuiResult ZuiInitDynArray(ZuiDynArray *array, ZuiArena *arena, unsigned int initialCapacity,
                              size_t itemSize, size_t itemAlignment, const char *typeName);

    /*@
      requires \valid(array);
      requires \valid(arena);
      requires ZuiIsDynArrayValid_logic(array);

      assigns array->items, array->capacity;

      ensures ZuiIsDynArrayValid_logic(array);
      ensures array->capacity == 2 * \old(array->capacity);
      ensures array->count == \old(array->count);
      ensures \result == ZUI_OK ==> \valid((char*)array->items + (0..array->itemSize*array->capacity-1));
    */
    ZuiResult ZuiGrowDynArray(ZuiDynArray *array, ZuiArena *arena);

    /*@
      requires \valid(array);
      requires \valid(arena);
      requires ZuiIsDynArrayValid_logic(array);

      assigns array->count, array->items;

      ensures \result != \null;
      ensures \valid((char*)\result + (0..array->itemSize-1));
      ensures \result == (char*)array->items + (\old(array->count) * array->itemSize);
      ensures array->count == \old(array->count) + 1;
    */
    void *ZuiPushDynArray(ZuiDynArray *array, ZuiArena *arena);

    /*@
      requires \valid_read(array);
      requires ZuiIsDynArrayValid_logic(array);
      requires index < array->count;

      ensures \result != \null;
      ensures \valid_read((char*)\result + (0..array->itemSize-1));
      ensures \result == (char*)array->items + (index * array->itemSize);
    */
    void *ZuiGetDynArray(const ZuiDynArray *array, unsigned int index);

    /*@
      requires \valid(array);
      assigns array->count;
      ensures array->count == 0;
    */
    void ZuiClearDynArray(ZuiDynArray *array);

    /*@
      requires array != \null;
      ensures \result <==> (
              array->items != \null &&
              array->count <= array->capacity &&
              array->itemSize > 0 &&
              array->capacity > 0
      );
    */
    bool ZuiIsDynArrayValid(const ZuiDynArray *array);

    /*@
      requires \valid(array);
      ensures \result.count == array->count;
      ensures \result.capacity == array->capacity;
      ensures \result.memoryUsed == array->itemSize * array->capacity;
    */
    ZuiDynArrayStats ZuiGetDynArrayStats(const ZuiDynArray *array);

    void ZuiPrintDynArrayStats(const ZuiDynArray *array);

    ///----------------------------------------------------------- ZuiLabel

    typedef struct ZuiLabelId
    {
        unsigned int value;
    } ZuiLabelId;

    typedef struct ZuiLabelData
    {
        char text[ZUI_MAX_TEXT_LENGTH];
        Font font;
        Rectangle bounds;
        Color textColor;
        Color backgroundColor;
        float spacing;
        float fontSize;
        bool hasBackground;
    } ZuiLabelData;

    typedef struct ZuiLabelPreset
    {
        ZuiOptionString text;
        ZuiOptionFont font;
        ZuiOptionRectangle bounds;
        ZuiOptionColor textColor;
        ZuiOptionColor backgroundColor;
        ZuiOptionFloat spacing;
        ZuiOptionFloat fontSize;
        ZuiOptionBool hasBackground;
    } ZuiLabelPreset;

    typedef struct ZuiLabel
    {
        ZuiLabelData data;
        Vector2 parent;
        ZuiLabelId id;
        bool isVisible;
        bool isInsideWindow;
    } ZuiLabel;
    ///-----------------------------------------------------Label
    ZuiResult ZuiInitLabel(ZuiDynArray *array, ZuiArena *arena, unsigned int initialCapacity);
    ZuiLabel *ZuiPushLabel(ZuiDynArray *array, ZuiArena *arena);
    ZuiLabel *ZuiGetLabel(const ZuiDynArray *array, unsigned int index);
    Vector2 ZuiGetLabelSize(ZuiLabel const *label);
    void ZuiClearLabel(ZuiDynArray *array);
    unsigned int ZuiCountLabel(ZuiDynArray const *array);
    void ZuiRenderLabel(void *item);
    void ZuiConfigureLabel(ZuiLabel *label, const ZuiLabelPreset *preset);
    ZuiLabelPreset ZuiGetDefaultLabelPreset(void);
    ZuiLabelId ZuiCreateLabel(const char *text, ZuiDynArray *array, Font font, ZuiArena *arena);
    bool ZuiValidateBounds(Rectangle bounds);
    bool ZuiValidateLabelId(const ZuiDynArray *array, ZuiLabelId id);
    bool ZuiValidateSetLabelText(ZuiLabelId id, const char *text, const ZuiDynArray *array);
    bool ZuiValidateSetLabelFont(ZuiLabelId id, Font font, const ZuiDynArray *array);
    bool ZuiValidateSetLabelBounds(ZuiLabelId id, Rectangle bounds, const ZuiDynArray *array);
    void ZuiInternalPrintLabel(ZuiLabel *label);

    ///-----------------------------------------------------------ZuiFrame

    typedef struct ZuiFrameId
    {
        unsigned int value;
    } ZuiFrameId;

    typedef struct ZuiFrameData
    {
        Color backgroundColor;
        Color outlineColor;
        Rectangle bounds;
        float padding;
        float gap;
        float outlineThickness;
        bool hasBackground;
        bool hasOutline;
    } ZuiFrameData;

    typedef struct ZuiFramePreset
    {
        ZuiOptionColor backgroundColor;
        ZuiOptionColor outlineColor;
        ZuiOptionRectangle bounds;
        ZuiOptionFloat padding;
        ZuiOptionFloat gap;
        ZuiOptionFloat outlineThickness;
        ZuiOptionBool hasBackground;
        ZuiOptionBool hasOutline;
    } ZuiFramePreset;

    typedef struct ZuiFrame
    {
        ZuiItem *items;
        unsigned int itemsCapacity;
        ZuiFrameData data;
        ZuiFrameId id;
        ZuiFrameId parentId;
        int itemCounter;
        Vector2 parent;
        bool isVisible;
        bool isInsideWindow;
    } ZuiFrame;

    typedef struct ZuiPadding
    {
        float left;
        float right;
        float top;
        float bottom;
    } ZuiPadding;

    typedef struct ZuiItemOptions
    {
        Vector2 cursor;
        ZuiPlacement placement;
        ZuiAlignment alignment;
        ZuiItemType type;
        Vector2 *parent;
        unsigned int id;
        bool isWindowFrame;
    } ZuiItemOptions;

    ///------------------------------------------------------------------------------- Frame

    ZuiResult ZuiInitFrame(ZuiDynArray *array, ZuiArena *arena, unsigned int initialCapacity);
    ZuiFrame *ZuiPushFrame(ZuiDynArray *array, ZuiArena *arena);
    ZuiFrame *ZuiGetFrame(const ZuiDynArray *array, unsigned int index);
    void ZuiClearFrame(ZuiDynArray *array);
    unsigned int ZuiGetFrameCount(ZuiDynArray const *array);
    ZuiResult ZuiInitFrameItems(ZuiFrame *frame, ZuiArena *arena, size_t initialCapacity);
    ZuiResult ZuiGrowFrameItems(ZuiFrame *frame, ZuiArena *arena);
    ZuiResult ZuiAddFrameItem(ZuiFrame *frame, void *data, ZuiRenderFunction render_func,
                              ZuiUpdateFunction update_func, ZuiItemOptions options, ZuiArena *arena);
    void ZuiUpdateFrame(void *itemFrame);

    void ZuiRenderFrame(void *itemFrame);
    void ZuiRenderFrameItem(void *item);
    bool ZuiIsChildAncestor(const ZuiFrame *potentialParent, const ZuiFrame *frame);
    void ZuiUpdateFrameItem(void *item);
    Rectangle ZuiGetContentArea(Rectangle parent, ZuiPadding padding);
    Rectangle ZuiAlignRectangle(Rectangle parent, Vector2 childSize, ZuiPadding padding, ZuiAlignment alignment);
    Rectangle ZuiGetBoundsFromVectors(Vector2 position, Vector2 size);
    Vector2 ZuiGetPositionFromBounds(Rectangle bounds);
    Vector2 ZuiGetSizeFromBounds(Rectangle bounds);
    Rectangle ZuiCalculateChildBounds(Rectangle childBounds, Rectangle bounds, ZuiItemOptions options);
    Vector2 ZuiCalculateChildPosition(Vector2 position, Vector2 size, Rectangle bounds, ZuiItemOptions options);
    const char *ZuiGetAlignmentName(ZuiAlignment alignment);
    ZuiFrameId ZuiCreateFrame(Rectangle bounds, Color color, ZuiDynArray *frameArray, ZuiArena *arena);
    bool ZuiValidateFrameId(const ZuiDynArray *array, ZuiFrameId id);
    bool ZuiValidateFrameBounds(Rectangle bounds);
    bool ZuiValidateSetFrameBounds(ZuiFrameId id, Rectangle bounds, const ZuiDynArray *array);
    bool ZuiValidateFramePadding(float padding);
    bool ZuiValidateFrameSpacing(float spacing);
    bool ZuiValidateFrameOutlineThickness(float thickness);
    void ZuiInternalPrintFrame(const ZuiFrame *frame);
    bool ZuiIsChildAncestorIterative(const ZuiFrame *potential_parent, const ZuiFrame *frame);
    // -----------------------------------------------------------------------------

    ///----------------------------------------------------------------Cursor

    typedef struct ZuiCursor
    {
        Vector2 position;
        Vector2 restPosition;
        Vector2 tempRestPosition;
        Rectangle lastItemBounds;
        ZuiFrameId root;
        ZuiFrameId activeFrame;
        ZuiFrameId parentFrame;
        ZuiLabelId activeLabel;
        int rowStartIndex;
        int rowEndIndex;
        bool isWindowRoot;
        bool isRow;
    } ZuiCursor;

    ///----------------------------------------------------------------Context

    typedef struct ZuiContext
    {
        ZuiCursor cursor;
        ZuiDynArray frameArray;
        ZuiDynArray labelArray;

        Font font;
        Font monoFont;
        int dpiScale;
    } ZuiContext;

#ifdef __cplusplus
    extern "C"
    {
#endif

        extern ZuiContext *g_zui_ctx;
        extern ZuiArena g_zui_arena;

#ifdef __cplusplus
    }
#endif

    ///----------------------------------------------------------- Cursor

    void ZuiAdvanceCursor(float width, float height);
    void ZuiPrintCursor(void);
    void ZuiAdvanceLine(void);
    void ZuiPlaceAt(float x, float y);
    void ZuiOffset(float x, float y);
    void ZuiBeginRow(void);
    void ZuiEndRow(void);

    ///----------------------------------------------------------- Context

    bool ZuiIsInitialized(void);
    ZuiContext *ZuiGetContext(void);
    void ZuiUpdate(void);
    void ZuiRender(void);
    bool ZuiInit(void);
    void ZuiExit(void);

    ///-----------------------------------------------------Label
    void ZuiPrintLabel(ZuiLabelId id);
    void ZuiSetLabelText(ZuiLabelId id, const char *text);
    void ZuiSetLabelFont(ZuiLabelId id, Font font);
    void ZuiSetLabelTextColor(ZuiLabelId id, Color textColor);
    void ZuiSetLabelBackgroundColor(ZuiLabelId id, Color backgroundColor);
    void ZuiSetLabelBounds(ZuiLabelId id, Rectangle bounds);
    void ZuiSetLabelSpacing(ZuiLabelId id, unsigned int spacing);
    void ZuiSetLabelFontSize(ZuiLabelId id, unsigned int fontSize);
    void ZuiSetLabelHasBackground(ZuiLabelId id, bool hasBackground);
    void ZuiSetLabelIsVisible(ZuiLabelId id, bool isVisible);

    const char *ZuiGetLabelText(ZuiLabelId id);
    Font ZuiGetLabelFont(ZuiLabelId id);
    Color ZuiGetLabelTextColor(ZuiLabelId id);
    Rectangle ZuiGetLabelBounds(ZuiLabelId id);
    float ZuiGetLabelSpacing(ZuiLabelId id);
    float ZuiGetLabelFontSize(ZuiLabelId id);
    bool ZuiGetLabelIsVisible(ZuiLabelId id);

    ///------------------------------------------------------------------------------- Frame

    ZuiResult ZuiAddChildFrame(ZuiFrameId parentId, ZuiFrameId childId, ZuiItemOptions options);
    ZuiResult ZuiAddLabel(ZuiFrameId parentId, ZuiLabelId childId, ZuiItemOptions options, ZuiArena *arena);
    Rectangle *ZuiGetItemBoundsFromFrames(const ZuiItem *item);
    void ZuiAlignHorizontalItem(ZuiFrame *frame, int startIndex, int endIndex, ZuiAlignmentY alignment);
    void ZuiSetFrameBounds(ZuiFrameId id, Rectangle bounds);
    void ZuiSetFrameBackgroundColor(ZuiFrameId id, Color color);
    void ZuiSetFrameOutlineColor(ZuiFrameId id, Color color);
    void ZuiSetFramePadding(ZuiFrameId id, float padding);
    void ZuiSetFrameGap(ZuiFrameId id, float gap);
    void ZuiSetFrameOutlineThickness(ZuiFrameId id, float thickness);
    void ZuiSetFrameHasBackground(ZuiFrameId id, bool hasBackground);
    void ZuiSetFrameHasOutline(ZuiFrameId id, bool hasOutline);
    void ZuiSetFrameIsVisible(ZuiFrameId id, bool isVisible);
    void ZuiSetFrameParentId(ZuiFrameId id, ZuiFrameId parentId);
    Rectangle ZuiGetFrameBounds(ZuiFrameId id);
    Color ZuiGetFrameBackgroundColor(ZuiFrameId id);
    Color ZuiGetFrameOutlineColor(ZuiFrameId id);
    float ZuiGetFramePadding(ZuiFrameId id);
    float ZuiGetFrameSpacing(ZuiFrameId id);
    float ZuiGetFrameOutlineThickness(ZuiFrameId id);
    bool ZuiGetFrameHasBackground(ZuiFrameId id);
    bool ZuiGetFrameHasOutline(ZuiFrameId id);
    bool ZuiGetFrameIsVisible(ZuiFrameId id);
    int ZuiGetFrameItemCount(ZuiFrameId id);
    unsigned int ZuiGetFrameItemsCapacity(ZuiFrameId id);
    Vector2 ZuiGetFrameParentPosition(ZuiFrameId id);
    ZuiFrameId ZuiGetFrameParentId(ZuiFrameId id);

    ///------------------------------------------------------------ API

    ZuiFrameId ZuiBeginFrame(Rectangle bounds, Color color);
    void ZuiEndFrame(void);
    ZuiFrameId ZuiNewFrame(Color color, float width, float height);
    ZuiLabelId ZuiLabelEx(const char *text, bool isMono);
    ZuiLabelId ZuiNewMonoLabel(const char *text);
    ZuiLabelId ZuiNewLabel(const char *text);
    void ZuiFrameBackground(Color color);
    void ZuiFrameOutline(Color color, float thickness);
    void ZuiFrameGap(float gap);
    void ZuiFramePad(float pad);
    void ZuiFrameOffset(float x, float y);
    void ZuiLabelColor(Color color);
    void ZuiLabelBackgroundColor(Color backgroundColor);
    void ZuiLabelAlignX(ZuiAlignmentX align);
    void ZuiLabelOffset(float x, float y);
    // -----------------------------------------------------------------------------

#ifdef ZUI_IMPLEMENTATION

    // ----------------------------------------------------------------------------Api
    void ZuiAdvanceCursor(const float width, const float height)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        // Validate dimensions
        if (width < 0.0F || height < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE,
                             "Negative dimensions (width=%.2F, height=%.2F)",
                             (double)width, (double)height);
            return;
        }

        // Get active frame with error checking
        ZuiFrame *activeFrame = ZuiGetFrame(&g_zui_ctx->frameArray,
                                            g_zui_ctx->cursor.activeFrame.value);
        if (!activeFrame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active frame ID %u",
                             g_zui_ctx->cursor.activeFrame.value);
            return;
        }

        // Advance cursor position
        Vector2 size = (Vector2){width, height};
        if (g_zui_ctx->cursor.isRow)
        {
            g_zui_ctx->cursor.position.x += size.x + activeFrame->data.gap;
        }
        else
        {
            g_zui_ctx->cursor.position.x = g_zui_ctx->cursor.tempRestPosition.x;
            g_zui_ctx->cursor.position.y += size.y + activeFrame->data.gap;
        }
    }

    void ZuiPrintCursor(void)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        TraceLog(LOG_INFO, "[ZUI] Cursor position: x=%.1F, y=%.1F, activeFrame=%u",
                 (double)g_zui_ctx->cursor.position.x,
                 (double)g_zui_ctx->cursor.position.y,
                 g_zui_ctx->cursor.activeFrame.value);
    }

    void ZuiAdvanceLine(void)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        // Get active frame with error checking
        ZuiFrame *activeFrame = ZuiGetFrame(&g_zui_ctx->frameArray,
                                            g_zui_ctx->cursor.activeFrame.value);
        if (!activeFrame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active frame ID %u",
                             g_zui_ctx->cursor.activeFrame.value);
            return;
        }

        // Advance to next line
        g_zui_ctx->cursor.position.x = g_zui_ctx->cursor.restPosition.x;
        g_zui_ctx->cursor.tempRestPosition.x = g_zui_ctx->cursor.position.x;
        g_zui_ctx->cursor.position.y += g_zui_ctx->cursor.lastItemBounds.height +
                                        activeFrame->data.gap;
        g_zui_ctx->cursor.tempRestPosition.y = g_zui_ctx->cursor.position.y;
    }

    void ZuiPlaceAt(const float x, const float y)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        g_zui_ctx->cursor.position = (Vector2){x, y};
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }

    void ZuiOffset(const float x, const float y)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        g_zui_ctx->cursor.position.x += x;
        g_zui_ctx->cursor.position.y += y;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }

    void ZuiBeginRow(void)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        g_zui_ctx->cursor.isRow = true;
    }

    void ZuiEndRow(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        g_zui_ctx->cursor.isRow = false;
        ZuiAdvanceLine();
    }

    ZuiFrameId ZuiBeginFrame(const Rectangle bounds, const Color color)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return (ZuiFrameId){ZUI_ID_INVALID};
        }

        g_zui_ctx->cursor.isRow = false;
        ZuiFrameId id = ZuiCreateFrame(bounds, color, &g_zui_ctx->frameArray, &g_zui_arena);

        if (id.value == 0 || id.value >= g_zui_ctx->frameArray.count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to create frame");
            return (ZuiFrameId){ZUI_ID_INVALID};
        }

        g_zui_ctx->cursor.parentFrame = g_zui_ctx->cursor.activeFrame;
        float pad = ZuiGetFramePadding(id);
        g_zui_ctx->cursor.position = (Vector2){bounds.x + pad, bounds.y + pad};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.activeFrame = id;

        // zui_Frame *frame = zui_get_frame(&g_zui_ctx->frameArray, id.value);
        // ZuiInternalPrintFrame(frame);
        return id;
    }

    void ZuiEndFrame(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        g_zui_ctx->cursor.isRow = false;
        g_zui_ctx->cursor.activeFrame = g_zui_ctx->cursor.parentFrame;
    }

    ZuiFrameId ZuiNewFrame(const Color color, const float width, const float height)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return (ZuiFrameId){ZUI_ID_INVALID};
        }

        Rectangle bounds = (Rectangle){g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, width, height};
        ZuiFrameId id = ZuiCreateFrame(bounds, color, &g_zui_ctx->frameArray, &g_zui_arena);
        ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, id.value);
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to get frame");
            return (ZuiFrameId){ZUI_ID_INVALID};
        }

        frame->data.bounds = bounds;
        ZuiItemOptions options = (ZuiItemOptions){
            .placement = ZUI_PLACE_CURSOR,
            .cursor = g_zui_ctx->cursor.position,
            .isWindowFrame = g_zui_ctx->cursor.isWindowRoot,
            .id = id.value,
            .parent = &frame->parent,
            .type = ZUI_FRAME,
        };

        ZuiFrameId active = g_zui_ctx->cursor.activeFrame;
        ZuiAddChildFrame(active, id, options);

        ZuiAdvanceCursor(frame->data.bounds.width, frame->data.bounds.height);
        g_zui_ctx->cursor.lastItemBounds = frame->data.bounds;

        return id;
    }

    ZuiLabelId ZuiLabelEx(const char *text, const bool isMono)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return (ZuiLabelId){0};
        }

        if (!text)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Label text is NULL");
            return (ZuiLabelId){0};
        }

        ZuiLabelId id = ZuiCreateLabel(
            text,
            &g_zui_ctx->labelArray,
            isMono ? g_zui_ctx->monoFont : g_zui_ctx->font,
            &g_zui_arena);
        ZuiLabel *label = ZuiGetLabel(&g_zui_ctx->labelArray, id.value);
        if (!label)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to get label");
            return (ZuiLabelId){0};
        }
        ZuiItemOptions options = (ZuiItemOptions){
            .placement = ZUI_PLACE_CURSOR,
            .cursor = g_zui_ctx->cursor.position,
            .isWindowFrame = g_zui_ctx->cursor.isWindowRoot,
            .id = id.value,
            .parent = &label->parent,
            .type = ZUI_LABEL,
        };

        label->isInsideWindow = options.isWindowFrame;

        Rectangle bounds = (Rectangle){
            g_zui_ctx->cursor.position.x, //(options.isWindowFrame ? frame->data.bounds.x : 0.0F),
            g_zui_ctx->cursor.position.y, //(options.isWindowFrame ? frame->data.bounds.y : 0.0F),
            label->data.bounds.width,
            label->data.bounds.height};
        label->data.bounds = bounds;

        ZuiFrameId active = g_zui_ctx->cursor.activeFrame;
        ZuiAddLabel(active, id, options, &g_zui_arena);
        ZuiAdvanceCursor(label->data.bounds.width, label->data.bounds.height);
        g_zui_ctx->cursor.activeLabel = id;
        g_zui_ctx->cursor.lastItemBounds = label->data.bounds;
        return id;
    }

    ZuiLabelId ZuiNewMonoLabel(const char *text)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return (ZuiLabelId){0};
        }

        if (!text)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Text pointer is NULL");
            return (ZuiLabelId){0};
        }

        return ZuiLabelEx(text, true);
    }

    ZuiLabelId ZuiNewLabel(const char *text)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return (ZuiLabelId){0};
        }

        if (!text)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Text pointer is NULL");
            return (ZuiLabelId){0};
        }

        return ZuiLabelEx(text, false);
    }

    void ZuiFrameBackground(const Color color)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiFrameId id = g_zui_ctx->cursor.activeFrame;
        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active frame ID %u", id.value);
            return;
        }

        ZuiSetFrameBackgroundColor(id, color);
        ZuiSetFrameHasBackground(id, color.a > 0);
    }

    void ZuiFrameOutline(const Color color, const float thickness)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        if (thickness < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE,
                             "Negative thickness: %.2F", (double)thickness);
            return;
        }

        ZuiFrameId id = g_zui_ctx->cursor.activeFrame;
        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active frame ID %u", id.value);
            return;
        }

        ZuiSetFrameOutlineColor(id, color);
        ZuiSetFrameOutlineThickness(id, thickness);
        ZuiSetFrameHasOutline(id, color.a > 0);
    }

    void ZuiFrameGap(const float gap)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        if (gap < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE,
                             "Negative gap: %.2F", (double)gap);
            return;
        }

        ZuiFrameId id = g_zui_ctx->cursor.activeFrame;
        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active frame ID %u", id.value);
            return;
        }

        ZuiSetFrameGap(id, gap);
    }

    void ZuiFramePad(const float pad)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        if (pad < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Negative pad: %.2F", (double)pad);
            return;
        }

        ZuiFrameId id = g_zui_ctx->cursor.activeFrame;
        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid active frame ID %u", id.value);
            return;
        }

        ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, id.value);
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame pointer is NULL");
            return;
        }

        ZuiSetFramePadding(id, pad);

        // Safe cursor update
        Rectangle bounds = frame->data.bounds;
        g_zui_ctx->cursor.position = (Vector2){bounds.x + pad, bounds.y + pad};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }
    void ZuiFrameOffset(const float x, const float y)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiFrameId frame_id = g_zui_ctx->cursor.activeFrame;
        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, frame_id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active frame ID %u", frame_id.value);
            return;
        }

        Rectangle frame_bounds = ZuiGetFrameBounds(frame_id);
        frame_bounds.x += x;
        frame_bounds.y += y;
        ZuiSetFrameBounds(frame_id, frame_bounds);
    }

    void ZuiLabelColor(const Color color)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelId id = g_zui_ctx->cursor.activeLabel;
        if (!ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active label ID %u", id.value);
            return;
        }

        ZuiSetLabelTextColor(id, color);
    }

    void ZuiLabelBackgroundColor(const Color backgroundColor)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelId id = g_zui_ctx->cursor.activeLabel;
        if (!ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active label ID %u", id.value);
            return;
        }

        ZuiSetLabelBackgroundColor(id, backgroundColor);
        ZuiSetLabelHasBackground(id, backgroundColor.a > 0);
    }

    void ZuiLabelAlignX(const ZuiAlignmentX align)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelId label_id = g_zui_ctx->cursor.activeLabel;
        ZuiFrameId frame_id = g_zui_ctx->cursor.activeFrame;

        // Validate label and frame
        if (!ZuiValidateLabelId(&g_zui_ctx->labelArray, label_id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active label ID %u", label_id.value);
            return;
        }

        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, frame_id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active frame ID %u", frame_id.value);
            return;
        }

        Rectangle label_bounds = ZuiGetLabelBounds(label_id);
        Rectangle frame_bounds = ZuiGetFrameBounds(frame_id);
        float pad = ZuiGetFramePadding(frame_id);

        switch (align)
        {
        case ZUI_ALIGN_X_CENTER:
            label_bounds.x = frame_bounds.x + (frame_bounds.width * 0.5F) - (label_bounds.width * 0.5F);
            break;
        case ZUI_ALIGN_X_LEFT:
            label_bounds.x = frame_bounds.x + pad;
            break;
        case ZUI_ALIGN_X_RIGHT:
            label_bounds.x = frame_bounds.x + frame_bounds.width - pad - label_bounds.width;
            break;
        }

        g_zui_ctx->cursor.lastItemBounds.x = label_bounds.x;
        ZuiSetLabelBounds(label_id, label_bounds);
    }

    void ZuiLabelOffset(const float x, const float y)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelId label_id = g_zui_ctx->cursor.activeLabel;
        if (!ZuiValidateLabelId(&g_zui_ctx->labelArray, label_id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid active label ID %u", label_id.value);
            return;
        }

        Rectangle label_bounds = ZuiGetLabelBounds(label_id);
        label_bounds.x += x;
        label_bounds.y += y;

        g_zui_ctx->cursor.position.x += x;
        g_zui_ctx->cursor.position.y += y;

        ZuiSetLabelBounds(label_id, label_bounds);
    }

    ///------------------------------------------------------------Arena

    ZuiResult ZuiInitArena(ZuiArena *arena, const size_t capacity)
    {
        // Preconditions
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        // Initialize to safe state FIRST (prevents use of uninitialized arena)
        arena->buffer = NULL;
        arena->offset = 0;
        arena->capacity = 0;

        // Validate capacity
        if (capacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY,
                             "Capacity must be > 0, got %zu", capacity);
            return ZUI_ERROR_INVALID_CAPACITY;
        }

        // Sanity check for unreasonable sizes
        if (capacity > SIZE_MAX / 2)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY,
                             "Capacity %zu exceeds reasonable limits", capacity);
            return ZUI_ERROR_INVALID_CAPACITY;
        }

        // Attempt allocation
        arena->buffer = (unsigned char *)malloc(capacity);
        // malloc fail
        if (!arena->buffer)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY,
                             "Failed to allocate %zu bytes from system", capacity);
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        // Success - set final state
        arena->capacity = capacity;
        arena->offset = 0;

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: Arena initialized with %zu bytes capacity", capacity);
#endif

        return ZUI_OK;
    }

    void ZuiUnloadArena(ZuiArena *arena)
    {
        if (!arena)
        {
            return;
        }

        if (arena->buffer != NULL)
        {
            free(arena->buffer);
            arena->buffer = NULL;
        }
        arena->offset = 0;
        arena->capacity = 0;
    }

    void *ZuiAllocArena(ZuiArena *arena, size_t size, const size_t alignment)
    {
        // Validate parameters
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return NULL;
        }

        if (!ZuiIsPowerOfTwo(alignment))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ALIGNMENT,
                             "Alignment %zu must be a power of two", alignment);
            return NULL;
        }

        if (!ZuiIsArenaValid(arena))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_OBJECT_STATE, "Arena is in invalid state");
            return NULL;
        }

        // Explicit check for arena buffer (helps analyzers)
        if (arena->buffer == NULL)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena buffer is NULL");
            return NULL;
        }

        if (alignment > arena->capacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE,
                             "Alignment %zu exceeds arena capacity %zu",
                             alignment, arena->capacity);
            return NULL;
        }

        /* Zero-size allocation returns current position without advancing */
        if (size == 0)
        {
            return arena->buffer + arena->offset;
        }

        /* Check for potential overflow in size + padding */
        if (size > SIZE_MAX - alignment)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Allocation size + alignment padding overflow");
            return NULL;
        }

        size_t current_offset = arena->offset;
        size_t mask = alignment - 1;
        size_t aligned_offset = (current_offset + mask) & ~mask;

        /* Detect overflow in padding */
        if (aligned_offset < current_offset)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Alignment padding caused offset overflow");
            return NULL;
        }

        /* --- Help EVA prove the alignment mathematically --- */
        //@ assert aligned_offset % alignment == 0;

        /* Check if allocation fits */
        if (aligned_offset + size > arena->capacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ARENA_EXHAUSTED,
                             "Arena out of memory: requested %zu bytes (with alignment), available %zu bytes",
                             size, arena->capacity - current_offset);
            return NULL;
        }

        /* Safe pointer construction */
        void *ptr = (void *)(arena->buffer + aligned_offset);

        /* Existing debug assert (keep it) */
        ZUI_ASSERT(((uintptr_t)ptr & mask) == 0, "Arena allocation returned misaligned pointer");

        arena->offset = aligned_offset + size;

        memset(ptr, 0, size);

        return ptr;
    }

    void ZuiResetArena(ZuiArena *arena)
    {
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return;
        }

        arena->offset = 0;
    }

    bool ZuiIsArenaValid(const ZuiArena *arena)
    {
        return arena != NULL &&
               arena->buffer != NULL &&
               arena->offset <= arena->capacity;
    }

    void *ZuiAllocArenaDefault(ZuiArena *arena, const size_t size)
    {
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return NULL;
        }

        return ZuiAllocArena(arena, size, ZUI_DEFAULT_ALIGNMENT);
    }

    ZuiArenaStats ZuiGetArenaStats(const ZuiArena *arena)
    {
        ZuiArenaStats stats = {0};

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return stats;
        }

        if (arena->capacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY, "Arena capacity is zero");
            return stats;
        }

        stats.totalCapacity = arena->capacity;
        stats.usedBytes = arena->offset;
        stats.availableBytes = arena->capacity - arena->offset;
        stats.usagePercent = (float)arena->offset / (float)arena->capacity * 100.0F;
        return stats;
    }

    void ZuiPrintArenaStats(const ZuiArena *arena)
    {
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Arena pointer is NULL");
            return;
        }
        ZuiArenaStats stats = ZuiGetArenaStats(arena);

        TraceLog(LOG_INFO, "ZUI: Arena: %.2F%% used (%zu / %zu bytes)",
                 (double)stats.usagePercent, stats.usedBytes, stats.totalCapacity);
    }

    ZuiContext *g_zui_ctx = NULL;
    ZuiArena g_zui_arena = {0};

    // ----------------------------------------------------------------------------Context

    bool ZuiIsInitialized(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "ZUI context not initialized - call ZuiInit() first");
            return false;
        }
        return true;
    }

    ZuiContext *ZuiGetContext(void)
    {
        ZUI_ASSERT(g_zui_ctx != NULL, "ZUI context not initialized - call ZuiInit() first");

        if (g_zui_ctx == NULL)
        {
            return NULL;
        }

        return g_zui_ctx;
    }

    void ZuiUpdate(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "ZUI context not initialized");
            return;
        }

        const unsigned int frame_count = ZuiGetFrameCount(&g_zui_ctx->frameArray);

        // Iterate backwards for rendering order
        for (unsigned int i = frame_count; i > 0; i--)
        {
            ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, i - 1);

            if (frame && frame->isVisible)
            {
                ZuiUpdateFrame(frame);
            }
        }
    }

    void ZuiRender(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "ZUI context not initialized");
            return;
        }

        const unsigned int frame_count = ZuiGetFrameCount(&g_zui_ctx->frameArray);

        // Iterate backwards using unsigned arithmetic
        for (unsigned int i = frame_count; i > 0; i--)
        {
            ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, i - 1);

            if (frame && frame->isVisible)
            {
                ZuiRenderFrame(frame);
            }
        }
    }

    bool ZuiInit(void)
    {
        // Check for double initialization
        if (g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ALREADY_INITIALIZED,
                             "ZUI context already exists");
            return false;
        }

        // Initialize global arena with 1MB - intentional large allocation for memory pool
        // This is a one-time allocation that serves as backing memory for all UI elements
        ZuiResult result = ZuiInitArena(&g_zui_arena, ZUI_DEFAULT_ARENA_SIZE); // 1MB
        if (result != ZUI_OK)
        {
            // Error already reported by ZuiInitArena
            TraceLog(LOG_ERROR, "ZUI: Initialization aborted due to arena failure");
            ZuiUnloadArena(&g_zui_arena); // This deallocates everything
            g_zui_ctx = NULL;
            return false;
        }

        // Allocate context from arena
        ZuiContext *ctx = ZuiAllocArenaDefault(&g_zui_arena, sizeof(ZuiContext));

        if (!ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY,
                             "Failed to allocate ZuiContext from arena");
            ZuiUnloadArena(&g_zui_arena);
            return false;
        }

        *ctx = (ZuiContext){0};
        g_zui_ctx = ctx;

        // Initialize frame array - CRITICAL: Check the result!
        result = ZuiInitFrame(&ctx->frameArray, &g_zui_arena, ZUI_FRAMES_CAPACITY);
        if (result != ZUI_OK)
        {
            // Error already reported by ZuiInitFrame
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize frame array: %s",
                     ZuiResultToString(result));
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        result = ZuiInitLabel(&ctx->labelArray, &g_zui_arena, ZUI_LABELS_CAPACITY);
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize label array: %s",
                     ZuiResultToString(result));
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        g_zui_ctx->cursor = (ZuiCursor){0};

        int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();

        if (screen_width <= 0 || screen_height <= 0)
        {
            TraceLog(LOG_WARNING, "ZUI: Invalid screen dimensions (%dx%d), using defaults",
                     screen_width, screen_height);
            screen_width = ZUI_DEFAULT_SCREEN_WIDTH;
            screen_height = ZUI_DEFAULT_SCREEN_HEIGHT;
        }

        ZuiFrameId root_frame = ZuiCreateFrame(
            (Rectangle){0, 0, (float)screen_width, (float)screen_height},
            BLANK,
            &g_zui_ctx->frameArray,
            &g_zui_arena);

        if (root_frame.value != 0)
        {
            TraceLog(LOG_WARNING, "ZUI: Root frame ID is not 0 (got %u)", root_frame.value);
        }

        ZuiFrame *root = ZuiGetFrame(&g_zui_ctx->frameArray, ZUI_ROOT_FRAME_ID);
        if (root == NULL)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to create root frame");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        g_zui_ctx->cursor.position = (Vector2){root->data.padding, root->data.padding};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.activeFrame = root->id;

        Vector2 dpiScale = GetWindowScaleDPI();
        g_zui_ctx->dpiScale = (int)dpiScale.x;
        if (g_zui_ctx->dpiScale <= 0)
        {
            TraceLog(LOG_WARNING, "ZUI: Invalid DPI scale, defaulting to 1");
            g_zui_ctx->dpiScale = ZUI_DEFAULT_DPI_SCALE;
        }

        g_zui_ctx->font = LoadFontEx("src/resources/Inter_18pt-Regular.ttf",
                                     ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);

        if (g_zui_ctx->font.texture.id == 0 || g_zui_ctx->font.baseSize == 0)
        {
            TraceLog(LOG_WARNING, "ZUI: Failed to load custom font, using default");
            g_zui_ctx->font = GetFontDefault();
        }
        g_zui_ctx->monoFont = LoadFontEx("src/resources/Inconsolata-Regular.ttf",
                                         ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);

        if (g_zui_ctx->monoFont.texture.id == 0 || g_zui_ctx->monoFont.baseSize == 0)
        {
            TraceLog(LOG_WARNING, "ZUI: Failed to load custom mono font, using default");
            g_zui_ctx->monoFont = GetFontDefault();
        }

        TraceLog(LOG_INFO, "ZUI: Initialized successfully (scale: %d, arena: 1MB)", g_zui_ctx->dpiScale);
        return true;
    }

    void ZuiExit(void)
    {
        if (!ZuiIsInitialized())
        {
            return;
        }

        UnloadFont(g_zui_ctx->font);
        UnloadFont(g_zui_ctx->monoFont);

#ifdef ZUI_DEBUG
        ZuiPrintArenaStats(&g_zui_arena);
#endif

        if (g_zui_arena.buffer != NULL)
        {
            ZuiUnloadArena(&g_zui_arena);
        }

        g_zui_ctx = NULL;
    }
    // -----------------------------------------------------------------------------

    ///------------------------------------------------------------Internal Utility Functions

    bool ZuiIsPowerOfTwo(size_t const n)
    {
        return n > 0 && (n & (n - 1)) == 0;
    }

    const char *ZuiGetWidgetType(const ZuiItemType type)
    {
        switch (type)
        {
        case ZUI_FRAME:
            return "frame";
        case ZUI_LABEL:
            return "label";
        case ZUI_ITEM_TYPE_TEXTURE:
            return "texture";
        case ZUI_ITEM_TYPE_HORIZONTAL_3PATCH_TEXTURE:
            return "horizontal 3patch texture";
        case ZUI_ITEM_TYPE_VERTICAL_3PATCH_TEXTURE:
            return "vertical 3patch texture";
        case ZUI_ITEM_TYPE_9SLICE_TEXTURE:
            return "9 slice texture";
        case ZUI_ITEM_TYPE_NUMERIC_INPUT:
            return "numerical input";
        case ZUI_ITEM_TYPE_BUTTON:
            return "button";
        case ZUI_ITEM_TYPE_SLIDER:
            return "slider";
        case ZUI_ITEM_TYPE_KNOB:
            return "knob";
        default:
            return "unknown";
        }
    }

    const char *ZuiResultToString(const ZuiResult result)
    {
        switch (result)
        {
        case ZUI_OK:
            return "Success";
        case ZUI_ERROR_NULL_POINTER:
            return "NULL pointer argument";
        case ZUI_ERROR_NULL_CONTEXT:
            return "Global context is NULL";
        case ZUI_ERROR_NULL_ARENA:
            return "Arena pointer is NULL";
        case ZUI_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case ZUI_ERROR_ARENA_EXHAUSTED:
            return "Arena exhausted";
        case ZUI_ERROR_ALLOCATION_FAILED:
            return "Allocation failed";
        case ZUI_ERROR_INVALID_CAPACITY:
            return "Invalid capacity";
        case ZUI_ERROR_INVALID_ALIGNMENT:
            return "Invalid alignment (must be power of 2)";
        case ZUI_ERROR_INVALID_ID:
            return "Invalid ID";
        case ZUI_ERROR_INVALID_BOUNDS:
            return "Invalid bounds (negative dimensions)";
        case ZUI_ERROR_INVALID_VALUE:
            return "Invalid parameter value";
        case ZUI_ERROR_INVALID_ENUM:
            return "Invalid enum value";
        case ZUI_ERROR_INVALID_OBJECT_STATE:
            return "Invalid object state";
        case ZUI_ERROR_OVERFLOW:
            return "Integer overflow";
        case ZUI_ERROR_OUT_OF_BOUNDS:
            return "Array index out of bounds";
        case ZUI_ERROR_NOT_INITIALIZED:
            return "Not initialized";
        case ZUI_ERROR_ALREADY_INITIALIZED:
            return "Already initialized";
        case ZUI_ERROR_INVALID_INTERNAL_STATE:
            return "Invalid internal state";
        case ZUI_ERROR_FONT_LOAD_FAILED:
            return "Font load failed";
        case ZUI_ERROR_TEXTURE_LOAD_FAILED:
            return "Texture load failed";
        case ZUI_ERROR_RESOURCE_NOT_FOUND:
            return "Resource not found";
        case ZUI_ERROR_OPERATION_FAILED:
            return "Operation failed";
        case ZUI_ERROR_CIRCULAR_REFERENCE:
            return "Circular reference detected";
        case ZUI_ERROR_ITEM_NOT_FOUND:
            return "Item not found";
        case ZUI_ERROR_BUFFER_TOO_SMALL:
            return "Buffer too small";
        case ZUI_ERROR_INTERNAL_ERROR:
            return "Internal error";
        case ZUI_ERROR_NOT_IMPLEMENTED:
            return "Not implemented";
        default:
            return "Unknown error code";
        }
    }

    float ZuiPixelsToRoundness(const Rectangle rect, float radius_px)
    {
        float min_dimension = rect.width < rect.height ? rect.width : rect.height;

        // Handle zero dimension to avoid division by zero
        if (min_dimension <= 0.0F)
        {
            return 0.0F;
        }

        float max_radius = min_dimension * 0.5F;

        // Clamp radius so it never exceeds half the shortest side
        if (radius_px < 0.0F)
        {
            radius_px = 0.0F;
        }

        if (radius_px > max_radius)
        {
            radius_px = max_radius;
        }

        return radius_px / max_radius;
    }

    //-------------------------------------------------------------Option Preset
    ZuiOptionInt ZuiSomeInt(const int v)
    {
        return (ZuiOptionInt){.has = true, .value = v};
    }

    bool ZuiHasInt(ZuiOptionInt const o)
    {
        return o.has;
    }

    ZuiOptionColor ZuiSomeColor(const Color v)
    {
        return (ZuiOptionColor){.has = true, .value = v};
    }

    bool ZuiHasColor(ZuiOptionColor const o)
    {
        return o.has;
    }

    ZuiOptionString ZuiSomeString(const char *v)
    {
        return (ZuiOptionString){.has = true, .value = v};
    }

    bool ZuiHasString(ZuiOptionString const o)
    {
        return o.has;
    }

    ZuiOptionVector2 ZuiSomeVector2(const Vector2 v)
    {
        return (ZuiOptionVector2){.has = true, .value = v};
    }

    bool ZuiHasVector2(ZuiOptionVector2 const o)
    {
        return o.has;
    }

    ZuiOptionNPatchInfo ZuiSomeNPatchInfo(const NPatchInfo v)
    {
        return (ZuiOptionNPatchInfo){.has = true, .value = v};
    }

    bool ZuiHasNPatchInfo(ZuiOptionNPatchInfo const o)
    {
        return o.has;
    }

    ZuiOptionTexture2D ZuiSomeTexture2D(const Texture2D v)
    {
        return (ZuiOptionTexture2D){.has = true, .value = v};
    }

    bool ZuiHasTexture2D(ZuiOptionTexture2D const o)
    {
        return o.has;
    }

    ZuiOptionRectangle ZuiSomeRectangle(const Rectangle v)
    {
        return (ZuiOptionRectangle){.has = true, .value = v};
    }

    bool ZuiHasRectangle(ZuiOptionRectangle const o)
    {
        return o.has;
    }

    ZuiOptionFloat ZuiSomeFloat(const float v)
    {
        return (ZuiOptionFloat){.has = true, .value = v};
    }

    bool ZuiHasFloat(ZuiOptionFloat const o)
    {
        return o.has;
    }

    ZuiOptionFont ZuiSomeFont(const Font v)
    {
        return (ZuiOptionFont){.has = true, .value = v};
    }

    bool ZuiHasFont(ZuiOptionFont const o)
    {
        return o.has;
    }

    typedef ZuiOptionInt ZuiOptionAlignment;
    typedef ZuiOptionInt ZuiOptionPlacement;
    typedef ZuiOptionInt ZuiOptionBool;

#define ZUI_SOME_EVAL(x) _Generic((x), \
    int: ZuiSomeInt,                   \
    float: ZuiSomeFloat,               \
    char *: ZuiSomeString,             \
    const char *: ZuiSomeString,       \
    Color: ZuiSomeColor,               \
    Vector2: ZuiSomeVector2,           \
    Rectangle: ZuiSomeRectangle,       \
    Texture2D: ZuiSomeTexture2D,       \
    NPatchInfo: ZuiSomeNPatchInfo,     \
    Font: ZuiSomeFont)(x)

// Main macro - the extra () forces expansion before _Generic
#define zui_some(...) ZUI_SOME_EVAL((__VA_ARGS__))

#define zui_has(...) _Generic(__VA_ARGS__, \
    ZuiOptionInt: ZuiHasInt,               \
    ZuiOptionFloat: ZuiHasFloat,           \
    ZuiOptionString: ZuiHasString,         \
    ZuiOptionColor: ZuiHasColor,           \
    ZuiOptionVector2: ZuiHasVector2,       \
    ZuiOptionRectangle: ZuiHasRectangle,   \
    ZuiOptionTexture2D: ZuiHasTexture2D,   \
    ZuiOptionNPatchInfo: ZuiHasNPatchInfo, \
    ZuiOptionFont: ZuiHasFont)(__VA_ARGS__)
    // -----------------------------------------------------------------------------

    ///------------------------------------------------------------Dynamic Array

    bool ZuiIsDynArrayValid(const ZuiDynArray *array)
    {
        return array != NULL &&
               array->items != NULL &&
               array->count <= array->capacity &&
               array->itemSize > 0 &&
               array->capacity > 0;
    }

    ZuiResult ZuiInitDynArray(ZuiDynArray *array, ZuiArena *arena, const unsigned int initialCapacity,
                              const size_t itemSize, const size_t itemAlignment, const char *typeName)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Dynamic array pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!typeName)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Type name is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        // Validate capacity
        if (initialCapacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY,
                             "%s array capacity must be > 0", typeName);
            return ZUI_ERROR_INVALID_CAPACITY;
        }

        if (initialCapacity > ZUI_MAX_DYNARRAY_CAPACITY)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY,
                             "%s array capacity %u exceeds maximum %u",
                             typeName, initialCapacity, ZUI_MAX_DYNARRAY_CAPACITY);
            return ZUI_ERROR_INVALID_CAPACITY;
        }

        // Validate item size
        if (itemSize == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE,
                             "%s array item size must be > 0", typeName);
            return ZUI_ERROR_INVALID_VALUE;
        }

        // Validate alignment
        if (!ZuiIsPowerOfTwo(itemAlignment))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ALIGNMENT,
                             "%s array item alignment %zu must be power of 2",
                             typeName, itemAlignment);
            return ZUI_ERROR_INVALID_ALIGNMENT;
        }

        // Check for overflow
        if (itemSize > SIZE_MAX / initialCapacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW,
                             "%s array allocation size would overflow", typeName);
            return ZUI_ERROR_OVERFLOW;
        }

        size_t required_bytes = itemSize * initialCapacity;

        // Validate arena state
        if (!ZuiIsArenaValid(arena))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_OBJECT_STATE, "Arena is invalid");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        // Check available space
        if (required_bytes > arena->capacity - arena->offset)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ARENA_EXHAUSTED,
                             "Not enough arena space for %s array: need %zu, have %zu",
                             typeName, required_bytes, arena->capacity - arena->offset);
            return ZUI_ERROR_ARENA_EXHAUSTED;
        }

        // Allocate memory
        void *items = ZuiAllocArena(arena, required_bytes, itemAlignment);
        // occurs if ZuiAllocArena returns NULL even when the arena has sufficient capacity
        // This would require either a memory fault or a bug in the arena allocator itself.
        if (!items)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ALLOCATION_FAILED,
                             "Failed to allocate %s array from arena", typeName);
            return ZUI_ERROR_ALLOCATION_FAILED;
        }

        // Initialize metadata
        array->items = items;
        array->count = 0;
        array->capacity = initialCapacity;
        array->itemSize = itemSize;
        array->itemAlignment = itemAlignment;
        array->typeName = typeName;

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: %s array initialized with capacity %u", typeName, initialCapacity);
#endif

        return ZUI_OK;
    }

    ZuiResult ZuiGrowDynArray(ZuiDynArray *array, ZuiArena *arena)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Dynamic array pointer is NULL ");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!ZuiIsDynArrayValid(array))
        {
            TraceLog(LOG_ERROR, "ZUI: Dynamic array is in invalid state");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        /* Check for capacity overflow (doubling) */
        if (array->capacity > UINT_MAX / 2)
        {
            TraceLog(LOG_ERROR, "ZUI: %s array capacity overflow", array->typeName);
            return ZUI_ERROR_OVERFLOW;
        }

        unsigned int new_capacity = array->capacity * 2;

        /* Check against maximum capacity limit */
        if (new_capacity > ZUI_MAX_DYNARRAY_CAPACITY)
        {
            TraceLog(LOG_ERROR, "ZUI: %s array would exceed maximum capacity %u",
                     array->typeName, ZUI_MAX_DYNARRAY_CAPACITY);
            return ZUI_ERROR_OVERFLOW;
        }

        /* Check for byte allocation overflow */
        if (array->itemSize > SIZE_MAX / new_capacity)
        {
            TraceLog(LOG_ERROR, "ZUI: %s array byte allocation would overflow", array->typeName);
            return ZUI_ERROR_OVERFLOW;
        }

        size_t new_bytes = array->itemSize * new_capacity;

        TraceLog(LOG_INFO, "ZUI: Growing %s array from %u to %u",
                 array->typeName, array->capacity, new_capacity);

        void *new_items = ZuiAllocArena(arena, new_bytes, array->itemAlignment);
        if (new_items == NULL)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to grow %s array", array->typeName);
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        memcpy(new_items, array->items, array->itemSize * array->count);

        array->items = new_items;
        array->capacity = new_capacity;

        return ZUI_OK;
    }

    void *ZuiPushDynArray(ZuiDynArray *array, ZuiArena *arena)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Dynamic array pointer is NULL ");
            return NULL;
        }

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Arena pointer is NULL");
            return NULL;
        }

        if (!ZuiIsDynArrayValid(array))
        {
            TraceLog(LOG_ERROR, "ZUI: Dynamic array is in invalid state");
            return NULL;
        }

        if (array->count >= array->capacity)
        {
            ZuiResult result = ZuiGrowDynArray(array, arena);
            if (result != ZUI_OK)
            {
                TraceLog(LOG_ERROR, "ZUI: Failed to grow %s array: %s",
                         array->typeName, ZuiResultToString(result));
                return NULL;
            }
        }

        size_t byte_offset = array->itemSize * array->count;
        void *slot = (char *)array->items + byte_offset;
        array->count++;
        return slot;
    }

    void *ZuiGetDynArray(const ZuiDynArray *array, const unsigned int index)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Dynamic array pointer is NULL ");
            return NULL;
        }

        if (!ZuiIsDynArrayValid(array))
        {
            TraceLog(LOG_ERROR, "ZUI: Dynamic array is in invalid state");
            return NULL;
        }

        if (index >= array->count)
        {
            TraceLog(LOG_ERROR, "ZUI: Invalid %s array index %u (count: %u)",
                     array->typeName, index, array->count);
            return NULL;
        }

        size_t byte_offset = array->itemSize * (size_t)index;
        return (char *)array->items + byte_offset;
    }

    void ZuiClearDynArray(ZuiDynArray *array)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Dynamic array pointer is NULL");
            return;
        }

        array->count = 0;
    }

    ZuiDynArrayStats ZuiGetDynArrayStats(const ZuiDynArray *array)
    {
        ZuiDynArrayStats stats = {0};

        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Dynamic array pointer is NULL");
            return stats;
        }

        if (array->capacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY, "Dynamic array capacity is zero");
            return stats;
        }

        stats.count = array->count;
        stats.capacity = array->capacity;
        stats.usagePercent = (float)array->count / (float)array->capacity * 100.0F;
        stats.memoryUsed = array->itemSize * array->capacity;
        return stats;
    }

    void ZuiPrintDynArrayStats(const ZuiDynArray *array)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Dynamic array pointer is NULL ");
            return;
        }

        ZuiDynArrayStats stats = ZuiGetDynArrayStats(array);
        TraceLog(LOG_INFO, "ZUI: %s array: %.2F%% used (%u / %u items, %zu bytes)",
                 array->typeName, (double)stats.usagePercent,
                 stats.count, stats.capacity, stats.memoryUsed);
    }

    ZuiResult ZuiInitFrame(ZuiDynArray *array, ZuiArena *arena, const unsigned int initialCapacity)
    {
        // Preconditions
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Array pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (initialCapacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY,
                             "Initial capacity must be > 0, got %u", initialCapacity);
            return ZUI_ERROR_INVALID_CAPACITY;
        }

        // Delegate to generic dynamic array initialization
        return ZuiInitDynArray(array, arena, initialCapacity,
                               sizeof(ZuiFrame), ZUI_ALIGNOF(ZuiFrame), "Frame");
    }

    ZuiFrame *ZuiPushFrame(ZuiDynArray *array, ZuiArena *arena)
    {
        if (!array || !arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Invalid parameters to ZuiPushFrame");
            return NULL;
        }
        return ZuiPushDynArray(array, arena);
    }

    ZuiFrame *ZuiGetFrame(const ZuiDynArray *array, const unsigned int index)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Dynamic array pointer is NULL");
            return NULL;
        }

        if (!array->items || array->capacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NOT_INITIALIZED,
                             "Frame array not initialized (items: %p, capacity: %u)",
                             array->items, array->capacity);
            return NULL;
        }

        if (index >= array->count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS,
                             "Frame index %u out of bounds (count: %u)", index, array->count);
            return NULL;
        }

        return ZuiGetDynArray(array, index);
    }

    void ZuiClearFrame(ZuiDynArray *array)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame array pointer is NULL");
            return;
        }

        ZuiClearDynArray(array);
    }

    unsigned int ZuiGetFrameCount(ZuiDynArray const *array)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame array pointer is NULL");
            return 0;
        }

        return array->count;
    }

    ZuiResult ZuiInitFrameItems(ZuiFrame *frame, ZuiArena *arena, const size_t initialCapacity)
    {
        if (!frame || !arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Invalid parameters to ZuiInitFrameItems");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (initialCapacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_CAPACITY, "Initial capacity must be > 0");
            TraceLog(LOG_ERROR, "ZUI: initialCapacity must be > 0");
            return ZUI_ERROR_INVALID_CAPACITY;
        }

        /* Check for allocation size overflow */
        if (initialCapacity > SIZE_MAX / sizeof(ZuiItem))
        {
            TraceLog(LOG_ERROR, "ZUI: Frame items allocation would overflow");
            return ZUI_ERROR_OVERFLOW;
        }

        /* Check if arena has enough space */
        size_t required_bytes = sizeof(ZuiItem) * initialCapacity;
        if (required_bytes > arena->capacity - arena->offset)
        {
            TraceLog(LOG_ERROR, "ZUI: Not enough arena space for frame items");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        frame->items = (ZuiItem *)ZuiAllocArena(arena,
                                                required_bytes,
                                                ZUI_ALIGNOF(ZuiItem));
        if (!frame->items)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to allocate frame items array");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        frame->itemsCapacity = (unsigned int)initialCapacity;
        frame->itemCounter = 0;

        return ZUI_OK;
    }

    ZuiResult ZuiGrowFrameItems(ZuiFrame *frame, ZuiArena *arena)
    {
        if (!frame || !arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Invalid parameters to ZuiGrowFrameItems");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!frame->items)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_OBJECT_STATE, "Frame items array is NULL");
            return ZUI_ERROR_INVALID_OBJECT_STATE;
        }

        /* Check for capacity overflow */
        if (frame->itemsCapacity > UINT_MAX / 2)
        {
            TraceLog(LOG_ERROR, "ZUI: Frame items capacity overflow");
            return ZUI_ERROR_OVERFLOW;
        }

        unsigned int new_capacity = frame->itemsCapacity * 2;

        /* Check against maximum capacity limit */
        if (new_capacity > ZUI_MAX_DYNARRAY_CAPACITY)
        {
            TraceLog(LOG_ERROR, "ZUI: Frame items would exceed maximum capacity %u",
                     ZUI_MAX_DYNARRAY_CAPACITY);
            return ZUI_ERROR_OVERFLOW;
        }

        /* Check for byte allocation overflow */
        if (sizeof(ZuiItem) > SIZE_MAX / new_capacity)
        {
            TraceLog(LOG_ERROR, "ZUI: Frame items byte allocation would overflow");
            return ZUI_ERROR_OVERFLOW;
        }

        size_t new_bytes = sizeof(ZuiItem) * new_capacity;

        TraceLog(LOG_INFO, "ZUI: Growing frame items from %u to %u",
                 frame->itemsCapacity, new_capacity);

        ZuiItem *new_items = ZuiAllocArena(arena, new_bytes, ZUI_ALIGNOF(ZuiItem));
        if (!new_items)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to grow frame items");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        memcpy(new_items, frame->items, sizeof(ZuiItem) * (size_t)frame->itemCounter);
        frame->items = new_items;
        frame->itemsCapacity = new_capacity;

        return ZUI_OK;
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    ZuiResult ZuiAddFrameItem(ZuiFrame *frame, void *data, const ZuiRenderFunction render_func,
                              const ZuiUpdateFunction update_func, const ZuiItemOptions options, ZuiArena *arena)
    {
        // Validate all input pointers
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!data)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Data pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        // Invariant: itemCounter should never be negative (internal bug if true)
        ZUI_ASSERT(frame->itemCounter >= 0, "Frame itemCounter is negative");

        // Validate frame state
        if (!frame->items)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_OBJECT_STATE,
                             "Frame items array is NULL (frame not initialized?)");
            return ZUI_ERROR_INVALID_OBJECT_STATE;
        }

        // Check capacity and grow if needed
        if ((unsigned int)frame->itemCounter >= frame->itemsCapacity)
        {
            ZuiResult result = ZuiGrowFrameItems(frame, arena);
            if (result != ZUI_OK)
            {
                ZUI_REPORT_ERROR(result,
                                 "Failed to grow frame items from %u capacity",
                                 frame->itemsCapacity);
                return result;
            }
        }

        // Add the item
        ZuiItem *item = &frame->items[frame->itemCounter++];
        item->data = data;
        item->renderFunction = render_func;
        item->updateFunction = update_func;
        item->type = options.type;
        item->id = options.id;
        item->parentId = frame->id.value;
        item->parentPosition = options.parent;

        return ZUI_OK;
    }

    void ZuiUpdateFrame(void *itemFrame)
    {
        // Runtime check
        if (!itemFrame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame pointer is NULL");
            return;
        }

        ZuiFrame *frame = itemFrame;

        ZUI_ASSERT(frame->items != NULL, "Frame items array is NULL");

        for (int i = 0; i < frame->itemCounter; i++)
        {
            ZuiItem *item = &frame->items[i];

            ZUI_ASSERT(item->parentPosition != NULL, "Item parentPosition is NULL");
            ZUI_ASSERT(item->data != NULL, "Item data is NULL");

            item->parentPosition->x = frame->data.bounds.x;
            item->parentPosition->y = frame->data.bounds.y;
            if (item->updateFunction)
            {
                item->updateFunction(item->data);
            }
        }
    }

    void ZuiRenderFrame(void *itemFrame)
    {
        ZUI_ASSERT(itemFrame != NULL, "ZUI Error: frame pointer is NULL");
        ZuiFrame *frame = itemFrame;

        if (!frame->items)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_OBJECT_STATE, "Frame items array is NULL");
            return;
        }

        if (frame->itemCounter < 0 || (unsigned int)frame->itemCounter > frame->itemsCapacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_OBJECT_STATE,
                             "Frame itemCounter %d exceeds capacity %u",
                             frame->itemCounter, frame->itemsCapacity);
            return;
        }

        Vector2 position = (Vector2){frame->data.bounds.x, frame->data.bounds.y};

        if (frame->isInsideWindow)
        {
            position = (Vector2){frame->data.bounds.x + frame->parent.x, frame->data.bounds.y + frame->parent.y};
        }

        Rectangle dest = (Rectangle){position.x, position.y, frame->data.bounds.width, frame->data.bounds.height};
        float roundness = ZuiPixelsToRoundness(dest, ZUI_CORNER_RADIUS);
        if (frame->data.hasBackground)
        {
            DrawRectangleRounded(dest, roundness, ZUI_ROUNDNESS_SEGMENTS, frame->data.backgroundColor);
        }

        if (frame->data.hasOutline)
        {
            DrawRectangleRoundedLinesEx(dest, roundness, ZUI_ROUNDNESS_SEGMENTS, frame->data.outlineThickness,
                                        frame->data.outlineColor);
        }

        // Safety check
        if (frame->itemCounter <= 0)
        {
            // TraceLog(LOG_INFO, "Frame %d has no items to render", frame->id.value);
            return;
        }

        for (unsigned int i = 0; i < (unsigned int)frame->itemCounter; i++)
        {
            ZuiItem *item = &frame->items[i];

            // Validate fields before dereferencing
            if (!item->renderFunction || !item->data)
            {
                continue;
            }

            item->renderFunction(item->data);
        }
    }

    void ZuiRenderFrameItem(void *item)
    {
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Item pointer is NULL");
            return;
        }

        ZuiRenderItem *renderItem = item;
        renderItem->function(renderItem->data);
    }

    static bool is_already_visited(const ZuiFrame *node,
                                   const ZuiFrame *const *visited,
                                   int visited_count)
    {
        // visited_count == 0 → no need to scan at all
        if (visited_count == 0)
        {
            return false;
        }

        for (int i = 0; i < visited_count; ++i)
        {
            if (visited[i] == node)
            {
                return true;
            }
        }
        return false;
    }

    bool ZuiIsChildAncestorIterative(const ZuiFrame *potential_parent,
                                     const ZuiFrame *frame)
    {
        // Fast rejects
        if (!potential_parent || !frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Invalid frame pointers");
            return false;
        }
        if (frame == potential_parent)
        {
            return true;
        }

        const ZuiFrame *stack[ZUI_MAX_ANCESTOR_DEPTH];
        const ZuiFrame *visited[ZUI_MAX_ANCESTOR_DEPTH] = {0}; // explicit zero-init

        int stack_top = 1;
        int visited_count = 0;

        stack[0] = potential_parent;

        while (stack_top > 0)
        {
            const ZuiFrame *current = stack[--stack_top];

            // Cycle detection – now safe even when visited_count == 0
            if (is_already_visited(current, visited, visited_count))
            {
                continue;
            }

            // Visited overflow guard
            if (visited_count >= ZUI_MAX_ANCESTOR_DEPTH)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW,
                                 "Exceeded max visited frames %d", ZUI_MAX_ANCESTOR_DEPTH);
                return false;
            }

            visited[visited_count++] = current;

            // No children → backtrack
            if (!current->items || current->itemCounter <= 0)
            {
                continue;
            }

            // Expand children
            for (int i = 0; i < current->itemCounter; ++i)
            {
                const ZuiItem *item = &current->items[i];

                if (item->renderFunction != ZuiRenderFrameItem || !item->data)
                {
                    continue;
                }

                const ZuiFrame *child = (const ZuiFrame *)item->data;

                if (child == frame)
                {
                    return true;
                }

                // Stack overflow guard
                if (stack_top >= ZUI_MAX_ANCESTOR_DEPTH)
                {
                    ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW,
                                     "Stack overflow in ancestor search");
                    return false;
                }

                stack[stack_top++] = child;
            }
        }

        return false;
    }
    bool ZuiIsChildAncestor(const ZuiFrame *potentialParent, const ZuiFrame *frame)
    {
        if (!potentialParent || !frame)
        {
            return false;
        }

        if (frame == potentialParent)
        {
            return true;
        }
        return ZuiIsChildAncestorIterative(potentialParent, frame);
    }

    void ZuiUpdateFrameItem(void *item)
    {
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Item pointer is NULL");
            return;
        }
        ZuiFrame *frame = item;
        ZuiUpdateFrame(frame);
    }

    Rectangle ZuiGetContentArea(const Rectangle parent, const ZuiPadding padding)
    {
        return (Rectangle){
            parent.x + padding.left,
            parent.y + padding.top,
            parent.width - padding.left - padding.right,
            parent.height - padding.top - padding.bottom,
        };
    }

    Rectangle ZuiAlignRectangle(const Rectangle parent, const Vector2 childSize,
                                const ZuiPadding padding, const ZuiAlignment alignment)
    {
        // Get content area (parent minus padding)
        Rectangle content = ZuiGetContentArea(parent, padding);

        Rectangle child = {0};
        child.width = childSize.x;
        child.height = childSize.y;

        // Calculate position based on alignment
        switch (alignment)
        {
        case ZUI_ALIGN_TOP_LEFT:
            child.x = content.x;
            child.y = content.y;
            break;

        case ZUI_ALIGN_TOP_CENTER:
            child.x = content.x + ((content.width - child.width) * 0.5F);
            child.y = content.y;
            break;

        case ZUI_ALIGN_TOP_RIGHT:
            child.x = content.x + content.width - child.width;
            child.y = content.y;
            break;

        case ZUI_ALIGN_MIDDLE_LEFT:
            child.x = content.x;
            child.y = content.y + ((content.height - child.height) * 0.5F);
            break;

        case ZUI_ALIGN_CENTER:
            child.x = content.x + ((content.width - child.width) * 0.5F);
            child.y = content.y + ((content.height - child.height) * 0.5F);
            break;

        case ZUI_ALIGN_MIDDLE_RIGHT:
            child.x = content.x + content.width - child.width;
            child.y = content.y + ((content.height - child.height) * 0.5F);
            break;

        case ZUI_ALIGN_BOTTOM_LEFT:
            child.x = content.x;
            child.y = content.y + content.height - child.height;
            break;

        case ZUI_ALIGN_BOTTOM_CENTER:
            child.x = content.x + ((content.width - child.width) * 0.5F);
            child.y = content.y + content.height - child.height;
            break;

        case ZUI_ALIGN_BOTTOM_RIGHT:
            child.x = content.x + content.width - child.width;
            child.y = content.y + content.height - child.height;
            break;
        default:
            break;
        }

        return child;
    }

    Rectangle ZuiGetBoundsFromVectors(const Vector2 position, const Vector2 size)
    {
        return (Rectangle){position.x, position.y, size.x, size.y};
    }

    Vector2 ZuiGetPositionFromBounds(const Rectangle bounds)
    {
        return (Vector2){bounds.x, bounds.y};
    }

    Vector2 ZuiGetSizeFromBounds(const Rectangle bounds)
    {
        return (Vector2){bounds.width, bounds.height};
    }

    Rectangle ZuiCalculateChildBounds(const Rectangle childBounds, const Rectangle bounds, const ZuiItemOptions options)
    {
        // Vector2 new_position = position;
        Vector2 new_position = ZuiGetPositionFromBounds(childBounds);
        Vector2 size = ZuiGetSizeFromBounds(childBounds);

        if (options.placement == ZUI_PLACE_RELATIVE)
        {
            new_position.x += bounds.x;
            new_position.y += bounds.y;
        }

        if (options.placement == ZUI_PLACE_ALIGN)
        {
            Rectangle new = ZuiAlignRectangle(
                bounds, size,
                (ZuiPadding){ZUI_DEFAULT_FRAME_PADDING, ZUI_DEFAULT_FRAME_PADDING, ZUI_DEFAULT_FRAME_PADDING,
                             ZUI_DEFAULT_FRAME_PADDING},
                options.alignment);
            new_position = (Vector2){new.x, new.y};
        }

        if (options.placement == ZUI_PLACE_CURSOR)
        {
            new_position = options.cursor;
        }
        Rectangle new_bounds = ZuiGetBoundsFromVectors(new_position, size);
        return new_bounds;
    }

    Vector2 ZuiCalculateChildPosition(const Vector2 position, const Vector2 size,
                                      const Rectangle bounds, const ZuiItemOptions options)
    {
        Vector2 new_position = position;

        if (options.placement == ZUI_PLACE_RELATIVE)
        {
            new_position.x += bounds.x;
            new_position.y += bounds.y;
        }

        if (options.placement == ZUI_PLACE_ALIGN)
        {
            Rectangle new = ZuiAlignRectangle(
                bounds, size,
                (ZuiPadding){ZUI_DEFAULT_FRAME_PADDING, ZUI_DEFAULT_FRAME_PADDING, ZUI_DEFAULT_FRAME_PADDING,
                             ZUI_DEFAULT_FRAME_PADDING},
                options.alignment);
            new_position = (Vector2){new.x, new.y};
        }

        if (options.placement == ZUI_PLACE_CURSOR)
        {
            new_position = options.cursor;
        }

        return new_position;
    }

    const char *ZuiGetAlignmentName(const ZuiAlignment alignment)
    {
        switch (alignment)
        {
        case ZUI_ALIGN_TOP_LEFT:
            return "Top Left";
        case ZUI_ALIGN_TOP_CENTER:
            return "Top Center";
        case ZUI_ALIGN_TOP_RIGHT:
            return "Top Right";
        case ZUI_ALIGN_MIDDLE_LEFT:
            return "Middle Left";
        case ZUI_ALIGN_CENTER:
            return "Center";
        case ZUI_ALIGN_MIDDLE_RIGHT:
            return "Middle Right";
        case ZUI_ALIGN_BOTTOM_LEFT:
            return "Bottom Left";
        case ZUI_ALIGN_BOTTOM_CENTER:
            return "Bottom Center";
        case ZUI_ALIGN_BOTTOM_RIGHT:
            return "Bottom Right";
        default:
            return "Unknown";
        }
    }

    ZuiFrameId ZuiCreateFrame(const Rectangle bounds, const Color color, ZuiDynArray *frameArray, ZuiArena *arena)
    {
        if (!frameArray || !arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Invalid parameters to ZuiCreateFrame");
            return (ZuiFrameId){ZUI_ID_INVALID};
        }

        ZuiFrame *frame = ZuiPushFrame(frameArray, arena);
        if (!frame)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to create frame");
            return (ZuiFrameId){ZUI_ID_INVALID};
        }

        *frame = (ZuiFrame){
            .data.bounds = bounds,
            .id = (ZuiFrameId){frameArray->count - 1},
            .data.backgroundColor = color,
            .data.padding = ZUI_DEFAULT_FRAME_PADDING,
            .data.gap = ZUI_DEFAULT_FRAME_GAP,
            .isVisible = true,
        };

        if (color.a > 0)
        {
            frame->data.hasBackground = true;
        }

        ZuiResult result = ZuiInitFrameItems(frame, arena, ZUI_FRAME_ITEMS_CAPACITY);
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize frame items: %s",
                     ZuiResultToString(result));
            // rollback
            frameArray->count--;
            return (ZuiFrameId){ZUI_ID_INVALID};
        }

        return frame->id;
    }

    bool ZuiValidateFrameId(const ZuiDynArray *array, const ZuiFrameId id)
    {
        if (!array || !array->items || array->capacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NOT_INITIALIZED, "Array not initialized");
            return false;
        }

        if (id.value >= array->count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS, "Invalid frame ID");
            return false;
        }

        return true;
    }

    bool ZuiValidateFrameBounds(const Rectangle bounds)
    {
        if (bounds.width < 0 || bounds.height < 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Bounds have negative dimensions");
            return false;
        }
        return true;
    }

    bool ZuiValidateSetFrameBounds(const ZuiFrameId id, const Rectangle bounds, const ZuiDynArray *array)
    {
        if (!ZuiValidateFrameId(array, id))
        {
            return false;
        }

        return ZuiValidateFrameBounds(bounds);
    }

    bool ZuiValidateFramePadding(const float padding)
    {
        if (padding < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Padding must be non-negative");
            return false;
        }
        return true;
    }

    bool ZuiValidateFrameSpacing(const float spacing)
    {
        if (spacing < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Spacing must be non-negative");
            return false;
        }
        return true;
    }

    bool ZuiValidateFrameOutlineThickness(const float thickness)
    {
        // Runtime validation
        if (thickness < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Outline thickness must be non-negative");
            return false;
        }
        return true;
    }

    void ZuiInternalPrintFrame(const ZuiFrame *frame)
    {
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Cannot print NULL frame");
            return;
        }

        TraceLog(LOG_INFO, "---------------- ZUI FRAME DEBUG ----------------");

        // Basic info
        TraceLog(LOG_INFO, "│  ID:         %d", frame->id.value);
        TraceLog(LOG_INFO, "│  Visible:    %s", frame->isVisible ? "true" : "false");
        TraceLog(LOG_INFO, "│  Parent:     (%.2F, %.2F)", (double)frame->parent.x, (double)frame->parent.y);
        TraceLog(LOG_INFO, "│  Items:      %d / %u", frame->itemCounter, frame->itemsCapacity);

        // Bounds
        TraceLog(LOG_INFO, "┌Bounds");
        TraceLog(LOG_INFO, "│  Position:   (%.2F, %.2F)",
                 (double)frame->data.bounds.x, (double)frame->data.bounds.y);
        TraceLog(LOG_INFO, "│  Size:       %.2F x %.2F",
                 (double)frame->data.bounds.width, (double)frame->data.bounds.height);

        // Layout
        TraceLog(LOG_INFO, "┌Layout");
        TraceLog(LOG_INFO, "│  Padding:    %.2F", (double)frame->data.padding);
        TraceLog(LOG_INFO, "│  Spacing:    %.2F", (double)frame->data.gap);

        // Background
        TraceLog(LOG_INFO, "┌Background");
        TraceLog(LOG_INFO, "│  Has BG:     %s", frame->data.hasBackground ? "true" : "false");
        if (frame->data.hasBackground)
        {
            TraceLog(LOG_INFO, "│  BG Color:   (%d, %d, %d, %d)",
                     frame->data.backgroundColor.r,
                     frame->data.backgroundColor.g,
                     frame->data.backgroundColor.b,
                     frame->data.backgroundColor.a);
        }

        // Outline
        TraceLog(LOG_INFO, "┌Outline");
        TraceLog(LOG_INFO, "│  Has Outline: %s", frame->data.hasOutline ? "true" : "false");
        if (frame->data.hasOutline)
        {
            TraceLog(LOG_INFO, "│  Color:       (%d, %d, %d, %d)",
                     frame->data.outlineColor.r,
                     frame->data.outlineColor.g,
                     frame->data.outlineColor.b,
                     frame->data.outlineColor.a);
            TraceLog(LOG_INFO, "│  Thickness:   %.2F", (double)frame->data.outlineThickness);
        }

        // Items summary
        TraceLog(LOG_INFO, "┌Items (%d)", frame->itemCounter);
        for (int i = 0; i < frame->itemCounter; i++)
        {
            const ZuiItem *item = &frame->items[i];
            TraceLog(LOG_INFO, "│  [%d] Type: %s, ID: %u",
                     i,
                     ZuiGetWidgetType(item->type),
                     item->id);
        }
    }

    /// ----------------------------------------------------------------------------Frame set

    ZuiResult ZuiAddChildFrame(const ZuiFrameId parentId, const ZuiFrameId childId, ZuiItemOptions options)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ERROR_NULL_CONTEXT;
        }

        // Validate IDs are not the same
        if (parentId.value == childId.value)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_CIRCULAR_REFERENCE,
                             "Cannot add frame as its own child (ID: %u)", parentId.value);
            return ZUI_ERROR_CIRCULAR_REFERENCE;
        }

        // Get parent frame
        ZuiFrame *parentFrame = ZuiGetFrame(&g_zui_ctx->frameArray, parentId.value);
        if (!parentFrame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid parent frame ID: %u", parentId.value);
            return ZUI_ERROR_INVALID_ID;
        }

        // Get child frame
        ZuiFrame *childFrame = ZuiGetFrame(&g_zui_ctx->frameArray, childId.value);
        if (!childFrame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID,
                             "Invalid child frame ID: %u", childId.value);
            return ZUI_ERROR_INVALID_ID;
        }

        // Check for circular reference
        if (ZuiIsChildAncestor(parentFrame, childFrame))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_CIRCULAR_REFERENCE,
                             "Cannot add parent frame to its child (circular reference)");
            return ZUI_ERROR_CIRCULAR_REFERENCE;
        }

        // Prepare options for frame item
        options.id = childId.value;
        options.parent = &childFrame->parent;
        options.type = ZUI_FRAME;

        // Add as frame item
        ZuiResult result = ZuiAddFrameItem(parentFrame, childFrame,
                                           ZuiRenderFrame, ZuiUpdateFrame,
                                           options, &g_zui_arena);
        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result,
                             "Failed to add child frame %u to parent %u",
                             childId.value, parentId.value);
            return result;
        }

        return ZUI_OK;
    }

    ZuiResult ZuiAddLabel(const ZuiFrameId parentId, const ZuiLabelId childId, ZuiItemOptions options, ZuiArena *arena)
    {
        // Validate global context
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ERROR_NULL_CONTEXT;
        }

        // Validate arena
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        // Get label with validation
        ZuiLabel *label = ZuiGetLabel(&g_zui_ctx->labelArray, childId.value);
        if (!label)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid label ID: %u", childId.value);
            return ZUI_ERROR_INVALID_ID;
        }

        // Get parent frame with validation
        ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, parentId.value);
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid parent frame ID: %u", parentId.value);
            return ZUI_ERROR_INVALID_ID;
        }

        // Call AddFrameItem and propagate its result
        options.id = childId.value;
        options.parent = &label->parent;
        options.type = ZUI_LABEL;

        ZuiResult result = ZuiAddFrameItem(frame, label, ZuiRenderLabel, NULL, options, arena);
        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to add label %u to frame %u", childId.value, parentId.value);
        }

        return result;
    }

    Rectangle *ZuiGetItemBoundsFromFrames(const ZuiItem *item)
    {
        // Runtime check
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Item pointer is NULL");
            return NULL;
        }

        switch (item->type)
        {
        case ZUI_FRAME:
        {
            ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, item->id);
            return frame ? &frame->data.bounds : NULL;
        }

        case ZUI_LABEL:
        {
            ZuiLabel *label = ZuiGetLabel(&g_zui_ctx->labelArray, item->id);
            return label ? &label->data.bounds : NULL;
        }

        default:
            TraceLog(LOG_WARNING, "ZUI: Unknown item type: %d", item->type);
            return NULL;
        }
    }

    void ZuiAlignHorizontalItem(ZuiFrame *frame, const int startIndex, const int endIndex, const ZuiAlignmentY alignment)
    {
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame pointer is NULL");
            return;
        }

        if (!frame->items)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_OBJECT_STATE, "Frame items array is NULL");
            return;
        }

        // Invariant: alignment should be valid
        ZUI_ASSERT((alignment & (alignment - 1)) == 0, "Alignment must be a power of two");

        if (startIndex < 0 || endIndex < 0)
        {
            TraceLog(LOG_WARNING, "ZUI: Invalid indices for alignment (start: %d, end: %d)",
                     startIndex, endIndex);
            return;
        }

        if (startIndex > endIndex)
        {
            TraceLog(LOG_WARNING, "ZUI: Start index (%d) > end index (%d)",
                     startIndex, endIndex);
            return;
        }

        if (endIndex >= frame->itemCounter)
        {
            TraceLog(LOG_WARNING, "ZUI: End index (%d) >= item count (%d)",
                     endIndex, frame->itemCounter);
            return;
        }

        int count = endIndex - startIndex + 1;

        if (count > ZUI_ALIGNMENT_MAX_ITEMS)
        {
            TraceLog(LOG_WARNING, "ZUI: Too many items to align (%d), maximum is %d", count, ZUI_ALIGNMENT_MAX_ITEMS);
            return;
        }

        Rectangle bounds[ZUI_ALIGNMENT_MAX_ITEMS];
        bool bounds_valid[ZUI_ALIGNMENT_MAX_ITEMS] = {false}; // Track which bounds are valid

        for (int i = 0; i < count; i++)
        {
            Rectangle const *item_bounds = ZuiGetItemBoundsFromFrames(&frame->items[startIndex + i]);
            if (item_bounds != NULL)
            {
                bounds[i] = *item_bounds;
                bounds_valid[i] = true;
            }
            else
            {
                TraceLog(LOG_WARNING, "ZUI: Could not get bounds for item at index %d",
                         startIndex + i);
                bounds[i] = (Rectangle){0, 0, 0, 0};
                bounds_valid[i] = false;
            }
        }

        // Check if first item has valid bounds
        if (!bounds_valid[0] || bounds[0].height <= 0.0F)
        {
            TraceLog(LOG_WARNING, "ZUI: First item has invalid bounds for alignment");
            return;
        }

        Rectangle const *first_bounds = &bounds[0];
        float align_y = 0.0F;

        switch (alignment)
        {
        case ZUI_ALIGN_Y_TOP:
            align_y = first_bounds->y;
            break;
        case ZUI_ALIGN_Y_CENTER:
            align_y = first_bounds->y + (first_bounds->height * 0.5F);
            break;
        case ZUI_ALIGN_Y_BOTTOM:
            align_y = first_bounds->y + first_bounds->height;
            break;
        default:
            TraceLog(LOG_WARNING, "ZUI: Unknown horizontal alignment: %d", alignment);
            align_y = first_bounds->y + (first_bounds->height * 0.5F);
            break;
        }

        for (int i = 1; i < count; i++)
        {
            if (!bounds_valid[i] || bounds[i].height <= 0.0F)
            {
                continue; // Skip invalid items
            }

            ZuiItem const *item = &frame->items[startIndex + i];
            Rectangle *item_bounds = ZuiGetItemBoundsFromFrames(item);

            if (item_bounds != NULL)
            {
                switch (alignment)
                {
                case ZUI_ALIGN_Y_TOP:
                    item_bounds->y = align_y;
                    break;
                case ZUI_ALIGN_Y_CENTER:
                    item_bounds->y = align_y - (item_bounds->height * 0.5F);
                    break;
                case ZUI_ALIGN_Y_BOTTOM:
                    item_bounds->y = align_y - item_bounds->height;
                    break;
                default:
                    item_bounds->y = align_y - (item_bounds->height * 0.5F);
                    break;
                }
            }
        }
    }

    void ZuiSetFrameBounds(const ZuiFrameId id, const Rectangle bounds)
    {
        if (ZuiValidateSetFrameBounds(id, bounds, &g_zui_ctx->frameArray))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.bounds = bounds;
        }
    }

    void ZuiSetFrameBackgroundColor(const ZuiFrameId id, const Color color)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.backgroundColor = color;
        }
    }

    void ZuiSetFrameOutlineColor(const ZuiFrameId id, const Color color)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.outlineColor = color;
        }
    }

    void ZuiSetFramePadding(const ZuiFrameId id, const float padding)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        if (!ZuiValidateFramePadding(padding))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Invalid padding: %.2f", (double)padding);
            return;
        }

        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid frame ID: %u", id.value);
            return;
        }

        ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, id.value);
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame pointer is NULL for id: %u", id.value);
            return;
        }

        frame->data.padding = padding;
    }
    void ZuiSetFrameGap(const ZuiFrameId id, const float gap)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id) && ZuiValidateFrameSpacing(gap))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.gap = gap;
        }
    }

    void ZuiSetFrameOutlineThickness(const ZuiFrameId id, const float thickness)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id) && ZuiValidateFrameOutlineThickness(thickness))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.outlineThickness = thickness;
        }
    }

    void ZuiSetFrameHasBackground(const ZuiFrameId id, const bool hasBackground)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.hasBackground = hasBackground;
        }
    }

    void ZuiSetFrameHasOutline(const ZuiFrameId id, const bool hasOutline)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.hasOutline = hasOutline;
        }
    }

    void ZuiSetFrameIsVisible(const ZuiFrameId id, const bool isVisible)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->isVisible = isVisible;
        }
    }

    void ZuiSetFrameParentId(const ZuiFrameId id, const ZuiFrameId parentId)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->parentId = parentId;
        }
    }

    //------------------------------------------------------------------Frame get
    Rectangle ZuiGetFrameBounds(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.bounds;
        }

        return (Rectangle){0};
    }

    Color ZuiGetFrameBackgroundColor(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.backgroundColor;
        }

        return (Color){0};
    }

    Color ZuiGetFrameOutlineColor(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.outlineColor;
        }

        return (Color){0};
    }

    float ZuiGetFramePadding(const ZuiFrameId id)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return 0.0F;
        }

        if (!ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return 0.0F;
        }

        ZuiFrame *frame = ZuiGetFrame(&g_zui_ctx->frameArray, id.value);
        if (!frame)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Frame pointer is NULL");
            return 0.0F;
        }

        return frame->data.padding;
    }

    float ZuiGetFrameSpacing(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.gap;
        }

        return 0.0F;
    }

    float ZuiGetFrameOutlineThickness(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.outlineThickness;
        }

        return 0.0F;
    }

    bool ZuiGetFrameHasBackground(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.hasBackground;
        }

        return false;
    }

    bool ZuiGetFrameHasOutline(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->data.hasOutline;
        }

        return false;
    }

    bool ZuiGetFrameIsVisible(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->isVisible;
        }

        return false;
    }

    int ZuiGetFrameItemCount(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->itemCounter;
        }

        return -1;
    }

    unsigned int ZuiGetFrameItemsCapacity(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->itemsCapacity;
        }

        return 0;
    }

    Vector2 ZuiGetFrameParentPosition(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->parent;
        }

        return (Vector2){0};
    }

    ZuiFrameId ZuiGetFrameParentId(const ZuiFrameId id)
    {
        if (ZuiValidateFrameId(&g_zui_ctx->frameArray, id))
        {
            return ZuiGetFrame(&g_zui_ctx->frameArray, id.value)->parentId;
        }

        return (ZuiFrameId){ZUI_ID_INVALID};
    }

    // -----------------------------------------------------------------------------

    ZuiResult ZuiInitLabel(ZuiDynArray *array, ZuiArena *arena, const unsigned int initialCapacity)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Dynamic array pointer is NULL ");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        return ZuiInitDynArray(array, arena, initialCapacity,
                               sizeof(ZuiLabel), ZUI_ALIGNOF(ZuiLabel), "Label");
    }

    ZuiLabel *ZuiPushLabel(ZuiDynArray *array, ZuiArena *arena)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Dynamic array pointer is NULL ");
            return NULL;
        }

        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Arena pointer is NULL");
            return NULL;
        }
        return ZuiPushDynArray(array, arena);
    }

    ZuiLabel *ZuiGetLabel(const ZuiDynArray *array, const unsigned int index)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Dynamic array pointer is NULL");
            return NULL;
        }

        if (!array->items || array->capacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NOT_INITIALIZED,
                             "Label array not initialized (items: %p, capacity: %u)",
                             array->items, array->capacity);
            return NULL;
        }

        if (index >= array->count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS,
                             "Label index %u out of bounds (count: %u)", index, array->count);
            return NULL;
        }

        return ZuiGetDynArray(array, index);
    }

    void ZuiClearLabel(ZuiDynArray *array)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Dynamic array pointer is NULL");
            return;
        }

        ZuiClearDynArray(array);
    }

    unsigned int ZuiCountLabel(ZuiDynArray const *array)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Dynamic array pointer is NULL");
            return 0;
        }

        return array->count;
    }

    Vector2 ZuiGetLabelSize(ZuiLabel const *label)
    {

        if (!label)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Label pointer is NULL");
            return (Vector2){0, 0};
        }

        if (label->data.font.texture.id == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Label font texture is not loaded");
            return (Vector2){0, 0};
        }

        if (label->data.fontSize <= 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Label font size <= 0");
            return (Vector2){0, 0};
        }

        return MeasureTextEx(label->data.font, label->data.text, label->data.fontSize, label->data.spacing);
    }

    void ZuiRenderLabel(void *item)
    {
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Label item is NULL");
            return;
        }

        const ZuiLabel *label = item;

        if (!label->isVisible)
        {
            return;
        }

        Vector2 position = (Vector2){label->data.bounds.x, label->data.bounds.y};

        if (label->isInsideWindow)
        {
            position = (Vector2){label->data.bounds.x + label->parent.x,
                                 label->data.bounds.y + label->parent.y};
        }

        Rectangle dest = (Rectangle){position.x, position.y, label->data.bounds.width, label->data.bounds.height};
        if (label->data.hasBackground)
        {
            DrawRectangleRec(dest, label->data.backgroundColor);
        }

        DrawTextEx(label->data.font, label->data.text, position, label->data.fontSize,
                   label->data.spacing, label->data.textColor);
    }

    void ZuiConfigureLabel(ZuiLabel *label, const ZuiLabelPreset *preset)
    {
        if (!label)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Label pointer is NULL");
            return;
        }

        if (!preset)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Label preset is NULL");
            return;
        }

        if (zui_has(preset->text))
        {
            if (!preset->text.value)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Label preset text.value pointer is NULL");
                return;
            }

            if (strlen(preset->text.value) >= ZUI_MAX_TEXT_LENGTH)
            {
                TraceLog(LOG_WARNING, "Text truncation. Preset text is bigger than ZUI_MAX_TEXT_LENGTH");
            }
            strncpy(label->data.text, preset->text.value, ZUI_MAX_TEXT_LENGTH - 1);
            label->data.text[ZUI_MAX_TEXT_LENGTH - 1] = '\0';
        }

        if (zui_has(preset->font))
        {
            if (preset->font.value.texture.id == 0)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Font texture is invalid");
                return;
            }
            label->data.font = preset->font.value;
        }

        if (zui_has(preset->bounds))
        {
            if (preset->bounds.value.width < 0 || preset->bounds.value.height < 0)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Bounds have negative dimensions");
                return;
            }
            label->data.bounds = preset->bounds.value;
        }

        if (zui_has(preset->textColor))
        {
            label->data.textColor = preset->textColor.value;
        }

        if (zui_has(preset->backgroundColor))
        {
            label->data.backgroundColor = preset->backgroundColor.value;
        }

        if (zui_has(preset->spacing))
        {
            label->data.spacing = preset->spacing.value;
        }

        if (zui_has(preset->fontSize))
        {
            if (preset->fontSize.value <= 0)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "fontSize must be > 0");
                return;
            }
            label->data.fontSize = preset->fontSize.value;
        }

        if (zui_has(preset->hasBackground))
        {
            label->data.hasBackground = preset->hasBackground.value;
        }
    }

    ZuiLabelPreset ZuiGetDefaultLabelPreset(void)
    {
        return (ZuiLabelPreset){

            .textColor = zui_some((Color){50, 50, 50, 255}),
            .backgroundColor = zui_some(BLANK),
            .spacing = zui_some(ZUI_DEFAULT_SPACING),
            .hasBackground = zui_some(false),
            .fontSize = zui_some((float)ZUI_BASE_FONT_SIZE),
        };
    }

    ZuiLabelId ZuiCreateLabel(const char *text, ZuiDynArray *array, const Font font, ZuiArena *arena)
    {

        if (!text || !array || !arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Invalid parameters to ZuiCreateLabel");
            return (ZuiLabelId){ZUI_ID_INVALID};
        }

        ZuiLabel *label = ZuiPushLabel(array, arena);
        if (!label)
        {
            TraceLog(LOG_ERROR, "ZUI Error: Failed to create label");
            return (ZuiLabelId){ZUI_ID_INVALID};
        }

        *label = (ZuiLabel){
            .data.font = font,
            .id = (ZuiLabelId){array->count - 1},
            .isVisible = true,
            .data.text = {0},
        };

        ZuiLabelPreset default_label = ZuiGetDefaultLabelPreset();
        ZuiConfigureLabel(label, &default_label);

        strncpy(label->data.text, text, ZUI_MAX_TEXT_LENGTH - 1);
        label->data.text[ZUI_MAX_TEXT_LENGTH - 1] = '\0';

        Vector2 size = ZuiGetLabelSize(label);
        label->data.bounds.width = size.x;
        label->data.bounds.height = size.y;

        return label->id;
    }

    bool ZuiValidateBounds(const Rectangle bounds)
    {
        if (bounds.width < 0 || bounds.height < 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Bounds have negative dimensions");
            return false;
        }
        return true;
    }

    bool ZuiValidateLabelId(const ZuiDynArray *array, const ZuiLabelId id)
    {
        if (!array)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Null array pointer passed to ZuiValidateLabelId");
            return false;
        }

        if (id.value >= array->count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS, "Invalid label ID");
            return false;
        }

        return true;
    }

    bool ZuiValidateSetLabelText(const ZuiLabelId id, const char *text, const ZuiDynArray *array)
    {
        if (!ZuiValidateLabelId(array, id))
        {
            return false;
        }

        if (!text)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER,
                             "Text pointer is NULL for label ID %u", id.value);
            return false;
        }

        size_t text_length = strlen(text);

        if (text_length > ZUI_MAX_TEXT_LENGTH)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE,
                             "Text length %zu exceeds max %d for label ID %u",
                             text_length, ZUI_MAX_TEXT_LENGTH - 1, id.value);
            return false;
        }

        return true;
    }

    bool ZuiValidateSetLabelFont(const ZuiLabelId id, const Font font, const ZuiDynArray *array)
    {
        if (!ZuiValidateLabelId(array, id))
        {
            return false;
        }

        // Runtime validation
        if (font.texture.id == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Font is not loaded");
            return false;
        }
        return true;
    }

    bool ZuiValidateSetLabelBounds(const ZuiLabelId id, const Rectangle bounds, const ZuiDynArray *array)
    {
        if (!ZuiValidateLabelId(array, id))
        {
            return false;
        }

        return ZuiValidateBounds(bounds);
    }

    void ZuiInternalPrintLabel(ZuiLabel *label)
    {
        if (!label)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Cannot print NULL label");
            return;
        }

        TraceLog(LOG_INFO, "                   ZUI LABEL DEBUG                     ");

        // Basic info
        TraceLog(LOG_INFO, "│  ID:         %d", label->id.value);
        TraceLog(LOG_INFO, "│  Visible:    %s", label->isVisible ? "true" : "false");
        TraceLog(LOG_INFO, "│  Text:       \"%s\"", label->data.text);
        TraceLog(LOG_INFO, "│  Parent:     (%.2F, %.2F)",
                 (double)label->parent.x, (double)label->parent.y);

        // Text data
        TraceLog(LOG_INFO, "│  Font Size:  %.2F", (double)label->data.fontSize);
        TraceLog(LOG_INFO, "│  Spacing:    %.2F", (double)label->data.spacing);
        TraceLog(LOG_INFO, "│  Text Color: (%d, %d, %d, %d)",
                 label->data.textColor.r,
                 label->data.textColor.g,
                 label->data.textColor.b,
                 label->data.textColor.a);
        TraceLog(LOG_INFO, "│  Has BG:     %s", label->data.hasBackground ? "true" : "false");

        if (label->data.hasBackground)
        {
            TraceLog(LOG_INFO, "│  BG Color:   (%d, %d, %d, %d)",
                     label->data.backgroundColor.r,
                     label->data.backgroundColor.g,
                     label->data.backgroundColor.b,
                     label->data.backgroundColor.a);
        }
        TraceLog(LOG_INFO, "└");

        // Bounds
        TraceLog(LOG_INFO, "┌Bounds");
        TraceLog(LOG_INFO, "│  Position:   (%.2F, %.2F)",
                 (double)label->data.bounds.x, (double)label->data.bounds.y);
        TraceLog(LOG_INFO, "│  Size:       %.2F x %.2F",
                 (double)label->data.bounds.width, (double)label->data.bounds.height);
        TraceLog(LOG_INFO, "└");

        // Font info
        TraceLog(LOG_INFO, "┌Font");
        TraceLog(LOG_INFO, "│  Texture ID: %d", label->data.font.texture.id);
        TraceLog(LOG_INFO, "│  Base Size:  %d", label->data.font.baseSize);
        TraceLog(LOG_INFO, "│  Glyph Cnt:  %d", label->data.font.glyphCount);
        TraceLog(LOG_INFO, "└");
    }

    /// ----------------------------------------------------------------------------Label

    void ZuiPrintLabel(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZuiInternalPrintLabel(ZuiGetLabel(&g_zui_ctx->labelArray, id.value));
        }
    }

    void ZuiSetLabelText(const ZuiLabelId id, const char *text)
    {
        if (ZuiValidateSetLabelText(id, text, &g_zui_ctx->labelArray))
        {
            ZuiLabel *label = ZuiGetLabel(&g_zui_ctx->labelArray, id.value);

            if (label == NULL)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Failed to get label");
                return;
            }

            strncpy(label->data.text, text, ZUI_MAX_TEXT_LENGTH - 1);
            label->data.text[ZUI_MAX_TEXT_LENGTH - 1] = '\0';
        }
    }

    void ZuiSetLabelFont(const ZuiLabelId id, const Font font)
    {
        if (ZuiValidateSetLabelFont(id, font, &g_zui_ctx->labelArray))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.font = font;
        }
    }

    void ZuiSetLabelTextColor(const ZuiLabelId id, const Color textColor)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.textColor = textColor;
        }
    }

    void ZuiSetLabelBackgroundColor(const ZuiLabelId id, const Color backgroundColor)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.backgroundColor = backgroundColor;
        }
    }

    void ZuiSetLabelBounds(const ZuiLabelId id, const Rectangle bounds)
    {
        if (ZuiValidateSetLabelBounds(id, bounds, &g_zui_ctx->labelArray))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.bounds = bounds;
        }
    }

    void ZuiSetLabelSpacing(const ZuiLabelId id, const unsigned int spacing)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.spacing = (float)spacing;
        }
    }

    void ZuiSetLabelFontSize(const ZuiLabelId id, const unsigned int fontSize)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.fontSize = (float)fontSize;
        }
    }

    void ZuiSetLabelHasBackground(const ZuiLabelId id, const bool hasBackground)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.hasBackground = hasBackground;
        }
    }

    void ZuiSetLabelIsVisible(const ZuiLabelId id, const bool isVisible)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->isVisible = isVisible;
        }
    }

    const char *ZuiGetLabelText(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            return ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.text;
        }

        return NULL;
    }

    Font ZuiGetLabelFont(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            return ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.font;
        }

        return (Font){0};
    }

    Color ZuiGetLabelTextColor(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            return ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.textColor;
        }

        return (Color){0};
    }

    Rectangle ZuiGetLabelBounds(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            return ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.bounds;
        }

        return (Rectangle){0};
    }

    float ZuiGetLabelSpacing(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            return ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.spacing;
        }

        return 0.0F;
    }

    float ZuiGetLabelFontSize(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            return ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->data.fontSize;
        }

        return 0.0F;
    }

    bool ZuiGetLabelIsVisible(const ZuiLabelId id)
    {
        if (ZuiValidateLabelId(&g_zui_ctx->labelArray, id))
        {
            return ZuiGetLabel(&g_zui_ctx->labelArray, id.value)->isVisible;
        }

        return false;
    }
    // -----------------------------------------------------------------------------

#endif // ZUI_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // ZUI_H
