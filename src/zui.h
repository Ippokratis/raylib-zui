#ifndef ZUI_H
#define ZUI_H

#include "math.h"
#include "raylib.h"
#include <limits.h>
#include <math.h>
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

#ifndef ZUI_DEBUG
#define ZUI_DEBUG
#endif

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
        ZUI_MAX_COMPONENTS_PER_ITEM = 16,
        ZUI_MAX_COMPONENT_TYPES = 16,
        ZUI_ITEM_TYPE_COUNT = 3, // frame, label, texture
        ZUI_CHILDREN_CAPACITY = 8,
        ZUI_ALIGNMENT_MAX_ITEMS = 8,
        ZUI_MAX_TEXT_LENGTH = 64,
        ZUI_MAX_WINDOWS = 64,
        ZUI_BASE_FONT_SIZE = 18,
        ZUI_TITLEBAR_HEIGHT = 28,
        ZUI_WINDOWS_CAPACITY = 16,
        ZUI_MAX_INPUT_CHARS = 32,
        ZUI_ITEMS_CAPACITY = 64,
        ZUI_TRANSFORMS_CAPACITY = 64,
        ZUI_FRAMES_CAPACITY = 64,
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
        ZUI_DEFAULT_ARENA_SIZE = 1048576, // 1MB =1048576
        ZUI_DEFAULT_SCREEN_WIDTH = 800,
        ZUI_DEFAULT_SCREEN_HEIGHT = 600,
        ZUI_DEFAULT_DPI_SCALE = 1,
        ZUI_ROOT_FRAME_ID = 0,
        ZUI_MAX_TREE_DEPTH = 64,
    } ZuiConstants;

    static const float ZUI_FONT_SPACING = 0.2F;
    static const float ZUI_DEFAULT_FRAME_PADDING = 8.0F;
    static const float ZUI_DEFAULT_FRAME_GAP = 4.0F;
    static const unsigned int ZUI_ID_INVALID = UINT_MAX;

#define ZUI_BUILTIN_FRAME 0
#define ZUI_BUILTIN_LABEL 1
#define ZUI_BUILTIN_TEXTURE 2
#define ZUI_BUILTIN_TYPE_COUNT 3
    typedef enum ZuiItemType
    {
        ZUI_FRAME,
        ZUI_LABEL,
        ZUI_TEXTURE,
        ZUI_BUTTON,
        ZUI_NUMERIC_INPUT,
        ZUI_SLIDER,
        ZUI_KNOB,
    } ZuiItemType;

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

    ///--------------------------------------------------------Utility Functions

    bool ZuiIsPowerOfTwo(size_t n);

    const char *ZuiGetWidgetType(ZuiItemType type);

    const char *ZuiResultToString(ZuiResult result);

    float ZuiPixelsToRoundness(Rectangle rect, float radius_px);

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

    ///-------------------------------------Dynamic Array

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
    // zui_component.h

    typedef enum ZuiComponentId
    {
        ZUI_COMPONENT_TRANSFORM = 0,
        ZUI_COMPONENT_LAYOUT = 1,
        ZUI_COMPONENT_ALIGN = 2,
        ZUI_COMPONENT_INTERACTION = 3,
        ZUI_COMPONENT_STYLE = 4,
        ZUI_COMPONENT_STATE = 5,
        ZUI_COMPONENT_ANIMATION = 6,
        // ZUI_COMPONENT_FOCUS = 7,
        //  ZUI_COMPONENT_SCROLL = 8,
        ZUI_COMPONENT_LAYER = 7,
        // ZUI_COMPONENT_SPATIAL = 10,
        ZUI_COMPONENT_CUSTOM_START = 8,
    } ZuiComponentId;

    //--------------------------- COMPONENT SYSTEM TYPES

    typedef void (*ZuiComponentInitFunc)(void *data, unsigned int itemId);
    typedef void (*ZuiComponentCleanupFunc)(void *data);

    typedef struct ZuiComponentInfo
    {
        const char *name;
        size_t dataSize;
        size_t dataAlignment;
        unsigned int initialCapacity;
        ZuiComponentInitFunc init;
        ZuiComponentCleanupFunc cleanup;
        const unsigned int *requiredComponents;
        unsigned int requiredComponentCount;
        const unsigned int *exclusiveComponents;
        unsigned int exclusiveComponentCount;
    } ZuiComponentInfo;

    typedef struct ZuiComponentRegistration
    {
        ZuiComponentInfo info;
        ZuiDynArray dataArray;
        unsigned int componentId;
    } ZuiComponentRegistration;

    typedef struct ZuiComponentRegistry
    {
        ZuiDynArray registrations;
        unsigned int nextComponentId;
    } ZuiComponentRegistry;

    typedef struct ZuiItemComponent
    {
        unsigned int componentId;
        unsigned int dataIndex;
    } ZuiItemComponent;

    //--------------------------- TRANSFORM COMPONENT

    typedef struct ZuiTransform
    {
        unsigned int itemId;
        Rectangle bounds;
        Vector2 offset;
        bool isDirty;
    } ZuiTransform;

    //--------------------------- LAYOUT COMPONENT

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

    typedef enum ZuiLayoutKind
    {
        ZUI_LAYOUT_NONE = 0,
        ZUI_LAYOUT_VERTICAL,
        ZUI_LAYOUT_HORIZONTAL,
        ZUI_LAYOUT_GRID,
    } ZuiLayoutKind;

    typedef enum ZuiSizeMode
    {
        ZUI_SIZE_HUG,
        ZUI_SIZE_FIXED,
        ZUI_SIZE_FILL,
    } ZuiSizeMode;

    typedef enum ZuiJustify
    {
        ZUI_JUSTIFY_START,
        ZUI_JUSTIFY_CENTER,
        ZUI_JUSTIFY_END,
        ZUI_JUSTIFY_SPACE_BETWEEN,
        ZUI_JUSTIFY_SPACE_AROUND,
        ZUI_JUSTIFY_SPACE_EVENLY,
    } ZuiJustify;

    typedef struct ZuiLayoutData
    {
        unsigned int itemId;
        ZuiLayoutKind kind;
        float spacing;
        float padding;
        unsigned int gridColumns;
        ZuiAlignmentX childAlignX;
        ZuiAlignmentY childAlignY;
        ZuiJustify justify;
        float measuredWidth;
        float measuredHeight;
    } ZuiLayoutData;

    typedef struct ZuiAlignData
    {
        unsigned int itemId;
        bool overrideX;
        bool overrideY;
        ZuiAlignmentX alignX;
        ZuiAlignmentY alignY;
        ZuiSizeMode widthMode;
        ZuiSizeMode heightMode;
        float fixedWidth;
        float fixedHeight;
    } ZuiAlignData;

    //--------------------------- ANIMATION COMPONENT

    typedef enum ZuiAnimationType
    {
        ZUI_ANIM_LINEAR,
        ZUI_ANIM_EASE_OUT,
        ZUI_ANIM_SPRING,
    } ZuiAnimationType;

    typedef enum ZuiAnimationSlot
    {
        ZUI_ANIM_SLOT_HOVER = 0,
        ZUI_ANIM_SLOT_PRESS = 1,
        ZUI_ANIM_SLOT_FOCUS = 2,
        ZUI_ANIM_SLOT_ALPHA = 3,
        ZUI_ANIM_SLOT_OFFSET_X = 4,
        ZUI_ANIM_SLOT_OFFSET_Y = 5,
        ZUI_ANIM_SLOT_SCALE = 6,
        ZUI_ANIM_SLOT_CUSTOM = 7,
    } ZuiAnimationSlot;

    typedef struct ZuiAnimation
    {
        float value;
        float target;
        float velocity;
        float duration;
        float elapsed;
        bool active;
        ZuiAnimationType type;
        float damping;
        float stiffness;
    } ZuiAnimation;

    typedef struct ZuiAnimationData
    {
        unsigned int itemId;
        ZuiAnimation slots[8];
        unsigned int activeCount;
    } ZuiAnimationData;

    //---------------------------  INTERACTION COMPONENT

    typedef enum ZuiMouseButton
    {
        ZUI_MOUSE_LEFT = 0,
        ZUI_MOUSE_RIGHT = 1,
        ZUI_MOUSE_MIDDLE = 2,
    } ZuiMouseButton;

    typedef struct ZuiInteractionData
    {
        unsigned int itemId;
        bool isHovered;
        bool wasHovered;
        bool isPressed;
        bool wasPressed;
        bool isFocused;
        bool wasFocused;
        Vector2 mousePosition;
        Vector2 mouseDelta;
        ZuiMouseButton pressedButton;
        float hoverTime;
        float pressTime;
        bool capturesMouse;
        bool capturesKeyboard;
        bool acceptsFocus;
        bool blocksInput;
        void (*onHoverEnter)(unsigned int itemId);
        void (*onHoverExit)(unsigned int itemId);
        void (*onClick)(unsigned int itemId, ZuiMouseButton button);
        void (*onPress)(unsigned int itemId, ZuiMouseButton button);
        void (*onRelease)(unsigned int itemId, ZuiMouseButton button);
        void (*onFocus)(unsigned int itemId);
        void (*onBlur)(unsigned int itemId);
    } ZuiInteractionData;

    //---------------------------  STYLE COMPONENT

    typedef struct ZuiColorSet
    {
        Color normal;
        Color hovered;
        Color pressed;
        Color focused;
        Color disabled;
    } ZuiColorSet;

    typedef struct ZuiBorderStyle
    {
        float thickness;
        float radius;
        Color color;
        bool enabled;
    } ZuiBorderStyle;

    typedef struct ZuiStyleData
    {
        unsigned int itemId;
        ZuiColorSet background;
        ZuiColorSet foreground;
        ZuiColorSet border;
        ZuiBorderStyle borderNormal;
        ZuiBorderStyle borderHovered;
        ZuiBorderStyle borderPressed;
        ZuiBorderStyle borderFocused;
        ZuiBorderStyle borderDisabled;
        Font font;
        float fontSize;
        float fontSpacing;
        Color textColor;
        bool useCustomFont;
        bool hasShadow;
        Color shadowColor;
        Vector2 shadowOffset;
        float shadowBlur;
        const char *themeOverride;
    } ZuiStyleData;

    //---------------------------  LAYER COMPONENT

    typedef enum ZuiLayerPreset
    {
        ZUI_LAYER_BACKGROUND = 0,
        ZUI_LAYER_CONTENT = 100,
        ZUI_LAYER_OVERLAY = 1000,
        ZUI_LAYER_TOOLTIP = 2000,
        ZUI_LAYER_DEBUG = 3000,
    } ZuiLayerPreset;

    typedef struct ZuiLayerData
    {
        unsigned int itemId;
        int layer;
        int order;
        bool blocksInput;
    } ZuiLayerData;

    //---------------------------  STATE COMPONENT

    typedef enum ZuiValidationState
    {
        ZUI_VALIDATION_NONE = 0,
        ZUI_VALIDATION_SUCCESS = 1,
        ZUI_VALIDATION_WARNING = 2,
        ZUI_VALIDATION_ERROR = 3,
    } ZuiValidationState;

    typedef struct ZuiStateData
    {
        unsigned int itemId;
        bool isEnabled;
        bool isVisible;
        bool isReadOnly;
        bool isSelected;
        bool isActive;
        ZuiValidationState validationState;
        char validationMessage[256];
        bool isLoading;
        float loadingProgress;
        bool isDirty;
        bool needsRedraw;
    } ZuiStateData;

    //---------------------------  COMPONENT SYSTEM API

    ZuiResult ZuiInitComponentRegistry(void);
    unsigned int ZuiRegisterComponent(const ZuiComponentInfo *info);
    void ZuiRegisterAllComponents(void);

    void *ZuiItemAddComponent(unsigned int itemId, unsigned int componentId);
    void *ZuiItemGetComponent(unsigned int itemId, unsigned int componentId);
    const void *ZuiItemGetComponentConst(unsigned int itemId, unsigned int componentId);
    bool ZuiItemHasComponent(unsigned int itemId, unsigned int componentId);
    bool ZuiItemRemoveComponent(unsigned int itemId, unsigned int componentId);

    const ZuiComponentInfo *ZuiGetComponentInfo(unsigned int componentId);
    unsigned int ZuiGetComponentIdByName(const char *name);
    bool ZuiValidateComponentDependencies(unsigned int itemId, unsigned int componentId);

    ZuiComponentRegistration *ZuiGetComponentRegistration(unsigned int componentId);
    const ZuiComponentRegistration *ZuiGetComponentRegistrationConst(unsigned int componentId);

    void ZuiPrintItemComponents(unsigned int itemId);
    void ZuiPrintRegisteredComponents(void);

    //---------------------------  TRANSFORM API

    const ZuiTransform *ZuiGetTransform(unsigned int itemId);
    ZuiTransform *ZuiGetTransformMut(unsigned int itemId);
    unsigned int ZuiCreateTransform(unsigned int itemId, Rectangle bounds);

    void ZuiSetTransformBounds(unsigned int itemId, Rectangle bounds);
    void ZuiSetTransformOffset(unsigned int itemId, Vector2 offset);
    Rectangle ZuiGetTransformBounds(unsigned int itemId);
    void ZuiMarkTransformDirty(unsigned int itemId);
    void ZuiPrintTransform(unsigned int itemId);

    //---------------------------  LAYOUT API

    ZuiLayoutData *ZuiGetLayoutDataMut(unsigned int itemId);
    const ZuiLayoutData *ZuiGetLayoutData(unsigned int itemId);
    ZuiAlignData *ZuiGetAlignDataMut(unsigned int itemId);
    const ZuiAlignData *ZuiGetAlignData(unsigned int itemId);

    ZuiResult ZuiItemSetLayout(unsigned int itemId, ZuiLayoutKind kind);
    void ZuiLayoutSetSpacing(unsigned int itemId, float spacing);
    void ZuiLayoutSetPadding(unsigned int itemId, float padding);
    void ZuiLayoutSetGridColumns(unsigned int itemId, unsigned int columns);
    void ZuiLayoutSetChildAlignX(unsigned int itemId, ZuiAlignmentX align);
    void ZuiLayoutSetChildAlignY(unsigned int itemId, ZuiAlignmentY align);
    void ZuiLayoutSetJustify(unsigned int itemId, ZuiJustify justify);

    void ZuiItemSetAlignX(unsigned int itemId, ZuiAlignmentX align);
    void ZuiItemSetAlignY(unsigned int itemId, ZuiAlignmentY align);
    void ZuiItemSetSizeMode(unsigned int itemId, ZuiSizeMode widthMode, ZuiSizeMode heightMode);
    void ZuiItemSetFixedSize(unsigned int itemId, float width, float height);

    unsigned int ZuiBeginVertical(Vector2 pos, float spacing, Color color);
    unsigned int ZuiBeginHorizontal(Vector2 pos, float spacing, Color color);
    unsigned int ZuiBeginGrid(Vector2 pos, unsigned int columns, float spacing, Color color);
    unsigned int ZuiBeginVerticalCentered(float spacing, Color color);
    unsigned int ZuiBeginRow2(float spacing);
    unsigned int ZuiBeginColumn(float spacing);
    void ZuiEndRow(void);
    void ZuiBeginRow(void);
    void ZuiEndFrame2(void);

    //---------------------------  ANIMATION API

    ZuiAnimationData *ZuiGetAnimationMut(unsigned int itemId);
    const ZuiAnimationData *ZuiGetAnimation(unsigned int itemId);
    ZuiAnimationData *ZuiAddAnimation(unsigned int itemId);

    void ZuiAnimSetTarget(unsigned int itemId, ZuiAnimationSlot slot, float target);
    float ZuiAnimGetValue(unsigned int itemId, ZuiAnimationSlot slot);
    void ZuiAnimSetDuration(unsigned int itemId, ZuiAnimationSlot slot, float duration);
    void ZuiAnimSetSpring(unsigned int itemId, ZuiAnimationSlot slot, float damping, float stiffness);
    void ZuiAnimSetType(unsigned int itemId, ZuiAnimationSlot slot, ZuiAnimationType type);
    void ZuiAnimReset(unsigned int itemId, ZuiAnimationSlot slot, float value);

    //---------------------------  INTERACTION API

    ZuiInteractionData *ZuiGetInteractionMut(unsigned int itemId);
    const ZuiInteractionData *ZuiGetInteraction(unsigned int itemId);
    ZuiInteractionData *ZuiAddInteraction(unsigned int itemId);

    void ZuiEnableInteraction(unsigned int itemId);
    void ZuiSetHoverCallback(unsigned int itemId, void (*onHover)(unsigned int));
    void ZuiSetClickCallback(unsigned int itemId, void (*onClick)(unsigned int, ZuiMouseButton));
    bool ZuiIsItemHovered(unsigned int itemId);
    bool ZuiIsItemPressed(unsigned int itemId);
    bool ZuiIsItemFocused(unsigned int itemId);

    //---------------------------  STYLE API

    ZuiStyleData *ZuiGetStyleMut(unsigned int itemId);
    const ZuiStyleData *ZuiGetStyle(unsigned int itemId);
    ZuiStyleData *ZuiAddStyle(unsigned int itemId);

    void ZuiSetBackgroundColor(unsigned int itemId, Color normal, Color hovered, Color pressed);
    void ZuiSetBorderStyle(unsigned int itemId, float thickness, float radius, Color color);
    void ZuiSetTextStyle(unsigned int itemId, Font font, float size, Color color);
    void ZuiApplyTheme(unsigned int itemId, const char *themeName);

    //---------------------------  STATE API

    ZuiStateData *ZuiGetStateMut(unsigned int itemId);
    const ZuiStateData *ZuiGetState(unsigned int itemId);
    ZuiStateData *ZuiAddState(unsigned int itemId);

    void ZuiSetEnabled(unsigned int itemId, bool enabled);
    void ZuiSetVisible(unsigned int itemId, bool visible);
    void ZuiSetReadOnly(unsigned int itemId, bool readOnly);
    void ZuiSetValidationState(unsigned int itemId, ZuiValidationState state, const char *message);
    bool ZuiIsEnabled(unsigned int itemId);
    bool ZuiIsVisible(unsigned int itemId);

    //---------------------------  LAYER API

    ZuiLayerData *ZuiGetLayerMut(unsigned int itemId);
    const ZuiLayerData *ZuiGetLayer(unsigned int itemId);
    ZuiLayerData *ZuiAddLayer(unsigned int itemId);

    void ZuiSetLayer(unsigned int itemId, int layer, int order);
    void ZuiSetLayerPreset(unsigned int itemId, ZuiLayerPreset preset);
    void ZuiSetLayerBlocksInput(unsigned int itemId, bool blocks);
    void ZuiRaiseToFront(unsigned int itemId);
    void ZuiLowerToBack(unsigned int itemId);

    int ZuiGetLayerValue(unsigned int itemId);
    int ZuiGetLayerOrder(unsigned int itemId);
    bool ZuiLayerBlocksInput(unsigned int itemId);

    //---------------------------  SYSTEM UPDATE FUNCTIONS

    void ZuiUpdateInteractions(float deltaTime);
    void ZuiUpdateAnimations(float deltaTime);

    // -----------------------------------------------------------------------------
    // zui_item.h

    // #include "raylib.h"

    typedef struct ZuiItem
    {
        ZuiDynArray children; // costs 32kb per 1k items, 800 leaf, 200 container
        unsigned int type;
        unsigned int id;
        unsigned int parentId;
        unsigned int dataIndex;
        ZuiItemComponent components[ZUI_MAX_COMPONENTS_PER_ITEM];
        unsigned int componentCount;
        bool isContainer;
        bool canMove;
    } ZuiItem;

    const ZuiItem *ZuiGetItem(unsigned int id);

    ZuiItem *ZuiGetItemMut(unsigned int id);

    unsigned int ZuiCreateItem(ZuiItemType type, unsigned int dataIndex);

    ZuiResult ZuiItemAddChild(unsigned int parentId, unsigned int childId);

    void ZuiPrintItem(unsigned int id);

    void ZuiPrintItemTree(unsigned int id, int depth);

    void ZuiPrintFullItemTree(void);

    // -----------------------------------------------------------------------------
    // zui_behavior.h

    typedef void (*ZuiUpdateFunction)(unsigned int dataIndex);
    typedef void (*ZuiRenderFunction)(unsigned int dataIndex);

    typedef struct ZuiItemBehavior
    {
        ZuiUpdateFunction update;
        ZuiRenderFunction render;
    } ZuiItemBehavior;

    void ZuiDispatchUpdate(const ZuiItem *item);

    void ZuiDispatchRender(const ZuiItem *item);

    // -----------------------------------------------------------------------------
    // zui_type_registry.h

    typedef struct ZuiTypeInfo ZuiTypeInfo;
    typedef struct ZuiTypeRegistry ZuiTypeRegistry;

    typedef void (*ZuiTypeInitFunc)(void *data, unsigned int itemId);
    typedef void (*ZuiTypeCleanupFunc)(void *data);
    typedef Vector2 (*ZuiTypeMeasureFunc)(void *data);

    struct ZuiTypeInfo
    {
        const char *name;             // Human-readable type name
        size_t dataSize;              // sizeof(YourWidgetData)
        size_t dataAlignment;         // alignof(YourWidgetData)
        unsigned int initialCapacity; // Initial array capacity

        ZuiUpdateFunction update;   // Update function (can be NULL)
        ZuiRenderFunction render;   // Render function (required)
        ZuiTypeInitFunc init;       // Called when data is created (can be NULL)
        ZuiTypeCleanupFunc cleanup; // Called before data is freed (can be NULL)
        ZuiTypeMeasureFunc measure; // Custom measurement (can be NULL)
    };

    typedef struct ZuiTypeRegistration
    {
        ZuiTypeInfo info;
        ZuiDynArray dataArray; // Storage for this type's data
        unsigned int typeId;   // Assigned type ID
        bool isBuiltin;        // Is this a built-in type?
    } ZuiTypeRegistration;

    struct ZuiTypeRegistry
    {
        ZuiDynArray registrations; // Array of ZuiTypeRegistration
        unsigned int nextTypeId;   // Next available type ID
    };

    ZuiResult ZuiInitTypeRegistry(void);

    unsigned int ZuiRegisterType(const ZuiTypeInfo *typeInfo);

    const ZuiTypeInfo *ZuiGetTypeInfo(unsigned int typeId);

    unsigned int ZuiGetTypeIdByName(const char *name);

    bool ZuiIsValidTypeId(unsigned int typeId);

    ZuiTypeRegistration *ZuiGetTypeRegistration(unsigned int typeId);

    const ZuiTypeRegistration *ZuiGetTypeRegistrationConst(unsigned int typeId);

    unsigned int ZuiCreateTypedItem(unsigned int typeId);

    void *ZuiGetTypedData(unsigned int itemId);

    const void *ZuiGetTypedDataConst(unsigned int itemId);

    // -----------------------------------------------------------------------------
    // zui_frame.h (enhanced)

    // #include "zui_core.h"

    typedef struct ZuiFrameData
    {
        unsigned int itemId;
        float cornerRadius;
        bool hasShadow;
    } ZuiFrameData;

    // Creation
    unsigned int ZuiCreateFrame(Rectangle bounds, Color color);
    unsigned int ZuiBeginFrame(Rectangle bounds, Color color);
    void ZuiEndFrame(void);
    unsigned int ZuiNewFrame(Color color, float width, float height);

    // Data access
    const ZuiFrameData *ZuiGetFrameData(unsigned int itemId);
    ZuiFrameData *ZuiGetFrameDataMut(unsigned int itemId);

    // Styling
    void ZuiFrameBackground(Color color);
    void ZuiFrameOutline(Color color, float thickness);
    void ZuiFrameGap(float gap);
    void ZuiFramePad(float pad);
    void ZuiFrameOffset(float x, float y);
    void ZuiFrameCornerRadius(float radius);

    // Scrolling
    void ZuiFrameMakeScrollable(bool vertical, bool horizontal);

    // Rendering
    void ZuiUpdateFrame(unsigned int dataIndex);
    void ZuiRenderFrame(unsigned int dataIndex);
    void ZuiPrintFrame(unsigned int id);

    // Helper functions
    float ZuiGetFramePadding(unsigned int frameId);
    float ZuiGetFrameSpacing(unsigned int frameId);

    // -----------------------------------------------------------------------------
    // zui_label.h

    typedef struct ZuiLabelData
    {
        unsigned int itemId;
        char text[ZUI_MAX_TEXT_LENGTH + 1];
        Font font;
        float fontSize;
        float fontSpacing;
        bool isMono;
        void (*onClickSimple)(unsigned int); // Simple click callback (no button parameter)
    } ZuiLabelData;

    // Creation
    unsigned int ZuiCreateLabel(const char *text, Font font);
    unsigned int ZuiNewLabel(const char *text);
    unsigned int ZuiNewMonoLabel(const char *text);

    // Data access
    const ZuiLabelData *ZuiGetLabelData(unsigned int itemId);
    ZuiLabelData *ZuiGetLabelDataMut(unsigned int itemId);

    // Styling (uses Style component)
    void ZuiLabelTextColor(Color textColor);
    void ZuiLabelBackgroundColor(Color backgroundColor);

    // Positioning (uses Align component)
    void ZuiLabelAlignX(ZuiAlignmentX align);
    void ZuiLabelOffset(float x, float y);

    // Interactivity (adds Interaction component)
    void ZuiLabelMakeClickable(void (*onClick)(unsigned int));

    // Rendering
    void ZuiRenderLabel(unsigned int dataIndex);
    void ZuiPrintLabel(unsigned int id);

    // -----------------------------------------------------------------------------
    // zui_texture.h

    typedef struct ZuiTextureData
    {
        unsigned int itemId;
        Texture2D texture;
        NPatchInfo npatch;
        Color color;
        bool isPatch;
        bool isVisible;
    } ZuiTextureData;

    unsigned int ZuiCreateTexture(Texture2D texture);

    const ZuiTextureData *ZuiGetTextureData(unsigned int itemId);

    ZuiTextureData *ZuiGetTextureDataMut(unsigned int itemId);

    void ZuiRenderTexture(unsigned int dataIndex);

    void ZuiPrintTexture(unsigned int id);

    unsigned int ZuiNewTextureEx(Texture2D tex, NPatchInfo npatch, Rectangle bounds, bool isPatch);

    unsigned int ZuiNewTexture(Texture2D tex);

    unsigned int ZuiNew3XSlice(Texture2D tex, int width, int left, int right);

    unsigned int ZuiNew3YSlice(Texture2D tex, int height, int top, int bottom);

    unsigned int ZuiNew9Slice(Texture2D tex, int width, int height, int left, int top, int right, int bottom);

    // -----------------------------------------------------------------------------
    // zui_context.h

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
        ZuiTypeRegistry typeRegistry;
        ZuiComponentRegistry componentRegistry;
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

    // void ZuiBeginRow(void);

    // void ZuiEndRow(void);

    bool ZuiIsInitialized(void);

    ZuiContext *ZuiGetContext(void);

    void ZuiUpdateRestPosition(void);

    ZuiFrameData *ZuiGetActiveFrameDataMut(void);

    ZuiLayoutData *ZuiGetActiveLayoutMut(void);

    bool ZuiEnsureContext(ZuiResult error_code, const char *msg);

    void ZuiUpdate(void);

    void ZuiRender(void);

    bool ZuiInit(void);

    void ZuiExit(void);

    // -----------------------------------------------------------------------------

#ifdef ZUI_IMPLEMENTATION
    // --- ZUI IMPLEMENTATION ---
    // zui_core.c

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
        const float min_dim = fminf(rect.width, rect.height);
        if (min_dim <= 0.0F)
        {
            return 0.0F;
        }
        const float max_radius = min_dim * 0.5F;
        radius_px = fmaxf(0.0F, fminf(radius_px, max_radius));
        return radius_px / max_radius;
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
    // zui_arena.c
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
    // zui_dynarray.c
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

        TraceLog(LOG_INFO, "ZUI: Growing dynarray '%s' from %u to %u capacity (%zu to %zu bytes)",
                 array->typeName ? array->typeName : "unknown",
                 array->capacity, new_capacity, current_bytes, new_bytes);

        void *new_items = ZuiAllocArena(arena, new_bytes, _Alignof(max_align_t));
        if (!new_items)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to allocate %zu bytes from arena for %s array growth",
                     new_bytes, array->typeName);
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
        if (!array || !arena)
        {
            return NULL;
        }

        if (!ZuiIsDynArrayValid(array))
        {
            return NULL;
        }

        // ⚠️ CRITICAL: Add this bounds check BEFORE allocating
        if (array->count >= array->capacity)
        {
            ZuiResult result = ZuiGrowDynArray(array, arena);
            if (result != ZUI_OK)
            {
                return NULL;
            }
        }

        // ⚠️ CRITICAL: Add this overflow check
        size_t byte_offset = array->itemSize * array->count;

        // Check for multiplication overflow
        if (array->count > 0 && byte_offset < array->itemSize)
        {
            TraceLog(LOG_ERROR, "ZUI: Multiplication overflow in PushDynArray: %u * %zu = %zu",
                     array->count, array->itemSize, byte_offset);
            return NULL;
        }

        // Check if offset is within bounds
        if (byte_offset >= array->capacity * array->itemSize)
        {
            TraceLog(LOG_ERROR, "ZUI: Byte offset %zu exceeds allocated size %zu",
                     byte_offset, array->capacity * array->itemSize);
            return NULL;
        }

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
    // zui_component.c - Unified Component System Implementation
    // #include "raymath.h"

    //-------------------------------------- COMPONENT INITIALIZATION FUNCTIONS

    static void ZuiInitTransform(void *data, unsigned int itemId)
    {
        ZuiTransform *t = (ZuiTransform *)data;
        *t = (ZuiTransform){
            .itemId = itemId,
            .bounds = (Rectangle){0, 0, 0, 0},
            .offset = (Vector2){0, 0},
            .isDirty = false};
    }

    static void ZuiInitLayout(void *data, unsigned int itemId)
    {
        ZuiLayoutData *l = (ZuiLayoutData *)data;
        *l = (ZuiLayoutData){
            .itemId = itemId,
            .kind = ZUI_LAYOUT_NONE,
            .spacing = ZUI_DEFAULT_FRAME_GAP,
            .padding = ZUI_DEFAULT_FRAME_PADDING,
            .gridColumns = 3,
            .childAlignX = ZUI_ALIGN_X_LEFT,
            .childAlignY = ZUI_ALIGN_Y_TOP,
            .justify = ZUI_JUSTIFY_START,
            .measuredWidth = 0,
            .measuredHeight = 0};
    }

    static void ZuiInitAlign(void *data, unsigned int itemId)
    {
        ZuiAlignData *a = (ZuiAlignData *)data;
        *a = (ZuiAlignData){
            .itemId = itemId,
            .overrideX = false,
            .overrideY = false,
            .alignX = ZUI_ALIGN_X_LEFT,
            .alignY = ZUI_ALIGN_Y_TOP,
            .widthMode = ZUI_SIZE_HUG,
            .heightMode = ZUI_SIZE_HUG,
            .fixedWidth = 0,
            .fixedHeight = 0};
    }

    static void ZuiInitInteraction(void *data, unsigned int itemId)
    {
        ZuiInteractionData *i = (ZuiInteractionData *)data;
        *i = (ZuiInteractionData){
            .itemId = itemId,
            .isHovered = false,
            .wasHovered = false,
            .isPressed = false,
            .wasPressed = false,
            .isFocused = false,
            .wasFocused = false,
            .mousePosition = (Vector2){0, 0},
            .mouseDelta = (Vector2){0, 0},
            .pressedButton = ZUI_MOUSE_LEFT,
            .hoverTime = 0,
            .pressTime = 0,
            .capturesMouse = true,
            .capturesKeyboard = false,
            .acceptsFocus = true,
            .blocksInput = false,
            .onHoverEnter = NULL,
            .onHoverExit = NULL,
            .onClick = NULL,
            .onPress = NULL,
            .onRelease = NULL,
            .onFocus = NULL,
            .onBlur = NULL};
    }

    static void ZuiInitStyle(void *data, unsigned int itemId)
    {
        ZuiStyleData *s = (ZuiStyleData *)data;
        *s = (ZuiStyleData){
            .itemId = itemId,
            .background = (ZuiColorSet){RAYWHITE, LIGHTGRAY, GRAY, SKYBLUE, DARKGRAY},
            .foreground = (ZuiColorSet){BLACK, BLACK, BLACK, BLACK, GRAY},
            .border = (ZuiColorSet){BLACK, BLACK, BLACK, BLACK, GRAY},
            .borderNormal = (ZuiBorderStyle){0, 0, BLACK, false},
            .borderHovered = (ZuiBorderStyle){0, 0, BLACK, false},
            .borderPressed = (ZuiBorderStyle){0, 0, BLACK, false},
            .borderFocused = (ZuiBorderStyle){0, 0, BLACK, false},
            .borderDisabled = (ZuiBorderStyle){0, 0, BLACK, false},
            .font = GetFontDefault(),
            .fontSize = 16.0F,
            .fontSpacing = 1.0F,
            .textColor = BLACK,
            .useCustomFont = false,
            .hasShadow = false,
            .shadowColor = BLACK,
            .shadowOffset = (Vector2){0, 0},
            .shadowBlur = 0,
            .themeOverride = NULL};
    }

    static void ZuiInitState(void *data, unsigned int itemId)
    {
        ZuiStateData *s = (ZuiStateData *)data;
        *s = (ZuiStateData){
            .itemId = itemId,
            .isEnabled = true,
            .isVisible = true,
            .isReadOnly = false,
            .isSelected = false,
            .isActive = false,
            .validationState = ZUI_VALIDATION_NONE,
            .validationMessage = {0},
            .isLoading = false,
            .loadingProgress = 0,
            .isDirty = false,
            .needsRedraw = false};
    }

    static void ZuiInitAnimation(void *data, unsigned int itemId)
    {
        ZuiAnimationData *a = (ZuiAnimationData *)data;
        a->itemId = itemId;
        a->activeCount = 0;

        for (int i = 0; i < 8; i++)
        {
            a->slots[i] = (ZuiAnimation){
                .value = 0.0F,
                .target = 0.0F,
                .velocity = 0.0F,
                .duration = 0.2F,
                .elapsed = 0.0F,
                .active = false,
                .type = ZUI_ANIM_EASE_OUT,
                .damping = 0.7F,
                .stiffness = 300.0F};
        }
    }

    static void ZuiInitLayer(void *data, unsigned int itemId)
    {
        ZuiLayerData *l = (ZuiLayerData *)data;
        *l = (ZuiLayerData){
            .itemId = itemId,
            .layer = ZUI_LAYER_CONTENT,
            .order = 0,
            .blocksInput = false};
    }

    //-------------------------------------- COMPONENT REGISTRATION

    ZuiResult ZuiInitComponentRegistry(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ERROR_NULL_CONTEXT;
        }

        ZuiComponentRegistry *reg = &g_zui_ctx->componentRegistry;
        ZuiResult result = ZuiInitDynArray(&reg->registrations, &g_zui_arena, 16,
                                           sizeof(ZuiComponentRegistration),
                                           ZUI_ALIGNOF(ZuiComponentRegistration),
                                           "ComponentRegistrations");
        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to init component registry");
            return result;
        }

        reg->nextComponentId = 0;

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: Component registry initialized");
#endif

        return ZUI_OK;
    }

    unsigned int ZuiRegisterComponent(const ZuiComponentInfo *info)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }
        if (!info || !info->name)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "ComponentInfo or name is NULL");
            return ZUI_ID_INVALID;
        }
        if (info->dataSize == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Data size must be > 0");
            return ZUI_ID_INVALID;
        }
        if (!ZuiIsPowerOfTwo(info->dataAlignment))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ALIGNMENT, "Data alignment must be power of 2");
            return ZUI_ID_INVALID;
        }

        ZuiComponentRegistry *reg = &g_zui_ctx->componentRegistry;
        if (reg->nextComponentId >= ZUI_MAX_COMPONENT_TYPES)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Maximum component types reached");
            return ZUI_ID_INVALID;
        }

        // Check for duplicate name
        for (unsigned int i = 0; i < reg->registrations.count; i++)
        {
            const ZuiComponentRegistration *r = (ZuiComponentRegistration *)ZuiGetDynArray(&reg->registrations, i);
            if (r && strcmp(r->info.name, info->name) == 0)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Component '%s' already registered", info->name);
                return ZUI_ID_INVALID;
            }
        }

        ZuiComponentRegistration *newReg = (ZuiComponentRegistration *)ZuiPushDynArray(&reg->registrations, &g_zui_arena);
        if (!newReg)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate component registration");
            return ZUI_ID_INVALID;
        }

        newReg->info = *info;
        newReg->componentId = reg->nextComponentId++;

        const unsigned int capacity = info->initialCapacity > 0 ? info->initialCapacity : 32;
        ZuiResult result = ZuiInitDynArray(&newReg->dataArray, &g_zui_arena, capacity,
                                           info->dataSize, info->dataAlignment, info->name);
        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to init data array for component '%s'", info->name);
            reg->registrations.count--;
            return ZUI_ID_INVALID;
        }

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: Registered component '%s' with ID %u", info->name, newReg->componentId);
#endif

        return newReg->componentId;
    }

    void ZuiRegisterAllComponents(void)
    {
        // Transform
        {
            static const unsigned int empty[] = {0};
            ZuiComponentInfo info = {
                .name = "Transform",
                .dataSize = sizeof(ZuiTransform),
                .dataAlignment = ZUI_ALIGNOF(ZuiTransform),
                .initialCapacity = 64,
                .init = ZuiInitTransform,
                .cleanup = NULL,
                .requiredComponents = empty,
                .requiredComponentCount = 0};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_TRANSFORM, "Transform component ID mismatch");
        }

        // Layout
        {
            static const unsigned int deps[] = {ZUI_COMPONENT_TRANSFORM};
            ZuiComponentInfo info = {
                .name = "Layout",
                .dataSize = sizeof(ZuiLayoutData),
                .dataAlignment = ZUI_ALIGNOF(ZuiLayoutData),
                .initialCapacity = 64,
                .init = ZuiInitLayout,
                .cleanup = NULL,
                .requiredComponents = deps,
                .requiredComponentCount = 1};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_LAYOUT, "Layout component ID mismatch");
        }

        // Align
        {
            static const unsigned int empty[] = {0};
            ZuiComponentInfo info = {
                .name = "Align",
                .dataSize = sizeof(ZuiAlignData),
                .dataAlignment = ZUI_ALIGNOF(ZuiAlignData),
                .initialCapacity = 64,
                .init = ZuiInitAlign,
                .cleanup = NULL,
                .requiredComponents = empty,
                .requiredComponentCount = 0};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_ALIGN, "Align component ID mismatch");
        }

        // Interaction
        {
            static const unsigned int deps[] = {ZUI_COMPONENT_TRANSFORM};
            ZuiComponentInfo info = {
                .name = "Interaction",
                .dataSize = sizeof(ZuiInteractionData),
                .dataAlignment = ZUI_ALIGNOF(ZuiInteractionData),
                .initialCapacity = 64,
                .init = ZuiInitInteraction,
                .cleanup = NULL,
                .requiredComponents = deps,
                .requiredComponentCount = 1};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_INTERACTION, "Interaction component ID mismatch");
        }

        // Style
        {
            static const unsigned int empty[] = {0};
            ZuiComponentInfo info = {
                .name = "Style",
                .dataSize = sizeof(ZuiStyleData),
                .dataAlignment = ZUI_ALIGNOF(ZuiStyleData),
                .initialCapacity = 64,
                .init = ZuiInitStyle,
                .cleanup = NULL,
                .requiredComponents = empty,
                .requiredComponentCount = 0};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_STYLE, "Style component ID mismatch");
        }

        // State
        {
            static const unsigned int empty[] = {0};
            ZuiComponentInfo info = {
                .name = "State",
                .dataSize = sizeof(ZuiStateData),
                .dataAlignment = ZUI_ALIGNOF(ZuiStateData),
                .initialCapacity = 64,
                .init = ZuiInitState,
                .cleanup = NULL,
                .requiredComponents = empty,
                .requiredComponentCount = 0};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_STATE, "State component ID mismatch");
        }

        // Animation
        {
            static const unsigned int empty[] = {0};
            ZuiComponentInfo info = {
                .name = "Animation",
                .dataSize = sizeof(ZuiAnimationData),
                .dataAlignment = ZUI_ALIGNOF(ZuiAnimationData),
                .initialCapacity = 64,
                .init = ZuiInitAnimation,
                .cleanup = NULL,
                .requiredComponents = empty,
                .requiredComponentCount = 0};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_ANIMATION, "Animation component ID mismatch");
        }

        // Layer
        {
            static const unsigned int empty[] = {0};
            ZuiComponentInfo info = {
                .name = "Layer",
                .dataSize = sizeof(ZuiLayerData),
                .dataAlignment = ZUI_ALIGNOF(ZuiLayerData),
                .initialCapacity = 64,
                .init = ZuiInitLayer,
                .cleanup = NULL,
                .requiredComponents = empty,
                .requiredComponentCount = 0};
            unsigned int id = ZuiRegisterComponent(&info);
            ZUI_ASSERT(id == ZUI_COMPONENT_LAYER, "Layer component ID mismatch");
        }

        TraceLog(LOG_INFO, "ZUI: Registered 9 components");
    }

    //-------------------------------------- COMPONENT SYSTEM FUNCTIONS

    ZuiComponentRegistration *ZuiGetComponentRegistration(unsigned int componentId)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return NULL;
        }

        ZuiComponentRegistry *reg = &g_zui_ctx->componentRegistry;
        for (unsigned int i = 0; i < reg->registrations.count; i++)
        {
            ZuiComponentRegistration *r = (ZuiComponentRegistration *)ZuiGetDynArray(&reg->registrations, i);
            if (r && r->componentId == componentId)
            {
                return r;
            }
        }
        return NULL;
    }

    const ZuiComponentRegistration *ZuiGetComponentRegistrationConst(unsigned int componentId)
    {
        return ZuiGetComponentRegistration(componentId);
    }

    const ZuiComponentInfo *ZuiGetComponentInfo(unsigned int componentId)
    {
        const ZuiComponentRegistration *reg = ZuiGetComponentRegistrationConst(componentId);
        return reg ? &reg->info : NULL;
    }

    unsigned int ZuiGetComponentIdByName(const char *name)
    {
        if (!g_zui_ctx || !name)
        {
            return ZUI_ID_INVALID;
        }

        ZuiComponentRegistry *reg = &g_zui_ctx->componentRegistry;
        for (unsigned int i = 0; i < reg->registrations.count; i++)
        {
            ZuiComponentRegistration *r = (ZuiComponentRegistration *)ZuiGetDynArray(&reg->registrations, i);
            if (r && strcmp(r->info.name, name) == 0)
            {
                return r->componentId;
            }
        }
        return ZUI_ID_INVALID;
    }

    bool ZuiValidateComponentDependencies(unsigned int itemId, unsigned int componentId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return false;
        }

        const ZuiComponentInfo *info = ZuiGetComponentInfo(componentId);
        if (!info)
        {
            return false;
        }

        // Check required dependencies
        if (info->requiredComponents && info->requiredComponentCount > 0)
        {
            for (unsigned int i = 0; i < info->requiredComponentCount; i++)
            {
                unsigned int requiredId = info->requiredComponents[i];
                if (!ZuiItemHasComponent(itemId, requiredId))
                {
                    const ZuiComponentInfo *reqInfo = ZuiGetComponentInfo(requiredId);
                    ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE,
                                     "Component '%s' requires '%s' but item doesn't have it",
                                     info->name, reqInfo ? reqInfo->name : "unknown");
                    return false;
                }
            }
        }

        // Check mutual exclusivity
        if (info->exclusiveComponents && info->exclusiveComponentCount > 0)
        {
            for (unsigned int i = 0; i < info->exclusiveComponentCount; i++)
            {
                unsigned int exclusiveId = info->exclusiveComponents[i];
                if (ZuiItemHasComponent(itemId, exclusiveId))
                {
                    const ZuiComponentInfo *excInfo = ZuiGetComponentInfo(exclusiveId);
                    ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE,
                                     "Component '%s' is mutually exclusive with '%s'",
                                     info->name, excInfo ? excInfo->name : "unknown");
                    return false;
                }
            }
        }

        return true;
    }

    void *ZuiItemAddComponent(unsigned int itemId, unsigned int componentId)
    {
        ZuiItem *item = ZuiGetItemMut(itemId);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid item ID %u", itemId);
            return NULL;
        }

        ZuiComponentRegistration *reg = ZuiGetComponentRegistration(componentId);
        if (!reg)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid component ID %u", componentId);
            return NULL;
        }

        // Check if already has component
        if (ZuiItemHasComponent(itemId, componentId))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Item already has component '%s'", reg->info.name);
            return ZuiItemGetComponent(itemId, componentId);
        }

        // Check max components per item
        if (item->componentCount >= ZUI_MAX_COMPONENTS_PER_ITEM)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Item has max components (%d)", ZUI_MAX_COMPONENTS_PER_ITEM);
            return NULL;
        }

        // Validate dependencies
        if (!ZuiValidateComponentDependencies(itemId, componentId))
        {
            return NULL;
        }

        // Allocate component data
        void *data = ZuiPushDynArray(&reg->dataArray, &g_zui_arena);
        if (!data)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate component data");
            return NULL;
        }

        unsigned int dataIndex = reg->dataArray.count - 1;

        // Add to item's component list
        item->components[item->componentCount] = (ZuiItemComponent){
            .componentId = componentId,
            .dataIndex = dataIndex};
        item->componentCount++;

        // Call init callback
        if (reg->info.init)
        {
            reg->info.init(data, itemId);
        }

#ifdef ZUI_DEBUG
        TraceLog(LOG_DEBUG, "ZUI: Added component '%s' to item %u", reg->info.name, itemId);
#endif

        return data;
    }

    void *ZuiItemGetComponent(unsigned int itemId, unsigned int componentId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return NULL;
        }

        for (unsigned int i = 0; i < item->componentCount; i++)
        {
            if (item->components[i].componentId == componentId)
            {
                ZuiComponentRegistration *reg = ZuiGetComponentRegistration(componentId);
                if (!reg)
                {
                    return NULL;
                }

                unsigned int dataIndex = item->components[i].dataIndex;
                if (dataIndex >= reg->dataArray.count)
                {
                    ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS, "Component data index out of bounds");
                    return NULL;
                }

                return ZuiGetDynArray(&reg->dataArray, dataIndex);
            }
        }

        return NULL;
    }

    const void *ZuiItemGetComponentConst(unsigned int itemId, unsigned int componentId)
    {
        return ZuiItemGetComponent(itemId, componentId);
    }

    bool ZuiItemHasComponent(unsigned int itemId, unsigned int componentId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return false;
        }

        for (unsigned int i = 0; i < item->componentCount; i++)
        {
            if (item->components[i].componentId == componentId)
            {
                return true;
            }
        }

        return false;
    }

    bool ZuiItemRemoveComponent(unsigned int itemId, unsigned int componentId)
    {
        ZuiItem *item = ZuiGetItemMut(itemId);
        if (!item)
        {
            return false;
        }

        for (unsigned int i = 0; i < item->componentCount; i++)
        {
            if (item->components[i].componentId == componentId)
            {
                ZuiComponentRegistration *reg = ZuiGetComponentRegistration(componentId);
                if (reg && reg->info.cleanup)
                {
                    void *data = ZuiGetDynArray(&reg->dataArray, item->components[i].dataIndex);
                    if (data)
                    {
                        reg->info.cleanup(data);
                    }
                }

                // Shift remaining components
                for (unsigned int j = i; j < item->componentCount - 1; j++)
                {
                    item->components[j] = item->components[j + 1];
                }
                item->componentCount--;

                return true;
            }
        }

        return false;
    }

    void ZuiPrintItemComponents(unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            TraceLog(LOG_ERROR, "Invalid item ID: %u", itemId);
            return;
        }

        TraceLog(LOG_INFO, "┌─── ITEM %u COMPONENTS ───", itemId);
        TraceLog(LOG_INFO, "│ Component count: %u / %u", item->componentCount, ZUI_MAX_COMPONENTS_PER_ITEM);

        for (unsigned int i = 0; i < item->componentCount; i++)
        {
            const ZuiComponentInfo *info = ZuiGetComponentInfo(item->components[i].componentId);
            TraceLog(LOG_INFO, "│ [%u] %s (ID: %u, Data Index: %u)",
                     i,
                     info ? info->name : "unknown",
                     item->components[i].componentId,
                     item->components[i].dataIndex);
        }

        TraceLog(LOG_INFO, "└────────────────────────────");
    }

    void ZuiPrintRegisteredComponents(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiComponentRegistry *reg = &g_zui_ctx->componentRegistry;
        TraceLog(LOG_INFO, "========== REGISTERED COMPONENTS ==========");
        TraceLog(LOG_INFO, "Total: %u", reg->registrations.count);

        for (unsigned int i = 0; i < reg->registrations.count; i++)
        {
            const ZuiComponentRegistration *r = (ZuiComponentRegistration *)ZuiGetDynArray(&reg->registrations, i);
            if (!r)
            {
                continue;
            }

            TraceLog(LOG_INFO, "─────────────────────────");
            TraceLog(LOG_INFO, "Component: %s", r->info.name);
            TraceLog(LOG_INFO, "  ID: %u", r->componentId);
            TraceLog(LOG_INFO, "  Data size: %zu bytes", r->info.dataSize);
            TraceLog(LOG_INFO, "  Instances: %u / %u", r->dataArray.count, r->dataArray.capacity);

            if (r->info.requiredComponentCount > 0)
            {
                TraceLog(LOG_INFO, "  Dependencies:");
                for (unsigned int j = 0; j < r->info.requiredComponentCount; j++)
                {
                    const ZuiComponentInfo *depInfo = ZuiGetComponentInfo(r->info.requiredComponents[j]);
                    TraceLog(LOG_INFO, "    - %s", depInfo ? depInfo->name : "unknown");
                }
            }
        }

        TraceLog(LOG_INFO, "===========================================");
    }

    //-------------------------------------- TRANSFORM IMPLEMENTATION

    const ZuiTransform *ZuiGetTransform(unsigned int itemId)
    {
        return (const ZuiTransform *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_TRANSFORM);
    }

    ZuiTransform *ZuiGetTransformMut(unsigned int itemId)
    {
        return (ZuiTransform *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
    }

    unsigned int ZuiCreateTransform(unsigned int itemId, Rectangle bounds)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        ZuiTransform *t = (ZuiTransform *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!t)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to add transform component");
            return ZUI_ID_INVALID;
        }

        t->itemId = itemId;
        t->bounds = bounds;
        t->offset = (Vector2){0, 0};
        t->isDirty = false;

        return 0;
    }

    void ZuiSetTransformBounds(unsigned int itemId, Rectangle bounds)
    {
        ZuiTransform *t = ZuiGetTransformMut(itemId);
        if (t)
        {
            t->bounds = bounds;
            t->isDirty = true;
        }
    }

    void ZuiSetTransformOffset(unsigned int itemId, Vector2 offset)
    {
        ZuiTransform *t = ZuiGetTransformMut(itemId);
        if (t)
        {
            t->offset = offset;
            t->isDirty = true;
        }
    }

    Rectangle ZuiGetTransformBounds(unsigned int itemId)
    {
        const ZuiTransform *t = ZuiGetTransform(itemId);
        if (!t)
        {
            return (Rectangle){0, 0, 0, 0};
        }

        Rectangle bounds = t->bounds;
        bounds.x += t->offset.x;
        bounds.y += t->offset.y;
        return bounds;
    }

    void ZuiMarkTransformDirty(unsigned int itemId)
    {
        ZuiTransform *t = ZuiGetTransformMut(itemId);
        if (t)
        {
            t->isDirty = true;
        }
    }

    void ZuiPrintTransform(unsigned int itemId)
    {
        const ZuiTransform *t = ZuiGetTransform(itemId);
        if (!t)
        {
            TraceLog(LOG_ERROR, "Failed to get transform for item %u", itemId);
            return;
        }

        TraceLog(LOG_INFO, "┌─────── TRANSFORM ───────");
        TraceLog(LOG_INFO, "│ Item ID:  %u", t->itemId);
        TraceLog(LOG_INFO, "│ Bounds:   (%.1f, %.1f, %.1fx%.1f)",
                 (double)t->bounds.x, (double)t->bounds.y,
                 (double)t->bounds.width, (double)t->bounds.height);
        TraceLog(LOG_INFO, "│ Offset:   (%.1f, %.1f)",
                 (double)t->offset.x, (double)t->offset.y);
        TraceLog(LOG_INFO, "│ Dirty:    %s", t->isDirty ? "yes" : "no");
        TraceLog(LOG_INFO, "└─────────────────────────");
    }

    //-------------------------------------- LAYOUT ACCESSORS

    ZuiLayoutData *ZuiGetLayoutDataMut(unsigned int itemId)
    {
        return (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
    }

    const ZuiLayoutData *ZuiGetLayoutData(unsigned int itemId)
    {
        return (const ZuiLayoutData *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_LAYOUT);
    }

    ZuiAlignData *ZuiGetAlignDataMut(unsigned int itemId)
    {
        return (ZuiAlignData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_ALIGN);
    }

    const ZuiAlignData *ZuiGetAlignData(unsigned int itemId)
    {
        return (const ZuiAlignData *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_ALIGN);
    }

    static ZuiAlignData *ZuiGetOrCreateAlignData(unsigned int itemId)
    {
        ZuiAlignData *a = ZuiGetAlignDataMut(itemId);
        if (a)
        {
            return a;
        }

        a = (ZuiAlignData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_ALIGN);
        if (a)
        {
            a->itemId = itemId;
            a->alignX = ZUI_ALIGN_X_LEFT;
            a->alignY = ZUI_ALIGN_Y_TOP;
            a->widthMode = ZUI_SIZE_HUG;
            a->heightMode = ZUI_SIZE_HUG;
            a->overrideX = false;
            a->overrideY = false;
        }
        return a;
    }

    //-------------------------------------- LAYOUT MEASUREMENT

    static Vector2 ZuiMeasureWidget(unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return (Vector2){0, 0};
        }

        const ZuiAlignData *a = ZuiGetAlignData(itemId);
        if (a && a->widthMode == ZUI_SIZE_FIXED && a->heightMode == ZUI_SIZE_FIXED)
        {
            return (Vector2){a->fixedWidth, a->fixedHeight};
        }

        Vector2 size = {0};
        if (item->type == ZUI_FRAME && ZuiItemHasComponent(itemId, ZUI_COMPONENT_LAYOUT))
        {
            const ZuiLayoutData *l = ZuiGetLayoutData(itemId);
            size = l ? (Vector2){l->measuredWidth, l->measuredHeight}
                     : (Vector2){ZuiGetTransformBounds(itemId).width, ZuiGetTransformBounds(itemId).height};
        }
        else
        {
            Rectangle bounds = ZuiGetTransformBounds(itemId);
            size = (Vector2){bounds.width, bounds.height};
        }

        return (Vector2){
            (a && a->widthMode == ZUI_SIZE_FIXED) ? a->fixedWidth : size.x,
            (a && a->heightMode == ZUI_SIZE_FIXED) ? a->fixedHeight : size.y};
    }

    static void ZuiMeasure(unsigned int itemId, int depth, bool isVertical);
    static void ZuiArrange(unsigned int itemId, Rectangle bounds, int depth, bool isVertical);

    // NOLINTBEGIN(misc-no-recursion)
    static void ZuiProcessChildLayout(unsigned int childId, int depth)
    {
        const ZuiItem *child = ZuiGetItem(childId);
        if (!child || child->type != ZUI_FRAME)
        {
            return;
        }

        const ZuiLayoutData *l = ZuiGetLayoutData(childId);
        if (!l)
        {
            return;
        }

        const bool isVert = (l->kind == ZUI_LAYOUT_VERTICAL);
        const bool isHoriz = (l->kind == ZUI_LAYOUT_HORIZONTAL);

        if (isVert || isHoriz)
        {
            ZuiMeasure(childId, depth + 1, isVert);
        }
        else if (l->kind == ZUI_LAYOUT_GRID)
        {
            const unsigned int cols = l->gridColumns > 0 ? l->gridColumns : 1;
            const unsigned int childCount = child->children.count;
            if (childCount == 0 || cols > ZUI_MAX_TREE_DEPTH)
            {
                ZuiLayoutData *mut = ZuiGetLayoutDataMut(childId);
                if (mut)
                {
                    mut->measuredWidth = mut->measuredHeight = (l->padding * 2);
                }
                return;
            }

            const unsigned int rows = (childCount + cols - 1) / cols;
            if (rows > ZUI_MAX_TREE_DEPTH)
            {
                return;
            }

            const unsigned int *childIds = (const unsigned int *)child->children.items;
            float colWidths[ZUI_MAX_TREE_DEPTH] = {0};
            float rowHeights[ZUI_MAX_TREE_DEPTH] = {0};

            for (unsigned int i = 0; i < childCount; i++)
            {
                ZuiProcessChildLayout(childIds[i], depth + 1);
                Vector2 sz = ZuiMeasureWidget(childIds[i]);
                const unsigned int c = i % cols;
                const unsigned int r = i / cols;
                if (sz.x > colWidths[c])
                {
                    colWidths[c] = sz.x;
                }
                if (sz.y > rowHeights[r])
                {
                    rowHeights[r] = sz.y;
                }
            }

            float w = 0;
            float h = 0;
            for (unsigned int c = 0; c < cols; c++)
            {
                w += colWidths[c];
            }
            for (unsigned int r = 0; r < rows; r++)
            {
                h += rowHeights[r];
            }

            if (cols > 1)
            {
                w += l->spacing * (float)(cols - 1);
            }
            if (rows > 1)
            {
                h += l->spacing * (float)(rows - 1);
            }

            ZuiLayoutData *mut = ZuiGetLayoutDataMut(childId);
            if (mut)
            {
                mut->measuredWidth = w + (l->padding * 2);
                mut->measuredHeight = h + (l->padding * 2);
            }
        }
    }

    static void ZuiMeasure(unsigned int itemId, int depth, bool isVertical)
    {
        if (depth >= ZUI_MAX_TREE_DEPTH)
        {
            return;
        }

        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        const ZuiItem *item = ZuiGetItem(itemId);

        if (!l || !item || !item->isContainer)
        {
            if (l)
            {
                l->measuredWidth = l->measuredHeight = (l->padding * 2);
            }
            return;
        }

        unsigned int childCount = item->children.count;
        if (childCount == 0 || childCount > ZUI_MAX_TREE_DEPTH)
        {
            l->measuredWidth = l->measuredHeight = (l->padding * 2);
            return;
        }

        const unsigned int *childIds = (const unsigned int *)item->children.items;
        float primary = 0;
        float secondary = 0;

        for (unsigned int i = 0; i < childCount; i++)
        {
            ZuiProcessChildLayout(childIds[i], depth);
            Vector2 sz = ZuiMeasureWidget(childIds[i]);

            if (isVertical)
            {
                primary += sz.y;
                if (sz.x > secondary)
                {
                    secondary = sz.x;
                }
            }
            else
            {
                primary += sz.x;
                if (sz.y > secondary)
                {
                    secondary = sz.y;
                }
            }
        }

        if (childCount > 1)
        {
            primary += l->spacing * (float)(childCount - 1);
        }

        if (isVertical)
        {
            l->measuredWidth = secondary + (l->padding * 2);
            l->measuredHeight = primary + (l->padding * 2);
        }
        else
        {
            l->measuredWidth = primary + (l->padding * 2);
            l->measuredHeight = secondary + (l->padding * 2);
        }
    }

    //-------------------------------------- LAYOUT ARRANGEMENT

    static Rectangle ZuiAlignChild(Rectangle childBounds, Rectangle contentArea,
                                   ZuiAlignmentX alignX, ZuiAlignmentY alignY)
    {
        switch (alignX)
        {
        case ZUI_ALIGN_X_CENTER:
            childBounds.x = contentArea.x + ((contentArea.width - childBounds.width) * 0.5F);
            break;
        case ZUI_ALIGN_X_RIGHT:
            childBounds.x = contentArea.x + contentArea.width - childBounds.width;
            break;
        default:
            childBounds.x = contentArea.x;
            break;
        }

        switch (alignY)
        {
        case ZUI_ALIGN_Y_CENTER:
            childBounds.y = contentArea.y + ((contentArea.height - childBounds.height) * 0.5F);
            break;
        case ZUI_ALIGN_Y_BOTTOM:
            childBounds.y = contentArea.y + contentArea.height - childBounds.height;
            break;
        default:
            childBounds.y = contentArea.y;
            break;
        }

        return childBounds;
    }

    static void ZuiPositionChild(unsigned int childId, Rectangle bounds)
    {
        const ZuiItem *child = ZuiGetItem(childId);
        if (!child)
        {
            return;
        }

        if (child->type == ZUI_LABEL)
        {
            Rectangle curr = ZuiGetTransformBounds(childId);
            curr.x = bounds.x;
            curr.y = bounds.y;
            ZuiSetTransformBounds(childId, curr);
        }
        else
        {
            ZuiSetTransformBounds(childId, bounds);
        }
    }

    static void ZuiArrangeChildLayout(unsigned int childId, Rectangle bounds, int depth);

    static void ZuiArrange(unsigned int itemId, Rectangle bounds, int depth, bool isVertical)
    {
        if (depth >= ZUI_MAX_TREE_DEPTH)
        {
            return;
        }

        const ZuiLayoutData *l = ZuiGetLayoutData(itemId);
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!l || !item || !item->isContainer)
        {
            return;
        }

        ZuiSetTransformBounds(itemId, bounds);
        Rectangle content = {
            bounds.x + l->padding, bounds.y + l->padding,
            bounds.width - (l->padding * 2), bounds.height - (l->padding * 2)};

        const unsigned int childCount = item->children.count;
        if (childCount == 0)
        {
            return;
        }

        const unsigned int *childIds = (const unsigned int *)item->children.items;

        // Calculate FILL sizing
        unsigned int fillCount = 0;
        float fixedTotal = 0;
        for (unsigned int i = 0; i < childCount; i++)
        {
            const ZuiAlignData *a = ZuiGetAlignData(childIds[i]);
            Vector2 sz = ZuiMeasureWidget(childIds[i]);
            const bool fills = a && ((isVertical && a->heightMode == ZUI_SIZE_FILL) ||
                                     (!isVertical && a->widthMode == ZUI_SIZE_FILL));
            if (fills)
            {
                fillCount++;
            }
            else
            {
                fixedTotal += isVertical ? sz.y : sz.x;
            }
        }

        const float spacingTotal = (childCount > 1) ? l->spacing * (float)(childCount - 1) : 0;
        const float contentSize = isVertical ? content.height : content.width;
        const float fillSize = fillCount > 0 ? fmaxf(0, (contentSize - fixedTotal - spacingTotal) / (float)fillCount) : 0;

        // Calculate justification
        float remaining = contentSize - (fixedTotal + fillSize * (float)fillCount + spacingTotal);
        float primary = isVertical ? content.y : content.x;
        float spacing = l->spacing;

        switch (l->justify)
        {
        case ZUI_JUSTIFY_CENTER:
            primary += remaining * 0.5F;
            break;
        case ZUI_JUSTIFY_END:
            primary += remaining;
            break;
        case ZUI_JUSTIFY_SPACE_BETWEEN:
            if (childCount > 1)
            {
                spacing = (remaining + spacingTotal) / (float)(childCount - 1);
            }
            break;
        case ZUI_JUSTIFY_SPACE_AROUND:
            if (childCount > 0)
            {
                const float gap = remaining / (float)childCount;
                primary += gap * 0.5F;
                spacing += gap;
            }
            break;
        case ZUI_JUSTIFY_SPACE_EVENLY:
            if (childCount > 0)
            {
                const float gap = remaining / (float)(childCount + 1);
                primary += gap;
                spacing += gap;
            }
            break;
        default:
            break;
        }

        // Position children
        for (unsigned int i = 0; i < childCount; i++)
        {
            const ZuiAlignData *a = ZuiGetAlignData(childIds[i]);
            Vector2 sz = ZuiMeasureWidget(childIds[i]);

            float pSize = 0;
            float sSize = 0;
            ZuiAlignmentX ax = l->childAlignX;
            ZuiAlignmentY ay = l->childAlignY;

            if (isVertical)
            {
                pSize = (a && a->heightMode == ZUI_SIZE_FILL) ? fillSize : sz.y;
                sSize = (a && a->widthMode == ZUI_SIZE_FILL) ? content.width : sz.x;
                if (a && a->overrideX)
                {
                    ax = a->alignX;
                }
            }
            else
            {
                pSize = (a && a->widthMode == ZUI_SIZE_FILL) ? fillSize : sz.x;
                sSize = (a && a->heightMode == ZUI_SIZE_FILL) ? content.height : sz.y;
                if (a && a->overrideY)
                {
                    ay = a->alignY;
                }
            }

            Rectangle childBounds = isVertical ? (Rectangle){content.x, primary, sSize, pSize}
                                               : (Rectangle){primary, content.y, pSize, sSize};
            Rectangle contentArea = isVertical ? (Rectangle){content.x, primary, content.width, pSize}
                                               : (Rectangle){primary, content.y, pSize, content.height};

            childBounds = ZuiAlignChild(childBounds, contentArea, ax, ay);
            ZuiPositionChild(childIds[i], childBounds);
            ZuiArrangeChildLayout(childIds[i], childBounds, depth);

            primary += pSize + spacing;
        }
    }

    static void ZuiArrangeChildLayout(unsigned int childId, Rectangle bounds, int depth)
    {
        const ZuiItem *child = ZuiGetItem(childId);
        if (!child || child->type != ZUI_FRAME)
        {
            return;
        }

        const ZuiLayoutData *l = ZuiGetLayoutData(childId);
        if (!l)
        {
            return;
        }

        const bool isVert = (l->kind == ZUI_LAYOUT_VERTICAL);
        const bool isHoriz = (l->kind == ZUI_LAYOUT_HORIZONTAL);

        if (isVert || isHoriz)
        {
            ZuiArrange(childId, bounds, depth + 1, isVert);
        }
        else if (l->kind == ZUI_LAYOUT_GRID)
        {
            const unsigned int cols = l->gridColumns > 0 ? l->gridColumns : 1;
            const unsigned int childCount = child->children.count;
            if (childCount == 0 || cols > ZUI_MAX_TREE_DEPTH)
            {
                return;
            }

            const unsigned int rows = (childCount + cols - 1) / cols;
            if (rows > ZUI_MAX_TREE_DEPTH)
            {
                return;
            }

            ZuiSetTransformBounds(childId, bounds);
            Rectangle content = {
                bounds.x + l->padding, bounds.y + l->padding,
                bounds.width - (l->padding * 2), bounds.height - (l->padding * 2)};

            const unsigned int *childIds = (const unsigned int *)child->children.items;
            float colWidths[ZUI_MAX_TREE_DEPTH] = {0};
            float rowHeights[ZUI_MAX_TREE_DEPTH] = {0};
            float colX[ZUI_MAX_TREE_DEPTH] = {0};
            float rowY[ZUI_MAX_TREE_DEPTH] = {0};

            // Measure
            for (unsigned int i = 0; i < childCount; i++)
            {
                Vector2 sz = ZuiMeasureWidget(childIds[i]);
                const unsigned int c = i % cols;
                const unsigned int r = i / cols;
                if (sz.x > colWidths[c])
                {
                    colWidths[c] = sz.x;
                }
                if (sz.y > rowHeights[r])
                {
                    rowHeights[r] = sz.y;
                }
            }

            // Compute positions
            colX[0] = content.x;
            for (unsigned int c = 1; c < cols; c++)
            {
                colX[c] = colX[c - 1] + colWidths[c - 1] + l->spacing;
            }

            rowY[0] = content.y;
            for (unsigned int r = 1; r < rows; r++)
            {
                rowY[r] = rowY[r - 1] + rowHeights[r - 1] + l->spacing;
            }

            // Position children
            for (unsigned int i = 0; i < childCount; i++)
            {
                const unsigned int c = i % cols;
                const unsigned int r = i / cols;
                Rectangle cell = {colX[c], rowY[r], colWidths[c], rowHeights[r]};

                const ZuiAlignData *a = ZuiGetAlignData(childIds[i]);
                ZuiAlignmentX ax = a && a->overrideX ? a->alignX : l->childAlignX;
                ZuiAlignmentY ay = a && a->overrideY ? a->alignY : l->childAlignY;

                Vector2 sz = ZuiMeasureWidget(childIds[i]);
                Rectangle aligned = ZuiAlignChild((Rectangle){cell.x, cell.y, sz.x, sz.y}, cell, ax, ay);

                ZuiPositionChild(childIds[i], aligned);
                ZuiArrangeChildLayout(childIds[i], aligned, depth + 1);
            }
        }
    }
    // NOLINTEND(misc-no-recursion)

    //-------------------------------------- LAYOUT API IMPLEMENTATION

    ZuiResult ZuiItemSetLayout(unsigned int itemId, ZuiLayoutKind kind)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || !g_zui_ctx)
        {
            return ZUI_ERROR_INVALID_ID;
        }

        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        if (l)
        {
            l->kind = kind;
            return ZUI_OK;
        }

        l = (ZuiLayoutData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (!l)
        {
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        l->itemId = itemId;
        l->kind = kind;
        l->spacing = ZUI_DEFAULT_FRAME_GAP;
        l->padding = ZUI_DEFAULT_FRAME_PADDING;
        l->gridColumns = 3;
        l->childAlignX = ZUI_ALIGN_X_LEFT;
        l->childAlignY = ZUI_ALIGN_Y_TOP;
        l->justify = ZUI_JUSTIFY_START;

        return ZUI_OK;
    }

    void ZuiLayoutSetSpacing(unsigned int itemId, float spacing)
    {
        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        if (l)
        {
            l->spacing = spacing;
        }
    }

    void ZuiLayoutSetPadding(unsigned int itemId, float padding)
    {
        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        if (l)
        {
            l->padding = padding;
        }
    }

    void ZuiLayoutSetGridColumns(unsigned int itemId, unsigned int columns)
    {
        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        if (l && columns > 0)
        {
            l->gridColumns = columns;
        }
    }

    void ZuiLayoutSetChildAlignX(unsigned int itemId, ZuiAlignmentX align)
    {
        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        if (l)
        {
            l->childAlignX = align;
        }
    }

    void ZuiLayoutSetChildAlignY(unsigned int itemId, ZuiAlignmentY align)
    {
        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        if (l)
        {
            l->childAlignY = align;
        }
    }

    void ZuiLayoutSetJustify(unsigned int itemId, ZuiJustify justify)
    {
        ZuiLayoutData *l = ZuiGetLayoutDataMut(itemId);
        if (l)
        {
            l->justify = justify;
        }
    }

    void ZuiItemSetAlignX(unsigned int itemId, ZuiAlignmentX align)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (a)
        {
            a->alignX = align;
            a->overrideX = true;
        }
    }

    void ZuiItemSetAlignY(unsigned int itemId, ZuiAlignmentY align)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (a)
        {
            a->alignY = align;
            a->overrideY = true;
        }
    }

    void ZuiItemSetSizeMode(unsigned int itemId, ZuiSizeMode widthMode, ZuiSizeMode heightMode)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (a)
        {
            a->widthMode = widthMode;
            a->heightMode = heightMode;
        }
    }

    void ZuiItemSetFixedSize(unsigned int itemId, float width, float height)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (a)
        {
            a->widthMode = ZUI_SIZE_FIXED;
            a->heightMode = ZUI_SIZE_FIXED;
            a->fixedWidth = width;
            a->fixedHeight = height;
        }
    }

    unsigned int ZuiBeginVertical(Vector2 pos, float spacing, Color color)
    {
        unsigned int id = ZuiBeginFrame((Rectangle){pos.x, pos.y, 0, 0}, color);
        ZuiItemSetLayout(id, ZUI_LAYOUT_VERTICAL);
        ZuiLayoutSetSpacing(id, spacing);
        return id;
    }

    unsigned int ZuiBeginHorizontal(Vector2 pos, float spacing, Color color)
    {
        unsigned int id = ZuiBeginFrame((Rectangle){pos.x, pos.y, 0, 0}, color);
        ZuiItemSetLayout(id, ZUI_LAYOUT_HORIZONTAL);
        ZuiLayoutSetSpacing(id, spacing);
        return id;
    }

    unsigned int ZuiBeginGrid(Vector2 pos, unsigned int columns, float spacing, Color color)
    {
        unsigned int id = ZuiBeginFrame((Rectangle){pos.x, pos.y, 0, 0}, color);
        ZuiItemSetLayout(id, ZUI_LAYOUT_GRID);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetGridColumns(id, columns);
        return id;
    }

    unsigned int ZuiBeginVerticalCentered(float spacing, Color color)
    {
        unsigned int id = ZuiBeginFrame((Rectangle){0, 0, 0, 0}, color);
        ZuiItemSetLayout(id, ZUI_LAYOUT_VERTICAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiItemSetAlignX(id, ZUI_ALIGN_X_CENTER);
        ZuiItemSetAlignY(id, ZUI_ALIGN_Y_CENTER);
        return id;
    }

    unsigned int ZuiBeginRow2(float spacing)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }
        Rectangle b = {g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, 0, 0};
        unsigned int id = ZuiBeginFrame(b, BLANK);
        ZuiItemSetLayout(id, ZUI_LAYOUT_HORIZONTAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetPadding(id, 0);
        return id;
    }

    unsigned int ZuiBeginColumn(float spacing)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }
        Rectangle b = {g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, 0, 0};
        unsigned int id = ZuiBeginFrame(b, BLANK);
        ZuiItemSetLayout(id, ZUI_LAYOUT_VERTICAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetPadding(id, 0);
        return id;
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

    void ZuiBeginRow(void)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return;
        }

        g_zui_ctx->cursor.isRow = true;
    }

    void ZuiEndFrame2(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        const unsigned int frameId = g_zui_ctx->cursor.activeFrame;
        const ZuiItem *item = ZuiGetItem(frameId);
        if (!item)
        {
            ZuiEndFrame();
            return;
        }

        const ZuiLayoutData *l = ZuiGetLayoutData(frameId);
        if (!l)
        {
            ZuiEndFrame();
            return;
        }

        // Measure
        if (l->kind == ZUI_LAYOUT_VERTICAL)
        {
            ZuiMeasure(frameId, 0, true);
        }
        else if (l->kind == ZUI_LAYOUT_HORIZONTAL)
        {
            ZuiMeasure(frameId, 0, false);
        }
        else if (l->kind == ZUI_LAYOUT_GRID)
        {
            ZuiProcessChildLayout(frameId, 0);
        }

        // Get measured size
        l = ZuiGetLayoutData(frameId);
        if (!l)
        {
            ZuiEndFrame();
            return;
        }

        Rectangle bounds = ZuiGetTransformBounds(frameId);
        if (bounds.width == 0)
        {
            bounds.width = l->measuredWidth;
        }
        if (bounds.height == 0)
        {
            bounds.height = l->measuredHeight;
        }

        if (l->kind == ZUI_LAYOUT_VERTICAL)
        {
            ZuiArrange(frameId, bounds, 0, true);
        }
        else if (l->kind == ZUI_LAYOUT_HORIZONTAL)
        {
            ZuiArrange(frameId, bounds, 0, false);
        }
        else if (l->kind == ZUI_LAYOUT_GRID)
        {
            ZuiArrangeChildLayout(frameId, bounds, 0);
        }

        const ZuiFrameData *parentFrame = ZuiGetFrameData(g_zui_ctx->cursor.parentFrame);
        if (parentFrame)
        {
            bounds = ZuiGetTransformBounds(frameId);
            ZuiAdvanceCursor(bounds.width, bounds.height);
        }

        g_zui_ctx->cursor.isRow = false;
        g_zui_ctx->cursor.activeFrame = g_zui_ctx->cursor.parentFrame;
    }

    //-------------------------------------- ANIMATION IMPLEMENTATION

    ZuiAnimationData *ZuiGetAnimationMut(unsigned int itemId)
    {
        return (ZuiAnimationData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_ANIMATION);
    }

    const ZuiAnimationData *ZuiGetAnimation(unsigned int itemId)
    {
        return (const ZuiAnimationData *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_ANIMATION);
    }

    ZuiAnimationData *ZuiAddAnimation(unsigned int itemId)
    {
        return (ZuiAnimationData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_ANIMATION);
    }

    void ZuiAnimSetTarget(unsigned int itemId, ZuiAnimationSlot slot, float target)
    {
        ZuiAnimationData *a = ZuiGetAnimationMut(itemId);
        if (!a)
        {
            a = ZuiAddAnimation(itemId);
        }

        if (a && slot < 8)
        {
            a->slots[slot].target = target;
            if (!a->slots[slot].active)
            {
                a->slots[slot].active = true;
                a->activeCount++;
            }
        }
    }

    float ZuiAnimGetValue(unsigned int itemId, ZuiAnimationSlot slot)
    {
        const ZuiAnimationData *a = ZuiGetAnimation(itemId);
        if (a && slot < 8)
        {
            return a->slots[slot].value;
        }
        return 0.0F;
    }

    void ZuiAnimSetDuration(unsigned int itemId, ZuiAnimationSlot slot, float duration)
    {
        ZuiAnimationData *a = ZuiGetAnimationMut(itemId);
        if (a && slot < 8)
        {
            a->slots[slot].duration = duration;
        }
    }

    void ZuiAnimSetSpring(unsigned int itemId, ZuiAnimationSlot slot, float damping, float stiffness)
    {
        ZuiAnimationData *a = ZuiGetAnimationMut(itemId);
        if (a && slot < 8)
        {
            a->slots[slot].type = ZUI_ANIM_SPRING;
            a->slots[slot].damping = damping;
            a->slots[slot].stiffness = stiffness;
        }
    }

    void ZuiAnimSetType(unsigned int itemId, ZuiAnimationSlot slot, ZuiAnimationType type)
    {
        ZuiAnimationData *a = ZuiGetAnimationMut(itemId);
        if (a && slot < 8)
        {
            a->slots[slot].type = type;
        }
    }

    void ZuiAnimReset(unsigned int itemId, ZuiAnimationSlot slot, float value)
    {
        ZuiAnimationData *a = ZuiGetAnimationMut(itemId);
        if (a && slot < 8)
        {
            a->slots[slot].value = value;
            a->slots[slot].target = value;
            a->slots[slot].velocity = 0.0F;
            a->slots[slot].elapsed = 0.0F;
            a->slots[slot].active = false;
        }
    }

    //-------------------------------------- INTERACTION IMPLEMENTATION

    ZuiInteractionData *ZuiGetInteractionMut(unsigned int itemId)
    {
        return (ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
    }

    const ZuiInteractionData *ZuiGetInteraction(unsigned int itemId)
    {
        return (const ZuiInteractionData *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_INTERACTION);
    }

    ZuiInteractionData *ZuiAddInteraction(unsigned int itemId)
    {
        return (ZuiInteractionData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_INTERACTION);
    }

    void ZuiEnableInteraction(unsigned int itemId)
    {
        const ZuiInteractionData *i = ZuiGetInteraction(itemId);
        if (!i)
        {
            (void)ZuiAddInteraction(itemId);
        }
    }

    void ZuiSetHoverCallback(unsigned int itemId, void (*onHover)(unsigned int))
    {
        ZuiInteractionData *i = ZuiGetInteractionMut(itemId);
        if (i)
        {
            i->onHoverEnter = onHover;
        }
    }

    void ZuiSetClickCallback(unsigned int itemId, void (*onClick)(unsigned int, ZuiMouseButton))
    {
        ZuiInteractionData *i = ZuiGetInteractionMut(itemId);
        if (i)
        {
            i->onClick = onClick;
        }
    }

    bool ZuiIsItemHovered(unsigned int itemId)
    {
        const ZuiInteractionData *i = ZuiGetInteraction(itemId);
        return i ? i->isHovered : false;
    }

    bool ZuiIsItemPressed(unsigned int itemId)
    {
        const ZuiInteractionData *i = ZuiGetInteraction(itemId);
        return i ? i->isPressed : false;
    }

    bool ZuiIsItemFocused(unsigned int itemId)
    {
        const ZuiInteractionData *i = ZuiGetInteraction(itemId);
        return i ? i->isFocused : false;
    }

    //-------------------------------------- STYLE IMPLEMENTATION

    ZuiStyleData *ZuiGetStyleMut(unsigned int itemId)
    {
        return (ZuiStyleData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_STYLE);
    }

    const ZuiStyleData *ZuiGetStyle(unsigned int itemId)
    {
        return (const ZuiStyleData *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_STYLE);
    }

    ZuiStyleData *ZuiAddStyle(unsigned int itemId)
    {
        return (ZuiStyleData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_STYLE);
    }

    void ZuiSetBackgroundColor(unsigned int itemId, Color normal, Color hovered, Color pressed)
    {
        ZuiStyleData *s = ZuiGetStyleMut(itemId);
        if (!s)
        {
            s = ZuiAddStyle(itemId);
        }

        if (s)
        {
            s->background.normal = normal;
            s->background.hovered = hovered;
            s->background.pressed = pressed;
        }
    }

    void ZuiSetBorderStyle(unsigned int itemId, float thickness, float radius, Color color)
    {
        ZuiStyleData *s = ZuiGetStyleMut(itemId);
        if (!s)
        {
            s = ZuiAddStyle(itemId);
        }

        if (s)
        {
            s->borderNormal = (ZuiBorderStyle){
                .thickness = thickness,
                .radius = radius,
                .color = color,
                .enabled = true};
        }
    }

    void ZuiSetTextStyle(unsigned int itemId, Font font, float size, Color color)
    {
        ZuiStyleData *s = ZuiGetStyleMut(itemId);
        if (!s)
        {
            s = ZuiAddStyle(itemId);
        }

        if (s)
        {
            s->font = font;
            s->fontSize = size;
            s->textColor = color;
            s->useCustomFont = true;
        }
    }

    void ZuiApplyTheme(unsigned int itemId, const char *themeName)
    {
        ZuiStyleData *s = ZuiGetStyleMut(itemId);
        if (!s)
        {
            s = ZuiAddStyle(itemId);
        }

        if (s)
        {
            s->themeOverride = themeName;
        }
    }

    //-------------------------------------- STATE IMPLEMENTATION

    ZuiStateData *ZuiGetStateMut(unsigned int itemId)
    {
        return (ZuiStateData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_STATE);
    }

    const ZuiStateData *ZuiGetState(unsigned int itemId)
    {
        return (const ZuiStateData *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_STATE);
    }

    ZuiStateData *ZuiAddState(unsigned int itemId)
    {
        return (ZuiStateData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_STATE);
    }

    void ZuiSetEnabled(unsigned int itemId, bool enabled)
    {
        ZuiStateData *s = ZuiGetStateMut(itemId);
        if (!s)
        {
            s = ZuiAddState(itemId);
        }

        if (s)
        {
            s->isEnabled = enabled;
        }
    }

    void ZuiSetVisible(unsigned int itemId, bool visible)
    {
        ZuiStateData *s = ZuiGetStateMut(itemId);
        if (!s)
        {
            s = ZuiAddState(itemId);
        }

        if (s)
        {
            s->isVisible = visible;
        }
    }

    void ZuiSetReadOnly(unsigned int itemId, bool readOnly)
    {
        ZuiStateData *s = ZuiGetStateMut(itemId);
        if (!s)
        {
            s = ZuiAddState(itemId);
        }

        if (s)
        {
            s->isReadOnly = readOnly;
        }
    }

    void ZuiSetValidationState(unsigned int itemId, ZuiValidationState state, const char *message)
    {
        ZuiStateData *s = ZuiGetStateMut(itemId);
        if (!s)
        {
            s = ZuiAddState(itemId);
        }

        if (s)
        {
            s->validationState = state;
            if (message)
            {
                snprintf(s->validationMessage, sizeof(s->validationMessage), "%s", message);
            }
        }
    }

    bool ZuiIsEnabled(unsigned int itemId)
    {
        const ZuiStateData *s = ZuiGetState(itemId);
        return s ? s->isEnabled : true;
    }

    bool ZuiIsVisible(unsigned int itemId)
    {
        const ZuiStateData *s = ZuiGetState(itemId);
        return s ? s->isVisible : true;
    }

    // SYSTEM UPDATE FUNCTIONS

    static bool ZuiCheckHitTest(unsigned int itemId, Vector2 point)
    {
        Rectangle bounds = ZuiGetTransformBounds(itemId);
        return CheckCollisionPointRec(point, bounds);
    }

    void ZuiUpdateInteractions(float deltaTime)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        Vector2 mousePos = GetMousePosition();

        for (unsigned int i = 0; i < g_zui_ctx->items.count; i++)
        {
            const ZuiItem *item = ZuiGetItem(i);
            if (!item || !ZuiItemHasComponent(i, ZUI_COMPONENT_INTERACTION))
            {
                continue;
            }

            ZuiInteractionData *interaction = ZuiGetInteractionMut(i);
            if (!interaction)
            {
                continue;
            }

            interaction->wasHovered = interaction->isHovered;
            interaction->wasPressed = interaction->isPressed;
            interaction->isHovered = ZuiCheckHitTest(i, mousePos);
            interaction->mousePosition = mousePos;

            if (interaction->isHovered && !interaction->wasHovered)
            {
                interaction->hoverTime = 0.0F;
                if (interaction->onHoverEnter)
                {
                    interaction->onHoverEnter(i);
                }
                ZuiAnimSetTarget(i, ZUI_ANIM_SLOT_HOVER, 1.0F);
            }
            else if (!interaction->isHovered && interaction->wasHovered)
            {
                if (interaction->onHoverExit)
                {
                    interaction->onHoverExit(i);
                }
                ZuiAnimSetTarget(i, ZUI_ANIM_SLOT_HOVER, 0.0F);
            }

            if (interaction->isHovered)
            {
                interaction->hoverTime += deltaTime;
            }

            if (interaction->isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                interaction->isPressed = true;
                interaction->pressTime = 0.0F;
                interaction->pressedButton = ZUI_MOUSE_LEFT;
                ZuiAnimSetTarget(i, ZUI_ANIM_SLOT_PRESS, 1.0F);

                if (interaction->onPress)
                {
                    interaction->onPress(i, ZUI_MOUSE_LEFT);
                }
            }

            if (interaction->isPressed)
            {
                interaction->pressTime += deltaTime;
            }

            if (interaction->isPressed && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                interaction->isPressed = false;
                ZuiAnimSetTarget(i, ZUI_ANIM_SLOT_PRESS, 0.0F);

                if (interaction->onRelease)
                {
                    interaction->onRelease(i, interaction->pressedButton);
                }

                if (interaction->isHovered && interaction->onClick)
                {
                    interaction->onClick(i, interaction->pressedButton);
                }
            }
        }
    }

    void ZuiUpdateAnimations(float deltaTime)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        for (unsigned int i = 0; i < g_zui_ctx->items.count; i++)
        {
            if (!ZuiItemHasComponent(i, ZUI_COMPONENT_ANIMATION))
            {
                continue;
            }

            ZuiAnimationData *animData = ZuiGetAnimationMut(i);
            if (!animData)
            {
                continue;
            }

            for (unsigned int j = 0; j < 8; j++)
            {
                ZuiAnimation *anim = &animData->slots[j];
                if (!anim->active)
                {
                    continue;
                }

                float delta = anim->target - anim->value;

                if (fabsf(delta) < 0.001F && fabsf(anim->velocity) < 0.001F)
                {
                    anim->value = anim->target;
                    anim->velocity = 0.0F;
                    anim->active = false;
                    animData->activeCount--;
                    continue;
                }

                switch (anim->type)
                {
                case ZUI_ANIM_LINEAR:
                {
                    float speed = 1.0F / anim->duration;
                    float step = speed * deltaTime;
                    if (fabsf(delta) < step)
                    {
                        anim->value = anim->target;
                    }
                    else
                    {
                        anim->value += (delta > 0.0F ? step : -step);
                    }
                    break;
                }

                case ZUI_ANIM_EASE_OUT:
                {
                    float t = 1.0F - powf(0.001F, deltaTime / anim->duration);
                    anim->value += delta * t;
                    break;
                }

                case ZUI_ANIM_SPRING:
                {
                    float force = (anim->stiffness * delta) - (anim->damping * anim->velocity);
                    anim->velocity += force * deltaTime;
                    anim->value += anim->velocity * deltaTime;
                    break;
                }
                }

                anim->elapsed += deltaTime;
            }
        }
    }

    //-------------------------------------- LAYER IMPLEMENTATION

    ZuiLayerData *ZuiGetLayerMut(unsigned int itemId)
    {
        return (ZuiLayerData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYER);
    }

    const ZuiLayerData *ZuiGetLayer(unsigned int itemId)
    {
        return (const ZuiLayerData *)ZuiItemGetComponentConst(itemId, ZUI_COMPONENT_LAYER);
    }

    ZuiLayerData *ZuiAddLayer(unsigned int itemId)
    {
        return (ZuiLayerData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_LAYER);
    }

    void ZuiSetLayer(unsigned int itemId, int layer, int order)
    {
        ZuiLayerData *l = ZuiGetLayerMut(itemId);
        if (!l)
        {
            l = ZuiAddLayer(itemId);
        }

        if (l)
        {
            l->layer = layer;
            l->order = order;
        }
    }

    void ZuiSetLayerPreset(unsigned int itemId, ZuiLayerPreset preset)
    {
        ZuiLayerData *l = ZuiGetLayerMut(itemId);
        if (!l)
        {
            l = ZuiAddLayer(itemId);
        }

        if (l)
        {
            l->layer = (int)preset;
            l->order = 0;
        }
    }

    void ZuiSetLayerBlocksInput(unsigned int itemId, bool blocks)
    {
        ZuiLayerData *l = ZuiGetLayerMut(itemId);
        if (!l)
        {
            l = ZuiAddLayer(itemId);
        }

        if (l)
        {
            l->blocksInput = blocks;
        }
    }

    void ZuiRaiseToFront(unsigned int itemId)
    {
        ZuiLayerData *l = ZuiGetLayerMut(itemId);
        if (!l)
        {
            l = ZuiAddLayer(itemId);
        }

        if (!l)
        {
            return;
        }

        // Find max order in same layer
        int maxOrder = l->order;

        if (g_zui_ctx)
        {
            for (unsigned int i = 0; i < g_zui_ctx->items.count; i++)
            {
                if (i == itemId)
                {
                    continue;
                }

                const ZuiLayerData *other = ZuiGetLayer(i);
                if (other && other->layer == l->layer && other->order > maxOrder)
                {
                    maxOrder = other->order;
                }
            }
        }

        l->order = maxOrder + 1;
    }

    void ZuiLowerToBack(unsigned int itemId)
    {
        ZuiLayerData *l = ZuiGetLayerMut(itemId);
        if (!l)
        {
            l = ZuiAddLayer(itemId);
        }

        if (!l)
        {
            return;
        }

        // Find min order in same layer
        int minOrder = l->order;

        if (g_zui_ctx)
        {
            for (unsigned int i = 0; i < g_zui_ctx->items.count; i++)
            {
                if (i == itemId)
                {
                    continue;
                }

                const ZuiLayerData *other = ZuiGetLayer(i);
                if (other && other->layer == l->layer && other->order < minOrder)
                {
                    minOrder = other->order;
                }
            }
        }

        l->order = minOrder - 1;
    }

    int ZuiGetLayerValue(unsigned int itemId)
    {
        const ZuiLayerData *l = ZuiGetLayer(itemId);
        return l ? l->layer : ZUI_LAYER_CONTENT;
    }

    int ZuiGetLayerOrder(unsigned int itemId)
    {
        const ZuiLayerData *l = ZuiGetLayer(itemId);
        return l ? l->order : 0;
    }

    bool ZuiLayerBlocksInput(unsigned int itemId)
    {
        const ZuiLayerData *l = ZuiGetLayer(itemId);
        return l ? l->blocksInput : false;
    }
    // -----------------------------------------------------------------------------
    // zui_item.c

    const ZuiItem *ZuiGetItem(unsigned int id)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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

        if (!(parent->isContainer))
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
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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
            .children = (ZuiDynArray){0},
            .componentCount = 0,
            .isContainer = false,
            .canMove = false,
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

        if (item->isContainer)
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

        TraceLog(LOG_INFO, "├─ Item %u: %s (parent: %u)",
                 item->id,
                 ZuiGetWidgetType(item->type),
                 item->parentId);

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

    void ZuiPrintFullItemTree(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        TraceLog(LOG_INFO, "================ FULL ITEM TREE ================");
        TraceLog(LOG_INFO, "Total items: %u", g_zui_ctx->items.count);

        // Print from root (item 0)
        if (g_zui_ctx->items.count > 0)
        {
            for (unsigned int i = 0; i < g_zui_ctx->items.count - 1; ++i)
            {

                ZuiPrintItemTree(i, 0);
            }
        }

        TraceLog(LOG_INFO, "================================================");
    }
    // -----------------------------------------------------------------------------
    // zui_type_registry.c

    ZuiResult ZuiInitTypeRegistry(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ERROR_NULL_CONTEXT;
        }

        ZuiTypeRegistry *registry = &g_zui_ctx->typeRegistry;

        ZuiResult result = ZuiInitDynArray(
            &registry->registrations,
            &g_zui_arena,
            16,
            sizeof(ZuiTypeRegistration),
            ZUI_ALIGNOF(ZuiTypeRegistration),
            "TypeRegistrations");

        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to init type registry");
            return result;
        }

        registry->nextTypeId = 0;

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: Type registry initialized");
#endif

        return ZUI_OK;
    }

    unsigned int ZuiRegisterType(const ZuiTypeInfo *typeInfo)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }
        if (!typeInfo || !typeInfo->name)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "TypeInfo or name is NULL");
            return ZUI_ID_INVALID;
        }
        if (typeInfo->dataSize == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Data size must be > 0");
            return ZUI_ID_INVALID;
        }
        if (!ZuiIsPowerOfTwo(typeInfo->dataAlignment))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ALIGNMENT, "Data alignment must be power of 2");
            return ZUI_ID_INVALID;
        }
        if (!typeInfo->render)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Render function is required");
            return ZUI_ID_INVALID;
        }

        ZuiTypeRegistry *registry = &g_zui_ctx->typeRegistry;

        // Check for duplicate name
        for (unsigned int i = 0; i < registry->registrations.count; i++)
        {
            const ZuiTypeRegistration *reg = (ZuiTypeRegistration *)ZuiGetDynArray(&registry->registrations, i);
            if (!reg)
            {
                continue;
            }
            if (strcmp(reg->info.name, typeInfo->name) == 0)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Type '%s' already registered", typeInfo->name);
                return ZUI_ID_INVALID;
            }
        }

        ZuiTypeRegistration *reg = (ZuiTypeRegistration *)ZuiPushDynArray(&registry->registrations, &g_zui_arena);
        if (!reg)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate type registration");
            return ZUI_ID_INVALID;
        }

        *reg = (ZuiTypeRegistration){
            .info = *typeInfo,
            .typeId = registry->nextTypeId++,
            .isBuiltin = false,
        };

        const unsigned int capacity = typeInfo->initialCapacity > 0 ? typeInfo->initialCapacity : 32;
        ZuiResult result = ZuiInitDynArray(&reg->dataArray, &g_zui_arena, capacity,
                                           typeInfo->dataSize, typeInfo->dataAlignment, typeInfo->name);
        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to init data array for type '%s'", typeInfo->name);
            registry->registrations.count--;
            return ZUI_ID_INVALID;
        }

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: Registered type '%s' with ID %u", typeInfo->name, reg->typeId);
#endif
        return reg->typeId;
    }

    ZuiTypeRegistration *ZuiGetTypeRegistration(unsigned int typeId)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return NULL;
        }

        ZuiTypeRegistry *registry = &g_zui_ctx->typeRegistry;

        for (unsigned int i = 0; i < registry->registrations.count; i++)
        {
            ZuiTypeRegistration *reg = (ZuiTypeRegistration *)ZuiGetDynArray(&registry->registrations, i);
            if (reg && reg->typeId == typeId)
            {
                return reg;
            }
        }

        return NULL;
    }

    const ZuiTypeRegistration *ZuiGetTypeRegistrationConst(unsigned int typeId)
    {
        return ZuiGetTypeRegistration(typeId);
    }

    const ZuiTypeInfo *ZuiGetTypeInfo(unsigned int typeId)
    {
        const ZuiTypeRegistration *reg = ZuiGetTypeRegistrationConst(typeId);
        return reg ? &reg->info : NULL;
    }

    unsigned int ZuiGetTypeIdByName(const char *name)
    {
        if (!g_zui_ctx || !name)
        {
            return ZUI_ID_INVALID;
        }

        ZuiTypeRegistry *registry = &g_zui_ctx->typeRegistry;

        for (unsigned int i = 0; i < registry->registrations.count; i++)
        {
            ZuiTypeRegistration *reg = (ZuiTypeRegistration *)ZuiGetDynArray(&registry->registrations, i);
            if (reg && strcmp(reg->info.name, name) == 0)
            {
                return reg->typeId;
            }
        }

        return ZUI_ID_INVALID;
    }

    bool ZuiIsValidTypeId(unsigned int typeId)
    {
        return ZuiGetTypeRegistration(typeId) != NULL;
    }

    unsigned int ZuiCreateTypedItem(unsigned int typeId)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(typeId);
        if (!reg)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid type ID %u", typeId);
            return ZUI_ID_INVALID;
        }

        void *data = ZuiPushDynArray(&reg->dataArray, &g_zui_arena);
        if (!data)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate data for type '%s'", reg->info.name);
            return ZUI_ID_INVALID;
        }

        unsigned int dataIndex = reg->dataArray.count - 1;

        ZuiItem *item = (ZuiItem *)ZuiPushDynArray(&g_zui_ctx->items, &g_zui_arena);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate item");
            reg->dataArray.count--;
            return ZUI_ID_INVALID;
        }

        unsigned int itemId = g_zui_ctx->items.count - 1;

        *item = (ZuiItem){
            .id = itemId,
            .parentId = ZUI_ID_INVALID,
            .type = typeId,
            .dataIndex = dataIndex,
            .children = (ZuiDynArray){0},
            .isContainer = false,
            .canMove = false,
        };

        if (reg->info.init)
        {
            reg->info.init(data, itemId);
        }

        return itemId;
    }

    void *ZuiGetTypedData(unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return NULL;
        }

        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(item->type);
        if (!reg)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid type ID %u for item %u", item->type, itemId);
            return NULL;
        }

        if (item->dataIndex >= reg->dataArray.count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS, "Data index %u out of bounds", item->dataIndex);
            return NULL;
        }

        return ZuiGetDynArray(&reg->dataArray, item->dataIndex);
    }

    const void *ZuiGetTypedDataConst(unsigned int itemId)
    {
        return ZuiGetTypedData(itemId);
    }
    // -----------------------------------------------------------------------------
    // zui_behavior.c

    void ZuiDispatchUpdate(const ZuiItem *item)
    {
        if (!item)
        {
            return;
        }

        const ZuiTypeRegistration *reg = ZuiGetTypeRegistrationConst(item->type);
        if (!reg)
        {
            return;
        }

        if (reg->info.update)
        {
            reg->info.update(item->dataIndex);
        }
    }

    void ZuiDispatchRender(const ZuiItem *item)
    {
        if (!item)
        {
            return;
        }

        const ZuiTypeRegistration *reg = ZuiGetTypeRegistrationConst(item->type);
        if (!reg)
        {
            return;
        }

        if (reg->info.render)
        {
            reg->info.render(item->dataIndex);
        }
    }
    // -----------------------------------------------------------------------------
    // zui_frame.c (enhanced)

    const ZuiFrameData *ZuiGetFrameData(unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_BUILTIN_FRAME)
        {
            return NULL;
        }
        return (const ZuiFrameData *)ZuiGetTypedDataConst(itemId);
    }

    ZuiFrameData *ZuiGetFrameDataMut(unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_BUILTIN_FRAME)
        {
            return NULL;
        }
        return (ZuiFrameData *)ZuiGetTypedData(itemId);
    }

    unsigned int ZuiCreateFrame(Rectangle bounds, Color color)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }
        if (!ZuiValidateBounds(bounds))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_BOUNDS, "Invalid bounds");
            return ZUI_ID_INVALID;
        }

        // Create item
        unsigned int itemId = ZuiCreateTypedItem(ZUI_BUILTIN_FRAME);
        if (itemId == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        // Initialize frame data
        ZuiFrameData *frameData = ZuiGetFrameDataMut(itemId);
        if (!frameData)
        {
            return ZUI_ID_INVALID;
        }

        *frameData = (ZuiFrameData){
            .itemId = itemId,
            .cornerRadius = (float)ZUI_CORNER_RADIUS,
            .hasShadow = false,
        };

        // Add Transform component
        ZuiTransform *transform = (ZuiTransform *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!transform)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to add transform component");
            return ZUI_ID_INVALID;
        }
        *transform = (ZuiTransform){
            .itemId = itemId,
            .bounds = bounds,
            .offset = (Vector2){0, 0},
            .isDirty = false,
        };

        // Add Style component
        ZuiStyleData *style = ZuiAddStyle(itemId);
        if (style)
        {
            style->background.normal = color;
            style->borderNormal.enabled = false;
            style->borderNormal.thickness = 0.0F;
            style->borderNormal.color = BLACK;
            style->borderNormal.radius = frameData->cornerRadius;
        }

        // Add State component
        ZuiStateData *state = ZuiAddState(itemId);
        if (state)
        {
            state->isVisible = true;
            state->isEnabled = true;
        }

        // Add Layer component
        ZuiLayerData *layer = ZuiAddLayer(itemId);
        if (layer)
        {
            layer->layer = ZUI_LAYER_CONTENT;
            layer->order = 0;
            layer->blocksInput = false;
        }

        // Add Animation component (for smooth size/position changes)
        const ZuiAnimationData *anim = ZuiAddAnimation(itemId);
        if (anim)
        {
            ZuiAnimSetType(itemId, ZUI_ANIM_SLOT_ALPHA, ZUI_ANIM_EASE_OUT);
            ZuiAnimSetDuration(itemId, ZUI_ANIM_SLOT_ALPHA, 0.2F);
            ZuiAnimReset(itemId, ZUI_ANIM_SLOT_ALPHA, 1.0F);
        }

        // Setup as container
        ZuiItem *item = ZuiGetItemMut(itemId);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INTERNAL_ERROR, "Failed to get item");
            return ZUI_ID_INVALID;
        }

        item->isContainer = true;
        ZuiResult result = ZuiInitDynArray(&item->children, &g_zui_arena,
                                           ZUI_CHILDREN_CAPACITY, sizeof(unsigned int),
                                           _Alignof(unsigned int), "Children");
        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to initialize children");
            return ZUI_ID_INVALID;
        }

        return itemId;
    }

    void ZuiUpdateFrame(unsigned int dataIndex)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(ZUI_BUILTIN_FRAME);
        if (!reg)
        {
            return;
        }

        const ZuiFrameData *frameData = (const ZuiFrameData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!frameData)
        {
            return;
        }

        const ZuiItem *frameItem = ZuiGetItemMut(frameData->itemId);
        if (!frameItem || !frameItem->isContainer)
        {
            return;
        }

        // Update children
        const unsigned int *childIds = (unsigned int *)frameItem->children.items;
        for (unsigned int i = 0; i < frameItem->children.count; i++)
        {
            const ZuiItem *child = ZuiGetItem(childIds[i]);
            if (child)
            {
                ZuiDispatchUpdate(child);
            }
        }
    }

    void ZuiRenderFrame(unsigned int dataIndex)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(ZUI_BUILTIN_FRAME);
        if (!reg)
        {
            return;
        }

        const ZuiFrameData *frameData = (const ZuiFrameData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!frameData)
        {
            return;
        }

        unsigned int itemId = frameData->itemId;

        // Check State component
        const ZuiStateData *state = ZuiGetState(itemId);
        if (state && !state->isVisible)
        {
            return;
        }

        const ZuiItem *frameItem = ZuiGetItem(itemId);
        if (!frameItem)
        {
            return;
        }

        Rectangle bounds = ZuiGetTransformBounds(itemId);

        // Get style
        const ZuiStyleData *style = ZuiGetStyle(itemId);
        Color bgColor = style ? style->background.normal : BLANK;

        // Apply animation (alpha fade)
        float alpha = ZuiAnimGetValue(itemId, ZUI_ANIM_SLOT_ALPHA);
        bgColor.a = (unsigned char)((float)bgColor.a * alpha);

        // Calculate roundness
        float roundness = ZuiPixelsToRoundness(bounds, frameData->cornerRadius);

        // Draw shadow if enabled
        if (frameData->hasShadow && style && style->hasShadow)
        {
            Rectangle shadowBounds = bounds;
            shadowBounds.x += style->shadowOffset.x;
            shadowBounds.y += style->shadowOffset.y;
            DrawRectangleRounded(shadowBounds, roundness, ZUI_ROUNDNESS_SEGMENTS,
                                 Fade(style->shadowColor, alpha));
        }

        // Draw background
        if (bgColor.a > 0)
        {
            DrawRectangleRounded(bounds, roundness, ZUI_ROUNDNESS_SEGMENTS, bgColor);
        }

        // Draw border
        if (style && style->borderNormal.enabled && style->borderNormal.thickness > 0)
        {
            Color borderColor = style->borderNormal.color;
            borderColor.a = (unsigned char)((float)borderColor.a * alpha);
            DrawRectangleRoundedLinesEx(bounds, roundness, ZUI_ROUNDNESS_SEGMENTS,
                                        style->borderNormal.thickness, borderColor);
        }

        // Render children
        if (frameItem->isContainer)
        {
            const unsigned int *childIds = (unsigned int *)frameItem->children.items;
            for (unsigned int i = 0; i < frameItem->children.count; i++)
            {
                const ZuiItem *child = ZuiGetItem(childIds[i]);
                if (child)
                {
                    ZuiDispatchRender(child);
                }
            }
        }
    }

    unsigned int ZuiBeginFrame(Rectangle bounds, Color color)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        g_zui_ctx->cursor.isRow = false;
        unsigned int id = ZuiCreateFrame(bounds, color);

        if (id == ZUI_ID_INVALID)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to create frame");
            return ZUI_ID_INVALID;
        }

        ZuiSetTransformBounds(id, bounds);
        float pad = ZuiGetFramePadding(id);

        unsigned int active = g_zui_ctx->cursor.activeFrame;
        ZuiItemAddChild(active, id);

        g_zui_ctx->cursor.parentFrame = g_zui_ctx->cursor.activeFrame;
        g_zui_ctx->cursor.position = (Vector2){bounds.x + pad, bounds.y + pad};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.activeFrame = id;

        return id;
    }

    void ZuiEndFrame(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        g_zui_ctx->cursor.isRow = false;
        g_zui_ctx->cursor.activeFrame = g_zui_ctx->cursor.parentFrame;
    }

    unsigned int ZuiNewFrame(Color color, float width, float height)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        Rectangle bounds = (Rectangle){
            g_zui_ctx->cursor.position.x,
            g_zui_ctx->cursor.position.y,
            width, height};

        unsigned int id = ZuiCreateFrame(bounds, color);
        ZuiSetTransformBounds(id, bounds);

        unsigned int active = g_zui_ctx->cursor.activeFrame;
        ZuiItemAddChild(active, id);

        ZuiAdvanceCursor(width, height);
        g_zui_ctx->cursor.lastItemBounds = bounds;

        return id;
    }

    void ZuiFrameBackground(Color color)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiStyleData *style = ZuiGetStyleMut(g_zui_ctx->cursor.activeFrame);
        if (style)
        {
            style->background.normal = color;
        }
    }

    void ZuiFrameOutline(Color color, float thickness)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiStyleData *style = ZuiGetStyleMut(g_zui_ctx->cursor.activeFrame);
        if (style)
        {
            style->borderNormal.enabled = (color.a > 0 && thickness > 0);
            style->borderNormal.thickness = thickness;
            style->borderNormal.color = color;
        }
    }

    void ZuiFrameGap(float gap)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiLayoutData *layout = ZuiGetActiveLayoutMut();
        if (layout)
        {
            layout->spacing = gap;
        }
    }

    void ZuiFramePad(float pad)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiLayoutData *layout = ZuiGetActiveLayoutMut();
        if (layout)
        {
            layout->padding = pad;

            Rectangle bounds = ZuiGetTransformBounds(g_zui_ctx->cursor.activeFrame);
            g_zui_ctx->cursor.position = (Vector2){bounds.x + pad, bounds.y + pad};
            ZuiUpdateRestPosition();
        }
    }

    void ZuiFrameOffset(float x, float y)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiTransform *transform = ZuiGetTransformMut(g_zui_ctx->cursor.activeFrame);
        if (transform)
        {
            transform->offset.x += x;
            transform->offset.y += y;
        }
    }

    void ZuiFrameCornerRadius(float radius)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiFrameData *frame = ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
        if (frame)
        {
            frame->cornerRadius = radius;
        }
    }

    float ZuiGetFramePadding(unsigned int frameId)
    {
        const ZuiLayoutData *layout = ZuiGetLayoutData(frameId);
        return layout ? layout->padding : ZUI_DEFAULT_FRAME_PADDING;
    }

    float ZuiGetFrameSpacing(unsigned int frameId)
    {
        const ZuiLayoutData *layout = ZuiGetLayoutData(frameId);
        return layout ? layout->spacing : ZUI_DEFAULT_FRAME_GAP;
    }

    void ZuiPrintFrame(unsigned int id)
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

        Rectangle bounds = ZuiGetTransformBounds(id);
        float pad = ZuiGetFramePadding(id);
        float gap = ZuiGetFrameSpacing(id);

        TraceLog(LOG_INFO, "┌─────── FRAME DATA ───────");
        TraceLog(LOG_INFO, "│ Item ID:      %u", frame->itemId);
        TraceLog(LOG_INFO, "│ Bounds:       (%.1f, %.1f, %.1fx%.1f)",
                 (double)bounds.x, (double)bounds.y,
                 (double)bounds.width, (double)bounds.height);
        TraceLog(LOG_INFO, "│ Padding:      %.1f", (double)pad);
        TraceLog(LOG_INFO, "│ Gap:          %.1f", (double)gap);
        TraceLog(LOG_INFO, "│ Components:   %u", item->componentCount);
        TraceLog(LOG_INFO, "└──────────────────────────");
    }
    // -----------------------------------------------------------------------------
    // zui_label.c (enhanced)

    const ZuiLabelData *ZuiGetLabelData(unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_BUILTIN_LABEL)
        {
            return NULL;
        }
        return (const ZuiLabelData *)ZuiGetTypedDataConst(itemId);
    }

    ZuiLabelData *ZuiGetLabelDataMut(unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_BUILTIN_LABEL)
        {
            return NULL;
        }
        return (ZuiLabelData *)ZuiGetTypedData(itemId);
    }

    unsigned int ZuiCreateLabel(const char *text, Font font)
    {
        if (!g_zui_ctx || !text)
        {
            return ZUI_ID_INVALID;
        }

        // Create item
        unsigned int itemId = ZuiCreateTypedItem(ZUI_BUILTIN_LABEL);
        if (itemId == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        // Initialize label data
        ZuiLabelData *labelData = ZuiGetLabelDataMut(itemId);
        if (!labelData)
        {
            return ZUI_ID_INVALID;
        }

        *labelData = (ZuiLabelData){
            .itemId = itemId,
            .font = font,
            .fontSize = ZUI_BASE_FONT_SIZE,
            .fontSpacing = ZUI_FONT_SPACING,
            .isMono = false,
            .onClickSimple = NULL,
        };
        snprintf(labelData->text, ZUI_MAX_TEXT_LENGTH, "%s", text);

        Vector2 textSize = MeasureTextEx(font, text, labelData->fontSize, labelData->fontSpacing);

        // Add Transform component
        ZuiTransform *transform = (ZuiTransform *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!transform)
        {
            return ZUI_ID_INVALID;
        }
        *transform = (ZuiTransform){
            .itemId = itemId,
            .bounds = (Rectangle){0, 0, textSize.x, textSize.y},
            .offset = (Vector2){0, 0},
            .isDirty = false,
        };

        // Add Style component (for colors)
        ZuiStyleData *style = ZuiAddStyle(itemId);
        if (style)
        {
            style->textColor = BLACK;
            style->background.normal = BLANK;
            style->fontSize = labelData->fontSize;
            style->fontSpacing = labelData->fontSpacing;
        }

        // Add State component (visible, enabled)
        ZuiStateData *state = ZuiAddState(itemId);
        if (state)
        {
            state->isVisible = true;
            state->isEnabled = true;
        }

        // Add Layer component (default to content layer)
        ZuiLayerData *layer = ZuiAddLayer(itemId);
        if (layer)
        {
            layer->layer = ZUI_LAYER_CONTENT;
            layer->order = 0;
            layer->blocksInput = false;
        }

        // Add Animation component (for smooth transitions)
        const ZuiAnimationData *anim = ZuiAddAnimation(itemId);
        if (anim)
        {
            ZuiAnimSetDuration(itemId, ZUI_ANIM_SLOT_ALPHA, 0.2F);
            ZuiAnimSetType(itemId, ZUI_ANIM_SLOT_ALPHA, ZUI_ANIM_EASE_OUT);
            ZuiAnimReset(itemId, ZUI_ANIM_SLOT_ALPHA, 1.0F);
        }

        return itemId;
    }

    void ZuiRenderLabel(unsigned int dataIndex)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(ZUI_BUILTIN_LABEL);
        if (!reg)
        {
            return;
        }

        const ZuiLabelData *labelData = (const ZuiLabelData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!labelData)
        {
            return;
        }

        unsigned int itemId = labelData->itemId;

        // Check State component
        const ZuiStateData *state = ZuiGetState(itemId);
        if (state && !state->isVisible)
        {
            return;
        }

        // Get transform
        Rectangle bounds = ZuiGetTransformBounds(itemId);

        // Get style
        const ZuiStyleData *style = ZuiGetStyle(itemId);
        Color textColor = style ? style->textColor : BLACK;
        Color bgColor = style ? style->background.normal : BLANK;

        // Apply animation (alpha fade)
        float alpha = ZuiAnimGetValue(itemId, ZUI_ANIM_SLOT_ALPHA);
        textColor.a = (unsigned char)((float)textColor.a * alpha);
        bgColor.a = (unsigned char)((float)bgColor.a * alpha);

        // Apply interaction states (if interactive)
        const ZuiInteractionData *interaction = ZuiGetInteraction(itemId);
        if (interaction && style)
        {
            if (interaction->isPressed)
            {
                textColor = style->foreground.pressed;
                bgColor = style->background.pressed;
            }
            else if (interaction->isHovered)
            {
                textColor = style->foreground.hovered;
                bgColor = style->background.hovered;
            }
        }

        // Draw background
        if (bgColor.a > 0)
        {
            DrawRectangleRec(bounds, bgColor);
        }

        // Draw text
        DrawTextEx(labelData->font, labelData->text,
                   (Vector2){bounds.x, bounds.y},
                   labelData->fontSize, labelData->fontSpacing, textColor);
    }

    unsigned int ZuiNewLabel(const char *text)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        Font font = g_zui_ctx->font;
        unsigned int id = ZuiCreateLabel(text, font);

        Rectangle bounds = ZuiGetTransformBounds(id);
        bounds.x = g_zui_ctx->cursor.position.x;
        bounds.y = g_zui_ctx->cursor.position.y;
        ZuiSetTransformBounds(id, bounds);

        ZuiItemAddChild(g_zui_ctx->cursor.activeFrame, id);
        ZuiAdvanceCursor(bounds.width, bounds.height);
        g_zui_ctx->cursor.lastItemBounds = bounds;
        g_zui_ctx->cursor.activeLabel = id;

        return id;
    }

    unsigned int ZuiNewMonoLabel(const char *text)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        unsigned int id = ZuiCreateLabel(text, g_zui_ctx->monoFont);

        ZuiLabelData *data = ZuiGetLabelDataMut(id);
        if (data)
        {
            data->isMono = true;
        }

        Rectangle bounds = ZuiGetTransformBounds(id);
        bounds.x = g_zui_ctx->cursor.position.x;
        bounds.y = g_zui_ctx->cursor.position.y;
        ZuiSetTransformBounds(id, bounds);

        ZuiItemAddChild(g_zui_ctx->cursor.activeFrame, id);
        ZuiAdvanceCursor(bounds.width, bounds.height);
        g_zui_ctx->cursor.lastItemBounds = bounds;
        g_zui_ctx->cursor.activeLabel = id;

        return id;
    }

    void ZuiLabelTextColor(Color textColor)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiStyleData *style = ZuiGetStyleMut(g_zui_ctx->cursor.activeLabel);
        if (style)
        {
            style->textColor = textColor;
        }
    }

    void ZuiLabelBackgroundColor(Color backgroundColor)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiStyleData *style = ZuiGetStyleMut(g_zui_ctx->cursor.activeLabel);
        if (style)
        {
            style->background.normal = backgroundColor;
        }
    }

    void ZuiLabelAlignX(ZuiAlignmentX align)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        unsigned int labelId = g_zui_ctx->cursor.activeLabel;
        Rectangle labelBounds = ZuiGetTransformBounds(labelId);
        Rectangle frameBounds = ZuiGetTransformBounds(g_zui_ctx->cursor.activeFrame);
        float pad = ZuiGetFramePadding(g_zui_ctx->cursor.activeFrame);

        switch (align)
        {
        case ZUI_ALIGN_X_CENTER:
            labelBounds.x = frameBounds.x + (frameBounds.width * 0.5F) - (labelBounds.width * 0.5F);
            break;
        case ZUI_ALIGN_X_LEFT:
            labelBounds.x = frameBounds.x + pad;
            break;
        case ZUI_ALIGN_X_RIGHT:
            labelBounds.x = frameBounds.x + frameBounds.width - pad - labelBounds.width;
            break;
        }

        ZuiSetTransformBounds(labelId, labelBounds);
    }

    void ZuiLabelOffset(float x, float y)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiTransform *transform = ZuiGetTransformMut(g_zui_ctx->cursor.activeLabel);
        if (transform)
        {
            transform->offset.x += x;
            transform->offset.y += y;
            g_zui_ctx->cursor.position.x += x;
            g_zui_ctx->cursor.position.y += y;
        }
    }

    // Wrapper function to convert Interaction callback to simple callback
    static void ZuiLabelClickWrapper(unsigned int id, ZuiMouseButton button)
    {
        (void)button; // Ignore button parameter

        ZuiLabelData *label = ZuiGetLabelDataMut(id);
        if (label && label->onClickSimple)
        {
            label->onClickSimple(id);
        }
    }

    void ZuiLabelMakeClickable(void (*onClick)(unsigned int))
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        unsigned int labelId = g_zui_ctx->cursor.activeLabel;

        // Store the simple callback in label data
        ZuiLabelData *labelData = ZuiGetLabelDataMut(labelId);
        if (labelData)
        {
            labelData->onClickSimple = onClick;
        }

        // Add Interaction component with wrapper function
        ZuiInteractionData *interaction = ZuiAddInteraction(labelId);
        if (interaction)
        {
            interaction->onClick = ZuiLabelClickWrapper;
            interaction->capturesMouse = true;
            interaction->acceptsFocus = false;
        }

        // Add hover animation
        ZuiAnimSetType(labelId, ZUI_ANIM_SLOT_HOVER, ZUI_ANIM_EASE_OUT);
        ZuiAnimSetDuration(labelId, ZUI_ANIM_SLOT_HOVER, 0.15F);
    }

    void ZuiPrintLabel(unsigned int id)
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

        Rectangle bounds = ZuiGetTransformBounds(id);

        TraceLog(LOG_INFO, "┌─────── LABEL DATA ───────");
        TraceLog(LOG_INFO, "│ Item ID:      %u", label->itemId);
        TraceLog(LOG_INFO, "│ Text:         \"%s\"", label->text);
        TraceLog(LOG_INFO, "│ Bounds:       (%.1f, %.1f, %.1fx%.1f)",
                 (double)bounds.x, (double)bounds.y,
                 (double)bounds.width, (double)bounds.height);
        TraceLog(LOG_INFO, "│ Font Size:    %.1f", (double)label->fontSize);
        TraceLog(LOG_INFO, "│ Components:   %u", item->componentCount);
        TraceLog(LOG_INFO, "└──────────────────────────");
    }
    // -----------------------------------------------------------------------------
    // zui_texture.c

    const ZuiTextureData *ZuiGetTextureData(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_BUILTIN_TEXTURE)
        {
            return NULL;
        }
        return (const ZuiTextureData *)ZuiGetTypedDataConst(itemId);
    }

    ZuiTextureData *ZuiGetTextureDataMut(const unsigned int itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || item->type != ZUI_BUILTIN_TEXTURE)
        {
            return NULL;
        }
        return (ZuiTextureData *)ZuiGetTypedData(itemId);
    }

    unsigned int ZuiCreateTexture(const Texture2D texture)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        unsigned int itemId = ZuiCreateTypedItem(ZUI_BUILTIN_TEXTURE);
        if (itemId == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        ZuiTextureData *texData = ZuiGetTextureDataMut(itemId);
        if (!texData)
        {
            return ZUI_ID_INVALID;
        }

        *texData = (ZuiTextureData){
            .itemId = itemId,
            .texture = texture,
            .color = WHITE,
            .isVisible = true,
        };

        // Add transform component
        ZuiTransform *transform = (ZuiTransform *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!transform)
        {
            return ZUI_ID_INVALID;
        }

        *transform = (ZuiTransform){
            .itemId = itemId,
            .bounds = (Rectangle){0, 0, (float)texture.width, (float)texture.height},
            .offset = (Vector2){0, 0},
            .isDirty = false,
        };

        return itemId;
    }

    void ZuiRenderTexture(unsigned int dataIndex)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(ZUI_BUILTIN_TEXTURE);
        if (!reg)
        {
            return;
        }

        const ZuiTextureData *texData = (const ZuiTextureData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!texData || !texData->isVisible)
        {
            return;
        }

        Rectangle bounds = ZuiGetTransformBounds(texData->itemId);
        DrawTextureNPatch(texData->texture, texData->npatch, bounds, (Vector2){0}, 0.0F, texData->color);
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

        Rectangle bounds = ZuiGetTransformBounds(id);

        TraceLog(LOG_INFO, "┌─────── TEXTURE DATA ─────");
        TraceLog(LOG_INFO, "│ Item ID:      %u", texture->itemId);
        TraceLog(LOG_INFO, "│ Texture ID:   %u", texture->texture.id);
        TraceLog(LOG_INFO, "│ Texture Size: %dx%d",
                 texture->texture.width, texture->texture.height);
        TraceLog(LOG_INFO, "│ Bounds:       (%.1f, %.1f, %.1fx%.1f)",
                 (double)bounds.x, (double)bounds.y,
                 (double)bounds.width, (double)bounds.height);
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
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ID_INVALID;
        }

        unsigned int id = ZuiCreateTexture(tex);
        ZuiTextureData *texData = ZuiGetTextureDataMut(id);
        if (!texData)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to get texture data");
            return ZUI_ID_INVALID;
        }

        ZuiSetTransformBounds(id, bounds);
        texData->npatch = npatch;
        texData->isPatch = isPatch;

        unsigned int active = g_zui_ctx->cursor.activeFrame;
        ZuiItemAddChild(active, id);

        ZuiAdvanceCursor(bounds.width, bounds.height);
        g_zui_ctx->cursor.activeTexture = id;
        g_zui_ctx->cursor.lastItemBounds = bounds;
        return id;
    }

    unsigned int ZuiNewTexture(const Texture2D tex)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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
    // zui_context.c
    // #include "zui_button.h"

    ZuiContext *g_zui_ctx = NULL;
    ZuiArena g_zui_arena = {0};

    // ----------------------------------------------------------------------------Context

    void ZuiUpdateRestPosition(void)
    {
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }

    ZuiFrameData *ZuiGetActiveFrameDataMut(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return NULL;
        }
        return ZuiGetFrameDataMut(g_zui_ctx->cursor.activeFrame);
    }

    ZuiLayoutData *ZuiGetActiveLayoutMut(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return NULL;
        }
        return ZuiGetLayoutDataMut(g_zui_ctx->cursor.activeFrame);
    }

    bool ZuiIsInitialized(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
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

    bool ZuiEnsureContext(ZuiResult error_code, const char *msg)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(error_code, "%s", msg);
            return false;
        }
        return true;
    }

    static void ZuiRegisterBuiltinTypes(void)
    {
        // Frame type
        {
            ZuiTypeInfo frameType = {
                .name = "Frame",
                .dataSize = sizeof(ZuiFrameData),
                .dataAlignment = ZUI_ALIGNOF(ZuiFrameData),
                .initialCapacity = ZUI_FRAMES_CAPACITY,
                .update = ZuiUpdateFrame,
                .render = ZuiRenderFrame,
                .init = NULL,
                .cleanup = NULL,
            };

            unsigned int id = ZuiRegisterType(&frameType);
            ZUI_ASSERT(id == ZUI_BUILTIN_FRAME, "Frame type ID mismatch");

            ZuiTypeRegistration *reg = ZuiGetTypeRegistration(id);
            if (reg)
            {
                reg->isBuiltin = true;
            }
        }

        // Label type
        {
            ZuiTypeInfo labelType = {
                .name = "Label",
                .dataSize = sizeof(ZuiLabelData),
                .dataAlignment = ZUI_ALIGNOF(ZuiLabelData),
                .initialCapacity = ZUI_LABELS_CAPACITY,
                .update = NULL,
                .render = ZuiRenderLabel,
                .init = NULL,
                .cleanup = NULL,
            };

            unsigned int id = ZuiRegisterType(&labelType);
            ZUI_ASSERT(id == ZUI_BUILTIN_LABEL, "Label type ID mismatch");

            ZuiTypeRegistration *reg = ZuiGetTypeRegistration(id);
            if (reg)
            {
                reg->isBuiltin = true;
            }
        }

        // Texture type
        {
            ZuiTypeInfo textureType = {
                .name = "Texture",
                .dataSize = sizeof(ZuiTextureData),
                .dataAlignment = ZUI_ALIGNOF(ZuiTextureData),
                .initialCapacity = ZUI_TEXTURES_CAPACITY,
                .update = NULL,
                .render = ZuiRenderTexture,
                .init = NULL,
                .cleanup = NULL,
            };

            unsigned int id = ZuiRegisterType(&textureType);
            ZUI_ASSERT(id == ZUI_BUILTIN_TEXTURE, "Texture type ID mismatch");

            ZuiTypeRegistration *reg = ZuiGetTypeRegistration(id);
            if (reg)
            {
                reg->isBuiltin = true;
            }
        }

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: Registered %d built-in types", ZUI_BUILTIN_TYPE_COUNT);
#endif
        // ZuiRegisterButtonType();
    }

    bool ZuiInit(void)
    {
        if (g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ALREADY_INITIALIZED, "ZUI context already exists");
            return false;
        }

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

        // Initialize component registry FIRST
        result = ZuiInitComponentRegistry();
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Component registry init failed");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        // Register built-in components (before types, as types may use components)
        // ZuiRegisterBuiltinComponents();
        ZuiRegisterAllComponents();
        // Initialize type registry
        result = ZuiInitTypeRegistry();
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Type registry init failed");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        // Register built-in types
        ZuiRegisterBuiltinTypes();

        // Items
        result = ZuiInitDynArray(&ctx->items, &g_zui_arena, ZUI_ITEMS_CAPACITY, sizeof(ZuiItem), ZUI_ALIGNOF(ZuiItem), "Items");
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize items array");
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

        // CRITICAL: Create root frame with proper component initialization
        unsigned int root_item = ZuiCreateFrame((Rectangle){0, 0, (float)screen_width, (float)screen_height}, BLANK);
        if (root_item == ZUI_ID_INVALID)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to create root frame");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        // Verify root frame has transform component
        if (!ZuiItemHasComponent(root_item, ZUI_COMPONENT_TRANSFORM))
        {
            TraceLog(LOG_ERROR, "ZUI: Root frame missing transform component");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        // Add layout component to root (it doesn't have one by default from ZuiCreateFrame)
        ZuiItemSetLayout(root_item, ZUI_LAYOUT_NONE);

        float pad = ZuiGetFramePadding(root_item);
        g_zui_ctx->cursor.position = (Vector2){pad, pad};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.activeFrame = root_item;
        g_zui_ctx->cursor.rootItem = root_item;

        Vector2 dpiScale = GetWindowScaleDPI();
        g_zui_ctx->dpiScale = (int)dpiScale.x;
        if (g_zui_ctx->dpiScale <= 0)
        {
            g_zui_ctx->dpiScale = ZUI_DEFAULT_DPI_SCALE;
        }

        g_zui_ctx->font = LoadFontEx("src/resources/Inter_18pt-Regular.ttf", ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);
        if (g_zui_ctx->font.texture.id == 0)
        {
            g_zui_ctx->font = GetFontDefault();
        }

        g_zui_ctx->monoFont = LoadFontEx("src/resources/Inconsolata-Regular.ttf", ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);
        if (g_zui_ctx->monoFont.texture.id == 0)
        {
            g_zui_ctx->monoFont = GetFontDefault();
        }

        TraceLog(LOG_INFO, "ZUI: Initialized successfully");
        return true;
    }

    void ZuiUpdate(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        const ZuiItem *root =
            ZuiGetItem(g_zui_ctx->cursor.rootItem);

        if (root)
        {
            ZuiDispatchUpdate(root);
        }
        float dt = GetFrameTime();
        ZuiUpdateInteractions(dt);
        ZuiUpdateAnimations(dt);
        // ZuiUpdateFocus(dt);
        // ZuiUpdateScroll(dt);
    }

    void ZuiRender(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        const ZuiItem *root =
            ZuiGetItem(g_zui_ctx->cursor.rootItem);

        if (root)
        {
            ZuiDispatchRender(root);
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
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        if (width < 0.0F || height < 0.0F)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Negative dimensions");
            return;
        }

        const float gap = ZuiGetFrameSpacing(g_zui_ctx->cursor.activeFrame);

        if (g_zui_ctx->cursor.isRow)
        {
            g_zui_ctx->cursor.position.x += width + gap;
        }
        else
        {
            ZuiUpdateRestPosition();
            g_zui_ctx->cursor.position.y += height + gap;
        }
    }

    void ZuiPrintCursor(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        TraceLog(LOG_INFO, "[ZUI] Cursor position: x=%.1F, y=%.1F, activeFrame=%u",
                 (double)g_zui_ctx->cursor.position.x,
                 (double)g_zui_ctx->cursor.position.y,
                 g_zui_ctx->cursor.activeFrame);
    }

    void ZuiAdvanceLine(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        const float gap = ZuiGetFrameSpacing(g_zui_ctx->cursor.activeFrame);
        g_zui_ctx->cursor.position.x = g_zui_ctx->cursor.restPosition.x;
        g_zui_ctx->cursor.tempRestPosition.x = g_zui_ctx->cursor.position.x;
        g_zui_ctx->cursor.position.y += g_zui_ctx->cursor.lastItemBounds.height + gap;
        g_zui_ctx->cursor.tempRestPosition.y = g_zui_ctx->cursor.position.y;
    }

    void ZuiPlaceAt(const float x, const float y)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        g_zui_ctx->cursor.position = (Vector2){x, y};
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }

    void ZuiOffset(const float x, const float y)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return;
        }

        g_zui_ctx->cursor.position.x += x;
        g_zui_ctx->cursor.position.y += y;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }
    // -----------------------------------------------------------------------------

#endif // ZUI_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // ZUI_H
