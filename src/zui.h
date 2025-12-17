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
    // zui_core.h

#include "raylib.h"

    ///-----------------------------------------------------------------ZUI_ASSERT

#ifdef ZUI_DEBUG
#if defined(__APPLE__)

    // NOLINTBEGIN(misc-include-cleaner):
    // NOLINTEND(misc-include-cleaner)

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
            free((void *)strs);                                           \
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
            free((void *)strs);                                                           \
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
        ZUI_ERROR_INVALID_CAPACITY = 20,  // Capacity out of range
        ZUI_ERROR_INVALID_ALIGNMENT = 21, // Alignment not power of 2
        ZUI_ERROR_INVALID_ID = 22,        // Frame/Label ID out of bounds
        ZUI_ERROR_INVALID_BOUNDS = 23,    // Negative width/height
        ZUI_ERROR_INVALID_VALUE = 24,     // Invalid parameter value
        ZUI_ERROR_INVALID_ENUM = 25,      // Invalid enum value
        ZUI_ERROR_INVALID_STATE = 26,     // Invalid object state

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
        ZUI_MAX_TEXT_LENGTH = 64,
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
        ZUI_DEFAULT_ARENA_SIZE = 1048576, // 1MB
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
        ZUI_TEXTURE,
        ZUI_9SLICE_TEXTURE,
        ZUI_BUTTON,
        ZUI_HORIZONTAL_3PATCH_TEXTURE,
        ZUI_VERTICAL_3PATCH_TEXTURE,
        ZUI_NUMERIC_INPUT,
        ZUI_SLIDER,
        ZUI_KNOB,
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

    typedef enum ZuiPropertyType
    {
        ZUI_INT,
        ZUI_FLOAT
    } ZuiPropertyType;

    typedef struct ZuiPropertyInt
    {
        int value;
        int minValue;
        int maxValue;
        int step;
        bool hasMinMax;
        bool hasStep;
    } ZuiPropertyInt;

    typedef struct zuiPropertyFloat
    {
        float value;
        float minValue;
        float maxValue;
        float step;
        bool hasMinMax;
        bool hasStep;
    } zuiPropertyFloat;

    typedef struct ZuiProperty
    {
        ZuiPropertyType type;
        union
        {
            ZuiPropertyInt asInt;
            zuiPropertyFloat asFloat;
        };
    } ZuiProperty;

    ZuiOptionInt ZuiSomeInt(int v);

    bool ZuiHasInt(ZuiOptionInt o);

    ZuiOptionColor ZuiSomeColor(Color v);

    bool ZuiHasColor(ZuiOptionColor o);

    ZuiOptionString ZuiSomeString(const char *v);

    bool ZuiHasString(ZuiOptionString o);

    ZuiOptionVector2 ZuiSomeVector2(Vector2 v);

    bool ZuiHasVector2(ZuiOptionVector2 o);

    ZuiOptionNPatchInfo ZuiSomeNPatchInfo(NPatchInfo v);

    bool ZuiHasNPatchInfo(ZuiOptionNPatchInfo o);

    ZuiOptionTexture2D ZuiSomeTexture2D(Texture2D v);

    bool ZuiHasTexture2D(ZuiOptionTexture2D o);

    ZuiOptionRectangle ZuiSomeRectangle(Rectangle v);

    bool ZuiHasRectangle(ZuiOptionRectangle o);

    ZuiOptionFloat ZuiSomeFloat(float v);

    bool ZuiHasFloat(ZuiOptionFloat o);

    ZuiOptionFont ZuiSomeFont(Font v);

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

    bool ZuiIsPowerOfTwo(size_t n);

    const char *ZuiGetWidgetType(ZuiItemType type);

    const char *ZuiResultToString(ZuiResult result);

    float ZuiPixelsToRoundness(Rectangle rect, float radius_px);

    bool ZuiValidatePropertyPointer(const ZuiProperty *prop);

    float ZuiGetPropertyValueAsFloat(const ZuiProperty *prop);

    float ZuiGetPropertyMinAsFloat(const ZuiProperty *prop);

    float ZuiGetPropertyMaxAsFloat(const ZuiProperty *prop);

    float ZuiGetPropertyStepAsFloat(const ZuiProperty *prop);

    bool ZuiGetPropertyHasMinMax(const ZuiProperty *prop);

    bool ZuiGetPropertyHasStep(const ZuiProperty *prop);

    void ZuiSetPropertyValueFromFloat(ZuiProperty *prop, float value);

    size_t ZuiStrlen(const char *s, size_t max_len);

    bool ZuiValidateBounds(Rectangle bounds);

    // -----------------------------------------------------------------------------
    // zui_arena.h

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

#define ZUI_ARENA_ALLOC(arena, size) \
    ZuiAllocArenaDefault(arena, size)

#define ZUI_ARENA_ALLOC_TYPE(arena, type) \
    ((type *)ZuiAllocArena(arena, sizeof(type), ZUI_ALIGNOF(type)))

#define ZUI_ARENA_ALLOC_ARRAY(arena, type, count) \
    ((type *)ZuiAllocArena(arena, sizeof(type) * (count), ZUI_ALIGNOF(type)))

    ZuiResult ZuiInitArena(ZuiArena *arena, size_t capacity);

    void ZuiUnloadArena(ZuiArena *arena);

    void ZuiResetArena(ZuiArena *arena);

    void *ZuiAllocArena(ZuiArena *arena, size_t size, size_t alignment);

    void *ZuiAllocArenaDefault(ZuiArena *arena, size_t size);

    ZuiArenaStats ZuiGetArenaStats(const ZuiArena *arena);

    void ZuiPrintArenaStats(const ZuiArena *arena);

    bool ZuiIsArenaValid(const ZuiArena *arena);

    // -----------------------------------------------------------------------------
    // zui_dynarray.h

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

#define ZUI_DYNARRAY_PUSH_TYPE(array, arena, type) \
    ((type *)ZuiPushDynArray(array, arena))

#define ZUI_DYNARRAY_GET_TYPE(array, type, index) \
    ((type *)ZuiGetDynArray(array, index))

    ///----------------------------------------------------Dynamic Array

    ZuiResult ZuiInitDynArray(ZuiDynArray *array, ZuiArena *arena, unsigned int initialCapacity,
                              size_t itemSize, size_t itemAlignment, const char *typeName);

    ZuiResult ZuiGrowDynArray(ZuiDynArray *array, ZuiArena *arena);

    void *ZuiPushDynArray(ZuiDynArray *array, ZuiArena *arena);

    void *ZuiGetDynArray(const ZuiDynArray *array, unsigned int index);

    void ZuiClearDynArray(ZuiDynArray *array);

    bool ZuiIsDynArrayValid(const ZuiDynArray *array);

    ZuiDynArrayStats ZuiGetDynArrayStats(const ZuiDynArray *array);

    void ZuiPrintDynArrayStats(const ZuiDynArray *array);

    // -----------------------------------------------------------------------------
    // zui_item.h

    typedef void (*ZuiUpdateFunction)(unsigned int dataIndex);
    typedef void (*ZuiRenderFunction)(unsigned int dataIndex);

    typedef struct ZuiItem
    {
        unsigned int id;
        unsigned int parentId;
        ZuiItemType type;
        unsigned int dataIndex;
        ZuiRenderFunction renderFunction;
        ZuiUpdateFunction updateFunction;
        ZuiDynArray children;
        bool hasChildren;
        bool isVisible;
        bool isInsideWindow;
    } ZuiItem;

    const ZuiItem *ZuiGetItem(unsigned int id);

    ZuiItem *ZuiGetItemMut(unsigned int id);

    unsigned int ZuiCreateItem(ZuiItemType type, unsigned int dataIndex);

    ZuiResult ZuiItemAddChild(unsigned int parentId, unsigned int childId);

    void ZuiPrintItem(unsigned int id);

    void ZuiPrintItemTree(unsigned int id, int depth);

    void ZuiPrintFullItemTree(bool withData);

    void ZuiPrintItemTreeData(unsigned int id, int depth);

    // -----------------------------------------------------------------------------
    // zui_frame.h

#include "raylib.h"

    typedef struct ZuiFrameData
    {
        unsigned int itemId;
        Color backgroundColor;
        Color outlineColor;
        Rectangle bounds;
        float padding;
        float gap;
        float outlineThickness;
        bool hasBackground;
        bool hasOutline;
    } ZuiFrameData;

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
        unsigned int id;
        bool isWindowFrame;
    } ZuiItemOptions;

    unsigned int ZuiCreateRootFrame(Rectangle bounds, Color color);

    unsigned int ZuiCreateFrame(Rectangle bounds, Color color);

    const ZuiFrameData *ZuiGetFrameData(unsigned int itemId);

    ZuiFrameData *ZuiGetFrameDataMut(const unsigned int itemId);

    void ZuiUpdateFrame(unsigned int dataIndex);

    void ZuiRenderFrame(unsigned int dataIndex);

    Rectangle ZuiGetContentArea(Rectangle parent, ZuiPadding padding);

    Rectangle ZuiAlignRectangle(Rectangle parent, Vector2 childSize, ZuiPadding padding, ZuiAlignment alignment);

    Rectangle ZuiCalculateChildBounds(Rectangle childBounds, Rectangle bounds, ZuiItemOptions options);

    const char *ZuiGetAlignmentName(ZuiAlignment alignment);

    void ZuiPrintFrame(unsigned int id);

    unsigned int ZuiBeginFrame(Rectangle bounds, Color color);

    void ZuiEndFrame(void);

    unsigned int ZuiNewFrame(Color color, float width, float height);

    void ZuiFrameBackground(Color color);

    void ZuiFrameOutline(Color color, float thickness);

    void ZuiFrameGap(float gap);

    void ZuiFramePad(float pad);

    void ZuiFrameOffset(float x, float y);

    // -----------------------------------------------------------------------------
    // zui_label.h

#include "raylib.h"

    typedef struct ZuiLabelData
    {
        unsigned int itemId;
        // flawfinder: ignore (bounded by ZUI_MAX_TEXT_LENGTH +1, null-terminated)
        char text[ZUI_MAX_TEXT_LENGTH + 1];
        Font font;
        Rectangle bounds;
        Color textColor;
        Color backgroundColor;
        float spacing;
        float fontSize;
        bool hasBackground;
    } ZuiLabelData;

    unsigned int ZuiCreateLabel(const char *text, Font font);

    const ZuiLabelData *ZuiGetLabelData(unsigned int itemId);

    ZuiLabelData *ZuiGetLabelDataMut(unsigned int itemId);

    void ZuiRenderLabel(unsigned int dataIndex);

    void ZuiPrintLabel(unsigned int id);

    unsigned int ZuiLabelEx(const char *text, const bool isMono);

    unsigned int ZuiNewMonoLabel(const char *text);

    unsigned int ZuiNewLabel(const char *text);

    void ZuiLabelTextColor(Color textColor);

    void ZuiLabelBackgroundColor(Color backgroundColor);

    void ZuiLabelAlignX(ZuiAlignmentX align);

    void ZuiLabelOffset(float x, float y);

    // -----------------------------------------------------------------------------
    // zui_texture.h

#include "raylib.h"

    typedef struct ZuiTextureData
    {
        unsigned int itemId;
        Texture2D texture;
        NPatchInfo npatch;
        Rectangle bounds;
        Color color;
        bool isPatch;
    } ZuiTextureData;

    unsigned int ZuiCreateTexture(Texture2D texture);

    const ZuiTextureData *ZuiGetTextureData(unsigned int itemId);

    ZuiTextureData *ZuiGetTextureDataMut(unsigned int itemId);

    void ZuiRenderTexture(unsigned int dataIndex);

    void ZuiRenderPatchTexture(unsigned int dataIndex);

    void ZuiPrintTexture(unsigned int id);

    unsigned int ZuiNewTextureEx(Texture2D tex, NPatchInfo npatch, Rectangle bounds, bool isPatch);

    unsigned int ZuiNewTexture(Texture2D tex);

    unsigned int ZuiNew3XSlice(Texture2D tex, int width, int left, int right);

    unsigned int ZuiNew3YSlice(Texture2D tex, int height, int top, int bottom);

    unsigned int ZuiNew9Slice(Texture2D tex, int width, int height, int left, int top, int right, int bottom);
    // -----------------------------------------------------------------------------
    // zui_context.h

#include "raylib.h"

    typedef struct ZuiCursor
    {
        Vector2 position;
        Vector2 restPosition;
        Vector2 tempRestPosition;
        Rectangle lastItemBounds;
        unsigned int rootItem;
        unsigned int activeFrame;
        unsigned int parentFrame;
        unsigned int activeTexture;
        unsigned int activeLabel;
        int rowStartIndex;
        int rowEndIndex;
        bool isWindowRoot;
        bool isRow;
    } ZuiCursor;

    typedef struct ZuiContext
    {
        ZuiCursor cursor;
        ZuiDynArray items;

        ZuiDynArray frameData;
        ZuiDynArray textureData;
        ZuiDynArray labelData;

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

    void ZuiAdvanceCursor(float width, float height);
    void ZuiPrintCursor(void);
    void ZuiAdvanceLine(void);
    void ZuiPlaceAt(float x, float y);
    void ZuiOffset(float x, float y);
    void ZuiBeginRow(void);
    void ZuiEndRow(void);

    bool ZuiIsInitialized(void);
    ZuiContext *ZuiGetContext(void);
    void ZuiUpdate(void);
    void ZuiRender(void);
    bool ZuiInit(void);
    void ZuiExit(void);

    // -----------------------------------------------------------------------------

#ifdef ZUI_IMPLEMENTATION
// zui_arena.c
#include "raylib.h"
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
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Arena is in invalid state");
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

        const size_t current_offset = arena->offset;
        const size_t mask = alignment - 1;
        const size_t aligned_offset = (current_offset + mask) & ~mask;

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
// -----------------------------------------------------------------------------
// zui_context.c
#include "raylib.h"

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

    bool ZuiInit(void)
    {
        if (g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ALREADY_INITIALIZED, "ZUI context already exists");
            return false;
        }

        // Initialize global arena with 1MB - intentional large allocation for memory pool
        // This is a one-time allocation that serves as backing memory for all UI elements
        ZuiResult result = ZuiInitArena(&g_zui_arena, ZUI_DEFAULT_ARENA_SIZE);
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Arena initialization failed");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        ZuiContext *ctx = ZuiAllocArenaDefault(&g_zui_arena, sizeof(ZuiContext));
        if (!ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate ZuiContext");
            ZuiUnloadArena(&g_zui_arena);
            return false;
        }

        *ctx = (ZuiContext){0};
        g_zui_ctx = ctx;

        result = ZuiInitDynArray(&ctx->items, &g_zui_arena, ZUI_FRAMES_CAPACITY * 4,
                                 sizeof(ZuiItem), ZUI_ALIGNOF(ZuiItem), "Items");
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize items array");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        result = ZuiInitDynArray(&ctx->frameData, &g_zui_arena, ZUI_FRAMES_CAPACITY,
                                 sizeof(ZuiFrameData), ZUI_ALIGNOF(ZuiFrameData), "FrameData");
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize frameData array");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        result = ZuiInitDynArray(&ctx->labelData, &g_zui_arena, ZUI_LABELS_CAPACITY,
                                 sizeof(ZuiLabelData), ZUI_ALIGNOF(ZuiLabelData), "LabelData");
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize labelData array");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        result = ZuiInitDynArray(&ctx->textureData, &g_zui_arena, ZUI_TEXTURES_CAPACITY,
                                 sizeof(ZuiTextureData), ZUI_ALIGNOF(ZuiTextureData), "TextureData");
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize textureData array");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        g_zui_ctx->cursor = (ZuiCursor){0};

        int screen_width = GetScreenWidth();
        int screen_height = GetScreenHeight();

        if (screen_width <= 0 || screen_height <= 0)
        {
            TraceLog(LOG_WARNING, "ZUI: Invalid screen dimensions, using defaults");
            screen_width = ZUI_DEFAULT_SCREEN_WIDTH;
            screen_height = ZUI_DEFAULT_SCREEN_HEIGHT;
        }

        // Create root frame
        unsigned int root_item = ZuiCreateRootFrame(
            (Rectangle){0, 0, (float)screen_width, (float)screen_height}, BLANK);

        if (root_item == ZUI_ID_INVALID)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to create root frame");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        ZuiFrameData *root = ZuiGetFrameDataMut(root_item);
        if (!root)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to get root frame data");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        g_zui_ctx->cursor.position = (Vector2){root->padding, root->padding};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.activeFrame = root_item;
        g_zui_ctx->cursor.rootItem = root_item;

        Vector2 dpiScale = GetWindowScaleDPI();
        g_zui_ctx->dpiScale = (int)dpiScale.x;
        if (g_zui_ctx->dpiScale <= 0)
        {
            g_zui_ctx->dpiScale = ZUI_DEFAULT_DPI_SCALE;
        }

        g_zui_ctx->font = LoadFontEx("src/resources/Inter_18pt-Regular.ttf",
                                     ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);
        if (g_zui_ctx->font.texture.id == 0)
        {
            g_zui_ctx->font = GetFontDefault();
        }

        g_zui_ctx->monoFont = LoadFontEx("src/resources/Inconsolata-Regular.ttf",
                                         ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);
        if (g_zui_ctx->monoFont.texture.id == 0)
        {
            g_zui_ctx->monoFont = GetFontDefault();
        }

        TraceLog(LOG_INFO, "ZUI: Initialized successfully");
        return true;
    }

    void ZuiUpdate(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Context not initialized");
            return;
        }

        ZuiItem *root = ZuiGetItemMut(g_zui_ctx->cursor.rootItem);
        if (root && root->updateFunction)
        {
            root->updateFunction(root->dataIndex);
        }
    }

    void ZuiRender(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Context not initialized");
            return;
        }

        ZuiItem *root = ZuiGetItemMut(g_zui_ctx->cursor.rootItem);
        if (root && root->renderFunction)
        {
            root->renderFunction(root->dataIndex);
        }
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

    void ZuiAdvanceCursor(const float width, const float height)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        if (width < 0.0F || height < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Negative dimensions");
            return;
        }

        const ZuiFrameData *activeFrameData = ZuiGetFrameData(g_zui_ctx->cursor.activeFrame);
        if (!activeFrameData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid active frame");
            return;
        }

        Vector2 size = (Vector2){width, height};
        if (g_zui_ctx->cursor.isRow)
        {
            g_zui_ctx->cursor.position.x += size.x + activeFrameData->gap;
        }
        else
        {
            g_zui_ctx->cursor.position.x = g_zui_ctx->cursor.restPosition.x;
            g_zui_ctx->cursor.tempRestPosition.x = g_zui_ctx->cursor.position.x;
            g_zui_ctx->cursor.position.y += size.y + activeFrameData->gap;
        }
    }

    void ZuiPrintCursor(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        TraceLog(LOG_INFO, "[ZUI] Cursor position: x=%.1F, y=%.1F, activeFrame=%u",
                 (double)g_zui_ctx->cursor.position.x,
                 (double)g_zui_ctx->cursor.position.y,
                 g_zui_ctx->cursor.activeFrame);
    }

    void ZuiAdvanceLine(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        const ZuiFrameData *activeFrameData = ZuiGetFrameData(g_zui_ctx->cursor.activeFrame);
        if (!activeFrameData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid active frame");
            return;
        }

        // Advance to next line
        g_zui_ctx->cursor.position.x = g_zui_ctx->cursor.restPosition.x;
        g_zui_ctx->cursor.tempRestPosition.x = g_zui_ctx->cursor.position.x;
        g_zui_ctx->cursor.position.y += g_zui_ctx->cursor.lastItemBounds.height + activeFrameData->gap;
        g_zui_ctx->cursor.tempRestPosition.y = g_zui_ctx->cursor.position.y;
    }

    void ZuiPlaceAt(const float x, const float y)
    {
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
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        g_zui_ctx->cursor.position.x += x;
        g_zui_ctx->cursor.position.y += y;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }
    // -----------------------------------------------------------------------------

#include "math.h"
#include "raylib.h"

    ///------------------------------------------------------------Internal Utility Functions

    bool ZuiIsPowerOfTwo(const size_t n)
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
        case ZUI_TEXTURE:
            return "texture";
        case ZUI_HORIZONTAL_3PATCH_TEXTURE:
            return "horizontal 3patch texture";
        case ZUI_VERTICAL_3PATCH_TEXTURE:
            return "vertical 3patch texture";
        case ZUI_9SLICE_TEXTURE:
            return "9 slice texture";
        case ZUI_NUMERIC_INPUT:
            return "numerical input";
        case ZUI_BUTTON:
            return "button";
        case ZUI_SLIDER:
            return "slider";
        case ZUI_KNOB:
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
        case ZUI_ERROR_INVALID_STATE:
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
        const float min_dimension = rect.width < rect.height ? rect.width : rect.height;

        // Handle zero dimension to avoid division by zero
        if (min_dimension <= 0.0F)
        {
            return 0.0F;
        }

        const float max_radius = min_dimension * 0.5F;

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

    //-------------------------------------------------Property

    bool ZuiValidatePropertyPointer(const ZuiProperty *prop)
    {

        if (prop == NULL)
        {
            TraceLog(LOG_INFO, "ZUI INFO: Property pointer is NULL\n");
            return false;
        }
        return true;
    }

    float ZuiGetPropertyValueAsFloat(const ZuiProperty *prop)
    {

        if (!ZuiValidatePropertyPointer(prop))
        {
            return 0.0F;
        }

        switch (prop->type)
        {
        case ZUI_INT:
            return (float)prop->asInt.value;
        case ZUI_FLOAT:
            return prop->asFloat.value;
        default:
            // Handle unexpected types - return 0.0f or use assert/fallback
            ZUI_ASSERT(false, "ZUI ERROR: Unsupported property type");
            return 0.0F;
        }
    }

    float ZuiGetPropertyMinAsFloat(const ZuiProperty *prop)
    {
        if (!ZuiValidatePropertyPointer(prop))
        {
            return 0.0F;
        }

        switch (prop->type)
        {
        case ZUI_INT:
            return (float)prop->asInt.minValue;
        case ZUI_FLOAT:
            return prop->asFloat.minValue;
        default:
            // Handle unexpected types - return 0.0f or use assert/fallback
            ZUI_ASSERT(false, "ZUI ERROR: Unsupported property type");
            return 0.0F;
        }
    }

    float ZuiGetPropertyMaxAsFloat(const ZuiProperty *prop)
    {
        if (!ZuiValidatePropertyPointer(prop))
        {
            return 0.0F;
        }

        switch (prop->type)
        {
        case ZUI_INT:
            return (float)prop->asInt.maxValue;
        case ZUI_FLOAT:
            return prop->asFloat.maxValue;
        default:
            // Handle unexpected types - return 0.0f or use assert/fallback
            ZUI_ASSERT(false, "ZUI ERROR: Unsupported property type");
            return 0.0F;
        }
    }

    float ZuiGetPropertyStepAsFloat(const ZuiProperty *prop)
    {
        if (!ZuiValidatePropertyPointer(prop))
        {
            return 0.0F;
        }

        switch (prop->type)
        {
        case ZUI_INT:
            return (float)prop->asInt.step;
        case ZUI_FLOAT:
            return prop->asFloat.step;
        default:
            // Handle unexpected types - return 0.0f or use assert/fallback
            ZUI_ASSERT(false, "ZUI ERROR: Unsupported property type");
            return 0.0F;
        }
    }

    bool ZuiGetPropertyHasMinMax(const ZuiProperty *prop)
    {
        if (!ZuiValidatePropertyPointer(prop))
        {
            return false;
        }

        switch (prop->type)
        {
        case ZUI_INT:
            // NOLINTNEXTLINE(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
            return prop->asInt.hasMinMax;
        case ZUI_FLOAT:
            // NOLINTNEXTLINE(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
            return prop->asFloat.hasMinMax;
        default:
            // Handle unexpected types - return 0.0f or use assert/fallback
            ZUI_ASSERT(false, "ZUI ERROR: Unsupported property type");
            return false;
        }
    }

    bool ZuiGetPropertyHasStep(const ZuiProperty *prop)
    {
        if (!ZuiValidatePropertyPointer(prop))
        {
            return false;
        }

        switch (prop->type)
        {
        case ZUI_INT:
            // NOLINTNEXTLINE(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
            return prop->asInt.hasStep;
        case ZUI_FLOAT:
            // NOLINTNEXTLINE(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions)
            return prop->asFloat.hasStep;
        default:
            // Handle unexpected types - return 0.0f or use assert/fallback
            ZUI_ASSERT(false, "ZUI ERROR: Unsupported property type");
            return false;
        }
    }

    void ZuiSetPropertyValueFromFloat(ZuiProperty *prop, float value)
    {
        if (!ZuiValidatePropertyPointer(prop))
        {
            return;
        }

        switch (prop->type)
        {
        case ZUI_INT:
            prop->asInt.value = (int)roundf(value);
            break;
        case ZUI_FLOAT:
            prop->asFloat.value = value;
            break;
        default:
            return;
        }
    }

    size_t ZuiStrlen(const char *s, size_t max_len)
    {
        if (!s)
        {
            return 0;
        }

        const char *end = s;
        while (*end && max_len > 0)
        {
            end++;
            max_len--;
        }

        return (size_t)(end - s);
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
// -----------------------------------------------------------------------------
// zui_dynarray.c
#include "raylib.h"

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

        if (itemSize == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE,
                             "%s array item size must be > 0", typeName);
            return ZUI_ERROR_INVALID_VALUE;
        }

        if (!ZuiIsPowerOfTwo(itemAlignment))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ALIGNMENT,
                             "%s array item alignment %zu must be power of 2",
                             typeName, itemAlignment);
            return ZUI_ERROR_INVALID_ALIGNMENT;
        }

        if (itemSize > SIZE_MAX / initialCapacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW,
                             "%s array allocation size would overflow", typeName);
            return ZUI_ERROR_OVERFLOW;
        }

        size_t required_bytes = itemSize * initialCapacity;

        if (!ZuiIsArenaValid(arena))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Arena is invalid");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        if (required_bytes > arena->capacity - arena->offset)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ARENA_EXHAUSTED,
                             "Not enough arena space for %s array: need %zu, have %zu",
                             typeName, required_bytes, arena->capacity - arena->offset);
            return ZUI_ERROR_ARENA_EXHAUSTED;
        }

        void *items = ZuiAllocArena(arena, required_bytes, itemAlignment);
        // occurs if ZuiAllocArena returns NULL even when the arena has sufficient capacity
        // This would require either a memory fault or a bug in the arena allocator itself.
        if (!items)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ALLOCATION_FAILED,
                             "Failed to allocate %s array from arena", typeName);
            return ZUI_ERROR_ALLOCATION_FAILED;
        }

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
        if (!array || !arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Invalid parameters to ZuiGrowDynArray");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!array->items && array->count > 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Array items is NULL but count > 0");
            return ZUI_ERROR_INVALID_STATE;
        }

        if (array->capacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Array capacity is 0 (not initialized?)");
            return ZUI_ERROR_INVALID_STATE;
        }

        if (array->capacity > UINT_MAX / 2)
        {
            TraceLog(LOG_ERROR, "ZUI: DynArray capacity would overflow on growth");
            return ZUI_ERROR_OVERFLOW;
        }

        unsigned int new_capacity = array->capacity * 2;

        if (new_capacity > ZUI_MAX_DYNARRAY_CAPACITY)
        {
            TraceLog(LOG_ERROR, "ZUI: DynArray exceeds max capacity %u", ZUI_MAX_DYNARRAY_CAPACITY);
            return ZUI_ERROR_OVERFLOW;
        }

        if (new_capacity > SIZE_MAX / array->itemSize)
        {
            TraceLog(LOG_ERROR, "ZUI: DynArray byte allocation would overflow");
            return ZUI_ERROR_OVERFLOW;
        }

        size_t new_bytes = (size_t)new_capacity * array->itemSize;

        size_t current_bytes = 0;
        if (array->count > 0)
        {
            current_bytes = (size_t)array->count * array->itemSize;
        }

        TraceLog(LOG_INFO, "ZUI: Growing dynarray from %u to %u capacity (%zu to %zu bytes)",
                 array->capacity, new_capacity, current_bytes, new_bytes);

        void *new_items = ZuiAllocArena(arena, new_bytes, _Alignof(max_align_t));
        if (!new_items)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to allocate memory for dynarray growth");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        if (current_bytes > 0) // GUARD: Only copy if there's data
        {
            ZUI_ASSERT(current_bytes <= new_bytes, "Buffer overflow in dynarray growth");
            memcpy(new_items, array->items, current_bytes); /* Flawfinder: ignore */
        }

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
// -----------------------------------------------------------------------------
// zui_frame.c
#include "raylib.h"

    const ZuiFrameData *ZuiGetFrameData(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_FRAME)
        {
            return NULL;
        }

        unsigned int dataIndex = (unsigned int)(uintptr_t)item->dataIndex;
        return (ZuiFrameData *)ZuiGetDynArray(&g_zui_ctx->frameData, dataIndex);
    }

    ZuiFrameData *ZuiGetFrameDataMut(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_FRAME)
        {
            return NULL;
        }

        unsigned int dataIndex = (unsigned int)(uintptr_t)item->dataIndex;
        return (ZuiFrameData *)ZuiGetDynArray(&g_zui_ctx->frameData, dataIndex);
    }

    unsigned int ZuiCreateRootFrame(const Rectangle bounds, const Color color)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        if (!ZuiValidateBounds(bounds))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_BOUNDS, "Invalid bounds");
            return ZUI_ID_INVALID;
        }

        ZuiFrameData *frameData =
            (ZuiFrameData *)ZuiPushDynArray(&g_zui_ctx->frameData, &g_zui_arena);
        if (!frameData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate frame data");
            return ZUI_ID_INVALID;
        }

        unsigned int dataIndex = g_zui_ctx->frameData.count - 1;

        unsigned int item_id = ZuiCreateItem(ZUI_FRAME, dataIndex);
        if (item_id == ZUI_ID_INVALID)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to create item");
            return ZUI_ID_INVALID;
        }

        *frameData = (ZuiFrameData){
            .itemId = item_id,
            .bounds = bounds,
            .backgroundColor = color,
            .hasBackground = (color.a > 0),
            .padding = ZUI_DEFAULT_FRAME_PADDING,
            .gap = ZUI_DEFAULT_FRAME_GAP,
            .hasOutline = false,
            .outlineThickness = 0.0F,
            .outlineColor = BLACK,
        };

        ZuiItem *item = ZuiGetItemMut(item_id);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INTERNAL_ERROR, "Failed to get item");
            return ZUI_ID_INVALID;
        }

        item->renderFunction = ZuiRenderFrame;
        item->updateFunction = ZuiUpdateFrame;
        item->isVisible = true;
        item->isInsideWindow = false;
        item->hasChildren = true;

        ZuiResult result = ZuiInitDynArray(
            &item->children,
            &g_zui_arena,
            ZUI_FRAMES_CAPACITY,
            sizeof(unsigned int),
            _Alignof(unsigned int),
            "Children");

        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to initialize children");
            return ZUI_ID_INVALID;
        }

        return item_id;
    }

    unsigned int ZuiCreateFrame(const Rectangle bounds, const Color color)
    {
        return ZuiCreateRootFrame(bounds, color);
    }

    void ZuiUpdateFrame(unsigned int dataIndex)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        const ZuiFrameData *frameItems = (ZuiFrameData *)g_zui_ctx->frameData.items;
        const ZuiFrameData *frameData = &frameItems[dataIndex];
        const ZuiItem *frameItem = ZuiGetItemMut(frameData->itemId);
        if (!frameItem || !frameItem->hasChildren)
        {
            return;
        }

        const unsigned int *childIds = (unsigned int *)frameItem->children.items;
        for (unsigned int i = 0; i < frameItem->children.count; i++)
        {
            const ZuiItem *child = ZuiGetItem(childIds[i]);
            if (child && child->updateFunction)
            {
                child->updateFunction(child->dataIndex);
            }
        }
    }

    void ZuiRenderFrame(unsigned int dataIndex)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        const ZuiFrameData *frameItems = (ZuiFrameData *)g_zui_ctx->frameData.items;
        const ZuiFrameData *frameData = &frameItems[dataIndex];
        const ZuiItem *frameItem = ZuiGetItem(frameData->itemId);
        if (!frameItem)
        {
            return;
        }

        const float roundness = ZuiPixelsToRoundness(frameData->bounds, ZUI_CORNER_RADIUS);

        if (frameData->hasBackground)
        {
            DrawRectangleRounded(frameData->bounds, roundness,
                                 ZUI_ROUNDNESS_SEGMENTS,
                                 frameData->backgroundColor);
        }

        if (frameData->hasOutline)
        {
            DrawRectangleRoundedLinesEx(frameData->bounds, roundness,
                                        ZUI_ROUNDNESS_SEGMENTS,
                                        frameData->outlineThickness,
                                        frameData->outlineColor);
        }

        if (frameItem->hasChildren)
        {
            const unsigned int *childIds = (unsigned int *)frameItem->children.items;

            for (unsigned int i = 0; i < frameItem->children.count; i++)
            {
                const ZuiItem *child = ZuiGetItem(childIds[i]);
                if (child && child->renderFunction)
                {
                    child->renderFunction(child->dataIndex);
                }
            }
        }
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
        const Rectangle content = ZuiGetContentArea(parent, padding);
        Rectangle child = {0};
        child.width = childSize.x;
        child.height = childSize.y;

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

    Rectangle ZuiCalculateChildBounds(const Rectangle childBounds, const Rectangle bounds, const ZuiItemOptions options)
    {
        Vector2 position = (Vector2){childBounds.x, childBounds.y};
        Vector2 size = (Vector2){childBounds.width, childBounds.height};

        if (options.placement == ZUI_PLACE_RELATIVE)
        {
            position.x += bounds.x;
            position.y += bounds.y;
        }

        if (options.placement == ZUI_PLACE_ALIGN)
        {
            Rectangle new = ZuiAlignRectangle(
                bounds, size,
                (ZuiPadding){ZUI_DEFAULT_FRAME_PADDING, ZUI_DEFAULT_FRAME_PADDING,
                             ZUI_DEFAULT_FRAME_PADDING, ZUI_DEFAULT_FRAME_PADDING},
                options.alignment);
            position = (Vector2){new.x, new.y};
        }

        if (options.placement == ZUI_PLACE_CURSOR)
        {
            position = options.cursor;
        }

        return (Rectangle){position.x, position.y, size.x, size.y};
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

    void ZuiPrintFrame(const unsigned int id)
    {
        const ZuiItem *item = ZuiGetItem(id);
        if (!item || item->type != ZUI_FRAME)
        {
            TraceLog(LOG_ERROR, "Item %u is not a frame", id);
            return;
        }

        const ZuiFrameData *frame = ZuiGetFrameData(id);
        if (!frame)
        {
            TraceLog(LOG_ERROR, "Failed to get frame data for item %u", id);
            return;
        }

        TraceLog(LOG_INFO, "┌─────── FRAME DATA ───────");
        TraceLog(LOG_INFO, "│ Item ID:      %u", frame->itemId);
        TraceLog(LOG_INFO, "│ Bounds:       (%.1f, %.1f, %.1fx%.1f)",
                 (double)frame->bounds.x, (double)frame->bounds.y,
                 (double)frame->bounds.width, (double)frame->bounds.height);
        TraceLog(LOG_INFO, "│ Padding:      %.1f", (double)frame->padding);
        TraceLog(LOG_INFO, "│ Gap:          %.1f", (double)frame->gap);
        TraceLog(LOG_INFO, "│ Has BG:       %s", frame->hasBackground ? "yes" : "no");
        if (frame->hasBackground)
        {
            TraceLog(LOG_INFO, "│ BG Color:     (%d, %d, %d, %d)",
                     frame->backgroundColor.r, frame->backgroundColor.g,
                     frame->backgroundColor.b, frame->backgroundColor.a);
        }
        TraceLog(LOG_INFO, "│ Has Outline:  %s", frame->hasOutline ? "yes" : "no");
        if (frame->hasOutline)
        {
            TraceLog(LOG_INFO, "│ Outline:      (%d, %d, %d, %d) thickness=%.1f",
                     frame->outlineColor.r, frame->outlineColor.g,
                     frame->outlineColor.b, frame->outlineColor.a,
                     (double)frame->outlineThickness);
        }
        TraceLog(LOG_INFO, "└──────────────────────────");
    }

    void ZuiBeginRow(void)
    {
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

    unsigned int ZuiBeginFrame(const Rectangle bounds, const Color color)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        g_zui_ctx->cursor.isRow = false;
        unsigned int id = ZuiCreateFrame(bounds, color);

        if (id == ZUI_ID_INVALID)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to create frame");
            return ZUI_ID_INVALID;
        }

        ZuiFrameData *frameData = ZuiGetFrameDataMut(id);
        if (!frameData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to get frame data");
            return ZUI_ID_INVALID;
        }

        frameData->bounds = bounds;
        unsigned int active = g_zui_ctx->cursor.activeFrame;
        ZuiItemAddChild(active, id);

        g_zui_ctx->cursor.parentFrame = g_zui_ctx->cursor.activeFrame;
        float pad = frameData->padding;
        g_zui_ctx->cursor.position = (Vector2){bounds.x + pad, bounds.y + pad};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.activeFrame = id;
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

    unsigned int ZuiNewFrame(const Color color, const float width, const float height)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        Rectangle bounds = (Rectangle){g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, width, height};
        unsigned int id = ZuiCreateFrame(bounds, color);

        ZuiFrameData *frameData = ZuiGetFrameDataMut(id);
        if (!frameData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to get frame data");
            return ZUI_ID_INVALID;
        }

        frameData->bounds = bounds;

        unsigned int active = g_zui_ctx->cursor.activeFrame;
        ZuiItemAddChild(active, id);

        ZuiAdvanceCursor(frameData->bounds.width, frameData->bounds.height);
        g_zui_ctx->cursor.lastItemBounds = frameData->bounds;

        return id;
    }

    void ZuiFrameBackground(const Color color)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        ZuiFrameData *frameData = ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->backgroundColor = color;
            frameData->hasBackground = (color.a > 0);
        }
    }

    void ZuiFrameOutline(const Color color, const float thickness)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        ZuiFrameData *frameData = ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->outlineColor = color;
            frameData->outlineThickness = thickness;
            frameData->hasOutline = (color.a > 0);
        }
    }

    void ZuiFrameGap(const float gap)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        ZuiFrameData *frameData = ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->gap = gap;
        }
    }

    void ZuiFramePad(const float pad)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        ZuiFrameData *frameData = ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->padding = pad;
            g_zui_ctx->cursor.position = (Vector2){frameData->bounds.x + pad, frameData->bounds.y + pad};
            g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
            g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
        }
    }

    void ZuiFrameOffset(const float x, const float y)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        ZuiFrameData *frameData = ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->bounds.x += x;
            frameData->bounds.y += y;
        }
    }
// -----------------------------------------------------------------------------
// zui_item.c
#include "raylib.h"

    const ZuiItem *ZuiGetItem(unsigned int id)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return NULL;
        }

        if (id >= g_zui_ctx->items.count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS,
                             "Item index %u out of bounds (count: %u)", id, g_zui_ctx->items.count);
            return NULL;
        }

        return (ZuiItem *)ZuiGetDynArray(&g_zui_ctx->items, id);
    }

    ZuiItem *ZuiGetItemMut(unsigned int id)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return NULL;
        }

        if (id >= g_zui_ctx->items.count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS,
                             "Item index %u out of bounds (count: %u)", id, g_zui_ctx->items.count);
            return NULL;
        }

        return (ZuiItem *)ZuiGetDynArray(&g_zui_ctx->items, id);
    }

    ZuiResult ZuiItemAddChild(const unsigned int parentId, const unsigned int childId)
    {
        ZuiItem *parent = ZuiGetItemMut(parentId);
        ZuiItem *child = ZuiGetItemMut(childId);

        if (!parent || !child)
        {
            return ZUI_ERROR_INVALID_ID;
        }

        if (!parent->hasChildren)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_CIRCULAR_REFERENCE,
                             "Item type %s is not a container", ZuiGetWidgetType(parent->type));
            return ZUI_ERROR_CIRCULAR_REFERENCE;
        }

        unsigned int *childIdSlot = (unsigned int *)ZuiPushDynArray(&parent->children, &g_zui_arena);
        if (!childIdSlot)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to add child");
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        *childIdSlot = childId;
        child->parentId = parentId;

        return ZUI_OK;
    }

    unsigned int ZuiCreateItem(const ZuiItemType type, const unsigned int dataIndex)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        ZuiItem *item = (ZuiItem *)ZuiPushDynArray(&g_zui_ctx->items, &g_zui_arena);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate item");
            return ZUI_ID_INVALID;
        }

        unsigned int item_id = g_zui_ctx->items.count - 1;

        *item = (ZuiItem){
            .id = item_id,
            .parentId = ZUI_ID_INVALID,
            .type = type,
            .dataIndex = dataIndex,
            .renderFunction = NULL,
            .updateFunction = NULL,
            .children = (ZuiDynArray){0},
            .hasChildren = false,
            .isVisible = true,
            .isInsideWindow = false,
        };

        return item_id;
    }

    void ZuiPrintItem(const unsigned int id)
    {
        ZuiItem *item = ZuiGetItemMut(id);
        if (!item)
        {
            TraceLog(LOG_ERROR, "Invalid item ID: %u", id);
            return;
        }

        TraceLog(LOG_INFO, "---------------- ZUI ITEM DEBUG ----------------");
        TraceLog(LOG_INFO, "│  Item ID:    %u", item->id);
        TraceLog(LOG_INFO, "│  Type:       %s", ZuiGetWidgetType(item->type));
        TraceLog(LOG_INFO, "│  Parent ID:  %u", item->parentId);
        TraceLog(LOG_INFO, "│  Visible:    %s", item->isVisible ? "true" : "false");

        if (item->hasChildren)
        {
            TraceLog(LOG_INFO, "│  Children:   %u / %u",
                     item->children.count, item->children.capacity);

            if (item->children.count > 0)
            {
                unsigned int *childIds = (unsigned int *)item->children.items;
                for (unsigned int i = 0; i < item->children.count; i++)
                {
                    const ZuiItem *child = ZuiGetItemMut(childIds[i]);
                    TraceLog(LOG_INFO, "│    [%u] Item %u (%s)",
                             i, childIds[i], child ? ZuiGetWidgetType(child->type) : "invalid");
                }
            }
        }
        else
        {
            TraceLog(LOG_INFO, "│  Children:   (not a container)");
        }

        TraceLog(LOG_INFO, "-------------------------------------------------");
    }

    void ZuiPrintItemTree(const unsigned int id, const int depth)
    {
        ZuiItem *item = ZuiGetItemMut(id);
        if (!item)
        {
            return;
        }

        // Indentation
        for (int i = 0; i < depth; i++)
        {
            TraceLog(LOG_INFO, "  ");
        }

        TraceLog(LOG_INFO, "├─ Item %u: %s (parent: %u, visible: %s)",
                 item->id,
                 ZuiGetWidgetType(item->type),
                 item->parentId,
                 item->isVisible ? "yes" : "no");

        if (item->children.count > 0)
        {
            const unsigned int *childIds = (unsigned int *)item->children.items;
            for (unsigned int i = 0; i < item->children.count; i++)
            {
                const ZuiItem *child = ZuiGetItem(childIds[i]);
                TraceLog(LOG_INFO, "│    [%u] Item %u (%s)",
                         i, childIds[i], child ? ZuiGetWidgetType(child->type) : "invalid");
            }
        }
    }

    void ZuiPrintItemTreeData(const unsigned int id, const int depth)
    {
        const ZuiItem *item = ZuiGetItem(id);
        if (!item)
        {
            return;
        }

        // Print indentation
        for (int i = 0; i < depth; i++)
        {
            TraceLog(LOG_INFO, "  ");
        }

        // Print item info
        TraceLog(LOG_INFO, "├─ Item %u: %s (parent: %u, visible: %s)",
                 item->id,
                 ZuiGetWidgetType(item->type),
                 item->parentId,
                 item->isVisible ? "yes" : "no");

        // Print type-specific data with extra indentation
        for (int i = 0; i < depth + 1; i++)
        {
            TraceLog(LOG_INFO, "  ");
        }

        switch (item->type)
        {
        case ZUI_FRAME:
        {
            const ZuiFrameData *frame = ZuiGetFrameData(id);
            if (frame)
            {
                TraceLog(LOG_INFO, "│  Frame: bounds=(%.0f,%.0f,%.0fx%.0f) pad=%.0f gap=%.0f",
                         (double)frame->bounds.x, (double)frame->bounds.y,
                         (double)frame->bounds.width, (double)frame->bounds.height,
                         (double)frame->padding, (double)frame->gap);
            }
            break;
        }
        case ZUI_LABEL:
        {
            const ZuiLabelData *label = ZuiGetLabelData(id);
            if (label)
            {
                TraceLog(LOG_INFO, "│  Label: \"%s\" size=%.0f pos=(%.0f,%.0f)",
                         label->text, (double)label->fontSize,
                         (double)label->bounds.x, (double)label->bounds.y);
            }
            break;
        }
        case ZUI_TEXTURE:
        {
            const ZuiTextureData *texture = ZuiGetTextureData(id);
            if (texture)
            {
                TraceLog(LOG_INFO, "│  Texture: %dx%d %s pos=(%.0f,%.0f)",
                         texture->texture.width, texture->texture.height,
                         texture->isPatch ? "patch" : "normal",
                         (double)texture->bounds.x, (double)texture->bounds.y);
            }
            break;
        }
        default:
            TraceLog(LOG_INFO, "│  (unknown type)");
            break;
        }

        // Recursively print children
        if (item->hasChildren && item->children.count > 0)
        {
            const unsigned int *childIds = (unsigned int *)item->children.items;
            for (unsigned int i = 0; i < item->children.count; i++)
            {
                ZuiPrintItemTreeData(childIds[i], depth + 1);
            }
        }
    }

    void ZuiPrintFullItemTree(bool withData)
    {
        if (!g_zui_ctx)
        {
            TraceLog(LOG_ERROR, "ZUI context not initialized");
            return;
        }

        TraceLog(LOG_INFO, "================ FULL ITEM TREE ================");
        TraceLog(LOG_INFO, "Total items: %u", g_zui_ctx->items.count);

        // Print from root (item 0)
        if (g_zui_ctx->items.count > 0)
        {
            for (unsigned int i = 0; i < g_zui_ctx->items.count - 1; ++i)
            {
                if (withData)
                {
                    ZuiPrintItemTreeData(i, 0);
                }
                else
                {
                    ZuiPrintItemTree(i, 0);
                }
            }
        }

        TraceLog(LOG_INFO, "================================================");
    }
// -----------------------------------------------------------------------------
// zui_label.c
#include "raylib.h"

    const ZuiLabelData *ZuiGetLabelData(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_LABEL)
        {
            return NULL;
        }

        unsigned int dataIndex = (unsigned int)(uintptr_t)item->dataIndex;
        return (ZuiLabelData *)ZuiGetDynArray(&g_zui_ctx->labelData, dataIndex);
    }

    ZuiLabelData *ZuiGetLabelDataMut(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_LABEL)
        {
            return NULL;
        }

        unsigned int dataIndex = (unsigned int)(uintptr_t)item->dataIndex;
        return (ZuiLabelData *)ZuiGetDynArray(&g_zui_ctx->labelData, dataIndex);
    }

    unsigned int ZuiCreateLabel(const char *text, const Font font)
    {
        if (!g_zui_ctx || !text)
        {
            return ZUI_ID_INVALID;
        }

        ZuiLabelData *labelData =
            (ZuiLabelData *)ZuiPushDynArray(&g_zui_ctx->labelData, &g_zui_arena);
        if (!labelData)
        {
            return ZUI_ID_INVALID;
        }

        unsigned int dataIndex = g_zui_ctx->labelData.count - 1;
        unsigned int item_id = ZuiCreateItem(ZUI_LABEL, dataIndex);
        if (item_id == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        *labelData = (ZuiLabelData){
            .itemId = item_id,
            .font = font,
            .textColor = BLACK,
            .backgroundColor = BLANK,
            .fontSize = ZUI_BASE_FONT_SIZE,
            .spacing = 0.0F,
            .hasBackground = false,
            .bounds = {0, 0, 0, 0},
        };

        snprintf(labelData->text, ZUI_MAX_TEXT_LENGTH, "%s", text);

        Vector2 size =
            MeasureTextEx(font, text,
                          labelData->fontSize,
                          labelData->spacing);

        labelData->bounds.width = size.x;
        labelData->bounds.height = size.y;

        ZuiItem *item = ZuiGetItemMut(item_id);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INTERNAL_ERROR,
                             "Failed to retrieve item after creation");
            return ZUI_ID_INVALID;
        }
        item->renderFunction = ZuiRenderLabel;
        item->updateFunction = NULL;
        item->hasChildren = false;

        return item_id;
    }

    void ZuiRenderLabel(const unsigned int dataIndex)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        const ZuiLabelData *labelItems = (ZuiLabelData *)g_zui_ctx->labelData.items;
        const ZuiLabelData *labelData = &labelItems[dataIndex];
        const ZuiItem *item = ZuiGetItem(labelData->itemId);
        if (!item || !item->isVisible)
        {
            return;
        }

        if (labelData->hasBackground)
        {
            DrawRectangleRec(labelData->bounds, labelData->backgroundColor);
        }
        //  if (tex->isInsideWindow)
        // {
        //     position = (Vector2){tex->data.bounds.x + tex->parent.x,
        //                          tex->data.bounds.y + tex->parent.y};
        // }

        DrawTextEx(labelData->font,
                   labelData->text,
                   (Vector2){labelData->bounds.x, labelData->bounds.y},
                   labelData->fontSize,
                   labelData->spacing,
                   labelData->textColor);
    }

    void ZuiPrintLabel(const unsigned int id)
    {
        const ZuiItem *item = ZuiGetItem(id);
        if (!item || item->type != ZUI_LABEL)
        {
            TraceLog(LOG_ERROR, "Item %u is not a label", id);
            return;
        }

        const ZuiLabelData *label = ZuiGetLabelData(id);
        if (!label)
        {
            TraceLog(LOG_ERROR, "Failed to get label data for item %u", id);
            return;
        }

        TraceLog(LOG_INFO, "┌─────── LABEL DATA ───────");
        TraceLog(LOG_INFO, "│ Item ID:      %u", label->itemId);
        TraceLog(LOG_INFO, "│ Text:         \"%s\"", label->text);
        TraceLog(LOG_INFO, "│ Bounds:       (%.1f, %.1f, %.1fx%.1f)",
                 (double)label->bounds.x, (double)label->bounds.y,
                 (double)label->bounds.width, (double)label->bounds.height);
        TraceLog(LOG_INFO, "│ Font Size:    %.1f", (double)label->fontSize);
        TraceLog(LOG_INFO, "│ Spacing:      %.1f", (double)label->spacing);
        TraceLog(LOG_INFO, "│ Text Color:   (%d, %d, %d, %d)",
                 label->textColor.r, label->textColor.g,
                 label->textColor.b, label->textColor.a);
        TraceLog(LOG_INFO, "│ Has BG:       %s", label->hasBackground ? "yes" : "no");
        if (label->hasBackground)
        {
            TraceLog(LOG_INFO, "│ BG Color:     (%d, %d, %d, %d)",
                     label->backgroundColor.r, label->backgroundColor.g,
                     label->backgroundColor.b, label->backgroundColor.a);
        }
        TraceLog(LOG_INFO, "└──────────────────────────");
    }

    unsigned int ZuiLabelEx(const char *text, const bool isMono)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        if (!text)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Label text is NULL");
            return ZUI_ID_INVALID;
        }

        Font font = isMono ? g_zui_ctx->monoFont : g_zui_ctx->font;
        unsigned int id = ZuiCreateLabel(text, font);

        ZuiLabelData *labelData = ZuiGetLabelDataMut(id);
        if (!labelData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to get label data");
            return ZUI_ID_INVALID;
        }

        labelData->bounds.x = g_zui_ctx->cursor.position.x;
        labelData->bounds.y = g_zui_ctx->cursor.position.y;

        unsigned int active = g_zui_ctx->cursor.activeFrame;

        ZuiItemAddChild(active, id);
        ZuiAdvanceCursor(labelData->bounds.width, labelData->bounds.height);
        g_zui_ctx->cursor.activeTexture = id;
        g_zui_ctx->cursor.lastItemBounds = labelData->bounds;
        g_zui_ctx->cursor.activeLabel = id;
        return id;
    }

    unsigned int ZuiNewMonoLabel(const char *text)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        if (!text)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Text pointer is NULL");
            return ZUI_ID_INVALID;
        }

        return ZuiLabelEx(text, true);
    }

    unsigned int ZuiNewLabel(const char *text)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        if (!text)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Text pointer is NULL");
            return ZUI_ID_INVALID;
        }

        return ZuiLabelEx(text, false);
    }

    void ZuiLabelTextColor(const Color textColor)
    {

        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelData *labelData = ZuiGetLabelDataMut(g_zui_ctx->cursor.activeLabel);
        if (labelData)
        {
            labelData->textColor = textColor;
        }
    }

    void ZuiLabelBackgroundColor(const Color backgroundColor)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelData *labelData = ZuiGetLabelDataMut(g_zui_ctx->cursor.activeLabel);
        if (labelData)
        {
            labelData->backgroundColor = backgroundColor;
            labelData->hasBackground = true;
        }
    }

    void ZuiLabelAlignX(const ZuiAlignmentX align)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelData *labelData = ZuiGetLabelDataMut(g_zui_ctx->cursor.activeLabel);
        ZuiFrameData *frameData = ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
        if (!labelData || !frameData)
        {
            return; // ADD ZUI ERROR
        }

        switch (align)
        {
        case ZUI_ALIGN_X_CENTER:
            labelData->bounds.x = frameData->bounds.x + (frameData->bounds.width * 0.5F) - (labelData->bounds.width * 0.5F);
            break;
        case ZUI_ALIGN_X_LEFT:
            labelData->bounds.x = frameData->bounds.x + frameData->padding;
            break;
        case ZUI_ALIGN_X_RIGHT:
            labelData->bounds.x = frameData->bounds.x + frameData->bounds.width - frameData->padding - labelData->bounds.width;
            break;
        }
    }

    void ZuiLabelOffset(const float x, const float y)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        ZuiLabelData *labelData = ZuiGetLabelDataMut(g_zui_ctx->cursor.activeLabel);
        if (labelData)
        {
            labelData->bounds.x += x;
            labelData->bounds.y += y;
            g_zui_ctx->cursor.position.x += x;
            g_zui_ctx->cursor.position.y += y;
        }
    }
// -----------------------------------------------------------------------------
#include "raylib.h"

    const ZuiTextureData *ZuiGetTextureData(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_TEXTURE)
        {
            return NULL;
        }

        unsigned int dataIndex = (unsigned int)(uintptr_t)item->dataIndex;
        return (ZuiTextureData *)ZuiGetDynArray(&g_zui_ctx->textureData, dataIndex);
    }

    ZuiTextureData *ZuiGetTextureDataMut(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_TEXTURE)
        {
            return NULL;
        }

        unsigned int dataIndex = (unsigned int)(uintptr_t)item->dataIndex;
        return (ZuiTextureData *)ZuiGetDynArray(&g_zui_ctx->textureData, dataIndex);
    }

    unsigned int ZuiCreateTexture(const Texture2D texture)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        ZuiTextureData *texData =
            (ZuiTextureData *)ZuiPushDynArray(&g_zui_ctx->textureData, &g_zui_arena);
        if (!texData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate texture data");
            return ZUI_ID_INVALID;
        }

        unsigned int dataIndex = g_zui_ctx->textureData.count - 1;

        unsigned int item_id = ZuiCreateItem(ZUI_TEXTURE, dataIndex);
        if (item_id == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        *texData = (ZuiTextureData){
            .itemId = item_id,
            .texture = texture,
            .color = WHITE,
            .bounds = {
                0.0F, 0.0F,
                (float)texture.width,
                (float)texture.height},
            .isPatch = false,
        };

        ZuiItem *item = ZuiGetItemMut(item_id);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INTERNAL_ERROR,
                             "Failed to retrieve item after creation");
            return ZUI_ID_INVALID;
        }
        item->renderFunction = ZuiRenderTexture;
        item->updateFunction = NULL;
        item->hasChildren = false;

        return item_id;
    }

    void ZuiRenderTexture(unsigned int dataIndex)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        const ZuiTextureData *textureItems = (ZuiTextureData *)g_zui_ctx->textureData.items;
        const ZuiTextureData *texData = &textureItems[dataIndex];
        const ZuiItem *texItem = ZuiGetItem(texData->itemId);
        if (!texItem || !texItem->isVisible)
        {
            return;
        }
        //  if (tex->isInsideWindow)
        // {
        //     position = (Vector2){tex->data.bounds.x + tex->parent.x,
        //                          tex->data.bounds.y + tex->parent.y};
        // }
        DrawTexturePro(texData->texture, texData->npatch.source, texData->bounds, (Vector2){0.0F, 0.0F}, 0.0F, texData->color);
    }

    void ZuiRenderPatchTexture(const unsigned int dataIndex)
    {
        if (!g_zui_ctx)
        {
            return;
        }

        const ZuiTextureData *textureItems = (ZuiTextureData *)g_zui_ctx->textureData.items;
        const ZuiTextureData *texData = &textureItems[dataIndex];
        const ZuiItem *texItem = ZuiGetItem(texData->itemId);
        if (!texItem || !texItem->isVisible)
        {
            return;
        }

        DrawTextureNPatch(texData->texture, texData->npatch, texData->bounds, (Vector2){0}, 0.0F, texData->color);
    }

    void ZuiPrintTexture(const unsigned int id)
    {
        const ZuiItem *item = ZuiGetItem(id);
        if (!item || item->type != ZUI_TEXTURE)
        {
            TraceLog(LOG_ERROR, "Item %u is not a texture", id);
            return;
        }

        const ZuiTextureData *texture = ZuiGetTextureData(id);
        if (!texture)
        {
            TraceLog(LOG_ERROR, "Failed to get texture data for item %u", id);
            return;
        }

        TraceLog(LOG_INFO, "┌─────── TEXTURE DATA ─────");
        TraceLog(LOG_INFO, "│ Item ID:      %u", texture->itemId);
        TraceLog(LOG_INFO, "│ Texture ID:   %u", texture->texture.id);
        TraceLog(LOG_INFO, "│ Texture Size: %dx%d",
                 texture->texture.width, texture->texture.height);
        TraceLog(LOG_INFO, "│ Bounds:       (%.1f, %.1f, %.1fx%.1f)",
                 (double)texture->bounds.x, (double)texture->bounds.y,
                 (double)texture->bounds.width, (double)texture->bounds.height);
        TraceLog(LOG_INFO, "│ Color:        (%d, %d, %d, %d)",
                 texture->color.r, texture->color.g,
                 texture->color.b, texture->color.a);
        TraceLog(LOG_INFO, "│ Is Patch:     %s", texture->isPatch ? "yes" : "no");
        if (texture->isPatch)
        {
            TraceLog(LOG_INFO, "│ NPatch:       L=%d T=%d R=%d B=%d",
                     texture->npatch.left, texture->npatch.top,
                     texture->npatch.right, texture->npatch.bottom);
        }
        TraceLog(LOG_INFO, "└──────────────────────────");
    }

    unsigned int ZuiNewTextureEx(const Texture2D tex, const NPatchInfo npatch, const Rectangle bounds, const bool isPatch)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        unsigned int id = ZuiCreateTexture(tex);
        ZuiTextureData *texData = ZuiGetTextureDataMut(id);
        if (!texData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to get texture data");
            return ZUI_ID_INVALID;
        }

        texData->bounds = bounds;
        texData->npatch = npatch;

        if (isPatch)
        {

            texData->isPatch = true;
            ZuiItem *item = ZuiGetItemMut(id);
            if (item)
            {
                item->renderFunction = ZuiRenderPatchTexture;
            }
        }

        unsigned int active = g_zui_ctx->cursor.activeFrame;
        ZuiItemAddChild(active, id);

        ZuiAdvanceCursor(texData->bounds.width, texData->bounds.height);
        g_zui_ctx->cursor.activeTexture = id;
        g_zui_ctx->cursor.lastItemBounds = texData->bounds;
        return id;
    }

    unsigned int ZuiNewTexture(const Texture2D tex)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
        };
        Rectangle bounds = (Rectangle){g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, (float)tex.width, (float)tex.height};
        return ZuiNewTextureEx(tex, npatch, bounds, false);
    }

    unsigned int ZuiNew3XSlice(const Texture2D tex, const int width, const int left, const int right)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
            .left = left,
            .right = right,
            .layout = NPATCH_NINE_PATCH,
        };
        Rectangle bounds = (Rectangle){g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, (float)width, (float)tex.height};
        return ZuiNewTextureEx(tex, npatch, bounds, true);
    }

    unsigned int ZuiNew3YSlice(const Texture2D tex, const int height, const int top, const int bottom)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
            .top = top,
            .bottom = bottom,
            .layout = NPATCH_NINE_PATCH};
        Rectangle bounds = (Rectangle){g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, (float)tex.width, (float)height};
        return ZuiNewTextureEx(tex, npatch, bounds, true);
    }

    unsigned int ZuiNew9Slice(const Texture2D tex, const int width, const int height, const int left, const int top, const int right, const int bottom)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
            .left = left,
            .top = top,
            .bottom = bottom,
            .right = right,
            .layout = NPATCH_NINE_PATCH};
        Rectangle bounds = (Rectangle){g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, (float)width, (float)height};
        return ZuiNewTextureEx(tex, npatch, bounds, true);
    }
    // -----------------------------------------------------------------------------

#endif // ZUI_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // ZUI_H
