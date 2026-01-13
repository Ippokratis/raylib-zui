#ifndef ZUI_H
#define ZUI_H

// --- EXTERNAL INCLUDES ---
#include "math.h"
#include "raylib.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32

#else

#endif

#ifdef __cplusplus
extern "C"
{
#endif

    // --- ZUI HEADERS ---
    // zui_core.h
    // The arhitecture is Retained Mode (ECS).
    // Lifecycle: Create once, modify per frame, destroy t exit. Do not create every frame.
    // Do not store pointers to UI data across frames. Use IDs.

#ifndef ZUI_DEBUG
#define ZUI_DEBUG
#endif

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
        ZUI_COMPONENT_TYPES = 12,
        ZUI_CHILDREN_CAPACITY = 2,
        ZUI_ALIGNMENT_MAX_ITEMS = 8,
        ZUI_MAX_TEXT_LENGTH = 64,
        ZUI_MAX_WINDOWS = 8,
        ZUI_BASE_FONT_SIZE = 18,
        ZUI_TITLEBAR_HEIGHT = 28,
        ZUI_WINDOWS_CAPACITY = 2,
        ZUI_MAX_INPUT_CHARS = 32,
        ZUI_ITEMS_CAPACITY = 2,
        ZUI_TRANSFORMS_CAPACITY = 2,
        ZUI_FRAMES_CAPACITY = 2,
        ZUI_FRAME_ITEMS_CAPACITY = 2,
        ZUI_LABELS_CAPACITY = 8,
        ZUI_TEXTURES_CAPACITY = 8,
        ZUI_BUTTONS_CAPACITY = 8,
        ZUI_SLIDERS_CAPACITY = 8,
        ZUI_KNOBS_CAPACITY = 8,
        ZUI_NUMERIC_INPUTS_CAPACITY = 8,
        ZUI_MAX_FOCUSABLE_WIDGETS = 8,
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
    static const float ZUI_CORNER_RADIUS = 8.0F;
    static const float MIN_BUTTON_SIZE = 24.0F;
    static const uint32_t ZUI_ID_INVALID = UINT_MAX;

    typedef enum ZuiItemType
    {
        ZUI_FRAME = 0,
        ZUI_LABEL,
        ZUI_TEXTURE,
        ZUI_BUTTON,
        ZUI_WINDOW,
        ZUI_MENUBAR,
        ZUI_MENU,
        ZUI_MENUITEM,
        ZUI_SLIDER,
        ZUI_KNOB,
        ZUI_NUMERIC_INPUT,
        ZUI_TYPE_COUNT,
    } ZuiItemType;

    ///--------------------------------------------------------Utility Functions

    bool ZuiIsPowerOfTwo(size_t n);
    const char *ZuiGetWidgetType(ZuiItemType type);
    const char *ZuiResultToString(ZuiResult result);
    float ZuiPixelsToRoundness(Rectangle rect, float radius_px);
    size_t ZuiStrlen(const char *s, size_t max_len);
    float ZuiClamp(float value, float min, float max);
    float ZuiLerp(float start, float end, float amount);
    // -----------------------------------------------------------------------------
    // zui_arena.h

    typedef struct ZuiArena
    {
        unsigned char *buffer;
        size_t offset;
        size_t capacity;
        size_t initialCapacity;
    } ZuiArena;

    typedef struct ZuiArenaStats
    {
        size_t totalCapacity;
        size_t usedBytes;
        size_t availableBytes;
        float usagePercent;
        size_t initialCapacity;
        size_t maxCapacity;
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
    ZuiResult ZuiGrowArena(ZuiArena *arena, size_t requiredSize);

    // -----------------------------------------------------------------------------
    // zui_dynarray.h

    typedef struct ZuiDynArray
    {
        void *items;
        uint32_t count;
        uint32_t capacity;
        size_t itemSize;
        size_t itemAlignment;
        const char *typeName;
    } ZuiDynArray;

    typedef struct ZuiDynArrayStats
    {
        uint32_t count;
        uint32_t capacity;
        float usagePercent;
        size_t memoryUsed;
    } ZuiDynArrayStats;

#define ZUI_DYNARRAY_PUSH_TYPE(array, arena, type) \
    ((type *)ZuiPushDynArray(array, arena))

#define ZUI_DYNARRAY_GET_TYPE(array, type, index) \
    ((type *)ZuiGetDynArray(array, index))

    ///-------------------------------------Dynamic Array

    ZuiResult ZuiInitDynArray(ZuiDynArray *array, ZuiArena *arena, uint32_t initialCapacity,
                              size_t itemSize, size_t itemAlignment, const char *typeName);
    ZuiResult ZuiGrowDynArray(ZuiDynArray *array, ZuiArena *arena);
    void *ZuiPushDynArray(ZuiDynArray *array, ZuiArena *arena);
    void *ZuiGetDynArray(const ZuiDynArray *array, uint32_t index);
    void ZuiClearDynArray(ZuiDynArray *array);
    bool ZuiIsDynArrayValid(const ZuiDynArray *array);
    ZuiDynArrayStats ZuiGetDynArrayStats(const ZuiDynArray *array);

    // -----------------------------------------------------------------------------
    // zui_component.h

    typedef struct ZuiColorSet
    {
        Color active;
        Color normal;
        Color hovered;
        Color pressed;
        Color selected;
        Color selectedHovered;
        Color selectedPressed;
        Color focused;
        Color disabled;
    } ZuiColorSet;

    typedef struct ZuiTextureSet
    {
        uint32_t active;
        uint32_t normal;
        uint32_t hovered;
        uint32_t pressed;
        uint32_t selected;
        uint32_t selectedHovered;
        uint32_t selectedPressed;
        uint32_t focused;
        uint32_t disabled;
        NPatchInfo npatch;
    } ZuiTextureSet;

    typedef enum ZuiComponentId
    {
        ZUI_COMPONENT_TRANSFORM = 0,
        ZUI_COMPONENT_LAYOUT,
        ZUI_COMPONENT_ALIGN,
        ZUI_COMPONENT_INTERACTION,
        ZUI_COMPONENT_STATE,
        ZUI_COMPONENT_ANIMATION,
        ZUI_COMPONENT_FOCUS,
        ZUI_COMPONENT_SCROLL,
        ZUI_COMPONENT_LAYER,
        ZUI_COMPONENT_SPATIAL,
        ZUI_COMPONENT_COUNT,
    } ZuiComponentId;

    typedef void (*ZuiComponentInitFunc)(void *data, uint32_t itemId, ZuiComponentId componentId);

    typedef struct ZuiComponentRegistration
    {
        const char *name;
        size_t dataSize;
        size_t dataAlignment;
        uint32_t initialCapacity;
        ZuiDynArray dataArray;
        uint32_t id;
        ZuiComponentInitFunc init;
        const uint32_t *requiredComponents;
        uint32_t requiredComponentCount;
    } ZuiComponentRegistration;

    typedef struct ZuiComponentRegistry
    {
        ZuiComponentRegistration registrations[ZUI_COMPONENT_TYPES];
        uint32_t registeredCount;
    } ZuiComponentRegistry;

    typedef struct ZuiItemComponent
    {
        uint32_t componentId;
        uint32_t dataIndex;
    } ZuiItemComponent;

    static const uint32_t COMPONENT_DEPS_TRANSFORM[] = {ZUI_COMPONENT_TRANSFORM};

    //--------------------------- TRANSFORM COMPONENT

    typedef struct ZuiTransformData
    {
        uint32_t itemId;
        Rectangle bounds;
        Vector2 offset;
    } ZuiTransformData;

    //--------------------------- LAYOUT COMPONENT

    typedef enum ZuiLayoutKind
    {
        ZUI_LAYOUT_NONE = 0,
        ZUI_LAYOUT_VERTICAL,
        ZUI_LAYOUT_HORIZONTAL,
    } ZuiLayoutKind;

    typedef enum ZuiSizeMode
    {
        ZUI_SIZE_AUTO,  // Hug
        ZUI_SIZE_FIXED, // Explicit size
        ZUI_SIZE_FILL,  // Fill available space
    } ZuiSizeMode;

    typedef enum ZuiAlign
    {
        ZUI_ALIGN_START,  // Left/Top
        ZUI_ALIGN_CENTER, // Center
        ZUI_ALIGN_END,    // Right/Bottom
    } ZuiAlign;

    typedef struct ZuiLayoutData
    {
        uint32_t itemId;
        ZuiLayoutKind kind;
        float spacing;       // Gap between children
        float padding;       // Inner padding
        ZuiAlign childAlign; // How children align in cross-axis
        float measuredWidth;
        float measuredHeight;
        bool isFinalized;
    } ZuiLayoutData;

    typedef struct ZuiAlignData
    {
        uint32_t itemId;
        ZuiAlign align; // Alignment within parent
        ZuiSizeMode widthMode;
        ZuiSizeMode heightMode;
        float fixedWidth;
        float fixedHeight;
        float margin; // Uniform margin on all sides
    } ZuiAlignData;

    //--------------------------- ANIMATION COMPONENT

    typedef enum ZuiAnimationType
    {
        ZUI_ANIM_LINEAR,
        ZUI_ANIM_EASE_OUT,
        ZUI_ANIM_TYPE_COUNT
    } ZuiAnimationType;

    typedef enum ZuiAnimationSlot
    {
        ZUI_ANIM_SLOT_HOVER = 0,
        ZUI_ANIM_SLOT_PRESS,
        ZUI_ANIM_SLOT_FOCUS,
        ZUI_ANIM_SLOT_ALPHA,
        ZUI_ANIM_SLOT_OFFSET_X,
        ZUI_ANIM_SLOT_OFFSET_Y,
        ZUI_ANIM_SLOT_SCALE,
        ZUI_ANIM_SLOT_CUSTOM,
        ZUI_ANIM_SLOT_COUNT,
    } ZuiAnimationSlot;

    typedef struct ZuiAnimation
    {
        float value;
        float target;
        float duration;
        ZuiAnimationType type;
        bool active;
    } ZuiAnimation;

    typedef struct ZuiAnimationData
    {
        uint32_t itemId;
        ZuiAnimation slots[ZUI_ANIM_SLOT_COUNT];
        uint8_t activeCount;
    } ZuiAnimationData;

    typedef void (*ZuiAnimUpdateFunc)(ZuiAnimation *anim, float deltaTime);

    //---------------------------  INTERACTION COMPONENT

    typedef enum ZuiMouseButton
    {
        ZUI_MOUSE_LEFT = 0,
        ZUI_MOUSE_RIGHT = 1,
        ZUI_MOUSE_MIDDLE = 2,
    } ZuiMouseButton;

    typedef struct ZuiInteractionData
    {
        uint32_t itemId;
        bool isHovered;
        bool wasHovered;
        bool isPressed;
        bool wasPressed;
        bool isFocused;
        bool wasFocused;
        Vector2 mousePosition;
        Vector2 mouseDelta;
        Vector2 dragOffset;
        ZuiMouseButton pressedButton;
        float hoverTime;
        float pressTime;
        bool capturesMouse;
        bool capturesKeyboard;
        bool acceptsFocus;
        bool blocksInput;
        bool isDragging;

        void (*onHoverEnter)(uint32_t itemId);
        void (*onHoverExit)(uint32_t itemId);
        void (*onClick)(uint32_t itemId, ZuiMouseButton button);
        void (*onPress)(uint32_t itemId, ZuiMouseButton button);
        void (*onRelease)(uint32_t itemId, ZuiMouseButton button);
        void (*onFocus)(uint32_t itemId);
    } ZuiInteractionData;

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
        uint32_t itemId;
        int layer;
        int order;
        bool blocksInput;
    } ZuiLayerData;

    //---------------------------  STATE COMPONENT

    typedef struct ZuiStateData
    {
        uint32_t itemId;
        bool isEnabled;
        bool isVisible;
    } ZuiStateData;

    //---------------------------  FOCUS COMPONENT

    // while (!WindowShouldClose())
    // {
    // if (IsKeyPressed(KEY_TAB))
    // {
    //     int direction = IsKeyDown(KEY_LEFT_SHIFT) ? -1 : 1;
    //     ZuiFocusNavigate(direction);
    // }

    // if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    // {
    //     ZuiFocusActivate();
    // }

    // if (IsGamepadAvailable(0))
    // {
    //     if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
    //     {
    //         ZuiFocusNavigate(-1);
    //         g_zui_ctx->g_focusContext.gamepadMode = true;
    //     }
    //     else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
    //     {
    //         ZuiFocusNavigate(1);
    //         g_zui_ctx->g_focusContext.gamepadMode = true;
    //     }

    //     if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
    //     {
    //         ZuiFocusActivate();
    //     }
    // }
    // ZuiUpdate();
    typedef struct ZuiFocusData
    {
        uint32_t itemId;
        bool isFocused;
        bool canReceiveFocus;
        int tabIndex;
        uint32_t nextItem;
        uint32_t prevItem;
        // Removed: parentItem, autoLinkHorizontal, autoLinkVertical for simplicity
        void (*onFocusGained)(uint32_t itemId);
        void (*onFocusLost)(uint32_t itemId);
        void (*onActivate)(uint32_t itemId);
    } ZuiFocusData;

    typedef struct ZuiFocusContext
    {
        uint32_t currentFocus;
        uint32_t rootFocus;
        bool keyboardMode;
        bool gamepadMode;
    } ZuiFocusContext;

    //---------------------------  SCROLL COMPONENT

    typedef enum ZuiScrollbarDragMode
    {
        ZUI_SCROLL_DRAG_NONE = 0,
        ZUI_SCROLL_DRAG_VERTICAL,
        ZUI_SCROLL_DRAG_HORIZONTAL,
    } ZuiScrollbarDragMode;

    typedef struct ZuiScrollData
    {

        Vector2 offset;
        Vector2 targetOffset;
        Vector2 contentSize;
        Vector2 dragStart;
        Rectangle viewport;
        float scrollbarSize;
        uint32_t itemId;
        ZuiScrollbarDragMode dragMode;
        bool verticalEnabled;
        bool horizontalEnabled;
        bool isDragging;
    } ZuiScrollData;

    //---------------------------  SPATIAL COMPONENT

    typedef enum ZuiSpatialMode
    {
        ZUI_SPATIAL_NONE = 0,
        ZUI_SPATIAL_TILT_Y,
        ZUI_SPATIAL_BEND_Y,
    } ZuiSpatialMode;

    typedef struct ZuiSpatialData
    {
        uint32_t itemId;
        ZuiSpatialMode mode;
        float tiltY;
        float depth;
        float bendAmount;
        float bendRadius;
        float parallax;
        bool affectsInput;
    } ZuiSpatialData;

    typedef enum ZuiComponentFlags
    {
        ZUI_COMP_NONE = 0,
        ZUI_COMP_TRANSFORM = 1U << (uint32_t)0,
        ZUI_COMP_STATE = 1U << (uint32_t)1,
        ZUI_COMP_LAYER = 1U << (uint32_t)2,
        ZUI_COMP_ANIMATION = 1U << (uint32_t)3,
        ZUI_COMP_INTERACTION = 1U << (uint32_t)4,
        ZUI_COMP_FOCUS = 1U << (uint32_t)5,
        ZUI_COMP_LAYOUT = 1U << (uint32_t)6,
        // Common presets

        ZUI_COMP_STANDARD = (uint32_t)ZUI_COMP_TRANSFORM | (uint32_t)ZUI_COMP_STATE |
                            (uint32_t)ZUI_COMP_LAYER | (uint32_t)ZUI_COMP_ANIMATION,
        ZUI_COMP_INTERACTIVE = (uint32_t)ZUI_COMP_STANDARD | (uint32_t)ZUI_COMP_INTERACTION | (uint32_t)ZUI_COMP_FOCUS,
    } ZuiComponentFlags;

    //-------------------types

    typedef void (*ZuiUpdateFunction)(uint32_t dataIndex);
    typedef void (*ZuiRenderFunction)(uint32_t dataIndex);
    typedef void (*ZuiTypeInitFunc)(void *data, uint32_t itemId);

    typedef struct ZuiTypeRegistration
    {
        const char *name;
        size_t dataSize;
        size_t dataAlignment;
        uint32_t initialCapacity;
        ZuiDynArray dataArray;
        uint32_t id;
        ZuiTypeInitFunc init;
        ZuiUpdateFunction update;
        ZuiRenderFunction render;
    } ZuiTypeRegistration;

    typedef struct ZuiTypeRegistry
    {
        ZuiTypeRegistration registrations[16];
        uint32_t registeredCount;
    } ZuiTypeRegistry;

    //---------------------------  COMPONENT SYSTEM API

    ZuiResult ZuiInitComponentRegistry(void);
    ZuiResult ZuiRegisterComponent(ZuiComponentId componentId, const ZuiComponentRegistration *info);
    void ZuiRegisterAllComponents(void);
    void *ZuiEnsureComponent(uint32_t itemId, ZuiComponentId componentId);
    Vector2 ZuiGetAbsolutePosition(uint32_t itemId);
    void *ZuiItemAddComponent(uint32_t itemId, uint32_t componentId);
    void *ZuiItemGetComponent(uint32_t itemId, uint32_t componentId);
    bool ZuiItemHasComponent(uint32_t itemId, uint32_t componentId);
    bool ZuiValidateComponentDependencies(uint32_t itemId, uint32_t componentId);
    ZuiComponentRegistration *ZuiGetComponentRegistration(uint32_t componentId);
    const ZuiComponentRegistration *ZuiGetComponentInfo(uint32_t componentId);

    //---------------------------  TRANSFORM API

    uint32_t ZuiCreateTransform(uint32_t itemId, Rectangle bounds);
    void ZuiSetTransformBounds(uint32_t itemId, Rectangle bounds);
    void ZuiSetTransformOffset(uint32_t itemId, Vector2 offset);
    void ZuiSetTransformBoundsPosition(uint32_t itemId, Vector2 offset);
    Rectangle ZuiGetTransformBounds(uint32_t itemId);

    //---------------------------  LAYOUT API

    ZuiAlignData *ZuiGetOrCreateAlignData(uint32_t itemId);
    ZuiResult ZuiItemSetLayout(uint32_t itemId, ZuiLayoutKind kind);
    void ZuiLayoutSetSpacing(uint32_t itemId, float spacing);
    void ZuiLayoutSetPadding(uint32_t itemId, float padding);
    void ZuiLayoutSetChildAlign(uint32_t itemId, ZuiAlign align);
    void ZuiItemSetAlign(uint32_t itemId, ZuiAlign align);
    void ZuiItemAlignLast(ZuiAlign align);
    void ZuiItemSetSizeMode(uint32_t itemId, ZuiSizeMode widthMode, ZuiSizeMode heightMode);
    void ZuiItemSetFixedSize(uint32_t itemId, float width, float height);
    void ZuiItemSetMargin(uint32_t itemId, float margin);
    uint32_t ZuiBeginVertical(Vector2 pos, float spacing, Color color);
    uint32_t ZuiBeginHorizontal(Vector2 pos, float spacing, Color color);
    void ZuiEndLayout(void);
    uint32_t ZuiBeginRow(float spacing);
    uint32_t ZuiBeginColumn(float spacing);
    uint32_t ZuiBeginCentered(float spacing, Color color);
    void ZuiMeasureRecursive(uint32_t itemId, int depth);
    void ZuiArrangeRecursive(uint32_t itemId, Rectangle bounds, int depth);
    void ZuiSpace(float x, float y);
    void ZuiEndRow(void);
    void ZuiEndColumn(void);
    void ZuiEndVertical(void);
    void ZuiEndHorizontal(void);
    void ZuiEndFrameAuto(void);

    //---------------------------  ANIMATION API

    ZuiAnimation *ZuiGetAnimSlot(uint32_t itemId, ZuiAnimationSlot slot);
    void ZuiAnimSetTarget(uint32_t itemId, ZuiAnimationSlot slot, float target);
    void ZuiAnimSetDuration(uint32_t itemId, ZuiAnimationSlot slot, float duration);
    void ZuiAnimSetType(uint32_t itemId, ZuiAnimationSlot slot, ZuiAnimationType type);
    void ZuiAnimReset(uint32_t itemId, ZuiAnimationSlot slot, float value);
    float ZuiAnimGetValue(uint32_t itemId, ZuiAnimationSlot slot);
    void ZuiAnimSetupInteractive(uint32_t itemId, float duration);

    //---------------------------  INTERACTION API

    void ZuiSetHoverCallback(uint32_t itemId, void (*onHover)(uint32_t));
    void ZuiSetClickCallback(uint32_t itemId, void (*onClick)(uint32_t, ZuiMouseButton));
    bool ZuiIsItemHovered(uint32_t itemId);
    bool ZuiIsItemPressed(uint32_t itemId);
    bool ZuiIsItemFocused(uint32_t itemId);

    //---------------------------  STATE API

    void ZuiSetEnabled(uint32_t itemId, bool enabled);
    void ZuiSetVisible(uint32_t itemId, bool visible);
    bool ZuiIsEnabled(uint32_t itemId);
    bool ZuiIsVisible(uint32_t itemId);

    //---------------------------  LAYER API

    void ZuiSetLayer(uint32_t itemId, int layer, int order);
    void ZuiRaiseToFront(uint32_t itemId);
    int ZuiGetLayerValue(uint32_t itemId);
    int ZuiGetLayerOrder(uint32_t itemId);
    bool ZuiLayerBlocksInput(uint32_t itemId);
    void ZuiBringToFrontInTree(uint32_t itemId);
    void ZuiPropagateLayerToChildren(uint32_t parentId);
    //---------------------------  FOCUS API

    void ZuiEnableFocus(uint32_t itemId);
    void ZuiFocusItem(uint32_t itemId);
    void ZuiUnfocusItem(uint32_t itemId);
    uint32_t ZuiGetFocusedItem(void);
    void ZuiFocusSetTabIndex(uint32_t itemId, int index);
    void ZuiFocusLinkNext(uint32_t itemId, uint32_t toItem);
    void ZuiFocusLinkPrev(uint32_t itemId, uint32_t toItem);
    void ZuiFocusSetCallback(uint32_t itemId,
                             void (*onGain)(uint32_t),
                             void (*onLose)(uint32_t),
                             void (*onActivate)(uint32_t));

    void ZuiCreateFocusChain(const uint32_t *ids, uint32_t count);
    void ZuiBreakFocusChain(const uint32_t *ids, uint32_t count);
    bool ZuiFocusNavigate(int direction);
    void ZuiFocusActivate(void);

    //---------------------------  SCROLL API

    void ZuiApplyScrollOffsetRecursive(uint32_t itemId, Vector2 offset);
    void ZuiEnableScroll(uint32_t frameId, bool vertical, bool horizontal);
    Rectangle ZuiGetChildContentBounds(uint32_t childId);
    Vector2 ZuiCalculateContentSize(uint32_t itemId);
    void ZuiScrollTo(uint32_t frameId, Vector2 offset);
    void ZuiScrollBy(uint32_t frameId, Vector2 delta);
    Rectangle ZuiScrollGetBarRect(const ZuiScrollData *scroll, bool isVertical);
    Vector2 ZuiScrollGetMaxOffset(const ZuiScrollData *scroll);
    Vector2 ZuiScrollClampOffset(const ZuiScrollData *scroll, Vector2 offset);

    //---------------------------  SPATIAL API

    void ZuiSetSpatialMode(uint32_t itemId, ZuiSpatialMode mode);
    void ZuiSetSpatialTilt(uint32_t itemId, float tiltY, float depth);
    void ZuiSetSpatialBend(uint32_t itemId, float amount, float radius);
    void ZuiSetSpatialParallax(uint32_t itemId, float parallax);
    void ZuiSetSpatialAffectsInput(uint32_t itemId, bool affects);
    Vector2 ZuiSpatialTransformPoint(uint32_t itemId, Vector2 point);
    Vector2 ZuiSpatialInverseTransformPoint(uint32_t itemId, Vector2 point);

    //---------------------------  HELPERS

    void ZuiUpdateFocus(void);
    bool ZuiAddComponentsEx(uint32_t itemId, Rectangle bounds, uint32_t flags);
    bool ZuiSetupAsContainer(uint32_t itemId);

    //---------------------------  TYPES

    ZuiResult ZuiInitRegistryEntry(ZuiDynArray *dataArray, ZuiTypeRegistration *entry, uint32_t id,
                                   const char *name, size_t dataSize,
                                   size_t dataAlignment, uint32_t capacity);
    void *ZuiGetRegistryData(ZuiDynArray *dataArray, const ZuiTypeRegistration *entry, uint32_t dataIndex);
    const void *ZuiGetRegistryDataConst(ZuiDynArray *dataArray, const ZuiTypeRegistration *entry, uint32_t dataIndex);
    void *ZuiAllocRegistryData(ZuiDynArray *dataArray, ZuiTypeRegistration *entry);
    void ZuiRegisterAllTypes(void);
    ZuiResult ZuiRegisterType(ZuiItemType typeId, const ZuiTypeRegistration *info);
    const ZuiTypeRegistration *ZuiGetTypeInfo(uint32_t typeId);
    bool ZuiIsValidTypeId(uint32_t typeId);
    ZuiTypeRegistration *ZuiGetTypeRegistration(uint32_t typeId);
    const ZuiTypeRegistration *ZuiGetTypeRegistrationConst(uint32_t typeId);
    uint32_t ZuiCreateTypedItem(uint32_t typeId);
    void *ZuiGetTypedData(uint32_t itemId);
    const void *ZuiGetTypedDataConst(uint32_t itemId);
    // -----------------------------------------------------------------------------
    // zui_item.h

    typedef struct ZuiItem
    {
        ZuiDynArray children; // costs 32kb per 1k items, 800 leaf, 200 container
        uint32_t type;
        uint32_t id;
        uint32_t parentId;
        uint32_t dataIndex;
        uint32_t componentMask;
        ZuiItemComponent components[ZUI_COMPONENT_TYPES];
        uint32_t componentCount;
        bool isContainer;
        bool canMove;
    } ZuiItem;

    const ZuiItem *ZuiGetItem(uint32_t id);
    ZuiItem *ZuiGetItemMut(uint32_t id);
    uint32_t ZuiCreateItem(ZuiItemType type, uint32_t dataIndex);
    ZuiResult ZuiItemAddChild(uint32_t parentId, uint32_t childId);
    uint32_t ZuiAddChild(uint32_t id);
    void ZuiUpdateItem(const ZuiItem *item);
    void ZuiRenderItem(const ZuiItem *item);
    // -----------------------------------------------------------------------------
    // zui_frame.h

    typedef struct ZuiFrameStyle
    {
        Color bgColor;
        Color borderColor;
        Color scrollbarColor;
        float cornerRadius;
        int roudnesSegments;
        float borderThickness;
        bool hasBorder;
    } ZuiFrameStyle;

    typedef struct ZuiFrameData
    {
        ZuiFrameStyle style;
        uint32_t itemId;
        bool enableScissor;
    } ZuiFrameData;

    uint32_t ZuiCreateFrame(Rectangle bounds, ZuiFrameStyle style);
    uint32_t ZuiBeginFrame(Rectangle bounds, Color color);
    void ZuiEndFrame(void);
    uint32_t ZuiNewFrame(Color color, float width, float height);
    void ZuiFrameBackground(Color color);
    void ZuiFrameOutline(Color color, float thickness);
    void ZuiFrameGap(float gap);
    void ZuiFramePad(float pad);
    void ZuiFrameOffset(float x, float y);
    void ZuiFrameCornerRadius(float radius);
    void ZuiFrameEnableScissor(uint32_t itemId, bool enable);
    void ZuiFrameMakeScrollable(bool vertical, bool horizontal);
    void ZuiUpdateFrame(uint32_t dataIndex);
    void ZuiRenderFrame(uint32_t dataIndex);
    float ZuiGetFramePadding(uint32_t itemId);
    float ZuiGetFrameSpacing(uint32_t itemId);

    // -----------------------------------------------------------------------------
    // zui_label.h

    typedef struct ZuiLabelStyle
    {
        Font font;
        Color textColor;
        Color textBgColor;
        float fontSize;
        float fontSpacing;
    } ZuiLabelStyle;

    typedef struct ZuiLabelData
    {
        ZuiLabelStyle style;
        char text[ZUI_MAX_TEXT_LENGTH + 1];
        uint32_t itemId;
    } ZuiLabelData;

    uint32_t ZuiCreateLabel(const char *text, ZuiLabelStyle style);
    uint32_t ZuiAddLabelToFrame(uint32_t id);
    uint32_t ZuiNewLabel(const char *text);
    uint32_t ZuiNewMonoLabel(const char *text);
    void ZuiLabelTextColor(Color textColor);
    void ZuiLabelBackgroundColor(Color backgroundColor);
    void ZuiLabelOffset(float x, float y);
    void ZuiRenderLabel(uint32_t dataIndex);

    // -----------------------------------------------------------------------------
    // zui_texture.h

    typedef struct ZuiTextureStyle
    {
        NPatchInfo npatch;
        Color tintColor;
    } ZuiTextureStyle;

    typedef struct ZuiTextureData
    {
        ZuiTextureStyle style;
        Texture2D texture;
        uint32_t itemId;
    } ZuiTextureData;

    uint32_t ZuiCreateTexture(Texture2D texture, ZuiTextureStyle style);
    void ZuiRenderTexture(uint32_t dataIndex);
    uint32_t ZuiNewTextureEx(Texture2D tex, NPatchInfo npatch, Rectangle bounds);
    uint32_t ZuiNewTexture(Texture2D tex);
    uint32_t ZuiNew3XSlice(Texture2D tex, int width, int left, int right);
    uint32_t ZuiNew3YSlice(Texture2D tex, int height, int top, int bottom);
    uint32_t ZuiNew9Slice(Texture2D tex, int width, int height, int left, int top, int right, int bottom);
    void ZuiTextureTintColor(Color tintColor);
    // -----------------------------------------------------------------------------
    // zui_button.h

    typedef enum ZuiButtonState
    {
        ZUI_BUTTON_NORMAL,
        ZUI_BUTTON_HOVERED,
        ZUI_BUTTON_PRESSED,
        ZUI_BUTTON_DISABLED,
    } ZuiButtonState;

    typedef enum ZuiButtonMode
    {
        ZUI_BUTTON_PUSH,   // Normal click button
        ZUI_BUTTON_TOGGLE, // On/off toggle
        ZUI_BUTTON_RADIO,  // Part of mutually exclusive group
    } ZuiButtonMode;

    typedef struct ZuiButtonStyle
    {
        float gap;
        Vector2 pad;
        Vector2 size;
        Vector2 minSize;
        ZuiColorSet colors;
        ZuiTextureSet textures;
        ZuiFrameStyle frameStyle;
        ZuiLabelStyle labelStyle;
    } ZuiButtonStyle;

    typedef struct ZuiButtonData
    {
        ZuiButtonStyle style;
        uint32_t itemId;
        uint32_t labelId; // Child label (ZUI_ID_INVALID if no text)
        uint32_t iconId;  // Child texture (ZUI_ID_INVALID if no icon)
        void (*onClick)(uint32_t);
        void (*onPress)(uint32_t);
        void (*onRelease)(uint32_t);
        void (*onToggle)(uint32_t, bool);
        ZuiButtonMode mode;
        bool isToggled;
        uint32_t groupId;
    } ZuiButtonData;

    void ZuiCenterWidgetInBounds(uint32_t itemId, Rectangle bounds);
    uint32_t ZuiCreateButton(const char *text, Texture2D *icon, ZuiButtonStyle style);
    void ZuiUpdateButton(uint32_t dataIndex);
    void ZuiRenderButton(uint32_t dataIndex);
    uint32_t ZuiNewButtonEx(const char *text, Texture2D *icon);
    uint32_t ZuiNewButton(const char *text);
    uint32_t ZuiNewButtonIcon(Texture2D icon);
    void ZuiButtonOnClick(void (*onClick)(uint32_t));
    void ZuiButtonOnPress(void (*onPress)(uint32_t));
    void ZuiButtonOnRelease(void (*onRelease)(uint32_t));
    void ZuiButtonOnToggle(void (*onToggle)(uint32_t, bool));
    void ZuiButtonTextColor(Color color);
    void ZuiButtonPadding(float x, float y);
    void ZuiButtonIconGap(float gap);
    void ZuiButtonEnable(void);
    void ZuiButtonDisable(void);
    bool ZuiIsButtonPressed(uint32_t itemId);
    void ZuiButtonSetMode(ZuiButtonMode mode);
    void ZuiButtonSetGroup(uint32_t groupId);
    bool ZuiIsButtonToggled(uint32_t itemId);
    void ZuiButtonSetToggled(uint32_t itemId, bool toggled);

    // -----------------------------------------------------------------------------
    // zui_theme.h - Theme/Resource Management

    typedef struct ZuiTheme
    {
        Font monoFont;
        Font font;
        ZuiLabelStyle defaultLabelStyle;
        ZuiTextureStyle defaultTextureStyle;
        ZuiFrameStyle defaultFrameStyle;
        ZuiButtonStyle defaultButtonStyle;
        float windowTitleHeight;
        uint32_t windowFrameTexture;
        uint32_t windowTitlebarTexture;
        uint32_t windowCloseButtonTexture;
        NPatchInfo windowFrameNPatch;
        NPatchInfo windowTitlebarNPatch;
        uint32_t buttonNormalTexture;
        uint32_t buttonHoverTexture;
        uint32_t buttonPressedTexture;
        uint32_t buttonDidabledTexture;
        NPatchInfo buttonNPatch;
        Color windowFrameColor;
        Color windowTitlebarColor;
        Color windowTitleTextColor;
        Color windowBodyColor;
        bool initialized;
    } ZuiTheme;

    ZuiResult ZuiInitTheme(ZuiTheme *theme, int dpiScale);
    void ZuiUnloadTheme(ZuiTheme *theme);
    Font ZuiGetFont(bool isMono);
    // -----------------------------------------------------------------------------
    // zui_context.h

    typedef struct ZuiCursor
    {
        Vector2 position;
        Vector2 restPosition;
        Vector2 tempRestPosition;
        Rectangle lastItemBounds;
        uint32_t rootItem;
        uint32_t activeFrame;
        uint32_t parentFrame;
        uint32_t activeItem;
        int rowStartIndex;
        int rowEndIndex;
        bool isWindowRoot;
        bool isRow;
    } ZuiCursor;

    typedef struct ZuiWindowManager
    {
        ZuiDynArray windows;    // Array of ZuiWindowData*
        uint32_t focusedWindow; // Currently focused window ID
        int nextZOrder;         // For assigning layer.order values
    } ZuiWindowManager;

    typedef struct ZuiMenuContext
    {
        uint32_t activeMenuBar;
        uint32_t activeMenu;
        uint32_t hoveredItem;
        uint32_t lastMenuItem; // For chaining
        bool isInMenu;
        bool closeRequested;
        int menuDepth;
    } ZuiMenuContext;

    typedef struct ZuiContext
    {
        ZuiCursor cursor;
        ZuiWindowManager windowManager;
        ZuiDynArray items;
        ZuiTypeRegistry typeRegistry;
        ZuiComponentRegistry componentRegistry;
        int dpiScale;
        ZuiTheme defaultTheme;
        ZuiFocusContext g_focusContext;
        ZuiMenuContext g_menuContext;
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

#define ZUI_ASSERT_CTX_RETURN_FALSE()                    \
    do                                                   \
    {                                                    \
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT,    \
                              "Global context is NULL")) \
        {                                                \
            return false;                                \
        }                                                \
    } while (0)

#define ZUI_ASSERT_CTX_RETURN()                          \
    do                                                   \
    {                                                    \
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT,    \
                              "Global context is NULL")) \
        {                                                \
            return;                                      \
        }                                                \
    } while (0)

#define ZUI_ASSERT_CTX_RETURN_ID_INVALID()               \
    do                                                   \
    {                                                    \
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT,    \
                              "Global context is NULL")) \
        {                                                \
            return ZUI_ID_INVALID;                       \
        }                                                \
    } while (0)

    void ZuiAdvanceCursor(float width, float height);
    void ZuiAdvanceLine(void);
    void ZuiPlaceAt(float x, float y);
    void ZuiOffset(float x, float y);
    void ZuiUpdateRestPosition(void);
    bool ZuiEnsureContext(ZuiResult error_code, const char *msg);
    void ZuiUpdateComponents(void);
    void ZuiUpdate(void);
    void ZuiRender(void);
    bool ZuiInit(void);
    void ZuiExit(void);
    void ZuiOffsetLast(float x, float y);
    void ZuiPaddingLast(float x, float y);
    void ZuiDisableLast(void);

    // -----------------------------------------------------------------------------

#ifdef ZUI_IMPLEMENTATION
    // --- ZUI IMPLEMENTATION ---
    // zui_core.c

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

    float ZuiClamp(float value, float min, float max)
    {
        float result = (value < min) ? min : value;
        result = (result > max) ? max : result;
        return result;
    }

    float ZuiLerp(float start, float end, float amount)
    {
        return start + (amount * (end - start));
    }
    // -----------------------------------------------------------------------------
    // zui_arena.c

    ZuiResult ZuiInitArena(ZuiArena *arena, const size_t capacity)
    {
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        // safe state FIRST (prevents use of uninitialized arena)
        arena->buffer = NULL;
        arena->offset = 0;
        arena->capacity = 0;
        arena->initialCapacity = 0;

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

        arena->capacity = capacity;
        arena->initialCapacity = capacity;
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
        arena->initialCapacity = 0;
    }

    ZuiResult ZuiGrowArena(ZuiArena *arena, const size_t requiredSize)
    {
        if (!arena)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!arena->buffer)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Arena buffer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (arena->initialCapacity == 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Arena initial capacity is zero");
            return ZUI_ERROR_INVALID_STATE;
        }

        // Calculate maximum allowed capacity (4x initial)
        const size_t maxCapacity = arena->initialCapacity * 4;

        // Check if we're already at max capacity
        if (arena->capacity >= maxCapacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ARENA_EXHAUSTED,
                             "Arena at maximum capacity (%zu bytes, 4x initial %zu bytes)",
                             arena->capacity, arena->initialCapacity);
            return ZUI_ERROR_ARENA_EXHAUSTED;
        }

        // Calculate new capacity (double current)
        size_t newCapacity = arena->capacity * 2;

        // Check for overflow in doubling
        if (newCapacity < arena->capacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Arena capacity doubling caused overflow");
            return ZUI_ERROR_OVERFLOW;
        }

        // Cap at maximum capacity
        if (newCapacity > maxCapacity)
        {
            newCapacity = maxCapacity;
        }

        // Ensure new capacity can fit the required size
        if (requiredSize > newCapacity)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_ARENA_EXHAUSTED,
                             "Required size %zu exceeds maximum arena capacity %zu",
                             requiredSize, newCapacity);
            return ZUI_ERROR_ARENA_EXHAUSTED;
        }

        // Attempt reallocation
        unsigned char *newBuffer = (unsigned char *)realloc(arena->buffer, newCapacity);
        if (!newBuffer)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY,
                             "Failed to reallocate arena from %zu to %zu bytes",
                             arena->capacity, newCapacity);
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        arena->buffer = newBuffer;
        arena->capacity = newCapacity;

#ifdef ZUI_DEBUG
        TraceLog(LOG_INFO, "ZUI: Arena grew from %zu to %zu bytes (max: %zu bytes)",
                 arena->capacity / 2, arena->capacity, maxCapacity);
#endif

        return ZUI_OK;
    }

    void *ZuiAllocArena(ZuiArena *arena, size_t size, const size_t alignment)
    {
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

        if (!(arena->buffer != NULL &&
              arena->offset <= arena->capacity))
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

        // Zero-size allocation returns current position without advancing
        if (size == 0)
        {
            return arena->buffer + arena->offset;
        }

        // overflow in size + padding
        if (size > SIZE_MAX - alignment)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Allocation size + alignment padding overflow");
            return NULL;
        }

        const size_t current_offset = arena->offset;
        const size_t mask = alignment - 1;
        const size_t aligned_offset = (current_offset + mask) & ~mask;

        // overflow in padding
        if (aligned_offset < current_offset)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Alignment padding caused offset overflow");
            return NULL;
        }

        // Check if we need to grow the arena
        if (aligned_offset + size > arena->capacity)
        {
            // Try to grow the arena
            ZuiResult growResult = ZuiGrowArena(arena, aligned_offset + size);
            if (growResult != ZUI_OK)
            {
                // Growth failed, report the original exhaustion error
                ZUI_REPORT_ERROR(ZUI_ERROR_ARENA_EXHAUSTED,
                                 "Arena out of memory: requested %zu bytes (with alignment), available %zu bytes",
                                 size, arena->capacity - current_offset);
                return NULL;
            }

            // After successful growth, verify we have enough space
            if (aligned_offset + size > arena->capacity)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_ARENA_EXHAUSTED,
                                 "Arena still out of memory after growth: requested %zu bytes, available %zu bytes",
                                 size, arena->capacity - current_offset);
                return NULL;
            }
        }

        void *ptr = (void *)(arena->buffer + aligned_offset);

        if (((uintptr_t)ptr & mask) != 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ALIGNMENT, "Arena alignment is invalid");
            return NULL;
        }

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
        stats.initialCapacity = arena->initialCapacity;
        stats.maxCapacity = arena->initialCapacity * 4;

        return stats;
    }
    // -----------------------------------------------------------------------------
    // zui_dynarray.c

    bool ZuiIsDynArrayValid(const ZuiDynArray *array)
    {
        return array != NULL &&
               array->items != NULL &&
               array->count <= array->capacity &&
               array->itemSize > 0 &&
               array->capacity > 0;
    }

    ZuiResult ZuiInitDynArray(ZuiDynArray *array, ZuiArena *arena, const uint32_t initialCapacity,
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

        if (!(arena->buffer != NULL &&
              arena->offset <= arena->capacity))
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
        // memory fault or a bug in the arena allocator itself.
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

        uint32_t new_capacity = array->capacity * 2;

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

        if (array->count >= array->capacity)
        {
            ZuiResult result = ZuiGrowDynArray(array, arena);
            if (result != ZUI_OK)
            {
                return NULL;
            }
        }

        // overflow check
        size_t byte_offset = array->itemSize * array->count;

        // multiplication overflow check
        if (array->count > 0 && byte_offset < array->itemSize)
        {
            TraceLog(LOG_ERROR, "ZUI: Multiplication overflow in PushDynArray: %u * %zu = %zu",
                     array->count, array->itemSize, byte_offset);
            return NULL;
        }

        // offset is within bounds
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

    void *ZuiGetDynArray(const ZuiDynArray *array, const uint32_t index)
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
    // -----------------------------------------------------------------------------
    // zui_component.c
    // #include "zui_menu.h"
    // #include "zui_window.h"

    const ZuiComponentRegistration COMPONENT_TABLE[] = {
        {
            .name = "Transform",
            .dataSize = sizeof(ZuiTransformData),
            .dataAlignment = ZUI_ALIGNOF(ZuiTransformData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
        },
        {
            .name = "Layout",
            .dataSize = sizeof(ZuiLayoutData),
            .dataAlignment = ZUI_ALIGNOF(ZuiLayoutData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
            .requiredComponents = COMPONENT_DEPS_TRANSFORM,
            .requiredComponentCount = 1,
        },
        {
            .name = "Align",
            .dataSize = sizeof(ZuiAlignData),
            .dataAlignment = ZUI_ALIGNOF(ZuiAlignData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
        },
        {
            .name = "Interaction",
            .dataSize = sizeof(ZuiInteractionData),
            .dataAlignment = ZUI_ALIGNOF(ZuiInteractionData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
            .requiredComponents = COMPONENT_DEPS_TRANSFORM,
            .requiredComponentCount = 1,
        },
        {
            .name = "State",
            .dataSize = sizeof(ZuiStateData),
            .dataAlignment = ZUI_ALIGNOF(ZuiStateData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
        },
        {
            .name = "Animation",
            .dataSize = sizeof(ZuiAnimationData),
            .dataAlignment = ZUI_ALIGNOF(ZuiAnimationData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
        },
        {
            .name = "Focus",
            .dataSize = sizeof(ZuiFocusData),
            .dataAlignment = ZUI_ALIGNOF(ZuiFocusData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
        },
        {
            .name = "Scroll",
            .dataSize = sizeof(ZuiScrollData),
            .dataAlignment = ZUI_ALIGNOF(ZuiScrollData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
            .requiredComponents = COMPONENT_DEPS_TRANSFORM,
            .requiredComponentCount = 1,
        },
        {
            .name = "Layer",
            .dataSize = sizeof(ZuiLayerData),
            .dataAlignment = ZUI_ALIGNOF(ZuiLayerData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
        },
        {
            .name = "Spatial",
            .dataSize = sizeof(ZuiSpatialData),
            .dataAlignment = ZUI_ALIGNOF(ZuiSpatialData),
            .initialCapacity = ZUI_ITEMS_CAPACITY,
            .requiredComponents = COMPONENT_DEPS_TRANSFORM,
            .requiredComponentCount = 1,
        },
    };

    ZuiResult ZuiInitComponentRegistry(void)
    {
        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ERROR_NULL_CONTEXT;
        }
        memset(&g_zui_ctx->componentRegistry, 0, sizeof(ZuiComponentRegistry));
        TraceLog(LOG_INFO, "ZUI: Component registry initialized");
        return ZUI_OK;
    }

    ZuiResult ZuiRegisterComponent(ZuiComponentId componentId, const ZuiComponentRegistration *info)
    {
        if (!g_zui_ctx || !info)
        {
            return ZUI_ERROR_NULL_POINTER;
        }
        uint32_t idx = (uint32_t)componentId;
        if (idx >= ZUI_COMPONENT_COUNT)
        {
            return ZUI_ERROR_INVALID_VALUE;
        }

        ZuiComponentRegistry *reg = &g_zui_ctx->componentRegistry;
        ZuiComponentRegistration *slot = &reg->registrations[idx];

        slot->name = info->name;
        slot->dataSize = info->dataSize;
        slot->dataAlignment = info->dataAlignment;
        slot->initialCapacity = info->initialCapacity;
        slot->id = idx;
        uint32_t actualCapacity = slot->initialCapacity > 0 ? slot->initialCapacity : 32;
        if (actualCapacity > UINT32_MAX - 100)
        {
            actualCapacity = 32;
        }
        ZuiResult result = ZuiInitDynArray(&slot->dataArray, &g_zui_arena,
                                           actualCapacity, slot->dataSize,
                                           slot->dataAlignment, slot->name);
        slot->init = info->init;
        slot->requiredComponents = info->requiredComponents;
        slot->requiredComponentCount = info->requiredComponentCount;
        if (result != ZUI_OK)
        {
            return result;
        }
        reg->registeredCount++;
        return ZUI_OK;
    }

    void ZuiRegisterAllComponents(void)
    {
        for (int i = 0; i < ZUI_COMPONENT_COUNT; i++)
        {
            ZuiRegisterComponent((ZuiComponentId)i, &COMPONENT_TABLE[i]);
        }
    }

    void *ZuiEnsureComponent(uint32_t itemId, ZuiComponentId componentId)
    {
        void *component = ZuiItemGetComponent(itemId, componentId);
        return component ? component : ZuiItemAddComponent(itemId, componentId);
    }

    Vector2 ZuiGetAbsolutePosition(uint32_t itemId)
    {
        Vector2 position = {0, 0};
        uint32_t currentId = itemId;

        while (currentId != ZUI_ID_INVALID)
        {
            Rectangle bounds = ZuiGetTransformBounds(currentId);
            position.x += bounds.x;
            position.y += bounds.y;

            const ZuiItem *item = ZuiGetItem(currentId);
            if (!item || item->parentId == ZUI_ID_INVALID)
            {
                break;
            }
            currentId = item->parentId;
        }
        return position;
    }

    ZuiComponentRegistration *ZuiGetComponentRegistration(uint32_t componentId)
    {
        if (!g_zui_ctx || componentId >= ZUI_COMPONENT_COUNT)
        {
            return NULL;
        }
        return &g_zui_ctx->componentRegistry.registrations[componentId];
    }

    const ZuiComponentRegistration *ZuiGetComponentInfo(uint32_t componentId)
    {
        const ZuiComponentRegistration *reg = ZuiGetComponentRegistration(componentId);
        if (!reg)
        {
            return NULL;
        }

        static ZuiComponentRegistration info;
        info.name = reg->name;
        info.dataSize = reg->dataSize;
        info.dataAlignment = reg->dataAlignment;
        info.initialCapacity = reg->initialCapacity;
        info.init = reg->init;
        info.requiredComponents = reg->requiredComponents;
        info.requiredComponentCount = reg->requiredComponentCount;
        return &info;
    }

    bool ZuiValidateComponentDependencies(uint32_t itemId, uint32_t componentId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return false;
        }

        const ZuiComponentRegistration *info = ZuiGetComponentInfo(componentId);
        if (!info || !info->requiredComponents || info->requiredComponentCount == 0)
        {
            return true;
        }

        for (uint32_t i = 0; i < info->requiredComponentCount; i++)
        {
            if (!ZuiItemHasComponent(itemId, info->requiredComponents[i]))
            {
                const ZuiComponentRegistration *reqInfo = ZuiGetComponentInfo(info->requiredComponents[i]);
                ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Component '%s' requires '%s' but item doesn't have it",
                                 info->name, reqInfo ? reqInfo->name : "unknown");
                return false;
            }
        }
        return true;
    }

    void *ZuiItemAddComponent(uint32_t itemId, uint32_t componentId)
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

        if (item->componentMask & ((uint32_t)1 << componentId))
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_STATE, "Item already has component '%s'", reg->name);
            return ZuiItemGetComponent(itemId, componentId);
        }

        if (item->componentCount >= ZUI_COMPONENT_TYPES)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OVERFLOW, "Item has max components (%d)", ZUI_COMPONENT_TYPES);
            return NULL;
        }

        if (!ZuiValidateComponentDependencies(itemId, componentId))
        {
            return NULL;
        }

        void *data = ZuiPushDynArray(&reg->dataArray, &g_zui_arena);
        if (!data)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate component data");
            return NULL;
        }

        uint32_t dataIndex = reg->dataArray.count - 1;
        item->components[item->componentCount++] = (ZuiItemComponent){.componentId = componentId, .dataIndex = dataIndex};
        item->componentMask |= ((uint32_t)1 << componentId);

        if (reg->init)
        {
            reg->init(data, itemId, componentId);
        }

#ifdef ZUI_DEBUG
        TraceLog(LOG_DEBUG, "ZUI: Added component '%s' to item %u", reg->name, itemId);
#endif
        return data;
    }

    void *ZuiItemGetComponent(uint32_t itemId, uint32_t componentId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || !(item->componentMask & ((uint32_t)1U << componentId)))
        {
            return NULL;
        }

        for (uint32_t i = 0; i < item->componentCount; i++)
        {
            if (item->components[i].componentId == componentId)
            {
                ZuiComponentRegistration *reg = ZuiGetComponentRegistration(componentId);
                if (!reg)
                {
                    return NULL;
                }

                uint32_t dataIndex = item->components[i].dataIndex;
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

    bool ZuiItemHasComponent(uint32_t itemId, uint32_t componentId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return false;
        }

        return (item->componentMask & ((uint32_t)1 << componentId)) != 0;
    }

    //--------------------------------------------------------TRANSFORM

    uint32_t ZuiCreateTransform(uint32_t itemId, Rectangle bounds)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        ZuiTransformData *t = (ZuiTransformData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!t)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to add transform component");
            return ZUI_ID_INVALID;
        }
        t->itemId = itemId;
        t->bounds = bounds;
        return 0;
    }

    void ZuiSetTransformBounds(uint32_t itemId, Rectangle bounds)
    {
        ZuiTransformData *t = (ZuiTransformData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (t)
        {
            t->bounds = bounds;
        }
    }

    void ZuiSetTransformOffset(uint32_t itemId, Vector2 offset)
    {
        ZuiTransformData *t = (ZuiTransformData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (t)
        {
            t->offset = offset;
        }
    }

    void ZuiSetTransformBoundsPosition(uint32_t itemId, Vector2 offset)
    {
        ZuiTransformData *t = (ZuiTransformData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (t)
        {
            t->bounds.x += offset.x;
            t->bounds.y += offset.y;
        }
    }

    Rectangle ZuiGetTransformBounds(uint32_t itemId)
    {
        const ZuiTransformData *t = (const ZuiTransformData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!t)
        {
            return (Rectangle){0, 0, 0, 0};
        }

        Rectangle bounds = t->bounds;
        bounds.x += t->offset.x;
        bounds.y += t->offset.y;

        const ZuiAnimationData *anim = (const ZuiAnimationData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_ANIMATION);
        if (anim)
        {
            if (anim->slots[ZUI_ANIM_SLOT_OFFSET_X].active)
            {
                bounds.x += anim->slots[ZUI_ANIM_SLOT_OFFSET_X].value;
            }
            if (anim->slots[ZUI_ANIM_SLOT_OFFSET_Y].active)
            {
                bounds.y += anim->slots[ZUI_ANIM_SLOT_OFFSET_Y].value;
            }
            if (anim->slots[ZUI_ANIM_SLOT_SCALE].active)
            {
                float scale = anim->slots[ZUI_ANIM_SLOT_SCALE].value;
                if (scale != 1.0F)
                {
                    float center_x = bounds.x + (bounds.width * 0.5F);
                    float center_y = bounds.y + (bounds.height * 0.5F);
                    bounds.width *= scale;
                    bounds.height *= scale;
                    bounds.x = center_x - (bounds.width * 0.5F);
                    bounds.y = center_y - (bounds.height * 0.5F);
                }
            }
        }
        return bounds;
    }

    //-------------------------------------------------------- LAYOUT

    ZuiAlignData *ZuiGetOrCreateAlignData(uint32_t itemId)
    {
        ZuiAlignData *a = (ZuiAlignData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_ALIGN);
        if (a)
        {
            return a;
        }

        a = (ZuiAlignData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_ALIGN);
        if (a)
        {
            a->itemId = itemId;
            a->align = ZUI_ALIGN_START;
            a->widthMode = ZUI_SIZE_AUTO;
            a->heightMode = ZUI_SIZE_AUTO;
            a->margin = 0.0F;
        }
        return a;
    }

    static Vector2 ZuiMeasureWidget(uint32_t itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return (Vector2){0, 0};
        }

        const ZuiAlignData *a = (const ZuiAlignData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_ALIGN);
        if (a && a->widthMode == ZUI_SIZE_FIXED && a->heightMode == ZUI_SIZE_FIXED)
        {
            return (Vector2){a->fixedWidth, a->fixedHeight};
        }

        Vector2 size = {0};
        if (item->type == ZUI_FRAME && ZuiItemHasComponent(itemId, ZUI_COMPONENT_LAYOUT))
        {
            const ZuiLayoutData *l = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
            size = l ? (Vector2){l->measuredWidth, l->measuredHeight} : (Vector2){0, 0};
        }
        else
        {
            Rectangle bounds = ZuiGetTransformBounds(itemId);
            size = (Vector2){bounds.width, bounds.height};
        }

        if (a)
        {
            if (a->widthMode == ZUI_SIZE_FIXED)
            {
                size.x = a->fixedWidth;
            }
            if (a->heightMode == ZUI_SIZE_FIXED)
            {
                size.y = a->fixedHeight;
            }
        }
        return size;
    }

    // NOLINTBEGIN(misc-no-recursion)
    void ZuiMeasureRecursive(uint32_t itemId, int depth)
    {
        if (depth >= ZUI_MAX_TREE_DEPTH)
        {
            return;
        }

        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || !item->isContainer)
        {
            return;
        }

        ZuiLayoutData *layout = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (!layout || layout->kind == ZUI_LAYOUT_NONE)
        {
            return;
        }

        const uint32_t *childIds = (uint32_t *)item->children.items;
        for (uint32_t i = 0; i < item->children.count; i++)
        {
            ZuiMeasureRecursive(childIds[i], depth + 1);
        }

        uint32_t childCount = item->children.count;
        if (childCount == 0)
        {
            layout->measuredWidth = layout->padding * 2.0F;
            layout->measuredHeight = layout->padding * 2.0F;
            return;
        }

        bool isVertical = (layout->kind == ZUI_LAYOUT_VERTICAL);
        float primary = 0.0F;
        float secondary = 0.0F;

        for (uint32_t i = 0; i < childCount; i++)
        {
            Vector2 childSize = ZuiMeasureWidget(childIds[i]);
            const ZuiAlignData *a = (const ZuiAlignData *)ZuiItemGetComponent(childIds[i], ZUI_COMPONENT_ALIGN);
            float margin = a ? a->margin : 0.0F;

            if (isVertical)
            {
                primary += childSize.y + (margin * 2.0F);
                secondary = fmaxf(secondary, childSize.x + (margin * 2.0F));
            }
            else
            {
                primary += childSize.x + (margin * 2.0F);
                secondary = fmaxf(secondary, childSize.y + (margin * 2.0F));
            }
        }

        if (childCount > 1)
        {
            primary += layout->spacing * (float)(childCount - 1);
        }

        if (isVertical)
        {
            layout->measuredWidth = secondary + (layout->padding * 2.0F);
            layout->measuredHeight = primary + (layout->padding * 2.0F);
        }
        else
        {
            layout->measuredWidth = primary + (layout->padding * 2.0F);
            layout->measuredHeight = secondary + (layout->padding * 2.0F);
        }
    }
    // NOLINTEND(misc-no-recursion)

    // NOLINTBEGIN(misc-no-recursion)
    static float ZuiAlignValue(float childSize, float parentSize, ZuiAlign align)
    {
        if (align == ZUI_ALIGN_CENTER)
        {
            return (parentSize - childSize) * 0.5F;
        }
        if (align == ZUI_ALIGN_END)
        {
            return parentSize - childSize;
        }
        return 0.0F;
    }

    void ZuiArrangeRecursive(uint32_t itemId, Rectangle bounds, int depth)
    {
        if (depth >= ZUI_MAX_TREE_DEPTH)
        {
            return;
        }

        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || !item->isContainer)
        {
            return;
        }

        ZuiLayoutData *layout = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (!layout || layout->kind == ZUI_LAYOUT_NONE)
        {
            return;
        }

        ZuiSetTransformBounds(itemId, bounds);

        Rectangle content = {
            bounds.x + layout->padding,
            bounds.y + layout->padding,
            bounds.width - (layout->padding * 2.0F),
            bounds.height - (layout->padding * 2.0F)};

        uint32_t childCount = item->children.count;
        if (childCount == 0)
        {
            layout->isFinalized = true;
            return;
        }

        const uint32_t *childIds = (uint32_t *)item->children.items;
        bool isVertical = (layout->kind == ZUI_LAYOUT_VERTICAL);

        uint32_t fillCount = 0;
        float fixedTotal = 0.0F;

        for (uint32_t i = 0; i < childCount; i++)
        {
            Vector2 childSize = ZuiMeasureWidget(childIds[i]);
            const ZuiAlignData *a = (const ZuiAlignData *)ZuiItemGetComponent(childIds[i], ZUI_COMPONENT_ALIGN);
            float margin = a ? a->margin : 0.0F;

            bool fillsPrimary = a && ((isVertical && a->heightMode == ZUI_SIZE_FILL) ||
                                      (!isVertical && a->widthMode == ZUI_SIZE_FILL));

            if (fillsPrimary)
            {
                fillCount++;
            }
            else
            {
                float size = isVertical ? childSize.y : childSize.x;
                fixedTotal += size + (margin * 2.0F);
            }
        }

        float spacingTotal = (childCount > 1) ? layout->spacing * (float)(childCount - 1) : 0.0F;
        float contentSize = isVertical ? content.height : content.width;
        float fillSize = (fillCount > 0) ? fmaxf(0.0F, (contentSize - fixedTotal - spacingTotal) / (float)fillCount) : 0.0F;

        float cursor = isVertical ? content.y : content.x;

        for (uint32_t i = 0; i < childCount; i++)
        {
            Vector2 childSize = ZuiMeasureWidget(childIds[i]);
            const ZuiAlignData *a = (const ZuiAlignData *)ZuiItemGetComponent(childIds[i], ZUI_COMPONENT_ALIGN);
            float margin = a ? a->margin : 0.0F;
            ZuiAlign align = a && a->align != ZUI_ALIGN_START ? a->align : layout->childAlign;

            cursor += margin;

            bool primaryFills =
                a && ((isVertical && a->heightMode == ZUI_SIZE_FILL) ||
                      (!isVertical && a->widthMode == ZUI_SIZE_FILL));

            bool secondaryFills =
                a && ((isVertical && a->widthMode == ZUI_SIZE_FILL) ||
                      (!isVertical && a->heightMode == ZUI_SIZE_FILL));

            float primaryMeasured = isVertical ? childSize.y : childSize.x;
            float secondaryMeasured = isVertical ? childSize.x : childSize.y;

            float primaryLimit = fillSize;
            float secondaryLimit = isVertical ? content.width : content.height;

            float primarySize = primaryFills ? primaryLimit : primaryMeasured;
            float secondarySize = secondaryFills ? secondaryLimit : secondaryMeasured;

            Rectangle childBounds;
            if (isVertical)
            {
                float offsetX = ZuiAlignValue(secondarySize, content.width, align);
                childBounds = (Rectangle){content.x + offsetX, cursor, secondarySize, primarySize};
            }
            else
            {
                float offsetY = ZuiAlignValue(secondarySize, content.height, align);
                childBounds = (Rectangle){cursor, content.y + offsetY, primarySize, secondarySize};
            }

            ZuiSetTransformBounds(childIds[i], childBounds);
            ZuiArrangeRecursive(childIds[i], childBounds, depth + 1);
            cursor += primarySize + margin + layout->spacing;
        }

        layout->isFinalized = true;
    }

    // NOLINTEND(misc-no-recursion)
    ZuiResult ZuiItemSetLayout(uint32_t itemId, ZuiLayoutKind kind)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || !g_zui_ctx)
        {
            return ZUI_ERROR_INVALID_ID;
        }

        ZuiLayoutData *l = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
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

        *l = (ZuiLayoutData){
            .itemId = itemId,
            .kind = kind,
            .spacing = ZUI_DEFAULT_FRAME_GAP,
            .padding = ZUI_DEFAULT_FRAME_PADDING,
            .childAlign = ZUI_ALIGN_START};
        return ZUI_OK;
    }

    void ZuiLayoutSetSpacing(uint32_t itemId, float spacing)
    {
        ZuiLayoutData *l = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (l)
        {
            l->spacing = spacing;
        }
    }

    void ZuiLayoutSetPadding(uint32_t itemId, float padding)
    {
        ZuiLayoutData *l = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (l)
        {
            l->padding = padding;
        }
    }

    void ZuiLayoutSetChildAlign(uint32_t itemId, ZuiAlign align)
    {
        ZuiLayoutData *l = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (l)
        {
            l->childAlign = align;
        }
    }

    void ZuiItemSetAlign(uint32_t itemId, ZuiAlign align)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (a)
        {
            a->align = align;
        }
    }

    void ZuiItemAlignLast(ZuiAlign align)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(g_zui_ctx->cursor.activeItem);
        if (a)
        {
            a->align = align;
        }
    }

    void ZuiItemSetSizeMode(uint32_t itemId, ZuiSizeMode widthMode, ZuiSizeMode heightMode)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (a)
        {
            a->widthMode = widthMode;
            a->heightMode = heightMode;
        }
    }

    void ZuiItemSetFixedSize(uint32_t itemId, float width, float height)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (!a)
        {
            return;
        }
        a->widthMode = ZUI_SIZE_FIXED;
        a->heightMode = ZUI_SIZE_FIXED;
        a->fixedWidth = width;
        a->fixedHeight = height;
    }

    void ZuiItemSetMargin(uint32_t itemId, float margin)
    {
        ZuiAlignData *a = ZuiGetOrCreateAlignData(itemId);
        if (!a)
        {
            return;
        }
        a->margin = margin;
    }

    uint32_t ZuiBeginVertical(Vector2 pos, float spacing, Color color)
    {
        uint32_t id = ZuiBeginFrame((Rectangle){pos.x, pos.y, 0, 0}, color);
        ZuiItemSetLayout(id, ZUI_LAYOUT_VERTICAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetChildAlign(id, ZUI_ALIGN_CENTER);
        return id;
    }

    uint32_t ZuiBeginHorizontal(Vector2 pos, float spacing, Color color)
    {
        uint32_t id = ZuiBeginFrame((Rectangle){pos.x, pos.y, 0, 0}, color);
        ZuiItemSetLayout(id, ZUI_LAYOUT_HORIZONTAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetChildAlign(id, ZUI_ALIGN_CENTER);
        return id;
    }

    // NOLINTBEGIN(misc-no-recursion)
    Vector2 ZuiCalculateContentSize(uint32_t itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || !item->isContainer)
        {
            return (Vector2){0, 0};
        }

        if (item->children.count == 0)
        {
            const ZuiLayoutData *layout = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
            if (layout)
            {
                return (Vector2){layout->measuredWidth, layout->measuredHeight};
            }
            return (Vector2){0, 0};
        }

        float minX = FLT_MAX;
        float minY = minX;
        float maxX = -FLT_MAX;
        float maxY = maxX;

        for (uint32_t i = 0; i < item->children.count; i++)
        {
            uint32_t childId = *(uint32_t *)ZuiGetDynArray(&item->children, i);
            Rectangle childBounds = ZuiGetTransformBounds(childId);
            const ZuiTransformData *transform = (const ZuiTransformData *)ZuiItemGetComponent(childId, ZUI_COMPONENT_TRANSFORM);
            if (transform)
            {
                childBounds.x += transform->offset.x;
                childBounds.y += transform->offset.y;
            }

            minX = fminf(minX, childBounds.x);
            minY = fminf(minY, childBounds.y);
            maxX = fmaxf(maxX, childBounds.x + childBounds.width);
            maxY = fmaxf(maxY, childBounds.y + childBounds.height);

            const ZuiItem *childItem = ZuiGetItem(childId);
            if (childItem && childItem->isContainer)
            {
                Vector2 childContent = ZuiCalculateContentSize(childId);
                float childMaxX = childBounds.x + childContent.x;
                float childMaxY = childBounds.y + childContent.y;
                maxX = fmaxf(maxX, childMaxX);
                maxY = fmaxf(maxY, childMaxY);
            }
        }

        const ZuiLayoutData *layout = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        float padding = layout ? layout->padding : 0.0F;

        float width = fmaxf(0.0F, maxX - minX + (padding * 2.0F));
        float height = fmaxf(0.0F, maxY - minY + (padding * 2.0F));

        return (Vector2){width, height};
    }
    // NOLINTEND(misc-no-recursion)

    void ZuiEndLayout(void)
    {
        ZUI_ASSERT_CTX_RETURN();
        const uint32_t itemId = g_zui_ctx->cursor.activeFrame;
        const ZuiLayoutData *layout = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);

        if (!layout)
        {
            g_zui_ctx->cursor.activeFrame = g_zui_ctx->cursor.parentFrame;
            return;
        }

        ZuiMeasureRecursive(itemId, 0);
        layout = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (!layout)
        {
            g_zui_ctx->cursor.activeFrame = g_zui_ctx->cursor.parentFrame;
            return;
        }

        Rectangle bounds = ZuiGetTransformBounds(itemId);
        if (bounds.width == 0)
        {
            bounds.width = layout->measuredWidth;
        }
        if (bounds.height == 0)
        {
            bounds.height = layout->measuredHeight;
        }

        ZuiArrangeRecursive(itemId, bounds, 0);

        ZuiScrollData *scroll = (ZuiScrollData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_SCROLL);
        if (scroll)
        {
            Vector2 contentSize = ZuiCalculateContentSize(itemId);
            scroll->contentSize = contentSize;
        }

        if (ZuiGetTypedDataConst(g_zui_ctx->cursor.parentFrame))
        {
            bounds = ZuiGetTransformBounds(itemId);
            ZuiAdvanceCursor(bounds.width, bounds.height);
        }

        g_zui_ctx->cursor.activeFrame = g_zui_ctx->cursor.parentFrame;
    }

    uint32_t ZuiBeginRow(float spacing)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        Rectangle b = {g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, 0, 0};
        uint32_t id = ZuiBeginFrame(b, BLANK);
        ZuiItemSetLayout(id, ZUI_LAYOUT_HORIZONTAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetPadding(id, 0);
        return id;
    }

    uint32_t ZuiBeginColumn(float spacing)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        Rectangle b = {g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, 0, 0};
        uint32_t id = ZuiBeginFrame(b, BLANK);
        ZuiItemSetLayout(id, ZUI_LAYOUT_VERTICAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetPadding(id, 0);
        return id;
    }

    uint32_t ZuiBeginCentered(float spacing, Color color)
    {
        Rectangle bounds = ZuiGetTransformBounds(g_zui_ctx->cursor.activeFrame);
        uint32_t id = ZuiBeginFrame(bounds, color);
        ZuiItemSetLayout(id, ZUI_LAYOUT_VERTICAL);
        ZuiLayoutSetSpacing(id, spacing);
        ZuiLayoutSetChildAlign(id, ZUI_ALIGN_CENTER);
        return id;
    }

    void ZuiSpace(float x, float y)
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t spacerId = ZuiNewFrame(BLANK, x, y);
        if (spacerId == ZUI_ID_INVALID)
        {
            return;
        }

        ZuiItemSetFixedSize(spacerId, x, y);
        ZuiItemAddChild(g_zui_ctx->cursor.activeFrame, spacerId);
        ZuiAdvanceCursor(x, y);
        g_zui_ctx->cursor.lastItemBounds = (Rectangle){g_zui_ctx->cursor.position.x, g_zui_ctx->cursor.position.y, x, y};
    }

    void ZuiEndVertical(void) { ZuiEndLayout(); }
    void ZuiEndHorizontal(void) { ZuiEndLayout(); }
    void ZuiEndRow(void) { ZuiEndLayout(); }
    void ZuiEndColumn(void) { ZuiEndLayout(); }
    void ZuiEndFrameAuto(void) { ZuiEndLayout(); }

    //-------------------------------------------------------- ANIMATION

    ZuiAnimation *ZuiGetAnimSlot(uint32_t itemId, ZuiAnimationSlot slot)
    {
        if (slot >= ZUI_ANIM_SLOT_COUNT)
        {
            return NULL;
        }
        ZuiAnimationData *a = (ZuiAnimationData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_ANIMATION);
        return a ? &a->slots[slot] : NULL;
    }

    void ZuiAnimSetTarget(uint32_t itemId, ZuiAnimationSlot slot, float target)
    {
        ZuiAnimation *anim = ZuiGetAnimSlot(itemId, slot);
        if (anim)
        {
            anim->target = target;

            if (!anim->active)
            {
                anim->active = true;
                ZuiAnimationData *a = (ZuiAnimationData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_ANIMATION);
                if (a)
                {
                    a->activeCount++;
                }
            }
        }
    }

    void ZuiAnimSetDuration(uint32_t itemId, ZuiAnimationSlot slot, float duration)
    {
        ZuiAnimation *anim = ZuiGetAnimSlot(itemId, slot);
        if (anim)
        {
            anim->duration = duration;
        }
    }

    void ZuiAnimSetType(uint32_t itemId, ZuiAnimationSlot slot, ZuiAnimationType type)
    {
        if (type >= ZUI_ANIM_TYPE_COUNT)
        {
            return;
        }

        ZuiAnimation *anim = ZuiGetAnimSlot(itemId, slot);
        if (anim)
        {
            anim->type = type;
        }
    }

    void ZuiAnimReset(uint32_t itemId, ZuiAnimationSlot slot, float value)
    {
        ZuiAnimation *anim = ZuiGetAnimSlot(itemId, slot);
        if (anim)
        {
            anim->value = value;
            anim->target = value;
            anim->active = false;
        }
    }

    float ZuiAnimGetValue(uint32_t itemId, ZuiAnimationSlot slot)
    {
        const ZuiAnimationData *a = (const ZuiAnimationData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_ANIMATION);
        if (!a || slot >= ZUI_ANIM_SLOT_COUNT)
        {
            return 0.0F;
        }
        return a->slots[slot].value;
    }

    void ZuiAnimSetupInteractive(uint32_t itemId, float duration)
    {
        ZuiAnimSetType(itemId, ZUI_ANIM_SLOT_HOVER, ZUI_ANIM_EASE_OUT);
        ZuiAnimSetDuration(itemId, ZUI_ANIM_SLOT_HOVER, duration);
        ZuiAnimSetType(itemId, ZUI_ANIM_SLOT_PRESS, ZUI_ANIM_EASE_OUT);
        ZuiAnimSetDuration(itemId, ZUI_ANIM_SLOT_PRESS, duration);
    }

    //--------------------------------------------------------FOCUS

    void ZuiEnableFocus(uint32_t itemId) { (void)ZuiEnsureComponent(itemId, ZUI_COMPONENT_FOCUS); }

    uint32_t ZuiGetFocusedItem(void) { return g_zui_ctx->g_focusContext.currentFocus; }

    void ZuiFocusItem(uint32_t itemId)
    {
        if (!ZuiIsVisible(itemId))
        {
            return;
        }

        ZuiFocusData *newFocus = (ZuiFocusData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_FOCUS);
        if (!newFocus || !newFocus->canReceiveFocus)
        {
            return;
        }

        if (g_zui_ctx->g_focusContext.currentFocus != UINT_MAX)
        {
            uint32_t oldId = g_zui_ctx->g_focusContext.currentFocus;

            ZuiFocusData *oldFocus = (ZuiFocusData *)ZuiItemGetComponent(oldId, ZUI_COMPONENT_FOCUS);
            if (oldFocus)
            {
                oldFocus->isFocused = false;
                if (oldFocus->onFocusLost)
                {
                    oldFocus->onFocusLost(oldId);
                }
            }

            ZuiInteractionData *oldInteract = (ZuiInteractionData *)ZuiItemGetComponent(oldId, ZUI_COMPONENT_INTERACTION);
            if (oldInteract)
            {
                oldInteract->isFocused = false;
            }
        }

        newFocus->isFocused = true;

        ZuiInteractionData *newInteract = (ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        if (newInteract)
        {
            newInteract->isFocused = true;
            newInteract->isPressed = false;
        }

        g_zui_ctx->g_focusContext.currentFocus = itemId;
        if (newFocus->onFocusGained)
        {
            newFocus->onFocusGained(itemId);
        }
    }

    void ZuiUnfocusItem(uint32_t itemId)
    {
        ZuiFocusData *f = (ZuiFocusData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_FOCUS);
        if (f && f->isFocused)
        {
            f->isFocused = false;
            if (f->onFocusLost)
            {
                f->onFocusLost(itemId);
            }
            if (g_zui_ctx->g_focusContext.currentFocus == itemId)
            {
                g_zui_ctx->g_focusContext.currentFocus = UINT_MAX;
            }
        }
    }

    void ZuiFocusSetTabIndex(uint32_t itemId, int index)
    {
        ZuiFocusData *f = (ZuiFocusData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_FOCUS);
        if (f)
        {
            f->tabIndex = index;
        }
    }

    void ZuiFocusLinkNext(uint32_t itemId, uint32_t toItem)
    {
        ZuiFocusData *f = (ZuiFocusData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_FOCUS);
        if (f)
        {
            f->nextItem = toItem;
        }
    }

    void ZuiFocusLinkPrev(uint32_t itemId, uint32_t toItem)
    {
        ZuiFocusData *f = (ZuiFocusData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_FOCUS);
        if (f)
        {
            f->prevItem = toItem;
        }
    }

    void ZuiFocusSetCallback(uint32_t itemId, void (*onGain)(uint32_t), void (*onLose)(uint32_t), void (*onActivate)(uint32_t))
    {
        ZuiFocusData *f = (ZuiFocusData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_FOCUS);
        if (f)
        {
            f->onFocusGained = onGain;
            f->onFocusLost = onLose;
            f->onActivate = onActivate;
        }
    }

    void ZuiCreateFocusChain(const uint32_t *ids, uint32_t count)
    {
        if (!ids || count == 0)
        {
            return;
        }
        for (uint32_t i = 0; i < count; i++)
        {
            uint32_t current = ids[i];
            uint32_t next = (i + 1 < count) ? ids[i + 1] : ids[0];
            uint32_t prev = (i > 0) ? ids[i - 1] : ids[count - 1];

            if (current == ZUI_ID_INVALID || next == ZUI_ID_INVALID || prev == ZUI_ID_INVALID)
            {
                continue;
            }

            ZuiFocusLinkNext(current, next);
            ZuiFocusLinkPrev(current, prev);
        }
    }

    void ZuiBreakFocusChain(const uint32_t *ids, uint32_t count)
    {
        if (!ids || count == 0)
        {
            return;
        }

        // Break circular link at the end of the chain
        // Last item's next becomes INVALID
        ZuiFocusData *last = (ZuiFocusData *)ZuiItemGetComponent(ids[count - 1], ZUI_COMPONENT_FOCUS);
        if (last)
        {
            last->nextItem = ZUI_ID_INVALID;
        }

        // First item's prev becomes INVALID
        ZuiFocusData *first = (ZuiFocusData *)ZuiItemGetComponent(ids[0], ZUI_COMPONENT_FOCUS);
        if (first)
        {
            first->prevItem = ZUI_ID_INVALID;
        }
    }

    bool ZuiFocusNavigate(int direction)
    {
        if (g_zui_ctx->g_focusContext.currentFocus == UINT_MAX)
        {
            return false;
        }

        const ZuiFocusData *current = (const ZuiFocusData *)ZuiItemGetComponent(g_zui_ctx->g_focusContext.currentFocus, ZUI_COMPONENT_FOCUS);
        if (!current)
        {
            return false;
        }

        uint32_t next = (direction > 0) ? current->nextItem : current->prevItem;
        if (next != UINT_MAX)
        {
            ZuiFocusItem(next);
            return true;
        }
        return false;
    }

    void ZuiFocusActivate(void)
    {
        if (g_zui_ctx->g_focusContext.currentFocus != UINT_MAX)
        {
            ZuiFocusData *f = (ZuiFocusData *)ZuiItemGetComponent(g_zui_ctx->g_focusContext.currentFocus, ZUI_COMPONENT_FOCUS);
            if (f && f->onActivate)
            {
                f->onActivate(g_zui_ctx->g_focusContext.currentFocus);
            }
        }
    }

    //--------------------------------------------------------SCROLL

    // NOLINTBEGIN(misc-no-recursion)
    void ZuiApplyScrollOffsetRecursive(uint32_t itemId, Vector2 offset)
    {
        ZuiTransformData *transform = (ZuiTransformData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (transform)
        {
            transform->offset.x += offset.x;
            transform->offset.y += offset.y;
        }

        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item || !item->isContainer)
        {
            return;
        }

        const uint32_t *childIds = (uint32_t *)item->children.items;
        for (uint32_t i = 0; i < item->children.count; i++)
        {
            ZuiApplyScrollOffsetRecursive(childIds[i], offset);
        }
    }

    // NOLINTEND(misc-no-recursion)
    Rectangle ZuiGetChildContentBounds(uint32_t childId)
    {
        const ZuiItem *child = ZuiGetItem(childId);
        if (!child)
        {
            return (Rectangle){0, 0, 0, 0};
        }

        Rectangle bounds = ZuiGetTransformBounds(childId);

        const ZuiTransformData *transform = (const ZuiTransformData *)ZuiItemGetComponent(childId, ZUI_COMPONENT_TRANSFORM);
        if (transform)
        {
            bounds.x += transform->offset.x;
            bounds.y += transform->offset.y;
        }

        if (child->isContainer)
        {
            Vector2 childContent = ZuiCalculateContentSize(childId);
            bounds.width = fmaxf(bounds.width, childContent.x);
            bounds.height = fmaxf(bounds.height, childContent.y);
        }
        return bounds;
    }

    Vector2 ZuiScrollGetMaxOffset(const ZuiScrollData *scroll)
    {
        return (Vector2){
            fmaxf(0.0F, scroll->contentSize.x - scroll->viewport.width),
            fmaxf(0.0F, scroll->contentSize.y - scroll->viewport.height)};
    }

    Vector2 ZuiScrollClampOffset(const ZuiScrollData *scroll, Vector2 offset)
    {
        Vector2 max = ZuiScrollGetMaxOffset(scroll);
        return (Vector2){ZuiClamp(offset.x, 0.0F, max.x), ZuiClamp(offset.y, 0.0F, max.y)};
    }

    Rectangle ZuiScrollGetBarRect(const ZuiScrollData *scroll, bool isVertical)
    {
        if ((isVertical && !scroll->verticalEnabled) || (!isVertical && !scroll->horizontalEnabled))
        {
            return (Rectangle){0};
        }

        Vector2 max = ZuiScrollGetMaxOffset(scroll);

        if (isVertical)
        {
            if (max.y <= 0.0F)
            {
                return (Rectangle){0};
            }
            float ratio = scroll->viewport.height / scroll->contentSize.y;
            float thumbHeight = fmaxf(20.0F, scroll->viewport.height * ratio);
            float trackHeight = scroll->viewport.height - thumbHeight;
            float progress = max.y > 0.0F ? scroll->offset.y / max.y : 0.0F;
            return (Rectangle){
                scroll->viewport.x + scroll->viewport.width - scroll->scrollbarSize - 2.0F,
                scroll->viewport.y + (progress * trackHeight),
                scroll->scrollbarSize,
                thumbHeight};
        }

        if (max.x <= 0.0F)
        {
            return (Rectangle){0};
        }
        float ratio = scroll->viewport.width / scroll->contentSize.x;
        float thumbWidth = fmaxf(20.0F, scroll->viewport.width * ratio);
        float trackWidth = scroll->viewport.width - thumbWidth;
        float progress = max.x > 0.0F ? scroll->offset.x / max.x : 0.0F;
        return (Rectangle){
            scroll->viewport.x + (progress * trackWidth),
            scroll->viewport.y + scroll->viewport.height - scroll->scrollbarSize - 2.0F,
            thumbWidth,
            scroll->scrollbarSize};
    }

    void ZuiEnableScroll(uint32_t frameId, bool vertical, bool horizontal)
    {
        if (!ZuiItemHasComponent(frameId, ZUI_COMPONENT_LAYOUT))
        {
            ZuiItemSetLayout(frameId, ZUI_LAYOUT_VERTICAL);
        }

        ZuiScrollData *scroll = (ZuiScrollData *)ZuiEnsureComponent(frameId, ZUI_COMPONENT_SCROLL);
        if (scroll)
        {
            *scroll = (ZuiScrollData){
                .itemId = frameId,
                .scrollbarSize = 8.0F,
                .verticalEnabled = vertical,
                .horizontalEnabled = horizontal,
                .offset = (Vector2){0, 0},
                .targetOffset = (Vector2){0, 0},
                .dragMode = ZUI_SCROLL_DRAG_NONE,
            };
            Vector2 contentSize = ZuiCalculateContentSize(frameId);
            scroll->contentSize = contentSize;
            ZuiFrameEnableScissor(frameId, true);
        }
    }

    void ZuiScrollTo(uint32_t frameId, Vector2 offset)
    {
        ZuiScrollData *scroll = (ZuiScrollData *)ZuiItemGetComponent(frameId, ZUI_COMPONENT_SCROLL);
        if (scroll)
        {
            scroll->targetOffset = ZuiScrollClampOffset(scroll, offset);
        }
    }

    void ZuiScrollBy(uint32_t frameId, Vector2 delta)
    {
        ZuiScrollData *scroll = (ZuiScrollData *)ZuiItemGetComponent(frameId, ZUI_COMPONENT_SCROLL);
        if (scroll)
        {
            scroll->targetOffset.x += delta.x;
            scroll->targetOffset.y += delta.y;
            scroll->targetOffset = ZuiScrollClampOffset(scroll, scroll->targetOffset);
        }
    }

    //--------------------------------------------------------INTERACTION

    void ZuiSetHoverCallback(uint32_t itemId, void (*onHover)(uint32_t))
    {
        ZuiInteractionData *i = (ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        if (i)
        {
            i->onHoverEnter = onHover;
        }
    }

    void ZuiSetClickCallback(uint32_t itemId, void (*onClick)(uint32_t, ZuiMouseButton))
    {
        ZuiInteractionData *i = (ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        if (i)
        {
            i->onClick = onClick;
        }
    }

    bool ZuiIsItemHovered(uint32_t itemId)
    {
        const ZuiInteractionData *i = (const ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        return i ? i->isHovered : false;
    }

    bool ZuiIsItemPressed(uint32_t itemId)
    {
        const ZuiInteractionData *i = (const ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        return i ? i->isPressed : false;
    }

    bool ZuiIsItemFocused(uint32_t itemId)
    {
        const ZuiInteractionData *i = (const ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        return i ? i->isFocused : false;
    }

    //--------------------------------------------------------STATE

    void ZuiSetEnabled(uint32_t itemId, bool enabled)
    {
        ZuiStateData *s = (ZuiStateData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_STATE);
        if (s)
        {
            s->isEnabled = enabled;
        }
    }

    void ZuiSetVisible(uint32_t itemId, bool visible)
    {
        ZuiStateData *s = (ZuiStateData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_STATE);
        if (s)
        {
            s->isVisible = visible;
        }
    }

    bool ZuiIsEnabled(uint32_t itemId)
    {
        const ZuiStateData *s = (const ZuiStateData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_STATE);
        return s ? s->isEnabled : true;
    }

    bool ZuiIsVisible(uint32_t itemId)
    {
        const ZuiStateData *s = (const ZuiStateData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_STATE);
        return s ? s->isVisible : true;
    }

    //--------------------------------------------------------LAYER

    void ZuiSetLayer(uint32_t itemId, int layer, int order)
    {
        ZuiLayerData *l = (ZuiLayerData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_LAYER);
        if (l)
        {
            l->layer = layer;
            l->order = order;
        }
    }

    void ZuiRaiseToFront(uint32_t itemId)
    {
        ZuiLayerData *l = (ZuiLayerData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_LAYER);
        if (l && g_zui_ctx)
        {
            int maxOrder = l->order;
            for (uint32_t i = 0; i < g_zui_ctx->items.count; i++)
            {
                const ZuiLayerData *other = (const ZuiLayerData *)ZuiItemGetComponent(i, ZUI_COMPONENT_LAYER);
                if (other && other->layer == l->layer && other->order > maxOrder)
                {
                    maxOrder = other->order;
                }
            }
            l->order = maxOrder + 1;
        }
    }

    int ZuiGetLayerValue(uint32_t itemId)
    {
        const ZuiLayerData *l = (const ZuiLayerData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYER);
        return l ? l->layer : ZUI_LAYER_CONTENT;
    }

    int ZuiGetLayerOrder(uint32_t itemId)
    {
        const ZuiLayerData *l = (const ZuiLayerData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYER);
        return l ? l->order : 0;
    }

    bool ZuiLayerBlocksInput(uint32_t itemId)
    {
        const ZuiLayerData *l = (const ZuiLayerData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYER);
        return l && l->blocksInput;
    }

    void ZuiBringToFrontInTree(uint32_t itemId)
    {
        ZuiItem *item = ZuiGetItemMut(itemId);
        if (!item || item->parentId == ZUI_ID_INVALID)
        {
            return;
        }

        ZuiItem *parent = ZuiGetItemMut(item->parentId);
        if (!parent || !parent->isContainer)
        {
            return;
        }

        int currentIdx = -1;
        for (uint32_t i = 0; i < parent->children.count; i++)
        {
            const uint32_t *childId = (uint32_t *)ZuiGetDynArray(&parent->children, i);
            if (*childId == itemId)
            {
                currentIdx = (int)i;
                break;
            }
        }

        if (currentIdx == -1 || currentIdx == (int)(parent->children.count - 1))
        {
            return;
        }

        for (uint32_t i = (uint32_t)currentIdx; i < parent->children.count - 1; i++)
        {
            uint32_t *curr = (uint32_t *)ZuiGetDynArray(&parent->children, i);
            const uint32_t *next = (uint32_t *)ZuiGetDynArray(&parent->children, i + 1);
            *curr = *next;
        }
        uint32_t *last = (uint32_t *)ZuiGetDynArray(&parent->children, parent->children.count - 1);
        *last = itemId;
    }

    // NOLINTBEGIN(misc-no-recursion)
    void ZuiPropagateLayerToChildren(uint32_t parentId)
    {
        const ZuiLayerData *parentLayer = (const ZuiLayerData *)ZuiItemGetComponent(parentId, ZUI_COMPONENT_LAYER);
        if (!parentLayer)
        {
            return;
        }

        const ZuiItem *parent = ZuiGetItem(parentId);
        if (!parent || !parent->isContainer)
        {
            return;
        }

        for (uint32_t i = 0; i < parent->children.count; i++)
        {
            uint32_t childId = *(uint32_t *)ZuiGetDynArray(&parent->children, i);
            ZuiSetLayer(childId, parentLayer->layer, parentLayer->order);
            ZuiPropagateLayerToChildren(childId);
        }
    }
    // NOLINTEND(misc-no-recursion)

    //--------------------------------------------------------SPATIAL

    void ZuiSetSpatialMode(uint32_t itemId, ZuiSpatialMode mode)
    {
        ZuiSpatialData *s = (ZuiSpatialData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_SPATIAL);
        if (!s)
        {
            return;
        }
        s->mode = mode;
    }

    void ZuiSetSpatialTilt(uint32_t itemId, float tiltY, float depth)
    {
        ZuiSpatialData *s = (ZuiSpatialData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_SPATIAL);
        if (!s)
        {
            return;
        }
        s->mode = ZUI_SPATIAL_TILT_Y;
        s->tiltY = tiltY;
        s->depth = depth;
    }

    void ZuiSetSpatialBend(uint32_t itemId, float amount, float radius)
    {
        ZuiSpatialData *s = (ZuiSpatialData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_SPATIAL);
        if (!s)
        {
            return;
        }
        s->mode = ZUI_SPATIAL_BEND_Y;
        s->bendAmount = amount;
        s->bendRadius = radius;
    }

    void ZuiSetSpatialParallax(uint32_t itemId, float parallax)
    {
        ZuiSpatialData *s = (ZuiSpatialData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_SPATIAL);
        if (!s)
        {
            return;
        }
        s->parallax = parallax;
    }

    void ZuiSetSpatialAffectsInput(uint32_t itemId, bool affects)
    {
        ZuiSpatialData *s = (ZuiSpatialData *)ZuiEnsureComponent(itemId, ZUI_COMPONENT_SPATIAL);
        if (!s)
        {
            return;
        }
        s->affectsInput = affects;
    }

    Vector2 ZuiSpatialTransformPoint(uint32_t itemId, Vector2 point)
    {
        const ZuiSpatialData *s = (const ZuiSpatialData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_SPATIAL);
        if (!s || s->mode == ZUI_SPATIAL_NONE)
        {
            return point;
        }

        const ZuiTransformData *t = (const ZuiTransformData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!t)
        {
            return point;
        }

        Vector2 result = point;
        Rectangle bounds = t->bounds;
        float centerY = bounds.y + (bounds.height * 0.5F);

        switch (s->mode)
        {
        case ZUI_SPATIAL_TILT_Y:
        {
            float relY = point.y - centerY;
            float z = relY * sinf(s->tiltY);
            float scale = 1.0F / (1.0F + z / s->depth);
            result.y = centerY + (relY * scale);
            result.x = bounds.x + (point.x - bounds.x) + (relY * s->parallax);
            break;
        }
        case ZUI_SPATIAL_BEND_Y:
        {
            float relY = point.y - centerY;
            float curve = sinf(relY / s->bendRadius) * s->bendAmount;
            result.x = point.x + curve;
            break;
        }
        default:
            break;
        }
        return result;
    }

    Vector2 ZuiSpatialInverseTransformPoint(uint32_t itemId, Vector2 point)
    {
        const ZuiSpatialData *s = (const ZuiSpatialData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_SPATIAL);
        if (!s || s->mode == ZUI_SPATIAL_NONE || !s->affectsInput)
        {
            return point;
        }

        const ZuiTransformData *t = (const ZuiTransformData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_TRANSFORM);
        if (!t)
        {
            return point;
        }

        Vector2 result = point;
        Rectangle bounds = t->bounds;
        float centerY = bounds.y + (bounds.height * 0.5F);

        switch (s->mode)
        {
        case ZUI_SPATIAL_TILT_Y:
        {
            float relY = point.y - centerY;
            float z = relY * sinf(s->tiltY);
            float scale = 1.0F + (z / s->depth);
            result.y = centerY + (relY * scale);
            result.x = bounds.x + (point.x - bounds.x) - (relY * s->parallax);
            break;
        }
        case ZUI_SPATIAL_BEND_Y:
        {
            float relY = point.y - centerY;
            float curve = sinf(relY / s->bendRadius) * s->bendAmount;
            result.x = point.x - curve;
            break;
        }
        default:
            break;
        }
        return result;
    }

    //-----------------------------------------------------------------HELPERS

    bool ZuiSetupAsContainer(uint32_t itemId)
    {
        ZuiItem *item = ZuiGetItemMut(itemId);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INTERNAL_ERROR, "Failed to get item");
            return false;
        }

        item->isContainer = true;
        ZuiResult result = ZuiInitDynArray(&item->children, &g_zui_arena, ZUI_CHILDREN_CAPACITY,
                                           sizeof(uint32_t), _Alignof(uint32_t), "Children");
        if (result != ZUI_OK)
        {
            ZUI_REPORT_ERROR(result, "Failed to initialize children");
            return false;
        }
        return true;
    }

    bool ZuiAddComponentsEx(uint32_t itemId, Rectangle bounds, uint32_t flags)
    {
        if (itemId == ZUI_ID_INVALID)
        {
            return false;
        }

        if (flags & (uint32_t)ZUI_COMP_TRANSFORM)
        {
            ZuiTransformData *transform = (ZuiTransformData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_TRANSFORM);
            if (!transform)
            {
                ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to add transform component");
                return false;
            }
            transform->itemId = itemId;
            transform->bounds = bounds;
        }

        if (flags & (uint32_t)ZUI_COMP_STATE)
        {
            ZuiStateData *state = (ZuiStateData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_STATE);
            if (state)
            {
                *state = (ZuiStateData){.itemId = itemId, .isVisible = true, .isEnabled = true};
            }
        }

        if (flags & (uint32_t)ZUI_COMP_LAYER)
        {
            ZuiLayerData *layer = (ZuiLayerData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_LAYER);
            if (layer)
            {
                *layer = (ZuiLayerData){.itemId = itemId, .layer = ZUI_LAYER_CONTENT};
            }
        }

        if (flags & (uint32_t)ZUI_COMP_ANIMATION)
        {
            ZuiAnimationData *anim = (ZuiAnimationData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_ANIMATION);
            if (anim)
            {
                anim->itemId = itemId;
                ZuiAnimSetType(itemId, ZUI_ANIM_SLOT_ALPHA, ZUI_ANIM_EASE_OUT);
                ZuiAnimSetDuration(itemId, ZUI_ANIM_SLOT_ALPHA, 0.2F);
                ZuiAnimReset(itemId, ZUI_ANIM_SLOT_ALPHA, 1.0F);
            }
        }

        if (flags & (uint32_t)ZUI_COMP_INTERACTION)
        {
            ZuiInteractionData *interaction = (ZuiInteractionData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_INTERACTION);
            if (interaction)
            {
                *interaction = (ZuiInteractionData){.itemId = itemId, .capturesMouse = true, .acceptsFocus = true};
            }
        }

        if (flags & (uint32_t)ZUI_COMP_FOCUS)
        {
            ZuiFocusData *focus = (ZuiFocusData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_FOCUS);
            if (focus)
            {
                *focus = (ZuiFocusData){
                    .itemId = itemId,
                    .canReceiveFocus = true,
                };
            }
        }

        if (flags & (uint32_t)ZUI_COMP_LAYOUT)
        {
            ZuiLayoutData *layout = (ZuiLayoutData *)ZuiItemAddComponent(itemId, ZUI_COMPONENT_LAYOUT);
            if (layout)
            {
                *layout = (ZuiLayoutData){
                    .itemId = itemId,
                    .kind = ZUI_LAYOUT_NONE,
                    .spacing = ZUI_DEFAULT_FRAME_GAP,
                    .padding = ZUI_DEFAULT_FRAME_PADDING};
            }
        }

        return true;
    }

    //-----------------------------------------------------------------TYPES

    ZuiResult ZuiRegisterType(ZuiItemType typeId, const ZuiTypeRegistration *info)
    {
        if (!g_zui_ctx || !info)
        {
            return ZUI_ERROR_NULL_POINTER;
        }

        uint32_t idx = (uint32_t)typeId;
        if (idx >= ZUI_TYPE_COUNT)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Invalid type ID %d", typeId);
            return ZUI_ERROR_INVALID_VALUE;
        }

        ZuiTypeRegistry *reg = &g_zui_ctx->typeRegistry;
        ZuiTypeRegistration *slot = &reg->registrations[idx];

        ZuiResult result = ZuiInitRegistryEntry(&slot->dataArray, slot, idx, info->name,
                                                info->dataSize, info->dataAlignment, info->initialCapacity);
        if (result != ZUI_OK)
        {
            return result;
        }

        slot->update = info->update;
        slot->render = info->render;
        slot->init = info->init;
        reg->registeredCount++;
        return ZUI_OK;
    }

    void ZuiRegisterAllTypes(void)
    {
        static const ZuiTypeRegistration TYPE_TABLE[] = {
            {
                .name = "Frame",
                .dataSize = sizeof(ZuiFrameData),
                .dataAlignment = ZUI_ALIGNOF(ZuiFrameData),
                .initialCapacity = ZUI_FRAMES_CAPACITY,
                .update = ZuiUpdateFrame,
                .render = ZuiRenderFrame,
            },
            {
                .name = "Label",
                .dataSize = sizeof(ZuiLabelData),
                .dataAlignment = ZUI_ALIGNOF(ZuiLabelData),
                .initialCapacity = ZUI_LABELS_CAPACITY,
                .render = ZuiRenderLabel,
            },
            {
                .name = "Texture",
                .dataSize = sizeof(ZuiTextureData),
                .dataAlignment = ZUI_ALIGNOF(ZuiTextureData),
                .initialCapacity = ZUI_TEXTURES_CAPACITY,
                .render = ZuiRenderTexture,
            },
            {
                .name = "Button",
                .dataSize = sizeof(ZuiButtonData),
                .dataAlignment = ZUI_ALIGNOF(ZuiButtonData),
                .initialCapacity = ZUI_BUTTONS_CAPACITY,
                .update = ZuiUpdateButton,
                .render = ZuiRenderButton,
            },
            // {
            //     .name = "Window",
            //     .dataSize = sizeof(ZuiWindowData),
            //     .dataAlignment = ZUI_ALIGNOF(ZuiWindowData),
            //     .initialCapacity = ZUI_MAX_WINDOWS,
            //     .update = ZuiUpdateWindow,
            //     .render = ZuiRenderWindow,
            //     .init = ZuiWindowTypeInit,
            // },
            // {
            //     .name = "MenuBar",
            //     .dataSize = sizeof(ZuiMenuBarData),
            //     .dataAlignment = ZUI_ALIGNOF(ZuiMenuBarData),
            //     .initialCapacity = 1,
            //     .update = ZuiMenuBarTypeUpdate,
            //     .render = ZuiMenuBarTypeRender,
            //     .init = ZuiMenuBarTypeInit,
            // },
            // {
            //     .name = "Menu",
            //     .dataSize = sizeof(ZuiMenuData),
            //     .dataAlignment = ZUI_ALIGNOF(ZuiMenuData),
            //     .initialCapacity = 2,
            //     .update = ZuiMenuTypeUpdate,
            //     .render = ZuiMenuTypeRender,
            //     .init = ZuiMenuTypeInit,
            // },
            // {
            //     .name = "MenuItem",
            //     .dataSize = sizeof(ZuiMenuItemData),
            //     .dataAlignment = ZUI_ALIGNOF(ZuiMenuItemData),
            //     .initialCapacity = 4,
            //     .update = ZuiMenuItemTypeUpdate,
            //     .render = ZuiMenuItemTypeRender,
            //     .init = ZuiMenuItemTypeInit,
            // },
        };

        for (int i = 0; i < (int)(sizeof(TYPE_TABLE) / sizeof(TYPE_TABLE[0])); i++)
        {
            ZuiRegisterType((ZuiItemType)i, &TYPE_TABLE[i]);
        }
    }

    uint32_t ZuiCreateTypedItem(uint32_t typeId)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(typeId);
        if (!reg)
        {
            return ZUI_ID_INVALID;
        }

        void *data = ZuiAllocRegistryData(&reg->dataArray, reg);
        if (!data)
        {
            return ZUI_ID_INVALID;
        }

        uint32_t dataIndex = reg->dataArray.count - 1;

        ZuiItem *item = ZuiPushDynArray(&g_zui_ctx->items, &g_zui_arena);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate item");
            return ZUI_ID_INVALID;
        }

        uint32_t itemId = g_zui_ctx->items.count - 1;

        memset(item, 0, sizeof(ZuiItem));
        item->id = itemId;
        item->type = typeId;
        item->dataIndex = dataIndex;
        item->parentId = ZUI_ID_INVALID;
        item->isContainer = false;
        item->canMove = false;

        if (reg->init)
        {
            reg->init(data, itemId);
        }

        return itemId;
    }

    ZuiTypeRegistration *ZuiGetTypeRegistration(uint32_t typeId)
    {
        if (!g_zui_ctx || typeId >= 16)
        {
            return NULL;
        }
        return &g_zui_ctx->typeRegistry.registrations[typeId];
    }

    const ZuiTypeRegistration *ZuiGetTypeInfo(uint32_t typeId)
    {
        const ZuiTypeRegistration *reg = ZuiGetTypeRegistration(typeId);
        if (!reg)
        {
            return NULL;
        }

        static ZuiTypeRegistration info;
        info.name = reg->name;
        info.dataSize = reg->dataSize;
        info.dataAlignment = reg->dataAlignment;
        info.initialCapacity = reg->initialCapacity;
        info.update = reg->update;
        info.render = reg->render;
        info.init = reg->init;
        return &info;
    }

    bool ZuiIsValidTypeId(uint32_t typeId) { return ZuiGetTypeRegistration(typeId) != NULL; }

    void *ZuiGetTypedData(uint32_t itemId)
    {
        const ZuiItem *item = ZuiGetItem(itemId);
        if (!item)
        {
            return NULL;
        }

        ZuiTypeRegistration *reg = ZuiGetTypeRegistration(item->type);
        if (!reg)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_ID, "Invalid type ID %u for item %u",
                             item->type, itemId);
            return NULL;
        }

        return ZuiGetRegistryData(&reg->dataArray, reg, item->dataIndex);
    }

    const void *ZuiGetTypedDataConst(uint32_t itemId) { return ZuiGetTypedData(itemId); }

    ZuiResult ZuiInitRegistryEntry(ZuiDynArray *dataArray, ZuiTypeRegistration *entry, uint32_t id,
                                   const char *name, size_t dataSize,
                                   size_t dataAlignment, uint32_t capacity)
    {
        if (!entry || !name)
        {
            return ZUI_ERROR_NULL_POINTER;
        }

        entry->name = name;
        entry->dataSize = dataSize;
        entry->dataAlignment = dataAlignment;
        entry->initialCapacity = capacity;
        entry->id = id;

        uint32_t actualCapacity = capacity > 0 ? capacity : 32;
        ZuiResult result = ZuiInitDynArray(dataArray, &g_zui_arena,
                                           actualCapacity, dataSize, dataAlignment, name);
        if (result != ZUI_OK)
        {
            return result;
        }

        TraceLog(LOG_INFO, "ZUI: Initialized registry entry '%s' at slot %d", name, id);
        return ZUI_OK;
    }

    void *ZuiGetRegistryData(ZuiDynArray *dataArray, const ZuiTypeRegistration *entry, uint32_t dataIndex)
    {
        if (!entry)
        {
            return NULL;
        }

        if (dataIndex >= dataArray->count)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_BOUNDS, "Data index %u out of bounds for '%s'",
                             dataIndex, entry->name);
            return NULL;
        }
        return ZuiGetDynArray(dataArray, dataIndex);
    }

    void *ZuiAllocRegistryData(ZuiDynArray *dataArray, ZuiTypeRegistration *entry)
    {
        if (!entry)
        {
            return NULL;
        }

        void *data = ZuiPushDynArray(dataArray, &g_zui_arena);
        if (!data)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY,
                             "Failed to allocate data for '%s'", entry->name);
            return NULL;
        }
        return data;
    }
    // -----------------------------------------------------------------------------
    // zui_item.c

    const ZuiItem *ZuiGetItem(uint32_t id)
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

    ZuiItem *ZuiGetItemMut(uint32_t id)
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

    ZuiResult ZuiItemAddChild(const uint32_t parentId, const uint32_t childId)
    {
        ZuiItem *parent = ZuiGetItemMut(parentId);
        ZuiItem *child = ZuiGetItemMut(childId);

        if (!parent || !child || !parent->isContainer)
        {
            return ZUI_ERROR_CIRCULAR_REFERENCE;
        }

        // Inherit parent layer automatically
        const ZuiLayerData *parentLayer = (const ZuiLayerData *)ZuiItemGetComponent(parentId, ZUI_COMPONENT_LAYER);
        if (parentLayer)
        {
            ZuiLayerData *childLayer = (ZuiLayerData *)ZuiItemGetComponent(childId, ZUI_COMPONENT_LAYER);
            if (!childLayer)
            {
                childLayer = (ZuiLayerData *)ZuiItemAddComponent(childId, ZUI_COMPONENT_LAYER);
            }

            if (childLayer)
            {
                // Child gets same layer, order + 1
                if (childLayer->layer < parentLayer->layer ||
                    (childLayer->layer == parentLayer->layer && childLayer->order <= parentLayer->order))
                {
                    childLayer->layer = parentLayer->layer;
                    childLayer->order = parentLayer->order + 1;
                }
            }
        }

        uint32_t *childIdSlot = (uint32_t *)ZuiPushDynArray(&parent->children, &g_zui_arena);
        if (!childIdSlot)
        {
            return ZUI_ERROR_OUT_OF_MEMORY;
        }

        *childIdSlot = childId;
        child->parentId = parentId;
        return ZUI_OK;
    }

    uint32_t ZuiCreateItem(const ZuiItemType type, const uint32_t dataIndex)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        ZuiItem *item = (ZuiItem *)ZuiPushDynArray(&g_zui_ctx->items, &g_zui_arena);
        if (!item)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_OUT_OF_MEMORY, "Failed to allocate item");
            return ZUI_ID_INVALID;
        }

        uint32_t item_id = g_zui_ctx->items.count - 1;

        *item = (ZuiItem){
            .id = item_id,
            .parentId = ZUI_ID_INVALID,
            .type = type,
            .dataIndex = dataIndex,
            .children = (ZuiDynArray){0},
            .componentMask = 0,
            .componentCount = 0,
            .isContainer = false,
            .canMove = false,
        };

        return item_id;
    }

    uint32_t ZuiAddChild(uint32_t id)
    {
        Rectangle bounds = ZuiGetTransformBounds(id);
        ZuiItemAddChild(g_zui_ctx->cursor.activeFrame, id);
        ZuiAdvanceCursor(bounds.width, bounds.height);
        g_zui_ctx->cursor.lastItemBounds = bounds;
        return id;
    }

    void ZuiUpdateItem(const ZuiItem *item)
    {
        if (item)
        {
            const ZuiTypeRegistration *reg = ZuiGetTypeRegistration(item->type);
            if (reg && reg->update)
            {
                reg->update(item->dataIndex);
            }
        }
    }

    void ZuiRenderItem(const ZuiItem *item)
    {
        if (item)
        {
            const ZuiTypeRegistration *reg = ZuiGetTypeRegistration(item->type);
            if (reg && reg->render)
            {
                reg->render(item->dataIndex);
            }
        }
    }
    // -----------------------------------------------------------------------------
    // zui_frame.c

    uint32_t ZuiCreateFrame(Rectangle bounds, ZuiFrameStyle style)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        if (bounds.width < 0 || bounds.height < 0)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_BOUNDS, "Invalid bounds");
            return ZUI_ID_INVALID;
        }

        uint32_t itemId = ZuiCreateTypedItem(ZUI_FRAME);
        if (itemId == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        ZuiFrameData *frameData = (ZuiFrameData *)ZuiGetTypedData(itemId);
        if (!frameData)
        {
            return ZUI_ID_INVALID;
        }

        *frameData = (ZuiFrameData){.itemId = itemId};
        frameData->style = style;

        if (!ZuiAddComponentsEx(itemId, bounds, ZUI_COMP_STANDARD))
        {
            return ZUI_ID_INVALID;
        }

        if (!ZuiSetupAsContainer(itemId))
        {
            return ZUI_ID_INVALID;
        }

        return itemId;
    }

    void ZuiUpdateFrame(uint32_t dataIndex)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_FRAME];
        const ZuiFrameData *frameData = (const ZuiFrameData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!frameData)
        {
            return;
        }

        const ZuiItem *frameItem = ZuiGetItem(frameData->itemId);
        if (!frameItem || !frameItem->isContainer)
        {
            return;
        }

        const uint32_t *childIds = (uint32_t *)frameItem->children.items;
        for (uint32_t i = 0; i < frameItem->children.count; i++)
        {
            const ZuiItem *child = ZuiGetItem(childIds[i]);
            if (child)
            {
                ZuiUpdateItem(child);
            }
        }
    }

    void ZuiRenderFrame(uint32_t dataIndex)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_FRAME];
        const ZuiFrameData *frameData = (const ZuiFrameData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!frameData)
        {
            return;
        }

        uint32_t itemId = frameData->itemId;
        if (!ZuiIsVisible(itemId))
        {
            return;
        }

        const ZuiItem *frameItem = ZuiGetItem(itemId);
        if (!frameItem)
        {
            return;
        }

        Color bgColor = frameData->style.bgColor;
        float alpha = ZuiAnimGetValue(itemId, ZUI_ANIM_SLOT_ALPHA);
        bgColor.a = (unsigned char)((float)bgColor.a * alpha);

        Rectangle bounds = ZuiGetTransformBounds(itemId);
        float roundness = ZuiPixelsToRoundness(bounds, frameData->style.cornerRadius);

        if (bgColor.a > 0)
        {
            DrawRectangleRounded(bounds, roundness, frameData->style.roudnesSegments, bgColor);
        }

        if (frameData->style.hasBorder && frameData->style.borderThickness > 0)
        {
            Color borderColor = frameData->style.borderColor;
            borderColor.a = (unsigned char)((float)borderColor.a * alpha);
            DrawRectangleRoundedLinesEx(bounds, roundness, frameData->style.roudnesSegments,
                                        frameData->style.borderThickness, borderColor);
        }

        const ZuiScrollData *scroll = (const ZuiScrollData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_SCROLL);
        bool hasScroll = (scroll != NULL && (scroll->verticalEnabled || scroll->horizontalEnabled));

        Vector2 scrollOffset = {0, 0};
        if (hasScroll)
        {
            scrollOffset = scroll->offset;
        }

        bool useScissor = frameData->enableScissor || hasScroll;
        if (useScissor)
        {
            const ZuiLayoutData *layout = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
            float padding = layout ? layout->padding : 0.0F;

            Rectangle scissorRect = {
                bounds.x + padding,
                bounds.y + padding,
                bounds.width - (padding * 2.0F),
                bounds.height - (padding * 2.0F)};

            BeginScissorMode(
                (int)scissorRect.x,
                (int)scissorRect.y,
                (int)scissorRect.width,
                (int)scissorRect.height);
        }

        if (frameItem->isContainer)
        {
            const uint32_t *childIds = (uint32_t *)frameItem->children.items;
            for (uint32_t i = 0; i < frameItem->children.count; i++)
            {
                const ZuiItem *child = ZuiGetItem(childIds[i]);
                if (child)
                {
                    if (hasScroll)
                    {
                        ZuiApplyScrollOffsetRecursive(childIds[i],
                                                      (Vector2){-scrollOffset.x, -scrollOffset.y});
                        ZuiRenderItem(child);
                        ZuiApplyScrollOffsetRecursive(childIds[i], scrollOffset);
                    }
                    else
                    {
                        ZuiRenderItem(child);
                    }
                }
            }
        }

        if (useScissor)
        {
            EndScissorMode();
        }

        if (hasScroll)
        {
            Rectangle vbar = ZuiScrollGetBarRect(scroll, true);
            if (vbar.width > 0)
            {
                DrawRectangleRec(vbar, frameData->style.scrollbarColor);
            }

            Rectangle hbar = ZuiScrollGetBarRect(scroll, false);
            if (hbar.width > 0)
            {
                DrawRectangleRec(hbar, frameData->style.scrollbarColor);
            }
        }
    }

    uint32_t ZuiBeginFrame(Rectangle bounds, Color color)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        g_zui_ctx->cursor.isRow = false;
        ZuiFrameStyle frameStyle = g_zui_ctx->defaultTheme.defaultFrameStyle;
        frameStyle.bgColor = color;
        uint32_t id = ZuiCreateFrame(bounds, frameStyle);

        if (id == ZUI_ID_INVALID)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_INVALID_VALUE, "Failed to create frame");
            return ZUI_ID_INVALID;
        }

        ZuiSetTransformBounds(id, bounds);
        float pad = ZuiGetFramePadding(id);
        ZuiItemAddChild(g_zui_ctx->cursor.activeFrame, id);

        g_zui_ctx->cursor.parentFrame = g_zui_ctx->cursor.activeFrame;
        g_zui_ctx->cursor.position = (Vector2){bounds.x + pad, bounds.y + pad};
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.activeFrame = id;
        g_zui_ctx->cursor.activeItem = id;
        return id;
    }

    void ZuiEndFrame(void)
    {
        ZUI_ASSERT_CTX_RETURN();
        g_zui_ctx->cursor.isRow = false;
        g_zui_ctx->cursor.activeFrame = g_zui_ctx->cursor.parentFrame;
    }

    uint32_t ZuiNewFrame(Color color, float width, float height)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        Rectangle bounds = (Rectangle){
            g_zui_ctx->cursor.position.x,
            g_zui_ctx->cursor.position.y,
            width, height};

        ZuiFrameStyle style = g_zui_ctx->defaultTheme.defaultFrameStyle;
        style.bgColor = color;
        uint32_t id = ZuiCreateFrame(bounds, style);
        ZuiSetTransformBounds(id, bounds);
        ZuiItemAddChild(g_zui_ctx->cursor.activeFrame, id);
        ZuiAdvanceCursor(width, height);
        g_zui_ctx->cursor.lastItemBounds = bounds;
        g_zui_ctx->cursor.activeItem = id;
        return id;
    }

    void ZuiFrameBackground(Color color)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiFrameData *frameData = (ZuiFrameData *)ZuiGetTypedData(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->style.bgColor = color;
        }
    }

    void ZuiFrameOutline(Color color, float thickness)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiFrameData *frameData = (ZuiFrameData *)ZuiGetTypedData(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->style.hasBorder = true;
            frameData->style.borderThickness = thickness;
            frameData->style.borderColor = color;
        }
    }

    void ZuiFrameGap(float gap)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiLayoutData *layout = (ZuiLayoutData *)ZuiItemGetComponent(g_zui_ctx->cursor.activeFrame, ZUI_COMPONENT_LAYOUT);
        if (layout)
        {
            layout->spacing = gap;
        }
    }

    void ZuiFramePad(float pad)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiLayoutData *layout = (ZuiLayoutData *)ZuiItemGetComponent(g_zui_ctx->cursor.activeFrame, ZUI_COMPONENT_LAYOUT);
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
        ZUI_ASSERT_CTX_RETURN();
        ZuiTransformData *transform = (ZuiTransformData *)ZuiItemGetComponent(g_zui_ctx->cursor.activeFrame, ZUI_COMPONENT_TRANSFORM);
        if (transform)
        {
            transform->offset.x += x;
            transform->offset.y += y;
            g_zui_ctx->cursor.position.x += x;
            g_zui_ctx->cursor.position.y += y;
        }
    }

    void ZuiFrameCornerRadius(float radius)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiFrameData *frameData = (ZuiFrameData *)ZuiGetTypedData(g_zui_ctx->cursor.activeFrame);
        if (frameData)
        {
            frameData->style.cornerRadius = radius;
        }
    }

    void ZuiFrameEnableScissor(uint32_t itemId, bool enable)
    {
        ZuiFrameData *frameData = (ZuiFrameData *)ZuiGetTypedData(itemId);
        if (frameData)
        {
            frameData->enableScissor = enable;
        }
    }

    void ZuiFrameMakeScrollable(bool vertical, bool horizontal)
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeFrame;
        ZuiLayoutData *layout = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        if (!layout)
        {
            ZuiLayoutKind kind = ZUI_LAYOUT_VERTICAL; // default
            if (horizontal && !vertical)
            {
                kind = ZUI_LAYOUT_HORIZONTAL;
            }
            ZuiItemSetLayout(itemId, kind);
            layout = (ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        }

        if (layout)
        {
            if (layout->padding < 4.0F) // Minimum padding for scrollable areas
            {
                layout->padding = 4.0F;
            }
        }
        ZuiEnableScroll(itemId, vertical, horizontal);
    }

    float ZuiGetFramePadding(uint32_t itemId)
    {
        const ZuiLayoutData *layout = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        return layout ? layout->padding : ZUI_DEFAULT_FRAME_PADDING;
    }

    float ZuiGetFrameSpacing(uint32_t itemId)
    {
        const ZuiLayoutData *layout = (const ZuiLayoutData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_LAYOUT);
        return layout ? layout->spacing : ZUI_DEFAULT_FRAME_GAP;
    }
    // -----------------------------------------------------------------------------
    // zui_label.c

    uint32_t ZuiCreateLabel(const char *text, ZuiLabelStyle style)
    {
        if (!g_zui_ctx || !text)
        {
            return ZUI_ID_INVALID;
        }
        // item
        uint32_t itemId = ZuiCreateTypedItem(ZUI_LABEL);
        if (itemId == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        // labelData
        ZuiLabelData *labelData = (ZuiLabelData *)ZuiGetTypedData(itemId);
        if (!labelData)
        {
            return ZUI_ID_INVALID;
        }
        // init
        *labelData = (ZuiLabelData){
            .itemId = itemId,
            .style = style,
        };

        snprintf(labelData->text, ZUI_MAX_TEXT_LENGTH, "%s", text);
        Vector2 textSize = MeasureTextEx(labelData->style.font,
                                         text, labelData->style.fontSize, labelData->style.fontSpacing);
        Rectangle bounds = {0, 0, textSize.x, textSize.y};

        // + Transform, Style,  Layer, Animation components
        if (!ZuiAddComponentsEx(itemId, bounds, ZUI_COMP_STANDARD))
        {
            return ZUI_ID_INVALID;
        }
        g_zui_ctx->cursor.activeItem = itemId;
        return itemId;
    }

    void ZuiRenderLabel(uint32_t dataIndex)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_LABEL];
        const ZuiLabelData *labelData = (const ZuiLabelData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!labelData)
        {
            return;
        }

        uint32_t itemId = labelData->itemId;

        if (!ZuiIsVisible(itemId))
        {
            return;
        }

        Color textColor = labelData->style.textColor;
        Color bgColor = labelData->style.textBgColor;

        // Apply animation (alpha fade)
        float alpha = ZuiAnimGetValue(itemId, ZUI_ANIM_SLOT_ALPHA);
        textColor.a = (unsigned char)((float)textColor.a * alpha);
        bgColor.a = (unsigned char)((float)bgColor.a * alpha);

        Rectangle bounds = ZuiGetTransformBounds(itemId);

        if (bgColor.a > 0)
        {
            DrawRectangleRec(bounds, bgColor);
        }

        // Draw text
        DrawTextEx(labelData->style.font, labelData->text,
                   (Vector2){bounds.x, bounds.y},
                   labelData->style.fontSize, labelData->style.fontSpacing, textColor);
    }

    uint32_t ZuiAddLabelToFrame(uint32_t id)
    {
        Rectangle bounds = ZuiGetTransformBounds(id);
        bounds.x = g_zui_ctx->cursor.position.x;
        bounds.y = g_zui_ctx->cursor.position.y;
        ZuiSetTransformBounds(id, bounds);
        return ZuiAddChild(id);
    }

    uint32_t ZuiNewLabel(const char *text)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        uint32_t id = ZuiCreateLabel(text, g_zui_ctx->defaultTheme.defaultLabelStyle);
        return ZuiAddLabelToFrame(id);
    }

    uint32_t ZuiNewMonoLabel(const char *text)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        ZuiLabelStyle monoLabelStyle = g_zui_ctx->defaultTheme.defaultLabelStyle;
        monoLabelStyle.font = g_zui_ctx->defaultTheme.monoFont;
        uint32_t id = ZuiCreateLabel(text, monoLabelStyle);
        return ZuiAddLabelToFrame(id);
    }

    void ZuiLabelTextColor(Color textColor)
    {
        ZuiLabelData *labelData = (ZuiLabelData *)ZuiGetTypedData(g_zui_ctx->cursor.activeItem);
        if (labelData)
        {
            labelData->style.textColor = textColor;
        }
    }

    void ZuiLabelBackgroundColor(Color backgroundColor)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiLabelData *labelData = (ZuiLabelData *)ZuiGetTypedData(g_zui_ctx->cursor.activeItem);
        if (labelData)
        {
            labelData->style.textBgColor = backgroundColor;
        }
    }
    // -----------------------------------------------------------------------------
    // zui_texture.c

    uint32_t ZuiCreateTexture(Texture2D texture, ZuiTextureStyle style)
    {
        if (!g_zui_ctx)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL");
            return ZUI_ID_INVALID;
        }

        uint32_t itemId = ZuiCreateTypedItem(ZUI_TEXTURE);
        if (itemId == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        ZuiTextureData *texData = (ZuiTextureData *)ZuiGetTypedData(itemId);
        if (!texData)
        {
            return ZUI_ID_INVALID;
        }
        texData->itemId = itemId;
        texData->texture = texture;
        texData->style = style;
        Rectangle bounds = {0, 0, (float)texture.width, (float)texture.height};

        if (!ZuiAddComponentsEx(itemId, bounds, ZUI_COMP_STANDARD))
        {
            return ZUI_ID_INVALID;
        }
        g_zui_ctx->cursor.activeItem = itemId;
        return itemId;
    }

    void ZuiRenderTexture(uint32_t dataIndex)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_TEXTURE];
        const ZuiTextureData *texData = (const ZuiTextureData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!texData)
        {
            return;
        }

        uint32_t itemId = texData->itemId;
        if (!ZuiIsVisible(itemId))
        {
            return;
        }

        Color tintColor = texData->style.tintColor;
        float alpha = ZuiAnimGetValue(itemId, ZUI_ANIM_SLOT_ALPHA);
        tintColor.a = (unsigned char)((float)tintColor.a * alpha);
        Rectangle bounds = ZuiGetTransformBounds(itemId);
        DrawTextureNPatch(texData->texture, texData->style.npatch, bounds,
                          (Vector2){0, 0}, 0.0F, tintColor);
    }

    uint32_t ZuiNewTextureEx(const Texture2D tex, const NPatchInfo npatch, const Rectangle bounds)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        ZuiTextureStyle style = g_zui_ctx->defaultTheme.defaultTextureStyle;
        style.npatch = npatch;
        uint32_t id = ZuiCreateTexture(tex, style);
        ZuiSetTransformBounds(id, bounds);
        return ZuiAddChild(id);
    }

    uint32_t ZuiNewTexture(const Texture2D tex)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
        };
        Rectangle bounds = (Rectangle){
            g_zui_ctx->cursor.position.x,
            g_zui_ctx->cursor.position.y,
            (float)tex.width,
            (float)tex.height};

        return ZuiNewTextureEx(tex, npatch, bounds);
    }

    uint32_t ZuiNew3XSlice(const Texture2D tex, const int width, const int left, const int right)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
            .left = left,
            .right = right,
            .layout = NPATCH_NINE_PATCH,
        };
        Rectangle bounds = (Rectangle){
            g_zui_ctx->cursor.position.x,
            g_zui_ctx->cursor.position.y,
            (float)width,
            (float)tex.height};

        return ZuiNewTextureEx(tex, npatch, bounds);
    }

    uint32_t ZuiNew3YSlice(const Texture2D tex, const int height, const int top, const int bottom)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
            .top = top,
            .bottom = bottom,
            .layout = NPATCH_NINE_PATCH};
        Rectangle bounds = (Rectangle){
            g_zui_ctx->cursor.position.x,
            g_zui_ctx->cursor.position.y,
            (float)tex.width,
            (float)height};

        return ZuiNewTextureEx(tex, npatch, bounds);
    }

    uint32_t ZuiNew9Slice(const Texture2D tex, const int width, const int height,
                          const int left, const int top, const int right, const int bottom)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();
        NPatchInfo npatch = (NPatchInfo){
            .source = (Rectangle){0, 0, (float)tex.width, (float)tex.height},
            .left = left,
            .top = top,
            .bottom = bottom,
            .right = right,
            .layout = NPATCH_NINE_PATCH};
        Rectangle bounds = (Rectangle){
            g_zui_ctx->cursor.position.x,
            g_zui_ctx->cursor.position.y,
            (float)width,
            (float)height};

        return ZuiNewTextureEx(tex, npatch, bounds);
    }

    void ZuiTextureTintColor(Color tintColor)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTextureData *texData = (ZuiTextureData *)ZuiGetTypedData(g_zui_ctx->cursor.activeItem);
        if (!texData)
        {
            return;
        }
        texData->style.tintColor = tintColor;
    }
    // -----------------------------------------------------------------------------
    // zui_theme.c

    ZuiResult ZuiInitTheme(ZuiTheme *theme, int dpiScale)
    {
        if (!theme)
        {
            ZUI_REPORT_ERROR(ZUI_ERROR_NULL_POINTER, "Theme pointer is NULL");
            return ZUI_ERROR_NULL_POINTER;
        }

        if (!ZuiEnsureContext(ZUI_ERROR_NULL_CONTEXT, "Global context is NULL"))
        {
            return ZUI_ERROR_NULL_CONTEXT;
        }

        // Initialize theme to safe defaults
        *theme = (ZuiTheme){
            .windowFrameTexture = ZUI_ID_INVALID,
            .windowTitlebarTexture = ZUI_ID_INVALID,
            .windowCloseButtonTexture = ZUI_ID_INVALID,
            .buttonNormalTexture = ZUI_ID_INVALID,
            .buttonHoverTexture = ZUI_ID_INVALID,
            .buttonPressedTexture = ZUI_ID_INVALID,
            .windowFrameColor = (Color){220, 220, 220, 255},
            .windowTitlebarColor = (Color){150, 150, 150, 255},
            .windowTitleTextColor = RAYWHITE,
            .windowBodyColor = (Color){197, 197, 197, 255},
            .initialized = false,
        };

        // Load window decoration textures based on DPI
        Texture2D frameTex = {0};
        Texture2D titlebarTex = {0};
        Texture2D closeTex = {0};

        if (dpiScale < 2)
        {
            // Standard DPI
            frameTex = LoadTexture("src/resources/frame.png");
            titlebarTex = LoadTexture("src/resources/titlebar.png");
            closeTex = LoadTexture("src/resources/close_button.png");
            theme->windowTitleHeight = (float)titlebarTex.height;

            if (frameTex.id != 0)
            {
                theme->windowFrameTexture = ZuiCreateTexture(frameTex, g_zui_ctx->defaultTheme.defaultTextureStyle);

                float width = (float)frameTex.width;
                float height = (float)frameTex.height;
                theme->windowFrameNPatch = (NPatchInfo){
                    .source = (Rectangle){0, 0, width, height},
                    .left = (int)(width / 4),
                    .right = (int)(width / 4),
                    .top = (int)(width / 4),
                    .bottom = (int)(width / 4),
                    .layout = NPATCH_NINE_PATCH};
            }

            if (titlebarTex.id != 0)
            {
                theme->windowTitlebarTexture = ZuiCreateTexture(titlebarTex, g_zui_ctx->defaultTheme.defaultTextureStyle);

                float titleWidth = (float)titlebarTex.width;
                float titleHeight = (float)titlebarTex.height;
                theme->windowTitlebarNPatch = (NPatchInfo){
                    .source = (Rectangle){0, 0, titleWidth, titleHeight},
                    .left = 32,
                    .right = 32,
                    .top = 0,
                    .bottom = 0,
                    .layout = NPATCH_THREE_PATCH_HORIZONTAL};
            }

            if (closeTex.id != 0)
            {
                theme->windowCloseButtonTexture = ZuiCreateTexture(closeTex, g_zui_ctx->defaultTheme.defaultTextureStyle);
            }
        }
        else
        {
            // High DPI
            frameTex = LoadTexture("src/resources/frame.png");
            titlebarTex = LoadTexture("src/resources/titlebar_x2.png");
            closeTex = LoadTexture("src/resources/close_button_x2.png");
            theme->windowTitleHeight = (float)titlebarTex.height;

            if (frameTex.id != 0)
            {
                theme->windowFrameTexture = ZuiCreateTexture(frameTex, g_zui_ctx->defaultTheme.defaultTextureStyle);

                float width = (float)frameTex.width;
                float height = (float)frameTex.height;
                theme->windowFrameNPatch = (NPatchInfo){
                    .source = (Rectangle){0, 0, width, height},
                    .left = (int)(width / 4),
                    .right = (int)(width / 4),
                    .top = (int)(width / 4),
                    .bottom = (int)(width / 4),
                    .layout = NPATCH_NINE_PATCH};
            }

            if (titlebarTex.id != 0)
            {
                theme->windowTitlebarTexture = ZuiCreateTexture(titlebarTex, g_zui_ctx->defaultTheme.defaultTextureStyle);

                float titleWidth = (float)titlebarTex.width;
                float titleHeight = (float)titlebarTex.height - 28.0F; // Trim 28px from bottom
                theme->windowTitlebarNPatch = (NPatchInfo){
                    .source = (Rectangle){0, 0, titleWidth, titleHeight},
                    .left = 32,
                    .right = 32,
                    .top = 0,
                    .bottom = 0,
                    .layout = NPATCH_THREE_PATCH_HORIZONTAL};
            }

            if (closeTex.id != 0)
            {
                theme->windowCloseButtonTexture = ZuiCreateTexture(closeTex, g_zui_ctx->defaultTheme.defaultTextureStyle);
            }
        }

        theme->font = LoadFontEx("src/resources/Inter_18pt-Regular.ttf",
                                 ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);
        if (theme->font.texture.id == 0)
        {
            theme->font = GetFontDefault();
        }

        theme->monoFont = LoadFontEx("src/resources/Inconsolata-Regular.ttf",
                                     ZUI_BASE_FONT_SIZE * g_zui_ctx->dpiScale, 0, 0);
        if (theme->monoFont.texture.id == 0)
        {
            theme->monoFont = GetFontDefault();
        }

        if (theme->windowTitleHeight == 0)
        {
            theme->windowTitleHeight = 25.0F;
        }

        theme->defaultLabelStyle = (ZuiLabelStyle){
            .font = theme->font,
            .textColor = BLACK,
            .textBgColor = BLANK,
            .fontSize = ZUI_BASE_FONT_SIZE,
            .fontSpacing = ZUI_FONT_SPACING,
        };

        theme->defaultTextureStyle = (ZuiTextureStyle){
            .tintColor = WHITE,
        };

        theme->defaultFrameStyle = (ZuiFrameStyle){
            .bgColor = BLANK,
            .borderColor = BLANK,
            .scrollbarColor = (Color){100, 100, 100, 200},
            .cornerRadius = ZUI_CORNER_RADIUS,
            .roudnesSegments = ZUI_ROUNDNESS_SEGMENTS,
            .borderThickness = 2.0F,
            .hasBorder = false,
        };

        theme->defaultButtonStyle = (ZuiButtonStyle){
            .gap = 8.0F,
            .pad = (Vector2){8.0F, 8.0F},
            .size = (Vector2){24.0F, 24.0F},
            .minSize = (Vector2){24.0F, 24.0F},
            .colors = (ZuiColorSet){
                .active = (Color){220, 220, 225, 255},
                .normal = (Color){220, 220, 225, 255},
                .hovered = (Color){230, 230, 235, 255},
                .pressed = (Color){200, 200, 205, 255},
                .selected = (Color){100, 150, 220, 255},
                .selectedHovered = (Color){120, 170, 235, 255},
                .selectedPressed = (Color){80, 130, 200, 255},
                .focused = (Color){80, 150, 255, 200},
                .disabled = (Color){120, 120, 120, 255},
            },
            .textures = (ZuiTextureSet){
                .normal = ZUI_ID_INVALID,
                .hovered = ZUI_ID_INVALID,
                .pressed = ZUI_ID_INVALID,
                .selected = ZUI_ID_INVALID,
                .disabled = ZUI_ID_INVALID,
                .npatch = (NPatchInfo){0},
            },
            .frameStyle = (ZuiFrameStyle){
                .borderColor = (Color){160, 160, 165, 255},
                .cornerRadius = 4.0F,
                .roudnesSegments = ZUI_ROUNDNESS_SEGMENTS,
                .borderThickness = 2.0F,
                .hasBorder = true,
            },
            .labelStyle = theme->defaultLabelStyle,
        };
        theme->defaultButtonStyle.labelStyle.textColor = WHITE;

        theme->initialized = true;

        TraceLog(LOG_INFO, "ZUI: Theme initialized (DPI scale: %d)", dpiScale);
        return ZUI_OK;
    }

    void ZuiUnloadTheme(ZuiTheme *theme)
    {
        if (!theme || !theme->initialized)
        {
            return;
        }

        // Textures are managed by texture system, just clear IDs
        theme->windowFrameTexture = ZUI_ID_INVALID;
        theme->windowTitlebarTexture = ZUI_ID_INVALID;
        theme->windowCloseButtonTexture = ZUI_ID_INVALID;
        theme->buttonNormalTexture = ZUI_ID_INVALID;
        theme->buttonHoverTexture = ZUI_ID_INVALID;
        theme->buttonPressedTexture = ZUI_ID_INVALID;
        theme->initialized = false;
        TraceLog(LOG_INFO, "ZUI: Theme unloaded");
    }

    Font ZuiGetFont(bool isMono)
    {
        Font font = GetFontDefault();
        if (g_zui_ctx->defaultTheme.initialized)
        {

            font = isMono ? g_zui_ctx->defaultTheme.monoFont : g_zui_ctx->defaultTheme.font;
        }
        return font;
    }
    // -----------------------------------------------------------------------------
    // zui_context.c

    ZuiContext *g_zui_ctx = NULL;
    ZuiArena g_zui_arena = {0};

    static void ZuiAnimUpdateLinear(ZuiAnimation *anim, float deltaTime)
    {
        if (anim->duration <= 0.0F)
        {
            anim->value = anim->target;
            return;
        }

        float delta = anim->target - anim->value;

        if (fabsf(delta) < 0.01F)
        {
            anim->value = anim->target;
        }
        else
        {
            anim->value += (deltaTime / anim->duration) * delta;
        }
    }

    static void ZuiAnimUpdateEaseOut(ZuiAnimation *anim, float deltaTime)
    {
        if (anim->duration <= 0.0F)
        {
            anim->value = anim->target;
            return;
        }

        float delta = anim->target - anim->value;
        float t = 1.0F - powf(0.001F, deltaTime / anim->duration);
        anim->value += delta * t;
    }

    static void ZuiUpdateAnimation(ZuiAnimation *anim, float deltaTime)
    {
        if (fabsf(anim->target - anim->value) < 0.01F)
        {
            anim->value = anim->target;
            anim->active = false;
            return;
        }

        if (anim->type == ZUI_ANIM_LINEAR)
        {
            ZuiAnimUpdateLinear(anim, deltaTime);
        }
        else
        {
            ZuiAnimUpdateEaseOut(anim, deltaTime);
        }
    }

    void ZuiUpdateRestPosition(void)
    {
        g_zui_ctx->cursor.restPosition = g_zui_ctx->cursor.position;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
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

        ctx->g_focusContext = (ZuiFocusContext){
            .currentFocus = UINT_MAX,
            .rootFocus = UINT_MAX,
        };

        ctx->g_menuContext = (ZuiMenuContext){
            .activeMenuBar = ZUI_ID_INVALID,
            .activeMenu = ZUI_ID_INVALID,
            .hoveredItem = ZUI_ID_INVALID,
            .lastMenuItem = ZUI_ID_INVALID,
        };

        // Initialize component registry first
        result = ZuiInitComponentRegistry();
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Component registry init failed");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        ZuiRegisterAllComponents();
        ZuiRegisterAllTypes();
        // items
        result = ZuiInitDynArray(&ctx->items, &g_zui_arena, ZUI_ITEMS_CAPACITY,
                                 sizeof(ZuiItem), ZUI_ALIGNOF(ZuiItem), "Items");
        if (result != ZUI_OK)
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to initialize items array");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        g_zui_ctx->cursor = (ZuiCursor){0};
        g_zui_ctx->windowManager = (ZuiWindowManager){0};

        ZuiFrameStyle frameStyle = g_zui_ctx->defaultTheme.defaultFrameStyle;
        uint32_t root_item =
            ZuiCreateFrame((Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, frameStyle);

        if (root_item == ZUI_ID_INVALID || !ZuiItemHasComponent(root_item, ZUI_COMPONENT_TRANSFORM))
        {
            TraceLog(LOG_ERROR, "ZUI: Failed to create root frame");
            ZuiUnloadArena(&g_zui_arena);
            g_zui_ctx = NULL;
            return false;
        }

        ZuiItemSetLayout(root_item, ZUI_LAYOUT_VERTICAL);
        ZuiLayoutSetSpacing(root_item, 0);
        ZuiLayoutSetPadding(root_item, 0);
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

        result = ZuiInitTheme(&g_zui_ctx->defaultTheme, g_zui_ctx->dpiScale);
        if (result != ZUI_OK)
        {
            TraceLog(LOG_WARNING, "ZUI: Failed to initialize theme, using fallback colors");
        }

        TraceLog(LOG_INFO, "ZUI: Initialized successfully");
        return true;
    }

    void ZuiUpdateComponents(void)
    {
        ZUI_ASSERT_CTX_RETURN();
        float deltaTime = GetFrameTime();
        const Vector2 mousePos = GetMousePosition();
        const bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        const bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        const bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        uint32_t topItemId = ZUI_ID_INVALID;
        if (mousePressed || mouseDown || mouseReleased)
        {
            int highestZ = INT_MIN;
            for (uint32_t i = 0; i < g_zui_ctx->items.count; i++)
            {
                const ZuiItem *item = (const ZuiItem *)ZuiGetDynArray(&g_zui_ctx->items, i);
                if (!item || !(item->componentMask & (1U << (uint32_t)ZUI_COMPONENT_INTERACTION)))
                {
                    continue;
                }

                const ZuiStateData *state = (const ZuiStateData *)ZuiItemGetComponent(i, ZUI_COMPONENT_STATE);
                if (state && !state->isVisible)
                {
                    continue;
                }

                Rectangle bounds = ZuiGetTransformBounds(i);
                if (!CheckCollisionPointRec(mousePos, bounds))
                {
                    continue;
                }

                const ZuiLayerData *layer = (const ZuiLayerData *)ZuiItemGetComponent(i, ZUI_COMPONENT_LAYER);
                int z = layer ? ((layer->layer * 1000000) + layer->order) : ZUI_LAYER_CONTENT;

                if (z > highestZ || (z == highestZ && i > topItemId))
                {
                    highestZ = z;
                    topItemId = i;
                }
            }
        }

        for (uint32_t i = 0; i < g_zui_ctx->items.count; i++)
        {
            const ZuiItem *item = (const ZuiItem *)ZuiGetDynArray(&g_zui_ctx->items, i);
            if (!(item->componentMask & (1U << (uint32_t)ZUI_COMPONENT_INTERACTION)))
            {
                continue;
            }

            ZuiInteractionData *interaction = (ZuiInteractionData *)ZuiItemGetComponent(i, ZUI_COMPONENT_INTERACTION);
            if (!interaction)
            {
                continue;
            }

            const ZuiStateData *state = (const ZuiStateData *)ZuiItemGetComponent(i, ZUI_COMPONENT_STATE);
            if (state && !state->isVisible)
            {
                continue;
            }

            interaction->wasHovered = interaction->isHovered;
            interaction->wasPressed = interaction->isPressed;
            Rectangle bounds = ZuiGetTransformBounds(i);
            bool hitTest = CheckCollisionPointRec(mousePos, bounds);
            bool allowInteraction = !mousePressed && !mouseDown && !mouseReleased;
            allowInteraction = allowInteraction || (i == topItemId) || !hitTest;

            if (!allowInteraction && interaction->blocksInput)
            {
                interaction->isHovered = false;
                interaction->isPressed = false;
                continue;
            }
            interaction->isHovered = hitTest && allowInteraction;
            interaction->mousePosition = mousePos;

            if (interaction->isHovered && !interaction->wasHovered)
            {
                interaction->hoverTime = 0.0F;
                ZuiAnimSetTarget(i, ZUI_ANIM_SLOT_HOVER, 1.0F);
                if (interaction->onHoverEnter)
                {
                    interaction->onHoverEnter(i);
                }
            }
            else if (!interaction->isHovered && interaction->wasHovered)
            {
                ZuiAnimSetTarget(i, ZUI_ANIM_SLOT_HOVER, 0.0F);
                if (interaction->onHoverExit)
                {
                    interaction->onHoverExit(i);
                }
            }

            if (interaction->isHovered)
            {
                interaction->hoverTime += deltaTime;
            }

            if (interaction->isHovered && mousePressed)
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
                if (mouseReleased)
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

        for (uint32_t i = 0; i < g_zui_ctx->items.count; i++)
        {
            const ZuiItem *item = (const ZuiItem *)ZuiGetDynArray(&g_zui_ctx->items, i);

            if (item->componentMask & (1U << (uint32_t)ZUI_COMPONENT_ANIMATION))
            {
                ZuiAnimationData *animData = (ZuiAnimationData *)ZuiItemGetComponent(i, ZUI_COMPONENT_ANIMATION);
                if (animData && animData->activeCount > 0)
                {
                    for (uint32_t j = 0; j < ZUI_ANIM_SLOT_COUNT; j++)
                    {
                        ZuiAnimation *anim = &animData->slots[j];
                        if (anim->active)
                        {
                            ZuiUpdateAnimation(anim, deltaTime);
                            if (!anim->active)
                            {
                                animData->activeCount--;
                            }
                        }
                    }
                }
            }

            if (item->componentMask & (1U << (uint32_t)ZUI_COMPONENT_SCROLL))
            {
                ZuiScrollData *scroll = (ZuiScrollData *)ZuiItemGetComponent(i, ZUI_COMPONENT_SCROLL);
                if (!scroll)
                {
                    continue;
                }

                Rectangle bounds = ZuiGetTransformBounds(i);
                const ZuiLayoutData *layout = (const ZuiLayoutData *)ZuiItemGetComponent(i, ZUI_COMPONENT_LAYOUT);
                float padding = layout ? layout->padding : 0.0F;
                scroll->viewport = (Rectangle){
                    bounds.x + padding,
                    bounds.y + padding,
                    bounds.width - (padding * 2.0F),
                    bounds.height - (padding * 2.0F)};
                scroll->contentSize = ZuiCalculateContentSize(i);

                bool inViewport = CheckCollisionPointRec(mousePos, scroll->viewport);

                if (!scroll->isDragging)
                {
                    if (scroll->verticalEnabled)
                    {
                        Rectangle bar = ZuiScrollGetBarRect(scroll, true);
                        if (bar.width > 0 && CheckCollisionPointRec(mousePos, bar) &&
                            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            scroll->isDragging = true;
                            scroll->dragMode = ZUI_SCROLL_DRAG_VERTICAL;
                            scroll->dragStart = mousePos;
                        }
                    }

                    if (scroll->horizontalEnabled)
                    {
                        Rectangle bar = ZuiScrollGetBarRect(scroll, false);
                        if (bar.width > 0 && CheckCollisionPointRec(mousePos, bar) &&
                            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            scroll->isDragging = true;
                            scroll->dragMode = ZUI_SCROLL_DRAG_HORIZONTAL;
                            scroll->dragStart = mousePos;
                        }
                    }
                }

                if (scroll->isDragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    Vector2 delta = {
                        mousePos.x - scroll->dragStart.x,
                        mousePos.y - scroll->dragStart.y};
                    Vector2 max = ZuiScrollGetMaxOffset(scroll);

                    if (scroll->dragMode == ZUI_SCROLL_DRAG_VERTICAL && max.y > 0.0F)
                    {
                        float ratio = scroll->viewport.height / scroll->contentSize.y;
                        float thumbHeight = fmaxf(20.0F, scroll->viewport.height * ratio);
                        float trackHeight = scroll->viewport.height - thumbHeight;
                        scroll->targetOffset.y += (delta.y / trackHeight) * max.y;
                    }

                    if (scroll->dragMode == ZUI_SCROLL_DRAG_HORIZONTAL && max.x > 0.0F)
                    {
                        float ratio = scroll->viewport.width / scroll->contentSize.x;
                        float thumbWidth = fmaxf(20.0F, scroll->viewport.width * ratio);
                        float trackWidth = scroll->viewport.width - thumbWidth;
                        scroll->targetOffset.x += (delta.x / trackWidth) * max.x;
                    }

                    scroll->targetOffset = ZuiScrollClampOffset(scroll, scroll->targetOffset);
                    scroll->dragStart = mousePos;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    scroll->isDragging = false;
                    scroll->dragMode = ZUI_SCROLL_DRAG_NONE;
                }

                if (inViewport && !scroll->isDragging)
                {
                    Vector2 wheelV = GetMouseWheelMoveV();
                    bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

                    float wheelAmountX = 0.0F;
                    float wheelAmountY = 0.0F;

                    if (scroll->verticalEnabled && !shiftDown)
                    {
                        wheelAmountY = wheelV.y * 20.0F;
                    }

                    if (scroll->horizontalEnabled)
                    {
                        if (shiftDown)
                        {
                            wheelAmountX = wheelV.y * 20.0F;
                        }
                        else if (wheelV.x != 0.0F)
                        {
                            wheelAmountX = wheelV.x * 20.0F;
                        }
                    }

                    if (wheelAmountX != 0.0F || wheelAmountY != 0.0F)
                    {
                        scroll->targetOffset.x -= wheelAmountX;
                        scroll->targetOffset.y -= wheelAmountY;
                        scroll->targetOffset = ZuiScrollClampOffset(scroll, scroll->targetOffset);
                    }
                }

                float lerpSpeed = 10.0F * deltaTime;
                scroll->offset.x = ZuiLerp(scroll->offset.x, scroll->targetOffset.x, lerpSpeed);
                scroll->offset.y = ZuiLerp(scroll->offset.y, scroll->targetOffset.y, lerpSpeed);
                scroll->offset = ZuiScrollClampOffset(scroll, scroll->offset);
            }
        }
    }

    void ZuiUpdate(void)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiUpdateComponents();
        const ZuiItem *root = ZuiGetItem(g_zui_ctx->cursor.rootItem);
        if (root)
        {
            ZuiUpdateItem(root);
        }
    }

    void ZuiRender(void)
    {
        ZUI_ASSERT_CTX_RETURN();
        const ZuiItem *root = ZuiGetItem(g_zui_ctx->cursor.rootItem);
        if (root)
        {
            ZuiRenderItem(root);
        }
    }

    void ZuiExit(void)
    {
        UnloadFont(g_zui_ctx->defaultTheme.font);
        UnloadFont(g_zui_ctx->defaultTheme.monoFont);
        ZuiUnloadTheme(&g_zui_ctx->defaultTheme);

        if (g_zui_arena.buffer != NULL)
        {
#ifdef ZUI_DEBUG
            ZuiArenaStats stats = ZuiGetArenaStats(&g_zui_arena);
            TraceLog(LOG_INFO, "ZUI: Arena: %.2F%% used (%zu / %zu bytes)",
                     (double)stats.usagePercent, stats.usedBytes, stats.totalCapacity);
#endif
            ZuiUnloadArena(&g_zui_arena);
        }

        g_zui_ctx = NULL;
    }

    void ZuiAdvanceCursor(const float width, const float height)
    {
        ZUI_ASSERT_CTX_RETURN();
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

    void ZuiAdvanceLine(void)
    {
        ZUI_ASSERT_CTX_RETURN();
        const float gap = ZuiGetFrameSpacing(g_zui_ctx->cursor.activeFrame);
        g_zui_ctx->cursor.position.x = g_zui_ctx->cursor.restPosition.x;
        g_zui_ctx->cursor.tempRestPosition.x = g_zui_ctx->cursor.position.x;
        g_zui_ctx->cursor.position.y += g_zui_ctx->cursor.lastItemBounds.height + gap;
        g_zui_ctx->cursor.tempRestPosition.y = g_zui_ctx->cursor.position.y;
    }

    void ZuiPlaceAt(const float x, const float y)
    {
        ZUI_ASSERT_CTX_RETURN();
        g_zui_ctx->cursor.position = (Vector2){x, y};
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }

    void ZuiOffset(const float x, const float y)
    {
        ZUI_ASSERT_CTX_RETURN();
        g_zui_ctx->cursor.position.x += x;
        g_zui_ctx->cursor.position.y += y;
        g_zui_ctx->cursor.tempRestPosition = g_zui_ctx->cursor.position;
    }

    void ZuiOffsetLast(float x, float y)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTransformData *transform = (ZuiTransformData *)ZuiItemGetComponent(g_zui_ctx->cursor.activeItem, ZUI_COMPONENT_TRANSFORM);
        if (transform)
        {
            transform->offset.x += x;
            transform->offset.y += y;
            g_zui_ctx->cursor.position.x += x;
            g_zui_ctx->cursor.position.y += y;
        }
    }

    void ZuiPaddingLast(float x, float y)
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        ZuiLayoutSetPadding(itemId, (x + y) / 2.0F);
    }

    void ZuiDisableLast(void)
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        ZuiSetEnabled(itemId, false);
    }
    // -----------------------------------------------------------------------------
    // zui_button.c

    void ZuiCenterWidgetInBounds(uint32_t itemId, Rectangle bounds)
    {
        Rectangle iconBounds = ZuiGetTransformBounds(itemId);
        iconBounds.x = bounds.x + ((bounds.width - iconBounds.width) * 0.5F);
        iconBounds.y = bounds.y + ((bounds.height - iconBounds.height) * 0.5F);
        ZuiSetTransformBounds(itemId, iconBounds);
    }

    uint32_t ZuiCreateButton(const char *text, Texture2D *icon, ZuiButtonStyle style)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();

        uint32_t itemId = ZuiCreateTypedItem(ZUI_BUTTON);
        if (itemId == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
        if (!buttonData)
        {
            return ZUI_ID_INVALID;
        }

        buttonData->itemId = itemId;
        buttonData->labelId = ZUI_ID_INVALID;
        buttonData->iconId = ZUI_ID_INVALID;
        buttonData->mode = ZUI_BUTTON_PUSH;
        buttonData->style = style;

        Vector2 contentSize = {0, 0};

        // Measure icon if provided
        if (icon != NULL && icon->id != 0)
        {
            contentSize.x = (float)icon->width;
            contentSize.y = (float)icon->height;
        }

        // Measure text
        if (text != NULL && text[0] != '\0')
        {
            Vector2 textSize = MeasureTextEx(buttonData->style.labelStyle.font,
                                             text, buttonData->style.labelStyle.fontSize, buttonData->style.labelStyle.fontSpacing);

            if (contentSize.x > 0)
            {
                contentSize.x += buttonData->style.gap + textSize.x;
                contentSize.y = fmaxf(contentSize.y, textSize.y);
            }
            else
            {
                contentSize = textSize;
            }
        }

        Vector2 buttonSize;
        if (buttonData->style.size.x > 0 && buttonData->style.size.y > 0)
        {
            buttonSize = buttonData->style.size;
        }
        else if (contentSize.x > 0 && contentSize.y > 0)
        {
            buttonSize.x = contentSize.x + (buttonData->style.pad.x * 2.0F);
            buttonSize.y = contentSize.y + (buttonData->style.pad.y * 2.0F);
        }
        else
        {
            buttonSize = buttonData->style.minSize;
        }

        buttonSize.x = fmaxf(buttonSize.x, buttonData->style.minSize.x);
        buttonSize.y = fmaxf(buttonSize.y, buttonData->style.minSize.y);
        Rectangle bounds = {0, 0, buttonSize.x, buttonSize.y};

        if (!ZuiSetupAsContainer(itemId))
        {
            return ZUI_ID_INVALID;
        }

        if (!ZuiAddComponentsEx(itemId, bounds, ZUI_COMP_INTERACTIVE))
        {
            return ZUI_ID_INVALID;
        }

        ZuiAnimSetType(itemId, ZUI_ANIM_SLOT_HOVER, ZUI_ANIM_EASE_OUT);
        ZuiAnimSetDuration(itemId, ZUI_ANIM_SLOT_HOVER, 0.15F);
        ZuiAnimReset(itemId, ZUI_ANIM_SLOT_HOVER, 0.0F);

        ZuiAnimSetType(itemId, ZUI_ANIM_SLOT_SCALE, ZUI_ANIM_EASE_OUT);
        ZuiAnimSetDuration(itemId, ZUI_ANIM_SLOT_SCALE, 0.1F);
        ZuiAnimReset(itemId, ZUI_ANIM_SLOT_SCALE, 1.0F);

        Rectangle buttonBounds = (Rectangle){
            buttonData->style.pad.x,
            buttonData->style.pad.y,
            bounds.width - (buttonData->style.pad.x * 2.0F),
            bounds.height - (buttonData->style.pad.y * 2.0F)};

        if (icon != NULL && icon->id != 0)
        {
            ZuiTextureStyle iconStyle = g_zui_ctx->defaultTheme.defaultTextureStyle;
            iconStyle.npatch = (NPatchInfo){
                .source = (Rectangle){0, 0, (float)icon->width, (float)icon->height},
                .layout = NPATCH_NINE_PATCH};
            buttonData->iconId = ZuiCreateTexture(*icon, iconStyle);
            if (buttonData->iconId != ZUI_ID_INVALID)
            {
                ZuiItemAddChild(itemId, buttonData->iconId);

                ZuiCenterWidgetInBounds(buttonData->iconId, buttonBounds);
            }
        }

        if (text != NULL && text[0] != '\0')
        {
            buttonData->labelId = ZuiCreateLabel(text, buttonData->style.labelStyle);
            if (buttonData->labelId != ZUI_ID_INVALID)
            {
                ZuiItemAddChild(itemId, buttonData->labelId);
                ZuiCenterWidgetInBounds(buttonData->labelId, buttonBounds);
            }
        }

        g_zui_ctx->cursor.activeItem = itemId;
        return itemId;
    }

    void ZuiUpdateButton(uint32_t dataIndex)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_BUTTON];
        ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!buttonData || buttonData->itemId == ZUI_ID_INVALID)
        {
            return;
        }

        uint32_t itemId = buttonData->itemId;

        const ZuiInteractionData *interaction = (const ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        if (!interaction)
        {
            return;
        }

        // Determine Keyboard State
        bool keyHeld = IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE);
        bool keyPressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);

        // Determine Visual "Pressed" State (Mouse OR Keyboard)
        bool effectivePressed = interaction->isPressed || (interaction->isFocused && keyHeld);

        // --- Determine Colors based on State ---
        if (buttonData->isToggled)
        {
            if (effectivePressed)
            {
                buttonData->style.colors.active = buttonData->style.colors.selectedPressed;
            }
            else if (interaction->isHovered || interaction->isFocused)
            {
                buttonData->style.colors.active = buttonData->style.colors.selectedHovered;
            }
            else
            {
                buttonData->style.colors.active = buttonData->style.colors.selected;
            }
        }
        else
        {
            if (effectivePressed)
            {
                buttonData->style.colors.active = buttonData->style.colors.pressed;
            }
            else if (interaction->isHovered || interaction->isFocused)
            {
                buttonData->style.colors.active = buttonData->style.colors.hovered;
            }
            else
            {
                buttonData->style.colors.active = buttonData->style.colors.normal;
            }
        }

        // --- Update Animations ---
        // Press/Scale animation
        if (effectivePressed)
        {
            ZuiAnimSetTarget(itemId, ZUI_ANIM_SLOT_PRESS, 1.0F);
            ZuiAnimSetTarget(itemId, ZUI_ANIM_SLOT_SCALE, 0.95F);
        }
        else
        {
            ZuiAnimSetTarget(itemId, ZUI_ANIM_SLOT_PRESS, 0.0F);
            ZuiAnimSetTarget(itemId, ZUI_ANIM_SLOT_SCALE, 1.0F);
        }

        // Hover animation
        if (interaction->isHovered || interaction->isFocused)
        {
            ZuiAnimSetTarget(itemId, ZUI_ANIM_SLOT_HOVER, 1.0F);
        }
        else
        {
            ZuiAnimSetTarget(itemId, ZUI_ANIM_SLOT_HOVER, 0.0F);
        }

        // --- Handle Logic Changes (Toggle/Radio) on Keyboard Press ---
        // We handle the state change immediately to ensure visual sync
        if (interaction->isFocused && keyPressed)
        {
            switch (buttonData->mode)
            {
            case ZUI_BUTTON_TOGGLE:
                buttonData->isToggled = !buttonData->isToggled;
                if (buttonData->onToggle)
                {
                    buttonData->onToggle(itemId, buttonData->isToggled);
                }
                break;

            case ZUI_BUTTON_RADIO:
                if (!buttonData->isToggled)
                {
                    // Deselect others in group
                    for (uint32_t i = 0; i < reg->dataArray.count; i++)
                    {
                        ZuiButtonData *otherButton = (ZuiButtonData *)ZuiGetDynArray(&reg->dataArray, i);
                        if (otherButton &&
                            otherButton->mode == ZUI_BUTTON_RADIO &&
                            otherButton->groupId == buttonData->groupId &&
                            otherButton->itemId != itemId)
                        {
                            otherButton->isToggled = false;
                        }
                    }
                    buttonData->isToggled = true;
                    if (buttonData->onToggle)
                    {
                        buttonData->onToggle(itemId, true);
                    }
                }
                break;

            case ZUI_BUTTON_PUSH:
                // Logic handled by OnActivate callback, visuals handled above
                break;
            }
        }

        // --- Handle Mouse Logic (Preserved for mouse users) ---
        if (interaction->isPressed && !interaction->wasPressed)
        {
            if (buttonData->onPress)
            {
                buttonData->onPress(itemId);
            }
        }

        if (!interaction->isPressed && interaction->wasPressed)
        {
            if (buttonData->onRelease)
            {
                buttonData->onRelease(itemId);
            }

            if (interaction->isHovered)
            {
                switch (buttonData->mode)
                {
                case ZUI_BUTTON_PUSH:
                    if (buttonData->onClick)
                    {
                        buttonData->onClick(itemId);
                    }
                    break;

                case ZUI_BUTTON_TOGGLE:
                    // Mouse toggle (ensure not triggered by keyboard event in same frame)
                    buttonData->isToggled = !buttonData->isToggled;
                    if (buttonData->onToggle)
                    {
                        buttonData->onToggle(itemId, buttonData->isToggled);
                    }
                    if (buttonData->onClick)
                    {
                        buttonData->onClick(itemId);
                    }
                    break;

                case ZUI_BUTTON_RADIO:
                    if (!buttonData->isToggled)
                    {
                        for (uint32_t i = 0; i < reg->dataArray.count; i++)
                        {
                            ZuiButtonData *otherButton = (ZuiButtonData *)ZuiGetDynArray(&reg->dataArray, i);
                            if (otherButton &&
                                otherButton->mode == ZUI_BUTTON_RADIO &&
                                otherButton->groupId == buttonData->groupId &&
                                otherButton->itemId != itemId)
                            {
                                otherButton->isToggled = false;
                            }
                        }
                        buttonData->isToggled = true;
                        if (buttonData->onToggle)
                        {
                            buttonData->onToggle(itemId, true);
                        }
                        if (buttonData->onClick)
                        {
                            buttonData->onClick(itemId);
                        }
                    }
                    break;
                }
            }
        }
    }

    void ZuiRenderButton(uint32_t dataIndex)
    {
        ZUI_ASSERT_CTX_RETURN();
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_BUTTON];
        const ZuiButtonData *buttonData = (const ZuiButtonData *)ZuiGetDynArray(&reg->dataArray, dataIndex);
        if (!buttonData || buttonData->itemId == ZUI_ID_INVALID)
        {
            return;
        }

        uint32_t itemId = buttonData->itemId;

        const ZuiStateData *state = (const ZuiStateData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_STATE);
        if (state && !state->isVisible)
        {
            return;
        }

        Color bgColor = buttonData->style.colors.active;
        float alpha = ZuiAnimGetValue(itemId, ZUI_ANIM_SLOT_ALPHA);
        bgColor.a = (unsigned char)((float)bgColor.a * alpha);

        Rectangle bounds = ZuiGetTransformBounds(itemId);

        uint32_t label = buttonData->labelId;
        if (label != ZUI_ID_INVALID)
        {
            ZuiCenterWidgetInBounds(label, bounds);
        }

        uint32_t icon = buttonData->iconId;
        if (icon != ZUI_ID_INVALID)
        {
            ZuiCenterWidgetInBounds(icon, bounds);
        }

        float roundness = ZuiPixelsToRoundness(bounds, buttonData->style.frameStyle.cornerRadius);
        if (bgColor.a > 0)
        {
            DrawRectangleRounded(bounds, roundness, buttonData->style.frameStyle.roudnesSegments, bgColor);
        }

        if (buttonData->style.frameStyle.hasBorder && buttonData->style.frameStyle.borderThickness > 0)
        {
            Color borderColor = buttonData->style.frameStyle.borderColor;
            borderColor.a = (unsigned char)((float)borderColor.a * alpha);
            DrawRectangleRoundedLinesEx(bounds, roundness, buttonData->style.frameStyle.roudnesSegments,
                                        buttonData->style.frameStyle.borderThickness, borderColor);
        }

        const ZuiItem *buttonItem = ZuiGetItem(itemId);
        if (buttonItem && buttonItem->isContainer)
        {
            for (uint32_t i = 0; i < buttonItem->children.count; i++)
            {
                uint32_t childId = ((uint32_t *)buttonItem->children.items)[i];
                const ZuiItem *child = ZuiGetItem(childId);
                if (child)
                {
                    ZuiRenderItem(child);
                }
            }
        }

        const ZuiFocusData *focus = (const ZuiFocusData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_FOCUS);
        bool isDisabled = state && !state->isEnabled;

        if (focus && focus->isFocused && !isDisabled)
        {
            Rectangle focusRect = {
                bounds.x - 3.0F,
                bounds.y - 3.0F,
                bounds.width + 6.0F,
                bounds.height + 6.0F};

            DrawRectangleRoundedLines(focusRect, roundness, buttonData->style.frameStyle.roudnesSegments,
                                      buttonData->style.colors.focused);
        }
    }

    uint32_t ZuiNewButtonEx(const char *text, Texture2D *icon)
    {
        ZUI_ASSERT_CTX_RETURN_ID_INVALID();

        ZuiButtonStyle buttonStyle = g_zui_ctx->defaultTheme.defaultButtonStyle;
        buttonStyle.size = (Vector2){0, 0};
        uint32_t id = ZuiCreateButton(text, icon, buttonStyle);
        if (id == ZUI_ID_INVALID)
        {
            return ZUI_ID_INVALID;
        }

        // Get the button data to access its children
        ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(id);
        if (!buttonData)
        {
            return ZUI_ID_INVALID;
        }

        // Calculate movement from origin to cursor position
        Vector2 delta = g_zui_ctx->cursor.position;

        // Move the button itself to cursor position
        Rectangle buttonBounds = ZuiGetTransformBounds(id);
        buttonBounds.x = delta.x;
        buttonBounds.y = delta.y;
        ZuiSetTransformBounds(id, buttonBounds);

        if (buttonData->iconId != ZUI_ID_INVALID)
        {
            ZuiSetTransformBoundsPosition(buttonData->iconId, delta);
        }

        if (buttonData->labelId != ZUI_ID_INVALID)
        {
            ZuiSetTransformBoundsPosition(buttonData->labelId, delta);
        }

        // Add to frame and advance cursor
        ZuiItemAddChild(g_zui_ctx->cursor.activeFrame, id);
        ZuiAdvanceCursor(buttonBounds.width, buttonBounds.height);
        g_zui_ctx->cursor.lastItemBounds = buttonBounds;

        return id;
    }

    uint32_t ZuiNewButton(const char *text)
    {
        return ZuiNewButtonEx(text, NULL);
    }

    uint32_t ZuiNewButtonIcon(Texture2D icon)
    {
        return ZuiNewButtonEx(NULL, &icon);
    }

    void ZuiButtonOnClick(void (*onClick)(uint32_t))
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        if (itemId != ZUI_ID_INVALID)
        {
            ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
            if (buttonData)
            {
                buttonData->onClick = onClick;
            }
        }
    }

    void ZuiButtonOnPress(void (*onPress)(uint32_t))
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        if (itemId != ZUI_ID_INVALID)
        {
            ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
            if (buttonData)
            {
                buttonData->onPress = onPress;
            }
        }
    }

    void ZuiButtonOnRelease(void (*onRelease)(uint32_t))
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        if (itemId != ZUI_ID_INVALID)
        {
            ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
            if (buttonData)
            {
                buttonData->onRelease = onRelease;
            }
        }
    }

    void ZuiButtonOnToggle(void (*onToggle)(uint32_t, bool))
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        if (itemId != ZUI_ID_INVALID)
        {
            ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
            if (buttonData)
            {
                buttonData->onToggle = onToggle;
            }
        }
    }

    void ZuiButtonTextColor(Color color)
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        if (itemId != ZUI_ID_INVALID)
        {
            ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
            if (buttonData)
            {
                buttonData->style.labelStyle.textColor = color;
            }
        }
    }

    bool ZuiIsButtonPressed(uint32_t itemId)
    {
        const ZuiInteractionData *interaction = (const ZuiInteractionData *)ZuiItemGetComponent(itemId, ZUI_COMPONENT_INTERACTION);
        return interaction ? interaction->isPressed : false;
    }

    void ZuiButtonSetMode(ZuiButtonMode mode)
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        if (itemId != ZUI_ID_INVALID)
        {
            ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
            if (buttonData)
            {
                buttonData->mode = mode;
                if (mode == ZUI_BUTTON_RADIO)
                {
                    ZuiEnableFocus(buttonData->itemId);
                }
            }
        }
    }

    void ZuiButtonSetGroup(uint32_t groupId)
    {
        ZUI_ASSERT_CTX_RETURN();
        uint32_t itemId = g_zui_ctx->cursor.activeItem;
        if (itemId != ZUI_ID_INVALID)
        {
            ZuiButtonData *buttonData = (ZuiButtonData *)ZuiGetTypedData(itemId);
            if (buttonData)
            {
                buttonData->groupId = groupId;
            }
        }
    }

    bool ZuiIsButtonToggled(uint32_t itemId)
    {
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_BUTTON];
        for (uint32_t i = 0; i < reg->dataArray.count; i++)
        {
            const ZuiButtonData *button = (const ZuiButtonData *)ZuiGetDynArray(&reg->dataArray, i);
            if (button && button->itemId == itemId)
            {
                return button->isToggled;
            }
        }
        return false;
    }

    void ZuiButtonSetToggled(uint32_t itemId, bool toggled)
    {
        ZuiTypeRegistration *reg = &g_zui_ctx->typeRegistry.registrations[ZUI_BUTTON];
        for (uint32_t i = 0; i < reg->dataArray.count; i++)
        {
            ZuiButtonData *button = (ZuiButtonData *)ZuiGetDynArray(&reg->dataArray, i);
            if (button && button->itemId == itemId)
            {
                button->isToggled = toggled;
                return;
            }
        }
    }
    // -----------------------------------------------------------------------------

#endif // ZUI_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // ZUI_H
