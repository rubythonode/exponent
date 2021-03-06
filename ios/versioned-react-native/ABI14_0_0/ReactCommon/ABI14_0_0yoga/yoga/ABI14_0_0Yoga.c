/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <string.h>

#include "ABI14_0_0YGNodeList.h"
#include "ABI14_0_0Yoga.h"

#ifdef _MSC_VER
#include <float.h>
#ifndef isnan
#define isnan _isnan
#endif

#ifndef __cplusplus
#define inline __inline
#endif

/* define fmaxf if < VC12 */
#if _MSC_VER < 1800
__forceinline const float fmaxf(const float a, const float b) {
  return (a > b) ? a : b;
}
#endif
#endif

typedef struct ABI14_0_0YGCachedMeasurement {
  float availableWidth;
  float availableHeight;
  ABI14_0_0YGMeasureMode widthMeasureMode;
  ABI14_0_0YGMeasureMode heightMeasureMode;

  float computedWidth;
  float computedHeight;
} ABI14_0_0YGCachedMeasurement;

// This value was chosen based on empiracle data. Even the most complicated
// layouts should not require more than 16 entries to fit within the cache.
#define ABI14_0_0YG_MAX_CACHED_RESULT_COUNT 16

typedef struct ABI14_0_0YGLayout {
  float position[4];
  float dimensions[2];
  ABI14_0_0YGDirection direction;

  uint32_t computedFlexBasisGeneration;
  float computedFlexBasis;

  // Instead of recomputing the entire layout every single time, we
  // cache some information to break early when nothing changed
  uint32_t generationCount;
  ABI14_0_0YGDirection lastParentDirection;

  uint32_t nextCachedMeasurementsIndex;
  ABI14_0_0YGCachedMeasurement cachedMeasurements[ABI14_0_0YG_MAX_CACHED_RESULT_COUNT];
  float measuredDimensions[2];

  ABI14_0_0YGCachedMeasurement cachedLayout;
} ABI14_0_0YGLayout;

typedef struct ABI14_0_0YGStyle {
  ABI14_0_0YGDirection direction;
  ABI14_0_0YGFlexDirection flexDirection;
  ABI14_0_0YGJustify justifyContent;
  ABI14_0_0YGAlign alignContent;
  ABI14_0_0YGAlign alignItems;
  ABI14_0_0YGAlign alignSelf;
  ABI14_0_0YGPositionType positionType;
  ABI14_0_0YGWrap flexWrap;
  ABI14_0_0YGOverflow overflow;
  float flex;
  float flexGrow;
  float flexShrink;
  ABI14_0_0YGValue flexBasis;
  ABI14_0_0YGValue margin[ABI14_0_0YGEdgeCount];
  ABI14_0_0YGValue position[ABI14_0_0YGEdgeCount];
  ABI14_0_0YGValue padding[ABI14_0_0YGEdgeCount];
  ABI14_0_0YGValue border[ABI14_0_0YGEdgeCount];
  ABI14_0_0YGValue dimensions[2];
  ABI14_0_0YGValue minDimensions[2];
  ABI14_0_0YGValue maxDimensions[2];

  // Yoga specific properties, not compatible with flexbox specification
  float aspectRatio;
} ABI14_0_0YGStyle;

typedef struct ABI14_0_0YGNode {
  ABI14_0_0YGStyle style;
  ABI14_0_0YGLayout layout;
  uint32_t lineIndex;
  bool hasNewLayout;
  ABI14_0_0YGNodeRef parent;
  ABI14_0_0YGNodeListRef children;
  bool isDirty;

  struct ABI14_0_0YGNode *nextChild;

  ABI14_0_0YGMeasureFunc measure;
  ABI14_0_0YGPrintFunc print;
  void *context;
} ABI14_0_0YGNode;

#define ABI14_0_0YG_UNDEFINED_VALUES \
  { .value = ABI14_0_0YGUndefined, .unit = ABI14_0_0YGUnitUndefined }

#define ABI14_0_0YG_DEFAULT_EDGE_VALUES_UNIT                                                   \
  {                                                                                   \
    [ABI14_0_0YGEdgeLeft] = ABI14_0_0YG_UNDEFINED_VALUES, [ABI14_0_0YGEdgeTop] = ABI14_0_0YG_UNDEFINED_VALUES,            \
    [ABI14_0_0YGEdgeRight] = ABI14_0_0YG_UNDEFINED_VALUES, [ABI14_0_0YGEdgeBottom] = ABI14_0_0YG_UNDEFINED_VALUES,        \
    [ABI14_0_0YGEdgeStart] = ABI14_0_0YG_UNDEFINED_VALUES, [ABI14_0_0YGEdgeEnd] = ABI14_0_0YG_UNDEFINED_VALUES,           \
    [ABI14_0_0YGEdgeHorizontal] = ABI14_0_0YG_UNDEFINED_VALUES, [ABI14_0_0YGEdgeVertical] = ABI14_0_0YG_UNDEFINED_VALUES, \
    [ABI14_0_0YGEdgeAll] = ABI14_0_0YG_UNDEFINED_VALUES,                                                \
  }

#define ABI14_0_0YG_DEFAULT_DIMENSION_VALUES \
  { [ABI14_0_0YGDimensionWidth] = ABI14_0_0YGUndefined, [ABI14_0_0YGDimensionHeight] = ABI14_0_0YGUndefined, }

#define ABI14_0_0YG_DEFAULT_DIMENSION_VALUES_UNIT \
  { [ABI14_0_0YGDimensionWidth] = ABI14_0_0YG_UNDEFINED_VALUES, [ABI14_0_0YGDimensionHeight] = ABI14_0_0YG_UNDEFINED_VALUES, }

static ABI14_0_0YGNode gABI14_0_0YGNodeDefaults = {
    .parent = NULL,
    .children = NULL,
    .hasNewLayout = true,
    .isDirty = false,

    .style =
        {
            .flex = ABI14_0_0YGUndefined,
            .flexGrow = ABI14_0_0YGUndefined,
            .flexShrink = ABI14_0_0YGUndefined,
            .flexBasis = ABI14_0_0YG_UNDEFINED_VALUES,
            .justifyContent = ABI14_0_0YGJustifyFlexStart,
            .alignItems = ABI14_0_0YGAlignStretch,
            .alignContent = ABI14_0_0YGAlignFlexStart,
            .direction = ABI14_0_0YGDirectionInherit,
            .flexDirection = ABI14_0_0YGFlexDirectionColumn,
            .overflow = ABI14_0_0YGOverflowVisible,
            .dimensions = ABI14_0_0YG_DEFAULT_DIMENSION_VALUES_UNIT,
            .minDimensions = ABI14_0_0YG_DEFAULT_DIMENSION_VALUES_UNIT,
            .maxDimensions = ABI14_0_0YG_DEFAULT_DIMENSION_VALUES_UNIT,
            .position = ABI14_0_0YG_DEFAULT_EDGE_VALUES_UNIT,
            .margin = ABI14_0_0YG_DEFAULT_EDGE_VALUES_UNIT,
            .padding = ABI14_0_0YG_DEFAULT_EDGE_VALUES_UNIT,
            .border = ABI14_0_0YG_DEFAULT_EDGE_VALUES_UNIT,
            .aspectRatio = ABI14_0_0YGUndefined,
        },

    .layout =
        {
            .dimensions = ABI14_0_0YG_DEFAULT_DIMENSION_VALUES,
            .lastParentDirection = (ABI14_0_0YGDirection) -1,
            .nextCachedMeasurementsIndex = 0,
            .computedFlexBasis = ABI14_0_0YGUndefined,
            .measuredDimensions = ABI14_0_0YG_DEFAULT_DIMENSION_VALUES,

            .cachedLayout =
                {
                    .widthMeasureMode = (ABI14_0_0YGMeasureMode) -1,
                    .heightMeasureMode = (ABI14_0_0YGMeasureMode) -1,
                    .computedWidth = -1,
                    .computedHeight = -1,
                },
        },
};

static void ABI14_0_0YGNodeMarkDirtyInternal(const ABI14_0_0YGNodeRef node);

ABI14_0_0YGMalloc gABI14_0_0YGMalloc = &malloc;
ABI14_0_0YGCalloc gABI14_0_0YGCalloc = &calloc;
ABI14_0_0YGRealloc gABI14_0_0YGRealloc = &realloc;
ABI14_0_0YGFree gABI14_0_0YGFree = &free;

static ABI14_0_0YGValue ABI14_0_0YGValueUndefined = ABI14_0_0YG_UNDEFINED_VALUES;

static ABI14_0_0YGValue ABI14_0_0YGValueZero = {.value = 0, .unit = ABI14_0_0YGUnitPixel};

#ifdef ANDROID
#include <android/log.h>
static int ABI14_0_0YGAndroidLog(ABI14_0_0YGLogLevel level, const char *format, va_list args) {
  int androidLevel = ABI14_0_0YGLogLevelDebug;
  switch (level) {
    case ABI14_0_0YGLogLevelError:
      androidLevel = ANDROID_LOG_ERROR;
      break;
    case ABI14_0_0YGLogLevelWarn:
      androidLevel = ANDROID_LOG_WARN;
      break;
    case ABI14_0_0YGLogLevelInfo:
      androidLevel = ANDROID_LOG_INFO;
      break;
    case ABI14_0_0YGLogLevelDebug:
      androidLevel = ANDROID_LOG_DEBUG;
      break;
    case ABI14_0_0YGLogLevelVerbose:
      androidLevel = ANDROID_LOG_VERBOSE;
      break;
  }
  const int result = __android_log_vprint(androidLevel, "ABI14_0_0YG-layout", format, args);
  return result;
}
static ABI14_0_0YGLogger gLogger = &ABI14_0_0YGAndroidLog;
#else
static int ABI14_0_0YGDefaultLog(ABI14_0_0YGLogLevel level, const char *format, va_list args) {
  switch (level) {
    case ABI14_0_0YGLogLevelError:
      return vfprintf(stderr, format, args);
    case ABI14_0_0YGLogLevelWarn:
    case ABI14_0_0YGLogLevelInfo:
    case ABI14_0_0YGLogLevelDebug:
    case ABI14_0_0YGLogLevelVerbose:
    default:
      return vprintf(format, args);
  }
}
static ABI14_0_0YGLogger gLogger = &ABI14_0_0YGDefaultLog;
#endif

static inline const ABI14_0_0YGValue *ABI14_0_0YGComputedEdgeValue(const ABI14_0_0YGValue edges[ABI14_0_0YGEdgeCount],
                                                 const ABI14_0_0YGEdge edge,
                                                 const ABI14_0_0YGValue *const defaultValue) {
  ABI14_0_0YG_ASSERT(edge <= ABI14_0_0YGEdgeEnd, "Cannot get computed value of multi-edge shorthands");

  if (edges[edge].unit != ABI14_0_0YGUnitUndefined) {
    return &edges[edge];
  }

  if ((edge == ABI14_0_0YGEdgeTop || edge == ABI14_0_0YGEdgeBottom) &&
      edges[ABI14_0_0YGEdgeVertical].unit != ABI14_0_0YGUnitUndefined) {
    return &edges[ABI14_0_0YGEdgeVertical];
  }

  if ((edge == ABI14_0_0YGEdgeLeft || edge == ABI14_0_0YGEdgeRight || edge == ABI14_0_0YGEdgeStart || edge == ABI14_0_0YGEdgeEnd) &&
      edges[ABI14_0_0YGEdgeHorizontal].unit != ABI14_0_0YGUnitUndefined) {
    return &edges[ABI14_0_0YGEdgeHorizontal];
  }

  if (edges[ABI14_0_0YGEdgeAll].unit != ABI14_0_0YGUnitUndefined) {
    return &edges[ABI14_0_0YGEdgeAll];
  }

  if (edge == ABI14_0_0YGEdgeStart || edge == ABI14_0_0YGEdgeEnd) {
    return &ABI14_0_0YGValueUndefined;
  }

  return defaultValue;
}

static inline float ABI14_0_0YGValueResolve(const ABI14_0_0YGValue *const unit, const float parentSize) {
  if (unit->unit == ABI14_0_0YGUnitPixel) {
    return unit->value;
  } else {
    return unit->value * parentSize / 100.0f;
  }
}

int32_t gNodeInstanceCount = 0;

ABI14_0_0YGNodeRef ABI14_0_0YGNodeNew(void) {
  const ABI14_0_0YGNodeRef node = gABI14_0_0YGMalloc(sizeof(ABI14_0_0YGNode));
  ABI14_0_0YG_ASSERT(node, "Could not allocate memory for node");
  gNodeInstanceCount++;

  memcpy(node, &gABI14_0_0YGNodeDefaults, sizeof(ABI14_0_0YGNode));
  return node;
}

void ABI14_0_0YGNodeFree(const ABI14_0_0YGNodeRef node) {
  if (node->parent) {
    ABI14_0_0YGNodeListDelete(node->parent->children, node);
    node->parent = NULL;
  }

  const uint32_t childCount = ABI14_0_0YGNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeGetChild(node, i);
    child->parent = NULL;
  }

  ABI14_0_0YGNodeListFree(node->children);
  gABI14_0_0YGFree(node);
  gNodeInstanceCount--;
}

void ABI14_0_0YGNodeFreeRecursive(const ABI14_0_0YGNodeRef root) {
  while (ABI14_0_0YGNodeGetChildCount(root) > 0) {
    const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeGetChild(root, 0);
    ABI14_0_0YGNodeRemoveChild(root, child);
    ABI14_0_0YGNodeFreeRecursive(child);
  }
  ABI14_0_0YGNodeFree(root);
}

void ABI14_0_0YGNodeReset(const ABI14_0_0YGNodeRef node) {
  ABI14_0_0YG_ASSERT(ABI14_0_0YGNodeGetChildCount(node) == 0,
            "Cannot reset a node which still has children attached");
  ABI14_0_0YG_ASSERT(node->parent == NULL, "Cannot reset a node still attached to a parent");

  ABI14_0_0YGNodeListFree(node->children);
  memcpy(node, &gABI14_0_0YGNodeDefaults, sizeof(ABI14_0_0YGNode));
}

int32_t ABI14_0_0YGNodeGetInstanceCount(void) {
  return gNodeInstanceCount;
}

static void ABI14_0_0YGNodeMarkDirtyInternal(const ABI14_0_0YGNodeRef node) {
  if (!node->isDirty) {
    node->isDirty = true;
    node->layout.computedFlexBasis = ABI14_0_0YGUndefined;
    if (node->parent) {
      ABI14_0_0YGNodeMarkDirtyInternal(node->parent);
    }
  }
}

void ABI14_0_0YGNodeSetMeasureFunc(const ABI14_0_0YGNodeRef node, ABI14_0_0YGMeasureFunc measureFunc) {
  if (measureFunc == NULL) {
    node->measure = NULL;
  } else {
    ABI14_0_0YG_ASSERT(ABI14_0_0YGNodeGetChildCount(node) == 0,
              "Cannot set measure function: Nodes with measure functions cannot have children.");
    node->measure = measureFunc;
  }
}

ABI14_0_0YGMeasureFunc ABI14_0_0YGNodeGetMeasureFunc(const ABI14_0_0YGNodeRef node) {
  return node->measure;
}

void ABI14_0_0YGNodeInsertChild(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGNodeRef child, const uint32_t index) {
  ABI14_0_0YG_ASSERT(child->parent == NULL, "Child already has a parent, it must be removed first.");
  ABI14_0_0YG_ASSERT(node->measure == NULL,
            "Cannot add child: Nodes with measure functions cannot have children.");
  ABI14_0_0YGNodeListInsert(&node->children, child, index);
  child->parent = node;
  ABI14_0_0YGNodeMarkDirtyInternal(node);
}

void ABI14_0_0YGNodeRemoveChild(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGNodeRef child) {
  if (ABI14_0_0YGNodeListDelete(node->children, child) != NULL) {
    child->parent = NULL;
    ABI14_0_0YGNodeMarkDirtyInternal(node);
  }
}

ABI14_0_0YGNodeRef ABI14_0_0YGNodeGetChild(const ABI14_0_0YGNodeRef node, const uint32_t index) {
  return ABI14_0_0YGNodeListGet(node->children, index);
}

ABI14_0_0YGNodeRef ABI14_0_0YGNodeGetParent(const ABI14_0_0YGNodeRef node) {
  return node->parent;
}

inline uint32_t ABI14_0_0YGNodeGetChildCount(const ABI14_0_0YGNodeRef node) {
  return ABI14_0_0YGNodeListCount(node->children);
}

void ABI14_0_0YGNodeMarkDirty(const ABI14_0_0YGNodeRef node) {
  ABI14_0_0YG_ASSERT(node->measure != NULL,
            "Only leaf nodes with custom measure functions"
            "should manually mark themselves as dirty");
  ABI14_0_0YGNodeMarkDirtyInternal(node);
}

bool ABI14_0_0YGNodeIsDirty(const ABI14_0_0YGNodeRef node) {
  return node->isDirty;
}

void ABI14_0_0YGNodeCopyStyle(const ABI14_0_0YGNodeRef dstNode, const ABI14_0_0YGNodeRef srcNode) {
  if (memcmp(&dstNode->style, &srcNode->style, sizeof(ABI14_0_0YGStyle)) != 0) {
    memcpy(&dstNode->style, &srcNode->style, sizeof(ABI14_0_0YGStyle));
    ABI14_0_0YGNodeMarkDirtyInternal(dstNode);
  }
}

inline float ABI14_0_0YGNodeStyleGetFlexGrow(const ABI14_0_0YGNodeRef node) {
  if (!ABI14_0_0YGFloatIsUndefined(node->style.flexGrow)) {
    return node->style.flexGrow;
  }
  if (!ABI14_0_0YGFloatIsUndefined(node->style.flex) && node->style.flex > 0.0f) {
    return node->style.flex;
  }
  return 0.0f;
}

inline float ABI14_0_0YGNodeStyleGetFlexShrink(const ABI14_0_0YGNodeRef node) {
  if (!ABI14_0_0YGFloatIsUndefined(node->style.flexShrink)) {
    return node->style.flexShrink;
  }
  if (!ABI14_0_0YGFloatIsUndefined(node->style.flex) && node->style.flex < 0.0f) {
    return -node->style.flex;
  }
  return 0.0f;
}

static inline const ABI14_0_0YGValue *ABI14_0_0YGNodeStyleGetFlexBasisPtr(const ABI14_0_0YGNodeRef node) {
  if (node->style.flexBasis.unit != ABI14_0_0YGUnitUndefined) {
    return &node->style.flexBasis;
  }
  if (!ABI14_0_0YGFloatIsUndefined(node->style.flex) && node->style.flex > 0.0f) {
    return &ABI14_0_0YGValueZero;
  }
  return &ABI14_0_0YGValueUndefined;
}

inline ABI14_0_0YGValue ABI14_0_0YGNodeStyleGetFlexBasis(const ABI14_0_0YGNodeRef node) {
  return *ABI14_0_0YGNodeStyleGetFlexBasisPtr(node);
}

void ABI14_0_0YGNodeStyleSetFlex(const ABI14_0_0YGNodeRef node, const float flex) {
  if (node->style.flex != flex) {
    node->style.flex = flex;
    ABI14_0_0YGNodeMarkDirtyInternal(node);
  }
}

#define ABI14_0_0YG_NODE_PROPERTY_IMPL(type, name, paramName, instanceName) \
  void ABI14_0_0YGNodeSet##name(const ABI14_0_0YGNodeRef node, type paramName) {     \
    node->instanceName = paramName;                                \
  }                                                                \
                                                                   \
  type ABI14_0_0YGNodeGet##name(const ABI14_0_0YGNodeRef node) {                     \
    return node->instanceName;                                     \
  }

#define ABI14_0_0YG_NODE_STYLE_PROPERTY_SETTER_IMPL(type, name, paramName, instanceName) \
  void ABI14_0_0YGNodeStyleSet##name(const ABI14_0_0YGNodeRef node, const type paramName) {       \
    if (node->style.instanceName != paramName) {                                \
      node->style.instanceName = paramName;                                     \
      ABI14_0_0YGNodeMarkDirtyInternal(node);                                            \
    }                                                                           \
  }

#define ABI14_0_0YG_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(type, name, paramName, instanceName) \
  void ABI14_0_0YGNodeStyleSet##name(const ABI14_0_0YGNodeRef node, const type paramName) {            \
    if (node->style.instanceName.value != paramName ||                               \
        node->style.instanceName.unit != ABI14_0_0YGUnitPixel) {                              \
      node->style.instanceName.value = paramName;                                    \
      node->style.instanceName.unit =                                                \
          ABI14_0_0YGFloatIsUndefined(paramName) ? ABI14_0_0YGUnitUndefined : ABI14_0_0YGUnitPixel;             \
      ABI14_0_0YGNodeMarkDirtyInternal(node);                                                 \
    }                                                                                \
  }                                                                                  \
                                                                                     \
  void ABI14_0_0YGNodeStyleSet##name##Percent(const ABI14_0_0YGNodeRef node, const type paramName) {   \
    if (node->style.instanceName.value != paramName ||                               \
        node->style.instanceName.unit != ABI14_0_0YGUnitPercent) {                            \
      node->style.instanceName.value = paramName;                                    \
      node->style.instanceName.unit =                                                \
          ABI14_0_0YGFloatIsUndefined(paramName) ? ABI14_0_0YGUnitUndefined : ABI14_0_0YGUnitPercent;           \
      ABI14_0_0YGNodeMarkDirtyInternal(node);                                                 \
    }                                                                                \
  }

#define ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(type, name, paramName, instanceName)  \
  ABI14_0_0YG_NODE_STYLE_PROPERTY_SETTER_IMPL(type, name, paramName, instanceName) \
                                                                          \
  type ABI14_0_0YGNodeStyleGet##name(const ABI14_0_0YGNodeRef node) {                       \
    return node->style.instanceName;                                      \
  }

#define ABI14_0_0YG_NODE_STYLE_PROPERTY_UNIT_IMPL(type, name, paramName, instanceName)   \
  ABI14_0_0YG_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(float, name, paramName, instanceName) \
                                                                                \
  type ABI14_0_0YGNodeStyleGet##name(const ABI14_0_0YGNodeRef node) {                             \
    return node->style.instanceName;                                            \
  }

#define ABI14_0_0YG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(type, name, paramName, instanceName, defaultValue) \
  void ABI14_0_0YGNodeStyleSet##name(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGEdge edge, const float paramName) {    \
    if (node->style.instanceName[edge].value != paramName ||                                     \
        node->style.instanceName[edge].unit != ABI14_0_0YGUnitPixel) {                                    \
      node->style.instanceName[edge].value = paramName;                                          \
      node->style.instanceName[edge].unit =                                                      \
          ABI14_0_0YGFloatIsUndefined(paramName) ? ABI14_0_0YGUnitUndefined : ABI14_0_0YGUnitPixel;                         \
      ABI14_0_0YGNodeMarkDirtyInternal(node);                                                             \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  void ABI14_0_0YGNodeStyleSet##name##Percent(const ABI14_0_0YGNodeRef node,                                       \
                                     const ABI14_0_0YGEdge edge,                                          \
                                     const float paramName) {                                    \
    if (node->style.instanceName[edge].value != paramName ||                                     \
        node->style.instanceName[edge].unit != ABI14_0_0YGUnitPercent) {                                  \
      node->style.instanceName[edge].value = paramName;                                          \
      node->style.instanceName[edge].unit =                                                      \
          ABI14_0_0YGFloatIsUndefined(paramName) ? ABI14_0_0YGUnitUndefined : ABI14_0_0YGUnitPercent;                       \
      ABI14_0_0YGNodeMarkDirtyInternal(node);                                                             \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  type ABI14_0_0YGNodeStyleGet##name(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGEdge edge) {                           \
    return *ABI14_0_0YGComputedEdgeValue(node->style.instanceName, edge, &defaultValue);                  \
  }

#define ABI14_0_0YG_NODE_STYLE_EDGE_PROPERTY_IMPL(type, name, paramName, instanceName, defaultValue)   \
  void ABI14_0_0YGNodeStyleSet##name(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGEdge edge, const float paramName) { \
    if (node->style.instanceName[edge].value != paramName ||                                  \
        node->style.instanceName[edge].unit != ABI14_0_0YGUnitPixel) {                                 \
      node->style.instanceName[edge].value = paramName;                                       \
      node->style.instanceName[edge].unit =                                                   \
          ABI14_0_0YGFloatIsUndefined(paramName) ? ABI14_0_0YGUnitUndefined : ABI14_0_0YGUnitPixel;                      \
      ABI14_0_0YGNodeMarkDirtyInternal(node);                                                          \
    }                                                                                         \
  }                                                                                           \
                                                                                              \
  float ABI14_0_0YGNodeStyleGet##name(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGEdge edge) {                       \
    return ABI14_0_0YGComputedEdgeValue(node->style.instanceName, edge, &defaultValue)->value;         \
  }

#define ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(type, name, instanceName) \
  type ABI14_0_0YGNodeLayoutGet##name(const ABI14_0_0YGNodeRef node) {           \
    return node->layout.instanceName;                          \
  }

ABI14_0_0YG_NODE_PROPERTY_IMPL(void *, Context, context, context);
ABI14_0_0YG_NODE_PROPERTY_IMPL(ABI14_0_0YGPrintFunc, PrintFunc, printFunc, print);
ABI14_0_0YG_NODE_PROPERTY_IMPL(bool, HasNewLayout, hasNewLayout, hasNewLayout);

ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGDirection, Direction, direction, direction);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGFlexDirection, FlexDirection, flexDirection, flexDirection);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGJustify, JustifyContent, justifyContent, justifyContent);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGAlign, AlignContent, alignContent, alignContent);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGAlign, AlignItems, alignItems, alignItems);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGAlign, AlignSelf, alignSelf, alignSelf);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGPositionType, PositionType, positionType, positionType);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGWrap, FlexWrap, flexWrap, flexWrap);
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(ABI14_0_0YGOverflow, Overflow, overflow, overflow);

ABI14_0_0YG_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexGrow, flexGrow, flexGrow);
ABI14_0_0YG_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexShrink, flexShrink, flexShrink);
ABI14_0_0YG_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(float, FlexBasis, flexBasis, flexBasis);

ABI14_0_0YG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, Position, position, position, ABI14_0_0YGValueUndefined);
ABI14_0_0YG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, Margin, margin, margin, ABI14_0_0YGValueZero);
ABI14_0_0YG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, Padding, padding, padding, ABI14_0_0YGValueZero);
ABI14_0_0YG_NODE_STYLE_EDGE_PROPERTY_IMPL(float, Border, border, border, ABI14_0_0YGValueZero);

ABI14_0_0YG_NODE_STYLE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, Width, width, dimensions[ABI14_0_0YGDimensionWidth]);
ABI14_0_0YG_NODE_STYLE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, Height, height, dimensions[ABI14_0_0YGDimensionHeight]);
ABI14_0_0YG_NODE_STYLE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, MinWidth, minWidth, minDimensions[ABI14_0_0YGDimensionWidth]);
ABI14_0_0YG_NODE_STYLE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, MinHeight, minHeight, minDimensions[ABI14_0_0YGDimensionHeight]);
ABI14_0_0YG_NODE_STYLE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, MaxWidth, maxWidth, maxDimensions[ABI14_0_0YGDimensionWidth]);
ABI14_0_0YG_NODE_STYLE_PROPERTY_UNIT_IMPL(ABI14_0_0YGValue, MaxHeight, maxHeight, maxDimensions[ABI14_0_0YGDimensionHeight]);

// Yoga specific properties, not compatible with flexbox specification
ABI14_0_0YG_NODE_STYLE_PROPERTY_IMPL(float, AspectRatio, aspectRatio, aspectRatio);

ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(float, Left, position[ABI14_0_0YGEdgeLeft]);
ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(float, Top, position[ABI14_0_0YGEdgeTop]);
ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(float, Right, position[ABI14_0_0YGEdgeRight]);
ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(float, Bottom, position[ABI14_0_0YGEdgeBottom]);
ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(float, Width, dimensions[ABI14_0_0YGDimensionWidth]);
ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(float, Height, dimensions[ABI14_0_0YGDimensionHeight]);
ABI14_0_0YG_NODE_LAYOUT_PROPERTY_IMPL(ABI14_0_0YGDirection, Direction, direction);

uint32_t gCurrentGenerationCount = 0;

bool ABI14_0_0YGLayoutNodeInternal(const ABI14_0_0YGNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const ABI14_0_0YGDirection parentDirection,
                          const ABI14_0_0YGMeasureMode widthMeasureMode,
                          const ABI14_0_0YGMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason);

inline bool ABI14_0_0YGFloatIsUndefined(const float value) {
  return isnan(value);
}

static inline bool ABI14_0_0YGValueEqual(const ABI14_0_0YGValue a, const ABI14_0_0YGValue b) {
  if (a.unit != b.unit) {
    return false;
  }

  if (a.unit == ABI14_0_0YGUnitUndefined) {
    return true;
  }

  return fabs(a.value - b.value) < 0.0001f;
}

static inline bool ABI14_0_0YGFloatsEqual(const float a, const float b) {
  if (ABI14_0_0YGFloatIsUndefined(a)) {
    return ABI14_0_0YGFloatIsUndefined(b);
  }
  return fabs(a - b) < 0.0001f;
}

static void ABI14_0_0YGIndent(const uint32_t n) {
  for (uint32_t i = 0; i < n; i++) {
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "  ");
  }
}

static void ABI14_0_0YGPrintNumberIfNotZero(const char *str, const ABI14_0_0YGValue *const number) {
  if (!ABI14_0_0YGFloatsEqual(number->value, 0)) {
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug,
          "%s: %g%s, ",
          str,
          number->value,
          number->unit == ABI14_0_0YGUnitPixel ? "px" : "%");
  }
}

static void ABI14_0_0YGPrintNumberIfNotUndefinedf(const char *str, const float number) {
  if (!ABI14_0_0YGFloatIsUndefined(number)) {
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "%s: %g, ", str, number);
  }
}

static void ABI14_0_0YGPrintNumberIfNotUndefined(const char *str, const ABI14_0_0YGValue *const number) {
  if (number->unit != ABI14_0_0YGUnitUndefined) {
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug,
          "%s: %g%s, ",
          str,
          number->value,
          number->unit == ABI14_0_0YGUnitPixel ? "px" : "%");
  }
}

static bool ABI14_0_0YGFourValuesEqual(const ABI14_0_0YGValue four[4]) {
  return ABI14_0_0YGValueEqual(four[0], four[1]) && ABI14_0_0YGValueEqual(four[0], four[2]) &&
         ABI14_0_0YGValueEqual(four[0], four[3]);
}

static void ABI14_0_0YGNodePrintInternal(const ABI14_0_0YGNodeRef node,
                                const ABI14_0_0YGPrintOptions options,
                                const uint32_t level) {
  ABI14_0_0YGIndent(level);
  ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "{");

  if (node->print) {
    node->print(node);
  }

  if (options & ABI14_0_0YGPrintOptionsLayout) {
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "layout: {");
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "width: %g, ", node->layout.dimensions[ABI14_0_0YGDimensionWidth]);
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "height: %g, ", node->layout.dimensions[ABI14_0_0YGDimensionHeight]);
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "top: %g, ", node->layout.position[ABI14_0_0YGEdgeTop]);
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "left: %g", node->layout.position[ABI14_0_0YGEdgeLeft]);
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "}, ");
  }

  if (options & ABI14_0_0YGPrintOptionsStyle) {
    if (node->style.flexDirection == ABI14_0_0YGFlexDirectionColumn) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "flexDirection: 'column', ");
    } else if (node->style.flexDirection == ABI14_0_0YGFlexDirectionColumnReverse) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "flexDirection: 'column-reverse', ");
    } else if (node->style.flexDirection == ABI14_0_0YGFlexDirectionRow) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "flexDirection: 'row', ");
    } else if (node->style.flexDirection == ABI14_0_0YGFlexDirectionRowReverse) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "flexDirection: 'row-reverse', ");
    }

    if (node->style.justifyContent == ABI14_0_0YGJustifyCenter) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "justifyContent: 'center', ");
    } else if (node->style.justifyContent == ABI14_0_0YGJustifyFlexEnd) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "justifyContent: 'flex-end', ");
    } else if (node->style.justifyContent == ABI14_0_0YGJustifySpaceAround) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "justifyContent: 'space-around', ");
    } else if (node->style.justifyContent == ABI14_0_0YGJustifySpaceBetween) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "justifyContent: 'space-between', ");
    }

    if (node->style.alignItems == ABI14_0_0YGAlignCenter) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignItems: 'center', ");
    } else if (node->style.alignItems == ABI14_0_0YGAlignFlexEnd) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignItems: 'flex-end', ");
    } else if (node->style.alignItems == ABI14_0_0YGAlignStretch) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignItems: 'stretch', ");
    }

    if (node->style.alignContent == ABI14_0_0YGAlignCenter) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignContent: 'center', ");
    } else if (node->style.alignContent == ABI14_0_0YGAlignFlexEnd) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignContent: 'flex-end', ");
    } else if (node->style.alignContent == ABI14_0_0YGAlignStretch) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignContent: 'stretch', ");
    }

    if (node->style.alignSelf == ABI14_0_0YGAlignFlexStart) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignSelf: 'flex-start', ");
    } else if (node->style.alignSelf == ABI14_0_0YGAlignCenter) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignSelf: 'center', ");
    } else if (node->style.alignSelf == ABI14_0_0YGAlignFlexEnd) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignSelf: 'flex-end', ");
    } else if (node->style.alignSelf == ABI14_0_0YGAlignStretch) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "alignSelf: 'stretch', ");
    }

    ABI14_0_0YGPrintNumberIfNotUndefinedf("flexGrow", ABI14_0_0YGNodeStyleGetFlexGrow(node));
    ABI14_0_0YGPrintNumberIfNotUndefinedf("flexShrink", ABI14_0_0YGNodeStyleGetFlexShrink(node));
    ABI14_0_0YGPrintNumberIfNotUndefined("flexBasis", ABI14_0_0YGNodeStyleGetFlexBasisPtr(node));

    if (node->style.overflow == ABI14_0_0YGOverflowHidden) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "overflow: 'hidden', ");
    } else if (node->style.overflow == ABI14_0_0YGOverflowVisible) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "overflow: 'visible', ");
    } else if (node->style.overflow == ABI14_0_0YGOverflowScroll) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "overflow: 'scroll', ");
    }

    if (ABI14_0_0YGFourValuesEqual(node->style.margin)) {
      ABI14_0_0YGPrintNumberIfNotZero("margin",
                             ABI14_0_0YGComputedEdgeValue(node->style.margin, ABI14_0_0YGEdgeLeft, &ABI14_0_0YGValueZero));
    } else {
      ABI14_0_0YGPrintNumberIfNotZero("marginLeft",
                             ABI14_0_0YGComputedEdgeValue(node->style.margin, ABI14_0_0YGEdgeLeft, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("marginRight",
                             ABI14_0_0YGComputedEdgeValue(node->style.margin, ABI14_0_0YGEdgeRight, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("marginTop",
                             ABI14_0_0YGComputedEdgeValue(node->style.margin, ABI14_0_0YGEdgeTop, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("marginBottom",
                             ABI14_0_0YGComputedEdgeValue(node->style.margin, ABI14_0_0YGEdgeBottom, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("marginStart",
                             ABI14_0_0YGComputedEdgeValue(node->style.margin, ABI14_0_0YGEdgeStart, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("marginEnd",
                             ABI14_0_0YGComputedEdgeValue(node->style.margin, ABI14_0_0YGEdgeEnd, &ABI14_0_0YGValueZero));
    }

    if (ABI14_0_0YGFourValuesEqual(node->style.padding)) {
      ABI14_0_0YGPrintNumberIfNotZero("padding",
                             ABI14_0_0YGComputedEdgeValue(node->style.padding, ABI14_0_0YGEdgeLeft, &ABI14_0_0YGValueZero));
    } else {
      ABI14_0_0YGPrintNumberIfNotZero("paddingLeft",
                             ABI14_0_0YGComputedEdgeValue(node->style.padding, ABI14_0_0YGEdgeLeft, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("paddingRight",
                             ABI14_0_0YGComputedEdgeValue(node->style.padding, ABI14_0_0YGEdgeRight, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("paddingTop",
                             ABI14_0_0YGComputedEdgeValue(node->style.padding, ABI14_0_0YGEdgeTop, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("paddingBottom",
                             ABI14_0_0YGComputedEdgeValue(node->style.padding, ABI14_0_0YGEdgeBottom, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("paddingStart",
                             ABI14_0_0YGComputedEdgeValue(node->style.padding, ABI14_0_0YGEdgeStart, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("paddingEnd",
                             ABI14_0_0YGComputedEdgeValue(node->style.padding, ABI14_0_0YGEdgeEnd, &ABI14_0_0YGValueZero));
    }

    if (ABI14_0_0YGFourValuesEqual(node->style.border)) {
      ABI14_0_0YGPrintNumberIfNotZero("borderWidth",
                             ABI14_0_0YGComputedEdgeValue(node->style.border, ABI14_0_0YGEdgeLeft, &ABI14_0_0YGValueZero));
    } else {
      ABI14_0_0YGPrintNumberIfNotZero("borderLeftWidth",
                             ABI14_0_0YGComputedEdgeValue(node->style.border, ABI14_0_0YGEdgeLeft, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("borderRightWidth",
                             ABI14_0_0YGComputedEdgeValue(node->style.border, ABI14_0_0YGEdgeRight, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("borderTopWidth",
                             ABI14_0_0YGComputedEdgeValue(node->style.border, ABI14_0_0YGEdgeTop, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("borderBottomWidth",
                             ABI14_0_0YGComputedEdgeValue(node->style.border, ABI14_0_0YGEdgeBottom, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("borderStartWidth",
                             ABI14_0_0YGComputedEdgeValue(node->style.border, ABI14_0_0YGEdgeStart, &ABI14_0_0YGValueZero));
      ABI14_0_0YGPrintNumberIfNotZero("borderEndWidth",
                             ABI14_0_0YGComputedEdgeValue(node->style.border, ABI14_0_0YGEdgeEnd, &ABI14_0_0YGValueZero));
    }

    ABI14_0_0YGPrintNumberIfNotUndefined("width", &node->style.dimensions[ABI14_0_0YGDimensionWidth]);
    ABI14_0_0YGPrintNumberIfNotUndefined("height", &node->style.dimensions[ABI14_0_0YGDimensionHeight]);
    ABI14_0_0YGPrintNumberIfNotUndefined("maxWidth", &node->style.maxDimensions[ABI14_0_0YGDimensionWidth]);
    ABI14_0_0YGPrintNumberIfNotUndefined("maxHeight", &node->style.maxDimensions[ABI14_0_0YGDimensionHeight]);
    ABI14_0_0YGPrintNumberIfNotUndefined("minWidth", &node->style.minDimensions[ABI14_0_0YGDimensionWidth]);
    ABI14_0_0YGPrintNumberIfNotUndefined("minHeight", &node->style.minDimensions[ABI14_0_0YGDimensionHeight]);

    if (node->style.positionType == ABI14_0_0YGPositionTypeAbsolute) {
      ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "position: 'absolute', ");
    }

    ABI14_0_0YGPrintNumberIfNotUndefined(
        "left", ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeLeft, &ABI14_0_0YGValueUndefined));
    ABI14_0_0YGPrintNumberIfNotUndefined(
        "right", ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeRight, &ABI14_0_0YGValueUndefined));
    ABI14_0_0YGPrintNumberIfNotUndefined(
        "top", ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeTop, &ABI14_0_0YGValueUndefined));
    ABI14_0_0YGPrintNumberIfNotUndefined(
        "bottom", ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeBottom, &ABI14_0_0YGValueUndefined));
  }

  const uint32_t childCount = ABI14_0_0YGNodeListCount(node->children);
  if (options & ABI14_0_0YGPrintOptionsChildren && childCount > 0) {
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "children: [\n");
    for (uint32_t i = 0; i < childCount; i++) {
      ABI14_0_0YGNodePrintInternal(ABI14_0_0YGNodeGetChild(node, i), options, level + 1);
    }
    ABI14_0_0YGIndent(level);
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "]},\n");
  } else {
    ABI14_0_0YGLog(ABI14_0_0YGLogLevelDebug, "},\n");
  }
}

void ABI14_0_0YGNodePrint(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGPrintOptions options) {
  ABI14_0_0YGNodePrintInternal(node, options, 0);
}

static const ABI14_0_0YGEdge leading[4] = {
        [ABI14_0_0YGFlexDirectionColumn] = ABI14_0_0YGEdgeTop,
        [ABI14_0_0YGFlexDirectionColumnReverse] = ABI14_0_0YGEdgeBottom,
        [ABI14_0_0YGFlexDirectionRow] = ABI14_0_0YGEdgeLeft,
        [ABI14_0_0YGFlexDirectionRowReverse] = ABI14_0_0YGEdgeRight,
};
static const ABI14_0_0YGEdge trailing[4] = {
        [ABI14_0_0YGFlexDirectionColumn] = ABI14_0_0YGEdgeBottom,
        [ABI14_0_0YGFlexDirectionColumnReverse] = ABI14_0_0YGEdgeTop,
        [ABI14_0_0YGFlexDirectionRow] = ABI14_0_0YGEdgeRight,
        [ABI14_0_0YGFlexDirectionRowReverse] = ABI14_0_0YGEdgeLeft,
};
static const ABI14_0_0YGEdge pos[4] = {
        [ABI14_0_0YGFlexDirectionColumn] = ABI14_0_0YGEdgeTop,
        [ABI14_0_0YGFlexDirectionColumnReverse] = ABI14_0_0YGEdgeBottom,
        [ABI14_0_0YGFlexDirectionRow] = ABI14_0_0YGEdgeLeft,
        [ABI14_0_0YGFlexDirectionRowReverse] = ABI14_0_0YGEdgeRight,
};
static const ABI14_0_0YGDimension dim[4] = {
        [ABI14_0_0YGFlexDirectionColumn] = ABI14_0_0YGDimensionHeight,
        [ABI14_0_0YGFlexDirectionColumnReverse] = ABI14_0_0YGDimensionHeight,
        [ABI14_0_0YGFlexDirectionRow] = ABI14_0_0YGDimensionWidth,
        [ABI14_0_0YGFlexDirectionRowReverse] = ABI14_0_0YGDimensionWidth,
};

static inline bool ABI14_0_0YGFlexDirectionIsRow(const ABI14_0_0YGFlexDirection flexDirection) {
  return flexDirection == ABI14_0_0YGFlexDirectionRow || flexDirection == ABI14_0_0YGFlexDirectionRowReverse;
}

static inline bool ABI14_0_0YGFlexDirectionIsColumn(const ABI14_0_0YGFlexDirection flexDirection) {
  return flexDirection == ABI14_0_0YGFlexDirectionColumn || flexDirection == ABI14_0_0YGFlexDirectionColumnReverse;
}

static inline float ABI14_0_0YGNodeLeadingMargin(const ABI14_0_0YGNodeRef node,
                                        const ABI14_0_0YGFlexDirection axis,
                                        const float widthSize) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis) && node->style.margin[ABI14_0_0YGEdgeStart].unit != ABI14_0_0YGUnitUndefined) {
    return ABI14_0_0YGValueResolve(&node->style.margin[ABI14_0_0YGEdgeStart], widthSize);
  }

  return ABI14_0_0YGValueResolve(ABI14_0_0YGComputedEdgeValue(node->style.margin, leading[axis], &ABI14_0_0YGValueZero),
                        widthSize);
}

static float ABI14_0_0YGNodeTrailingMargin(const ABI14_0_0YGNodeRef node,
                                  const ABI14_0_0YGFlexDirection axis,
                                  const float widthSize) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis) && node->style.margin[ABI14_0_0YGEdgeEnd].unit != ABI14_0_0YGUnitUndefined) {
    return ABI14_0_0YGValueResolve(&node->style.margin[ABI14_0_0YGEdgeEnd], widthSize);
  }

  return ABI14_0_0YGValueResolve(ABI14_0_0YGComputedEdgeValue(node->style.margin, trailing[axis], &ABI14_0_0YGValueZero),
                        widthSize);
}

static float ABI14_0_0YGNodeLeadingPadding(const ABI14_0_0YGNodeRef node,
                                  const ABI14_0_0YGFlexDirection axis,
                                  const float widthSize) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis) && node->style.padding[ABI14_0_0YGEdgeStart].unit != ABI14_0_0YGUnitUndefined &&
      ABI14_0_0YGValueResolve(&node->style.padding[ABI14_0_0YGEdgeStart], widthSize) >= 0.0f) {
    return ABI14_0_0YGValueResolve(&node->style.padding[ABI14_0_0YGEdgeStart], widthSize);
  }

  return fmaxf(ABI14_0_0YGValueResolve(ABI14_0_0YGComputedEdgeValue(node->style.padding, leading[axis], &ABI14_0_0YGValueZero),
                              widthSize),
               0.0f);
}

static float ABI14_0_0YGNodeTrailingPadding(const ABI14_0_0YGNodeRef node,
                                   const ABI14_0_0YGFlexDirection axis,
                                   const float widthSize) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis) && node->style.padding[ABI14_0_0YGEdgeEnd].unit != ABI14_0_0YGUnitUndefined &&
      ABI14_0_0YGValueResolve(&node->style.padding[ABI14_0_0YGEdgeEnd], widthSize) >= 0.0f) {
    return ABI14_0_0YGValueResolve(&node->style.padding[ABI14_0_0YGEdgeEnd], widthSize);
  }

  return fmaxf(ABI14_0_0YGValueResolve(ABI14_0_0YGComputedEdgeValue(node->style.padding, trailing[axis], &ABI14_0_0YGValueZero),
                              widthSize),
               0.0f);
}

static float ABI14_0_0YGNodeLeadingBorder(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGFlexDirection axis) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis) && node->style.border[ABI14_0_0YGEdgeStart].unit != ABI14_0_0YGUnitUndefined &&
      node->style.border[ABI14_0_0YGEdgeStart].value >= 0.0f) {
    return node->style.border[ABI14_0_0YGEdgeStart].value;
  }

  return fmaxf(ABI14_0_0YGComputedEdgeValue(node->style.border, leading[axis], &ABI14_0_0YGValueZero)->value, 0.0f);
}

static float ABI14_0_0YGNodeTrailingBorder(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGFlexDirection axis) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis) && node->style.border[ABI14_0_0YGEdgeEnd].unit != ABI14_0_0YGUnitUndefined &&
      node->style.border[ABI14_0_0YGEdgeEnd].value >= 0.0f) {
    return node->style.border[ABI14_0_0YGEdgeEnd].value;
  }

  return fmaxf(ABI14_0_0YGComputedEdgeValue(node->style.border, trailing[axis], &ABI14_0_0YGValueZero)->value, 0.0f);
}

static inline float ABI14_0_0YGNodeLeadingPaddingAndBorder(const ABI14_0_0YGNodeRef node,
                                                  const ABI14_0_0YGFlexDirection axis,
                                                  const float widthSize) {
  return ABI14_0_0YGNodeLeadingPadding(node, axis, widthSize) + ABI14_0_0YGNodeLeadingBorder(node, axis);
}

static inline float ABI14_0_0YGNodeTrailingPaddingAndBorder(const ABI14_0_0YGNodeRef node,
                                                   const ABI14_0_0YGFlexDirection axis,
                                                   const float widthSize) {
  return ABI14_0_0YGNodeTrailingPadding(node, axis, widthSize) + ABI14_0_0YGNodeTrailingBorder(node, axis);
}

static inline float ABI14_0_0YGNodeMarginForAxis(const ABI14_0_0YGNodeRef node,
                                        const ABI14_0_0YGFlexDirection axis,
                                        const float widthSize) {
  return ABI14_0_0YGNodeLeadingMargin(node, axis, widthSize) + ABI14_0_0YGNodeTrailingMargin(node, axis, widthSize);
}

static inline float ABI14_0_0YGNodePaddingAndBorderForAxis(const ABI14_0_0YGNodeRef node,
                                                  const ABI14_0_0YGFlexDirection axis,
                                                  const float widthSize) {
  return ABI14_0_0YGNodeLeadingPaddingAndBorder(node, axis, widthSize) +
         ABI14_0_0YGNodeTrailingPaddingAndBorder(node, axis, widthSize);
}

static inline ABI14_0_0YGAlign ABI14_0_0YGNodeAlignItem(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGNodeRef child) {
  return child->style.alignSelf == ABI14_0_0YGAlignAuto ? node->style.alignItems : child->style.alignSelf;
}

static inline ABI14_0_0YGDirection ABI14_0_0YGNodeResolveDirection(const ABI14_0_0YGNodeRef node,
                                                 const ABI14_0_0YGDirection parentDirection) {
  if (node->style.direction == ABI14_0_0YGDirectionInherit) {
    return parentDirection > ABI14_0_0YGDirectionInherit ? parentDirection : ABI14_0_0YGDirectionLTR;
  } else {
    return node->style.direction;
  }
}

static inline ABI14_0_0YGFlexDirection ABI14_0_0YGFlexDirectionResolve(const ABI14_0_0YGFlexDirection flexDirection,
                                                     const ABI14_0_0YGDirection direction) {
  if (direction == ABI14_0_0YGDirectionRTL) {
    if (flexDirection == ABI14_0_0YGFlexDirectionRow) {
      return ABI14_0_0YGFlexDirectionRowReverse;
    } else if (flexDirection == ABI14_0_0YGFlexDirectionRowReverse) {
      return ABI14_0_0YGFlexDirectionRow;
    }
  }

  return flexDirection;
}

static ABI14_0_0YGFlexDirection ABI14_0_0YGFlexDirectionCross(const ABI14_0_0YGFlexDirection flexDirection,
                                            const ABI14_0_0YGDirection direction) {
  return ABI14_0_0YGFlexDirectionIsColumn(flexDirection)
             ? ABI14_0_0YGFlexDirectionResolve(ABI14_0_0YGFlexDirectionRow, direction)
             : ABI14_0_0YGFlexDirectionColumn;
}

static inline bool ABI14_0_0YGNodeIsFlex(const ABI14_0_0YGNodeRef node) {
  return (node->style.positionType == ABI14_0_0YGPositionTypeRelative &&
          (ABI14_0_0YGNodeStyleGetFlexGrow(node) != 0 || ABI14_0_0YGNodeStyleGetFlexShrink(node) != 0));
}

static inline float ABI14_0_0YGNodeDimWithMargin(const ABI14_0_0YGNodeRef node,
                                        const ABI14_0_0YGFlexDirection axis,
                                        const float widthSize) {
  return node->layout.measuredDimensions[dim[axis]] + ABI14_0_0YGNodeLeadingMargin(node, axis, widthSize) +
         ABI14_0_0YGNodeTrailingMargin(node, axis, widthSize);
}

static inline bool ABI14_0_0YGNodeIsStyleDimDefined(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGFlexDirection axis) {
  return node->style.dimensions[dim[axis]].unit != ABI14_0_0YGUnitUndefined &&
         node->style.dimensions[dim[axis]].value >= 0.0f;
}

static inline bool ABI14_0_0YGNodeIsLayoutDimDefined(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGFlexDirection axis) {
  const float value = node->layout.measuredDimensions[dim[axis]];
  return !ABI14_0_0YGFloatIsUndefined(value) && value >= 0.0f;
}

static inline bool ABI14_0_0YGNodeIsLeadingPosDefined(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGFlexDirection axis) {
  return (ABI14_0_0YGFlexDirectionIsRow(axis) &&
          ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeStart, &ABI14_0_0YGValueUndefined)->unit !=
              ABI14_0_0YGUnitUndefined) ||
         ABI14_0_0YGComputedEdgeValue(node->style.position, leading[axis], &ABI14_0_0YGValueUndefined)->unit !=
             ABI14_0_0YGUnitUndefined;
}

static inline bool ABI14_0_0YGNodeIsTrailingPosDefined(const ABI14_0_0YGNodeRef node, const ABI14_0_0YGFlexDirection axis) {
  return (ABI14_0_0YGFlexDirectionIsRow(axis) &&
          ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeEnd, &ABI14_0_0YGValueUndefined)->unit !=
              ABI14_0_0YGUnitUndefined) ||
         ABI14_0_0YGComputedEdgeValue(node->style.position, trailing[axis], &ABI14_0_0YGValueUndefined)->unit !=
             ABI14_0_0YGUnitUndefined;
}

static float ABI14_0_0YGNodeLeadingPosition(const ABI14_0_0YGNodeRef node,
                                   const ABI14_0_0YGFlexDirection axis,
                                   const float axisSize) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis)) {
    const ABI14_0_0YGValue *leadingPosition =
        ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeStart, &ABI14_0_0YGValueUndefined);
    if (leadingPosition->unit != ABI14_0_0YGUnitUndefined) {
      return ABI14_0_0YGValueResolve(leadingPosition, axisSize);
    }
  }

  const ABI14_0_0YGValue *leadingPosition =
      ABI14_0_0YGComputedEdgeValue(node->style.position, leading[axis], &ABI14_0_0YGValueUndefined);

  return leadingPosition->unit == ABI14_0_0YGUnitUndefined ? 0.0f
                                                  : ABI14_0_0YGValueResolve(leadingPosition, axisSize);
}

static float ABI14_0_0YGNodeTrailingPosition(const ABI14_0_0YGNodeRef node,
                                    const ABI14_0_0YGFlexDirection axis,
                                    const float axisSize) {
  if (ABI14_0_0YGFlexDirectionIsRow(axis)) {
    const ABI14_0_0YGValue *trailingPosition =
        ABI14_0_0YGComputedEdgeValue(node->style.position, ABI14_0_0YGEdgeEnd, &ABI14_0_0YGValueUndefined);
    if (trailingPosition->unit != ABI14_0_0YGUnitUndefined) {
      return ABI14_0_0YGValueResolve(trailingPosition, axisSize);
    }
  }

  const ABI14_0_0YGValue *trailingPosition =
      ABI14_0_0YGComputedEdgeValue(node->style.position, trailing[axis], &ABI14_0_0YGValueUndefined);

  return trailingPosition->unit == ABI14_0_0YGUnitUndefined ? 0.0f
                                                   : ABI14_0_0YGValueResolve(trailingPosition, axisSize);
}

static float ABI14_0_0YGNodeBoundAxisWithinMinAndMax(const ABI14_0_0YGNodeRef node,
                                            const ABI14_0_0YGFlexDirection axis,
                                            const float value,
                                            const float axisSize) {
  float min = ABI14_0_0YGUndefined;
  float max = ABI14_0_0YGUndefined;

  if (ABI14_0_0YGFlexDirectionIsColumn(axis)) {
    min = ABI14_0_0YGValueResolve(&node->style.minDimensions[ABI14_0_0YGDimensionHeight], axisSize);
    max = ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionHeight], axisSize);
  } else if (ABI14_0_0YGFlexDirectionIsRow(axis)) {
    min = ABI14_0_0YGValueResolve(&node->style.minDimensions[ABI14_0_0YGDimensionWidth], axisSize);
    max = ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionWidth], axisSize);
  }

  float boundValue = value;

  if (!ABI14_0_0YGFloatIsUndefined(max) && max >= 0.0f && boundValue > max) {
    boundValue = max;
  }

  if (!ABI14_0_0YGFloatIsUndefined(min) && min >= 0.0f && boundValue < min) {
    boundValue = min;
  }

  return boundValue;
}

// Like ABI14_0_0YGNodeBoundAxisWithinMinAndMax but also ensures that the value doesn't go
// below the
// padding and border amount.
static inline float ABI14_0_0YGNodeBoundAxis(const ABI14_0_0YGNodeRef node,
                                    const ABI14_0_0YGFlexDirection axis,
                                    const float value,
                                    const float axisSize,
                                    const float widthSize) {
  return fmaxf(ABI14_0_0YGNodeBoundAxisWithinMinAndMax(node, axis, value, axisSize),
               ABI14_0_0YGNodePaddingAndBorderForAxis(node, axis, widthSize));
}

static void ABI14_0_0YGNodeSetChildTrailingPosition(const ABI14_0_0YGNodeRef node,
                                           const ABI14_0_0YGNodeRef child,
                                           const ABI14_0_0YGFlexDirection axis) {
  const float size = child->layout.measuredDimensions[dim[axis]];
  child->layout.position[trailing[axis]] =
      node->layout.measuredDimensions[dim[axis]] - size - child->layout.position[pos[axis]];
}

// If both left and right are defined, then use left. Otherwise return
// +left or -right depending on which is defined.
static float ABI14_0_0YGNodeRelativePosition(const ABI14_0_0YGNodeRef node,
                                    const ABI14_0_0YGFlexDirection axis,
                                    const float axisSize) {
  return ABI14_0_0YGNodeIsLeadingPosDefined(node, axis) ? ABI14_0_0YGNodeLeadingPosition(node, axis, axisSize)
                                               : -ABI14_0_0YGNodeTrailingPosition(node, axis, axisSize);
}

static void ABI14_0_0YGConstrainMaxSizeForMode(const float maxSize, ABI14_0_0YGMeasureMode *mode, float *size) {
  switch (*mode) {
    case ABI14_0_0YGMeasureModeExactly:
    case ABI14_0_0YGMeasureModeAtMost:
      *size = (ABI14_0_0YGFloatIsUndefined(maxSize) || *size < maxSize) ? *size : maxSize;
      break;
    case ABI14_0_0YGMeasureModeUndefined:
      if (!ABI14_0_0YGFloatIsUndefined(maxSize)) {
        *mode = ABI14_0_0YGMeasureModeAtMost;
        *size = maxSize;
      }
      break;
  }
}

static void ABI14_0_0YGNodeSetPosition(const ABI14_0_0YGNodeRef node,
                              const ABI14_0_0YGDirection direction,
                              const float mainSize,
                              const float crossSize,
                              const float parentWidth) {
  const ABI14_0_0YGFlexDirection mainAxis = ABI14_0_0YGFlexDirectionResolve(node->style.flexDirection, direction);
  const ABI14_0_0YGFlexDirection crossAxis = ABI14_0_0YGFlexDirectionCross(mainAxis, direction);
  const float relativePositionMain = ABI14_0_0YGNodeRelativePosition(node, mainAxis, mainSize);
  const float relativePositionCross = ABI14_0_0YGNodeRelativePosition(node, crossAxis, crossSize);

  node->layout.position[leading[mainAxis]] =
      ABI14_0_0YGNodeLeadingMargin(node, mainAxis, parentWidth) + relativePositionMain;
  node->layout.position[trailing[mainAxis]] =
      ABI14_0_0YGNodeTrailingMargin(node, mainAxis, parentWidth) + relativePositionMain;
  node->layout.position[leading[crossAxis]] =
      ABI14_0_0YGNodeLeadingMargin(node, crossAxis, parentWidth) + relativePositionCross;
  node->layout.position[trailing[crossAxis]] =
      ABI14_0_0YGNodeTrailingMargin(node, crossAxis, parentWidth) + relativePositionCross;
}

static void ABI14_0_0YGNodeComputeFlexBasisForChild(const ABI14_0_0YGNodeRef node,
                                           const ABI14_0_0YGNodeRef child,
                                           const float width,
                                           const ABI14_0_0YGMeasureMode widthMode,
                                           const float height,
                                           const float parentWidth,
                                           const float parentHeight,
                                           const ABI14_0_0YGMeasureMode heightMode,
                                           const ABI14_0_0YGDirection direction) {
  const ABI14_0_0YGFlexDirection mainAxis = ABI14_0_0YGFlexDirectionResolve(node->style.flexDirection, direction);
  const bool isMainAxisRow = ABI14_0_0YGFlexDirectionIsRow(mainAxis);
  const float mainAxisSize = isMainAxisRow ? width : height;
  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;

  float childWidth;
  float childHeight;
  ABI14_0_0YGMeasureMode childWidthMeasureMode;
  ABI14_0_0YGMeasureMode childHeightMeasureMode;

  const bool isRowStyleDimDefined = ABI14_0_0YGNodeIsStyleDimDefined(child, ABI14_0_0YGFlexDirectionRow);
  const bool isColumnStyleDimDefined = ABI14_0_0YGNodeIsStyleDimDefined(child, ABI14_0_0YGFlexDirectionColumn);

  if (ABI14_0_0YGNodeStyleGetFlexBasisPtr(child)->unit != ABI14_0_0YGUnitUndefined &&
      !ABI14_0_0YGFloatIsUndefined(mainAxisSize)) {
    if (ABI14_0_0YGFloatIsUndefined(child->layout.computedFlexBasis) ||
        (ABI14_0_0YGIsExperimentalFeatureEnabled(ABI14_0_0YGExperimentalFeatureWebFlexBasis) &&
         child->layout.computedFlexBasisGeneration != gCurrentGenerationCount)) {
      child->layout.computedFlexBasis =
          fmaxf(ABI14_0_0YGValueResolve(ABI14_0_0YGNodeStyleGetFlexBasisPtr(child), mainAxisParentSize),
                ABI14_0_0YGNodePaddingAndBorderForAxis(child, mainAxis, parentWidth));
    }
  } else if (isMainAxisRow && isRowStyleDimDefined) {
    // The width is definite, so use that as the flex basis.
    child->layout.computedFlexBasis =
        fmaxf(ABI14_0_0YGValueResolve(&child->style.dimensions[ABI14_0_0YGDimensionWidth], parentWidth),
              ABI14_0_0YGNodePaddingAndBorderForAxis(child, ABI14_0_0YGFlexDirectionRow, parentWidth));
  } else if (!isMainAxisRow && isColumnStyleDimDefined) {
    // The height is definite, so use that as the flex basis.
    child->layout.computedFlexBasis =
        fmaxf(ABI14_0_0YGValueResolve(&child->style.dimensions[ABI14_0_0YGDimensionHeight], parentHeight),
              ABI14_0_0YGNodePaddingAndBorderForAxis(child, ABI14_0_0YGFlexDirectionColumn, parentWidth));
  } else {
    // Compute the flex basis and hypothetical main size (i.e. the clamped
    // flex basis).
    childWidth = ABI14_0_0YGUndefined;
    childHeight = ABI14_0_0YGUndefined;
    childWidthMeasureMode = ABI14_0_0YGMeasureModeUndefined;
    childHeightMeasureMode = ABI14_0_0YGMeasureModeUndefined;

    if (isRowStyleDimDefined) {
      childWidth = ABI14_0_0YGValueResolve(&child->style.dimensions[ABI14_0_0YGDimensionWidth], parentWidth) +
                   ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionRow, parentWidth);
      childWidthMeasureMode = ABI14_0_0YGMeasureModeExactly;
    }
    if (isColumnStyleDimDefined) {
      childHeight = ABI14_0_0YGValueResolve(&child->style.dimensions[ABI14_0_0YGDimensionHeight], parentHeight) +
                    ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionColumn, parentWidth);
      childHeightMeasureMode = ABI14_0_0YGMeasureModeExactly;
    }

    // The W3C spec doesn't say anything about the 'overflow' property,
    // but all major browsers appear to implement the following logic.
    if ((!isMainAxisRow && node->style.overflow == ABI14_0_0YGOverflowScroll) ||
        node->style.overflow != ABI14_0_0YGOverflowScroll) {
      if (ABI14_0_0YGFloatIsUndefined(childWidth) && !ABI14_0_0YGFloatIsUndefined(width)) {
        childWidth = width;
        childWidthMeasureMode = ABI14_0_0YGMeasureModeAtMost;
      }
    }

    if ((isMainAxisRow && node->style.overflow == ABI14_0_0YGOverflowScroll) ||
        node->style.overflow != ABI14_0_0YGOverflowScroll) {
      if (ABI14_0_0YGFloatIsUndefined(childHeight) && !ABI14_0_0YGFloatIsUndefined(height)) {
        childHeight = height;
        childHeightMeasureMode = ABI14_0_0YGMeasureModeAtMost;
      }
    }

    // If child has no defined size in the cross axis and is set to stretch,
    // set the cross
    // axis to be measured exactly with the available inner width
    if (!isMainAxisRow && !ABI14_0_0YGFloatIsUndefined(width) && !isRowStyleDimDefined &&
        widthMode == ABI14_0_0YGMeasureModeExactly && ABI14_0_0YGNodeAlignItem(node, child) == ABI14_0_0YGAlignStretch) {
      childWidth = width;
      childWidthMeasureMode = ABI14_0_0YGMeasureModeExactly;
    }
    if (isMainAxisRow && !ABI14_0_0YGFloatIsUndefined(height) && !isColumnStyleDimDefined &&
        heightMode == ABI14_0_0YGMeasureModeExactly && ABI14_0_0YGNodeAlignItem(node, child) == ABI14_0_0YGAlignStretch) {
      childHeight = height;
      childHeightMeasureMode = ABI14_0_0YGMeasureModeExactly;
    }

    if (!ABI14_0_0YGFloatIsUndefined(child->style.aspectRatio)) {
      if (!isMainAxisRow && childWidthMeasureMode == ABI14_0_0YGMeasureModeExactly) {
        child->layout.computedFlexBasis =
            fmaxf(childWidth / child->style.aspectRatio,
                  ABI14_0_0YGNodePaddingAndBorderForAxis(child, ABI14_0_0YGFlexDirectionColumn, parentWidth));
        return;
      } else if (isMainAxisRow && childHeightMeasureMode == ABI14_0_0YGMeasureModeExactly) {
        child->layout.computedFlexBasis =
            fmaxf(childHeight * child->style.aspectRatio,
                  ABI14_0_0YGNodePaddingAndBorderForAxis(child, ABI14_0_0YGFlexDirectionRow, parentWidth));
        return;
      }
    }

    ABI14_0_0YGConstrainMaxSizeForMode(ABI14_0_0YGValueResolve(&child->style.maxDimensions[ABI14_0_0YGDimensionWidth],
                                             parentWidth),
                              &childWidthMeasureMode,
                              &childWidth);
    ABI14_0_0YGConstrainMaxSizeForMode(ABI14_0_0YGValueResolve(&child->style.maxDimensions[ABI14_0_0YGDimensionHeight],
                                             parentHeight),
                              &childHeightMeasureMode,
                              &childHeight);

    // Measure the child
    ABI14_0_0YGLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         parentWidth,
                         parentHeight,
                         false,
                         "measure");

    child->layout.computedFlexBasis =
        fmaxf(isMainAxisRow ? child->layout.measuredDimensions[ABI14_0_0YGDimensionWidth]
                            : child->layout.measuredDimensions[ABI14_0_0YGDimensionHeight],
              ABI14_0_0YGNodePaddingAndBorderForAxis(child, mainAxis, parentWidth));
  }

  child->layout.computedFlexBasisGeneration = gCurrentGenerationCount;
}

static void ABI14_0_0YGNodeAbsoluteLayoutChild(const ABI14_0_0YGNodeRef node,
                                      const ABI14_0_0YGNodeRef child,
                                      const float width,
                                      const ABI14_0_0YGMeasureMode widthMode,
                                      const float height,
                                      const ABI14_0_0YGDirection direction) {
  const ABI14_0_0YGFlexDirection mainAxis = ABI14_0_0YGFlexDirectionResolve(node->style.flexDirection, direction);
  const ABI14_0_0YGFlexDirection crossAxis = ABI14_0_0YGFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = ABI14_0_0YGFlexDirectionIsRow(mainAxis);

  float childWidth = ABI14_0_0YGUndefined;
  float childHeight = ABI14_0_0YGUndefined;
  ABI14_0_0YGMeasureMode childWidthMeasureMode = ABI14_0_0YGMeasureModeUndefined;
  ABI14_0_0YGMeasureMode childHeightMeasureMode = ABI14_0_0YGMeasureModeUndefined;

  if (ABI14_0_0YGNodeIsStyleDimDefined(child, ABI14_0_0YGFlexDirectionRow)) {
    childWidth = ABI14_0_0YGValueResolve(&child->style.dimensions[ABI14_0_0YGDimensionWidth], width) +
                 ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionRow, width);
  } else {
    // If the child doesn't have a specified width, compute the width based
    // on the left/right
    // offsets if they're defined.
    if (ABI14_0_0YGNodeIsLeadingPosDefined(child, ABI14_0_0YGFlexDirectionRow) &&
        ABI14_0_0YGNodeIsTrailingPosDefined(child, ABI14_0_0YGFlexDirectionRow)) {
      childWidth = node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] -
                   (ABI14_0_0YGNodeLeadingBorder(node, ABI14_0_0YGFlexDirectionRow) +
                    ABI14_0_0YGNodeTrailingBorder(node, ABI14_0_0YGFlexDirectionRow)) -
                   (ABI14_0_0YGNodeLeadingPosition(child, ABI14_0_0YGFlexDirectionRow, width) +
                    ABI14_0_0YGNodeTrailingPosition(child, ABI14_0_0YGFlexDirectionRow, width));
      childWidth = ABI14_0_0YGNodeBoundAxis(child, ABI14_0_0YGFlexDirectionRow, childWidth, width, width);
    }
  }

  if (ABI14_0_0YGNodeIsStyleDimDefined(child, ABI14_0_0YGFlexDirectionColumn)) {
    childHeight = ABI14_0_0YGValueResolve(&child->style.dimensions[ABI14_0_0YGDimensionHeight], height) +
                  ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionColumn, width);
  } else {
    // If the child doesn't have a specified height, compute the height
    // based on the top/bottom
    // offsets if they're defined.
    if (ABI14_0_0YGNodeIsLeadingPosDefined(child, ABI14_0_0YGFlexDirectionColumn) &&
        ABI14_0_0YGNodeIsTrailingPosDefined(child, ABI14_0_0YGFlexDirectionColumn)) {
      childHeight = node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] -
                    (ABI14_0_0YGNodeLeadingBorder(node, ABI14_0_0YGFlexDirectionColumn) +
                     ABI14_0_0YGNodeTrailingBorder(node, ABI14_0_0YGFlexDirectionColumn)) -
                    (ABI14_0_0YGNodeLeadingPosition(child, ABI14_0_0YGFlexDirectionColumn, height) +
                     ABI14_0_0YGNodeTrailingPosition(child, ABI14_0_0YGFlexDirectionColumn, height));
      childHeight = ABI14_0_0YGNodeBoundAxis(child, ABI14_0_0YGFlexDirectionColumn, childHeight, height, width);
    }
  }

  // Exactly one dimension needs to be defined for us to be able to do aspect ratio
  // calculation. One dimension being the anchor and the other being flexible.
  if (ABI14_0_0YGFloatIsUndefined(childWidth) ^ ABI14_0_0YGFloatIsUndefined(childHeight)) {
    if (!ABI14_0_0YGFloatIsUndefined(child->style.aspectRatio)) {
      if (ABI14_0_0YGFloatIsUndefined(childWidth)) {
        childWidth = fmaxf(childHeight * child->style.aspectRatio,
                           ABI14_0_0YGNodePaddingAndBorderForAxis(child, ABI14_0_0YGFlexDirectionColumn, width));
      } else if (ABI14_0_0YGFloatIsUndefined(childHeight)) {
        childHeight = fmaxf(childWidth / child->style.aspectRatio,
                            ABI14_0_0YGNodePaddingAndBorderForAxis(child, ABI14_0_0YGFlexDirectionRow, width));
      }
    }
  }

  // If we're still missing one or the other dimension, measure the content.
  if (ABI14_0_0YGFloatIsUndefined(childWidth) || ABI14_0_0YGFloatIsUndefined(childHeight)) {
    childWidthMeasureMode =
        ABI14_0_0YGFloatIsUndefined(childWidth) ? ABI14_0_0YGMeasureModeUndefined : ABI14_0_0YGMeasureModeExactly;
    childHeightMeasureMode =
        ABI14_0_0YGFloatIsUndefined(childHeight) ? ABI14_0_0YGMeasureModeUndefined : ABI14_0_0YGMeasureModeExactly;

    // According to the spec, if the main size is not definite and the
    // child's inline axis is parallel to the main axis (i.e. it's
    // horizontal), the child should be sized using "UNDEFINED" in
    // the main size. Otherwise use "AT_MOST" in the cross axis.
    if (!isMainAxisRow && ABI14_0_0YGFloatIsUndefined(childWidth) && widthMode != ABI14_0_0YGMeasureModeUndefined) {
      childWidth = width;
      childWidthMeasureMode = ABI14_0_0YGMeasureModeAtMost;
    }

    ABI14_0_0YGLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         childWidth,
                         childHeight,
                         false,
                         "abs-measure");
    childWidth = child->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] +
                 ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionRow, width);
    childHeight = child->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] +
                  ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionColumn, width);
  }

  ABI14_0_0YGLayoutNodeInternal(child,
                       childWidth,
                       childHeight,
                       direction,
                       ABI14_0_0YGMeasureModeExactly,
                       ABI14_0_0YGMeasureModeExactly,
                       childWidth,
                       childHeight,
                       true,
                       "abs-layout");

  if (ABI14_0_0YGNodeIsTrailingPosDefined(child, mainAxis) && !ABI14_0_0YGNodeIsLeadingPosDefined(child, mainAxis)) {
    child->layout.position[leading[mainAxis]] = node->layout.measuredDimensions[dim[mainAxis]] -
                                                child->layout.measuredDimensions[dim[mainAxis]] -
                                                ABI14_0_0YGNodeTrailingBorder(node, mainAxis) -
                                                ABI14_0_0YGNodeTrailingPosition(child, mainAxis, width);
  }

  if (ABI14_0_0YGNodeIsTrailingPosDefined(child, crossAxis) &&
      !ABI14_0_0YGNodeIsLeadingPosDefined(child, crossAxis)) {
    child->layout.position[leading[crossAxis]] = node->layout.measuredDimensions[dim[crossAxis]] -
                                                 child->layout.measuredDimensions[dim[crossAxis]] -
                                                 ABI14_0_0YGNodeTrailingBorder(node, crossAxis) -
                                                 ABI14_0_0YGNodeTrailingPosition(child, crossAxis, width);
  }
}

static void ABI14_0_0YGNodeWithMeasureFuncSetMeasuredDimensions(const ABI14_0_0YGNodeRef node,
                                                       const float availableWidth,
                                                       const float availableHeight,
                                                       const ABI14_0_0YGMeasureMode widthMeasureMode,
                                                       const ABI14_0_0YGMeasureMode heightMeasureMode) {
  ABI14_0_0YG_ASSERT(node->measure, "Expected node to have custom measure function");

  const float paddingAndBorderAxisRow =
      ABI14_0_0YGNodePaddingAndBorderForAxis(node, ABI14_0_0YGFlexDirectionRow, availableWidth);
  const float paddingAndBorderAxisColumn =
      ABI14_0_0YGNodePaddingAndBorderForAxis(node, ABI14_0_0YGFlexDirectionColumn, availableWidth);
  const float marginAxisRow = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionRow, availableWidth);
  const float marginAxisColumn = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionColumn, availableWidth);

  const float innerWidth = availableWidth - marginAxisRow - paddingAndBorderAxisRow;
  const float innerHeight = availableHeight - marginAxisColumn - paddingAndBorderAxisColumn;

  if (widthMeasureMode == ABI14_0_0YGMeasureModeExactly && heightMeasureMode == ABI14_0_0YGMeasureModeExactly) {
    // Don't bother sizing the text if both dimensions are already defined.
    node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] = ABI14_0_0YGNodeBoundAxis(
        node, ABI14_0_0YGFlexDirectionRow, availableWidth - marginAxisRow, availableWidth, availableWidth);
    node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] =
        ABI14_0_0YGNodeBoundAxis(node,
                        ABI14_0_0YGFlexDirectionColumn,
                        availableHeight - marginAxisColumn,
                        availableHeight,
                        availableWidth);
  } else if (innerWidth <= 0.0f || innerHeight <= 0.0f) {
    // Don't bother sizing the text if there's no horizontal or vertical
    // space.
    node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] =
        ABI14_0_0YGNodeBoundAxis(node, ABI14_0_0YGFlexDirectionRow, 0.0f, availableWidth, availableWidth);
    node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] =
        ABI14_0_0YGNodeBoundAxis(node, ABI14_0_0YGFlexDirectionColumn, 0.0f, availableHeight, availableWidth);
  } else {
    // Measure the text under the current constraints.
    const ABI14_0_0YGSize measuredSize =
        node->measure(node, innerWidth, widthMeasureMode, innerHeight, heightMeasureMode);

    node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] =
        ABI14_0_0YGNodeBoundAxis(node,
                        ABI14_0_0YGFlexDirectionRow,
                        (widthMeasureMode == ABI14_0_0YGMeasureModeUndefined ||
                         widthMeasureMode == ABI14_0_0YGMeasureModeAtMost)
                            ? measuredSize.width + paddingAndBorderAxisRow
                            : availableWidth - marginAxisRow,
                        availableWidth,
                        availableWidth);
    node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] =
        ABI14_0_0YGNodeBoundAxis(node,
                        ABI14_0_0YGFlexDirectionColumn,
                        (heightMeasureMode == ABI14_0_0YGMeasureModeUndefined ||
                         heightMeasureMode == ABI14_0_0YGMeasureModeAtMost)
                            ? measuredSize.height + paddingAndBorderAxisColumn
                            : availableHeight - marginAxisColumn,
                        availableHeight,
                        availableWidth);
  }
}

// For nodes with no children, use the available values if they were provided,
// or the minimum size as indicated by the padding and border sizes.
static void ABI14_0_0YGNodeEmptyContainerSetMeasuredDimensions(const ABI14_0_0YGNodeRef node,
                                                      const float availableWidth,
                                                      const float availableHeight,
                                                      const ABI14_0_0YGMeasureMode widthMeasureMode,
                                                      const ABI14_0_0YGMeasureMode heightMeasureMode,
                                                      const float parentWidth,
                                                      const float parentHeight) {
  const float paddingAndBorderAxisRow =
      ABI14_0_0YGNodePaddingAndBorderForAxis(node, ABI14_0_0YGFlexDirectionRow, parentWidth);
  const float paddingAndBorderAxisColumn =
      ABI14_0_0YGNodePaddingAndBorderForAxis(node, ABI14_0_0YGFlexDirectionColumn, parentWidth);
  const float marginAxisRow = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionRow, parentWidth);
  const float marginAxisColumn = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionColumn, parentWidth);

  node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] =
      ABI14_0_0YGNodeBoundAxis(node,
                      ABI14_0_0YGFlexDirectionRow,
                      (widthMeasureMode == ABI14_0_0YGMeasureModeUndefined ||
                       widthMeasureMode == ABI14_0_0YGMeasureModeAtMost)
                          ? paddingAndBorderAxisRow
                          : availableWidth - marginAxisRow,
                      parentWidth,
                      parentWidth);
  node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] =
      ABI14_0_0YGNodeBoundAxis(node,
                      ABI14_0_0YGFlexDirectionColumn,
                      (heightMeasureMode == ABI14_0_0YGMeasureModeUndefined ||
                       heightMeasureMode == ABI14_0_0YGMeasureModeAtMost)
                          ? paddingAndBorderAxisColumn
                          : availableHeight - marginAxisColumn,
                      parentHeight,
                      parentWidth);
}

static bool ABI14_0_0YGNodeFixedSizeSetMeasuredDimensions(const ABI14_0_0YGNodeRef node,
                                                 const float availableWidth,
                                                 const float availableHeight,
                                                 const ABI14_0_0YGMeasureMode widthMeasureMode,
                                                 const ABI14_0_0YGMeasureMode heightMeasureMode,
                                                 const float parentWidth,
                                                 const float parentHeight) {
  if ((widthMeasureMode == ABI14_0_0YGMeasureModeAtMost && availableWidth <= 0.0f) ||
      (heightMeasureMode == ABI14_0_0YGMeasureModeAtMost && availableHeight <= 0.0f) ||
      (widthMeasureMode == ABI14_0_0YGMeasureModeExactly && heightMeasureMode == ABI14_0_0YGMeasureModeExactly)) {
    const float marginAxisColumn = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionColumn, parentWidth);
    const float marginAxisRow = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionRow, parentWidth);

    node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] =
        ABI14_0_0YGNodeBoundAxis(node,
                        ABI14_0_0YGFlexDirectionRow,
                        ABI14_0_0YGFloatIsUndefined(availableWidth) ||
                                (widthMeasureMode == ABI14_0_0YGMeasureModeAtMost && availableWidth < 0.0f)
                            ? 0.0f
                            : availableWidth - marginAxisRow,
                        parentWidth,
                        parentWidth);

    node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] =
        ABI14_0_0YGNodeBoundAxis(node,
                        ABI14_0_0YGFlexDirectionColumn,
                        ABI14_0_0YGFloatIsUndefined(availableHeight) ||
                                (heightMeasureMode == ABI14_0_0YGMeasureModeAtMost && availableHeight < 0.0f)
                            ? 0.0f
                            : availableHeight - marginAxisColumn,
                        parentHeight,
                        parentWidth);

    return true;
  }

  return false;
}

//
// This is the main routine that implements a subset of the flexbox layout
// algorithm
// described in the W3C ABI14_0_0YG documentation: https://www.w3.org/TR/ABI14_0_0YG3-flexbox/.
//
// Limitations of this algorithm, compared to the full standard:
//  * Display property is always assumed to be 'flex' except for Text nodes,
//  which
//    are assumed to be 'inline-flex'.
//  * The 'zIndex' property (or any form of z ordering) is not supported. Nodes
//  are
//    stacked in document order.
//  * The 'order' property is not supported. The order of flex items is always
//  defined
//    by document order.
//  * The 'visibility' property is always assumed to be 'visible'. Values of
//  'collapse'
//    and 'hidden' are not supported.
//  * The 'wrap' property supports only 'nowrap' (which is the default) or
//  'wrap'. The
//    rarely-used 'wrap-reverse' is not supported.
//  * Rather than allowing arbitrary combinations of flexGrow, flexShrink and
//    flexBasis, this algorithm supports only the three most common
//    combinations:
//      flex: 0 is equiavlent to flex: 0 0 auto
//      flex: n (where n is a positive value) is equivalent to flex: n 1 auto
//          If POSITIVE_FLEX_IS_AUTO is 0, then it is equivalent to flex: n 0 0
//          This is faster because the content doesn't need to be measured, but
//          it's
//          less flexible because the basis is always 0 and can't be overriden
//          with
//          the width/height attributes.
//      flex: -1 (or any negative value) is equivalent to flex: 0 1 auto
//  * Margins cannot be specified as 'auto'. They must be specified in terms of
//  pixel
//    values, and the default value is 0.
//  * The 'baseline' value is not supported for alignItems and alignSelf
//  properties.
//  * Values of width, maxWidth, minWidth, height, maxHeight and minHeight must
//  be
//    specified as pixel values, not as percentages.
//  * There is no support for calculation of dimensions based on intrinsic
//  aspect ratios
//     (e.g. images).
//  * There is no support for forced breaks.
//  * It does not support vertical inline directions (top-to-bottom or
//  bottom-to-top text).
//
// Deviations from standard:
//  * Section 4.5 of the spec indicates that all flex items have a default
//  minimum
//    main size. For text blocks, for example, this is the width of the widest
//    word.
//    Calculating the minimum width is expensive, so we forego it and assume a
//    default
//    minimum main size of 0.
//  * Min/Max sizes in the main axis are not honored when resolving flexible
//  lengths.
//  * The spec indicates that the default value for 'flexDirection' is 'row',
//  but
//    the algorithm below assumes a default of 'column'.
//
// Input parameters:
//    - node: current node to be sized and layed out
//    - availableWidth & availableHeight: available size to be used for sizing
//    the node
//      or ABI14_0_0YGUndefined if the size is not available; interpretation depends on
//      layout
//      flags
//    - parentDirection: the inline (text) direction within the parent
//    (left-to-right or
//      right-to-left)
//    - widthMeasureMode: indicates the sizing rules for the width (see below
//    for explanation)
//    - heightMeasureMode: indicates the sizing rules for the height (see below
//    for explanation)
//    - performLayout: specifies whether the caller is interested in just the
//    dimensions
//      of the node or it requires the entire node and its subtree to be layed
//      out
//      (with final positions)
//
// Details:
//    This routine is called recursively to lay out subtrees of flexbox
//    elements. It uses the
//    information in node.style, which is treated as a read-only input. It is
//    responsible for
//    setting the layout.direction and layout.measuredDimensions fields for the
//    input node as well
//    as the layout.position and layout.lineIndex fields for its child nodes.
//    The
//    layout.measuredDimensions field includes any border or padding for the
//    node but does
//    not include margins.
//
//    The spec describes four different layout modes: "fill available", "max
//    content", "min
//    content",
//    and "fit content". Of these, we don't use "min content" because we don't
//    support default
//    minimum main sizes (see above for details). Each of our measure modes maps
//    to a layout mode
//    from the spec (https://www.w3.org/TR/ABI14_0_0YG3-sizing/#terms):
//      - ABI14_0_0YGMeasureModeUndefined: max content
//      - ABI14_0_0YGMeasureModeExactly: fill available
//      - ABI14_0_0YGMeasureModeAtMost: fit content
//
//    When calling ABI14_0_0YGNodelayoutImpl and ABI14_0_0YGLayoutNodeInternal, if the caller passes
//    an available size of
//    undefined then it must also pass a measure mode of ABI14_0_0YGMeasureModeUndefined
//    in that dimension.
//
static void ABI14_0_0YGNodelayoutImpl(const ABI14_0_0YGNodeRef node,
                             const float availableWidth,
                             const float availableHeight,
                             const ABI14_0_0YGDirection parentDirection,
                             const ABI14_0_0YGMeasureMode widthMeasureMode,
                             const ABI14_0_0YGMeasureMode heightMeasureMode,
                             const float parentWidth,
                             const float parentHeight,
                             const bool performLayout) {
  ABI14_0_0YG_ASSERT(ABI14_0_0YGFloatIsUndefined(availableWidth) ? widthMeasureMode == ABI14_0_0YGMeasureModeUndefined : true,
            "availableWidth is indefinite so widthMeasureMode must be "
            "ABI14_0_0YGMeasureModeUndefined");
  ABI14_0_0YG_ASSERT(ABI14_0_0YGFloatIsUndefined(availableHeight) ? heightMeasureMode == ABI14_0_0YGMeasureModeUndefined
                                                : true,
            "availableHeight is indefinite so heightMeasureMode must be "
            "ABI14_0_0YGMeasureModeUndefined");

  // Set the resolved resolution in the node's layout.
  const ABI14_0_0YGDirection direction = ABI14_0_0YGNodeResolveDirection(node, parentDirection);
  node->layout.direction = direction;

  if (node->measure) {
    ABI14_0_0YGNodeWithMeasureFuncSetMeasuredDimensions(
        node, availableWidth, availableHeight, widthMeasureMode, heightMeasureMode);
    return;
  }

  const uint32_t childCount = ABI14_0_0YGNodeListCount(node->children);
  if (childCount == 0) {
    ABI14_0_0YGNodeEmptyContainerSetMeasuredDimensions(node,
                                              availableWidth,
                                              availableHeight,
                                              widthMeasureMode,
                                              heightMeasureMode,
                                              parentWidth,
                                              parentHeight);
    return;
  }

  // If we're not being asked to perform a full layout we can skip the algorithm if we already know
  // the size
  if (!performLayout && ABI14_0_0YGNodeFixedSizeSetMeasuredDimensions(node,
                                                             availableWidth,
                                                             availableHeight,
                                                             widthMeasureMode,
                                                             heightMeasureMode,
                                                             parentWidth,
                                                             parentHeight)) {
    return;
  }

  // STEP 1: CALCULATE VALUES FOR REMAINDER OF ALGORITHM
  const ABI14_0_0YGFlexDirection mainAxis = ABI14_0_0YGFlexDirectionResolve(node->style.flexDirection, direction);
  const ABI14_0_0YGFlexDirection crossAxis = ABI14_0_0YGFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = ABI14_0_0YGFlexDirectionIsRow(mainAxis);
  const ABI14_0_0YGJustify justifyContent = node->style.justifyContent;
  const bool isNodeFlexWrap = node->style.flexWrap == ABI14_0_0YGWrapWrap;

  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;
  const float crossAxisParentSize = isMainAxisRow ? parentHeight : parentWidth;

  ABI14_0_0YGNodeRef firstAbsoluteChild = NULL;
  ABI14_0_0YGNodeRef currentAbsoluteChild = NULL;

  const float leadingPaddingAndBorderMain =
      ABI14_0_0YGNodeLeadingPaddingAndBorder(node, mainAxis, parentWidth);
  const float trailingPaddingAndBorderMain =
      ABI14_0_0YGNodeTrailingPaddingAndBorder(node, mainAxis, parentWidth);
  const float leadingPaddingAndBorderCross =
      ABI14_0_0YGNodeLeadingPaddingAndBorder(node, crossAxis, parentWidth);
  const float paddingAndBorderAxisMain = ABI14_0_0YGNodePaddingAndBorderForAxis(node, mainAxis, parentWidth);
  const float paddingAndBorderAxisCross =
      ABI14_0_0YGNodePaddingAndBorderForAxis(node, crossAxis, parentWidth);

  const ABI14_0_0YGMeasureMode measureModeMainDim = isMainAxisRow ? widthMeasureMode : heightMeasureMode;
  const ABI14_0_0YGMeasureMode measureModeCrossDim = isMainAxisRow ? heightMeasureMode : widthMeasureMode;

  const float paddingAndBorderAxisRow =
      isMainAxisRow ? paddingAndBorderAxisMain : paddingAndBorderAxisCross;
  const float paddingAndBorderAxisColumn =
      isMainAxisRow ? paddingAndBorderAxisCross : paddingAndBorderAxisMain;

  const float marginAxisRow = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionRow, parentWidth);
  const float marginAxisColumn = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionColumn, parentWidth);

  // STEP 2: DETERMINE AVAILABLE SIZE IN MAIN AND CROSS DIRECTIONS
  const float minInnerWidth =
      ABI14_0_0YGValueResolve(&node->style.minDimensions[ABI14_0_0YGDimensionWidth], parentWidth) - marginAxisRow -
      paddingAndBorderAxisRow;
  const float maxInnerWidth =
      ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionWidth], parentWidth) - marginAxisRow -
      paddingAndBorderAxisRow;
  const float minInnerHeight =
      ABI14_0_0YGValueResolve(&node->style.minDimensions[ABI14_0_0YGDimensionHeight], parentHeight) -
      marginAxisColumn - paddingAndBorderAxisColumn;
  const float maxInnerHeight =
      ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionHeight], parentHeight) -
      marginAxisColumn - paddingAndBorderAxisColumn;
  const float minInnerMainDim = isMainAxisRow ? minInnerWidth : minInnerHeight;
  const float maxInnerMainDim = isMainAxisRow ? maxInnerWidth : maxInnerHeight;

  // Max dimension overrides predefined dimension value; Min dimension in turn overrides both of the
  // above
  float availableInnerWidth = availableWidth - marginAxisRow - paddingAndBorderAxisRow;
  if (!ABI14_0_0YGFloatIsUndefined(availableInnerWidth)) {
    availableInnerWidth = fmaxf(fminf(availableInnerWidth, maxInnerWidth), minInnerWidth);
  }

  float availableInnerHeight = availableHeight - marginAxisColumn - paddingAndBorderAxisColumn;
  if (!ABI14_0_0YGFloatIsUndefined(availableInnerHeight)) {
    availableInnerHeight = fmaxf(fminf(availableInnerHeight, maxInnerHeight), minInnerHeight);
  }

  float availableInnerMainDim = isMainAxisRow ? availableInnerWidth : availableInnerHeight;
  const float availableInnerCrossDim = isMainAxisRow ? availableInnerHeight : availableInnerWidth;

  // If there is only one child with flexGrow + flexShrink it means we can set the
  // computedFlexBasis to 0 instead of measuring and shrinking / flexing the child to exactly
  // match the remaining space
  ABI14_0_0YGNodeRef singleFlexChild = NULL;
  if ((isMainAxisRow && widthMeasureMode == ABI14_0_0YGMeasureModeExactly) ||
      (!isMainAxisRow && heightMeasureMode == ABI14_0_0YGMeasureModeExactly)) {
    for (uint32_t i = 0; i < childCount; i++) {
      const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeGetChild(node, i);
      if (singleFlexChild) {
        if (ABI14_0_0YGNodeIsFlex(child)) {
          // There is already a flexible child, abort.
          singleFlexChild = NULL;
          break;
        }
      } else if (ABI14_0_0YGNodeStyleGetFlexGrow(child) > 0.0f && ABI14_0_0YGNodeStyleGetFlexShrink(child) > 0.0f) {
        singleFlexChild = child;
      }
    }
  }

  // STEP 3: DETERMINE FLEX BASIS FOR EACH ITEM
  for (uint32_t i = 0; i < childCount; i++) {
    const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeListGet(node->children, i);

    if (performLayout) {
      // Set the initial position (relative to the parent).
      const ABI14_0_0YGDirection childDirection = ABI14_0_0YGNodeResolveDirection(child, direction);
      ABI14_0_0YGNodeSetPosition(child,
                        childDirection,
                        availableInnerMainDim,
                        availableInnerCrossDim,
                        availableInnerWidth);
    }

    // Absolute-positioned children don't participate in flex layout. Add them
    // to a list that we can process later.
    if (child->style.positionType == ABI14_0_0YGPositionTypeAbsolute) {
      // Store a private linked list of absolutely positioned children
      // so that we can efficiently traverse them later.
      if (firstAbsoluteChild == NULL) {
        firstAbsoluteChild = child;
      }
      if (currentAbsoluteChild != NULL) {
        currentAbsoluteChild->nextChild = child;
      }
      currentAbsoluteChild = child;
      child->nextChild = NULL;
    } else {
      if (child == singleFlexChild) {
        child->layout.computedFlexBasisGeneration = gCurrentGenerationCount;
        child->layout.computedFlexBasis = 0;
      } else {
        ABI14_0_0YGNodeComputeFlexBasisForChild(node,
                                       child,
                                       availableInnerWidth,
                                       widthMeasureMode,
                                       availableInnerHeight,
                                       availableInnerWidth,
                                       availableInnerHeight,
                                       heightMeasureMode,
                                       direction);
      }
    }
  }

  // STEP 4: COLLECT FLEX ITEMS INTO FLEX LINES

  // Indexes of children that represent the first and last items in the line.
  uint32_t startOfLineIndex = 0;
  uint32_t endOfLineIndex = 0;

  // Number of lines.
  uint32_t lineCount = 0;

  // Accumulated cross dimensions of all lines so far.
  float totalLineCrossDim = 0;

  // Max main dimension of all the lines.
  float maxLineMainDim = 0;

  for (; endOfLineIndex < childCount; lineCount++, startOfLineIndex = endOfLineIndex) {
    // Number of items on the currently line. May be different than the
    // difference
    // between start and end indicates because we skip over absolute-positioned
    // items.
    uint32_t itemsOnLine = 0;

    // sizeConsumedOnCurrentLine is accumulation of the dimensions and margin
    // of all the children on the current line. This will be used in order to
    // either set the dimensions of the node if none already exist or to compute
    // the remaining space left for the flexible children.
    float sizeConsumedOnCurrentLine = 0;

    float totalFlexGrowFactors = 0;
    float totalFlexShrinkScaledFactors = 0;

    // Maintain a linked list of the child nodes that can shrink and/or grow.
    ABI14_0_0YGNodeRef firstRelativeChild = NULL;
    ABI14_0_0YGNodeRef currentRelativeChild = NULL;

    // Add items to the current line until it's full or we run out of items.
    for (uint32_t i = startOfLineIndex; i < childCount; i++, endOfLineIndex++) {
      const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeListGet(node->children, i);
      child->lineIndex = lineCount;

      if (child->style.positionType != ABI14_0_0YGPositionTypeAbsolute) {
        const float outerFlexBasis = child->layout.computedFlexBasis +
                                     ABI14_0_0YGNodeMarginForAxis(child, mainAxis, availableInnerWidth);

        // If this is a multi-line flow and this item pushes us over the
        // available size, we've
        // hit the end of the current line. Break out of the loop and lay out
        // the current line.
        if (sizeConsumedOnCurrentLine + outerFlexBasis > availableInnerMainDim && isNodeFlexWrap &&
            itemsOnLine > 0) {
          break;
        }

        sizeConsumedOnCurrentLine += outerFlexBasis;
        itemsOnLine++;

        if (ABI14_0_0YGNodeIsFlex(child)) {
          totalFlexGrowFactors += ABI14_0_0YGNodeStyleGetFlexGrow(child);

          // Unlike the grow factor, the shrink factor is scaled relative to the
          // child
          // dimension.
          totalFlexShrinkScaledFactors +=
              -ABI14_0_0YGNodeStyleGetFlexShrink(child) * child->layout.computedFlexBasis;
        }

        // Store a private linked list of children that need to be layed out.
        if (firstRelativeChild == NULL) {
          firstRelativeChild = child;
        }
        if (currentRelativeChild != NULL) {
          currentRelativeChild->nextChild = child;
        }
        currentRelativeChild = child;
        child->nextChild = NULL;
      }
    }

    // If we don't need to measure the cross axis, we can skip the entire flex
    // step.
    const bool canSkipFlex = !performLayout && measureModeCrossDim == ABI14_0_0YGMeasureModeExactly;

    // In order to position the elements in the main axis, we have two
    // controls. The space between the beginning and the first element
    // and the space between each two elements.
    float leadingMainDim = 0;
    float betweenMainDim = 0;

    // STEP 5: RESOLVING FLEXIBLE LENGTHS ON MAIN AXIS
    // Calculate the remaining available space that needs to be allocated.
    // If the main dimension size isn't known, it is computed based on
    // the line length, so there's no more space left to distribute.

    // We resolve main dimension to fit minimum and maximum values
    if (ABI14_0_0YGFloatIsUndefined(availableInnerMainDim)) {
      if (!ABI14_0_0YGFloatIsUndefined(minInnerMainDim) && sizeConsumedOnCurrentLine < minInnerMainDim) {
        availableInnerMainDim = minInnerMainDim;
      } else if (!ABI14_0_0YGFloatIsUndefined(maxInnerMainDim) &&
                 sizeConsumedOnCurrentLine > maxInnerMainDim) {
        availableInnerMainDim = maxInnerMainDim;
      }
    }

    float remainingFreeSpace = 0;
    if (!ABI14_0_0YGFloatIsUndefined(availableInnerMainDim)) {
      remainingFreeSpace = availableInnerMainDim - sizeConsumedOnCurrentLine;
    } else if (sizeConsumedOnCurrentLine < 0) {
      // availableInnerMainDim is indefinite which means the node is being sized
      // based on its
      // content.
      // sizeConsumedOnCurrentLine is negative which means the node will
      // allocate 0 pixels for
      // its content. Consequently, remainingFreeSpace is 0 -
      // sizeConsumedOnCurrentLine.
      remainingFreeSpace = -sizeConsumedOnCurrentLine;
    }

    const float originalRemainingFreeSpace = remainingFreeSpace;
    float deltaFreeSpace = 0;

    if (!canSkipFlex) {
      float childFlexBasis;
      float flexShrinkScaledFactor;
      float flexGrowFactor;
      float baseMainSize;
      float boundMainSize;

      // Do two passes over the flex items to figure out how to distribute the
      // remaining space.
      // The first pass finds the items whose min/max constraints trigger,
      // freezes them at those
      // sizes, and excludes those sizes from the remaining space. The second
      // pass sets the size
      // of each flexible item. It distributes the remaining space amongst the
      // items whose min/max
      // constraints didn't trigger in pass 1. For the other items, it sets
      // their sizes by forcing
      // their min/max constraints to trigger again.
      //
      // This two pass approach for resolving min/max constraints deviates from
      // the spec. The
      // spec (https://www.w3.org/TR/ABI14_0_0YG-flexbox-1/#resolve-flexible-lengths)
      // describes a process
      // that needs to be repeated a variable number of times. The algorithm
      // implemented here
      // won't handle all cases but it was simpler to implement and it mitigates
      // performance
      // concerns because we know exactly how many passes it'll do.

      // First pass: detect the flex items whose min/max constraints trigger
      float deltaFlexShrinkScaledFactors = 0;
      float deltaFlexGrowFactors = 0;
      currentRelativeChild = firstRelativeChild;
      while (currentRelativeChild != NULL) {
        childFlexBasis = currentRelativeChild->layout.computedFlexBasis;

        if (remainingFreeSpace < 0) {
          flexShrinkScaledFactor = -ABI14_0_0YGNodeStyleGetFlexShrink(currentRelativeChild) * childFlexBasis;

          // Is this child able to shrink?
          if (flexShrinkScaledFactor != 0) {
            baseMainSize =
                childFlexBasis +
                remainingFreeSpace / totalFlexShrinkScaledFactors * flexShrinkScaledFactor;
            boundMainSize = ABI14_0_0YGNodeBoundAxis(currentRelativeChild,
                                            mainAxis,
                                            baseMainSize,
                                            availableInnerMainDim,
                                            availableInnerWidth);
            if (baseMainSize != boundMainSize) {
              // By excluding this item's size and flex factor from remaining,
              // this item's
              // min/max constraints should also trigger in the second pass
              // resulting in the
              // item's size calculation being identical in the first and second
              // passes.
              deltaFreeSpace -= boundMainSize - childFlexBasis;
              deltaFlexShrinkScaledFactors -= flexShrinkScaledFactor;
            }
          }
        } else if (remainingFreeSpace > 0) {
          flexGrowFactor = ABI14_0_0YGNodeStyleGetFlexGrow(currentRelativeChild);

          // Is this child able to grow?
          if (flexGrowFactor != 0) {
            baseMainSize =
                childFlexBasis + remainingFreeSpace / totalFlexGrowFactors * flexGrowFactor;
            boundMainSize = ABI14_0_0YGNodeBoundAxis(currentRelativeChild,
                                            mainAxis,
                                            baseMainSize,
                                            availableInnerMainDim,
                                            availableInnerWidth);
            if (baseMainSize != boundMainSize) {
              // By excluding this item's size and flex factor from remaining,
              // this item's
              // min/max constraints should also trigger in the second pass
              // resulting in the
              // item's size calculation being identical in the first and second
              // passes.
              deltaFreeSpace -= boundMainSize - childFlexBasis;
              deltaFlexGrowFactors -= flexGrowFactor;
            }
          }
        }

        currentRelativeChild = currentRelativeChild->nextChild;
      }

      totalFlexShrinkScaledFactors += deltaFlexShrinkScaledFactors;
      totalFlexGrowFactors += deltaFlexGrowFactors;
      remainingFreeSpace += deltaFreeSpace;

      // Second pass: resolve the sizes of the flexible items
      deltaFreeSpace = 0;
      currentRelativeChild = firstRelativeChild;
      while (currentRelativeChild != NULL) {
        childFlexBasis = currentRelativeChild->layout.computedFlexBasis;
        float updatedMainSize = childFlexBasis;

        if (remainingFreeSpace < 0) {
          flexShrinkScaledFactor = -ABI14_0_0YGNodeStyleGetFlexShrink(currentRelativeChild) * childFlexBasis;
          // Is this child able to shrink?
          if (flexShrinkScaledFactor != 0) {
            float childSize;

            if (totalFlexShrinkScaledFactors == 0) {
              childSize = childFlexBasis + flexShrinkScaledFactor;
            } else {
              childSize =
                  childFlexBasis +
                  (remainingFreeSpace / totalFlexShrinkScaledFactors) * flexShrinkScaledFactor;
            }

            updatedMainSize = ABI14_0_0YGNodeBoundAxis(currentRelativeChild,
                                              mainAxis,
                                              childSize,
                                              availableInnerMainDim,
                                              availableInnerWidth);
          }
        } else if (remainingFreeSpace > 0) {
          flexGrowFactor = ABI14_0_0YGNodeStyleGetFlexGrow(currentRelativeChild);

          // Is this child able to grow?
          if (flexGrowFactor != 0) {
            updatedMainSize =
                ABI14_0_0YGNodeBoundAxis(currentRelativeChild,
                                mainAxis,
                                childFlexBasis +
                                    remainingFreeSpace / totalFlexGrowFactors * flexGrowFactor,
                                availableInnerMainDim,
                                availableInnerWidth);
          }
        }

        deltaFreeSpace -= updatedMainSize - childFlexBasis;

        float childWidth;
        float childHeight;
        ABI14_0_0YGMeasureMode childWidthMeasureMode;
        ABI14_0_0YGMeasureMode childHeightMeasureMode;

        if (isMainAxisRow) {
          childWidth =
              updatedMainSize +
              ABI14_0_0YGNodeMarginForAxis(currentRelativeChild, ABI14_0_0YGFlexDirectionRow, availableInnerWidth);
          childWidthMeasureMode = ABI14_0_0YGMeasureModeExactly;

          if (!ABI14_0_0YGFloatIsUndefined(availableInnerCrossDim) &&
              !ABI14_0_0YGNodeIsStyleDimDefined(currentRelativeChild, ABI14_0_0YGFlexDirectionColumn) &&
              heightMeasureMode == ABI14_0_0YGMeasureModeExactly &&
              ABI14_0_0YGNodeAlignItem(node, currentRelativeChild) == ABI14_0_0YGAlignStretch) {
            childHeight = availableInnerCrossDim;
            childHeightMeasureMode = ABI14_0_0YGMeasureModeExactly;
          } else if (!ABI14_0_0YGNodeIsStyleDimDefined(currentRelativeChild, ABI14_0_0YGFlexDirectionColumn)) {
            childHeight = availableInnerCrossDim;
            childHeightMeasureMode =
                ABI14_0_0YGFloatIsUndefined(childHeight) ? ABI14_0_0YGMeasureModeUndefined : ABI14_0_0YGMeasureModeAtMost;
          } else {
            childHeight = ABI14_0_0YGValueResolve(&currentRelativeChild->style.dimensions[ABI14_0_0YGDimensionHeight],
                                         availableInnerHeight) +
                          ABI14_0_0YGNodeMarginForAxis(currentRelativeChild,
                                              ABI14_0_0YGFlexDirectionColumn,
                                              availableInnerWidth);
            childHeightMeasureMode = ABI14_0_0YGMeasureModeExactly;
          }
        } else {
          childHeight =
              updatedMainSize +
              ABI14_0_0YGNodeMarginForAxis(currentRelativeChild, ABI14_0_0YGFlexDirectionColumn, availableInnerWidth);
          childHeightMeasureMode = ABI14_0_0YGMeasureModeExactly;

          if (!ABI14_0_0YGFloatIsUndefined(availableInnerCrossDim) &&
              !ABI14_0_0YGNodeIsStyleDimDefined(currentRelativeChild, ABI14_0_0YGFlexDirectionRow) &&
              widthMeasureMode == ABI14_0_0YGMeasureModeExactly &&
              ABI14_0_0YGNodeAlignItem(node, currentRelativeChild) == ABI14_0_0YGAlignStretch) {
            childWidth = availableInnerCrossDim;
            childWidthMeasureMode = ABI14_0_0YGMeasureModeExactly;
          } else if (!ABI14_0_0YGNodeIsStyleDimDefined(currentRelativeChild, ABI14_0_0YGFlexDirectionRow)) {
            childWidth = availableInnerCrossDim;
            childWidthMeasureMode =
                ABI14_0_0YGFloatIsUndefined(childWidth) ? ABI14_0_0YGMeasureModeUndefined : ABI14_0_0YGMeasureModeAtMost;
          } else {
            childWidth =
                ABI14_0_0YGValueResolve(&currentRelativeChild->style.dimensions[ABI14_0_0YGDimensionWidth],
                               availableInnerWidth) +
                ABI14_0_0YGNodeMarginForAxis(currentRelativeChild, ABI14_0_0YGFlexDirectionRow, availableInnerWidth);
            childWidthMeasureMode = ABI14_0_0YGMeasureModeExactly;
          }
        }

        if (!ABI14_0_0YGFloatIsUndefined(currentRelativeChild->style.aspectRatio)) {
          if (isMainAxisRow) {
            childHeight = fmaxf(childWidth / currentRelativeChild->style.aspectRatio,
                                ABI14_0_0YGNodePaddingAndBorderForAxis(currentRelativeChild,
                                                              ABI14_0_0YGFlexDirectionColumn,
                                                              availableInnerWidth));
            childHeightMeasureMode = ABI14_0_0YGMeasureModeExactly;

            childHeight = fminf(childHeight, availableInnerHeight);
            childWidth = childHeight * currentRelativeChild->style.aspectRatio;
          } else {
            childWidth = fmaxf(childHeight * currentRelativeChild->style.aspectRatio,
                               ABI14_0_0YGNodePaddingAndBorderForAxis(currentRelativeChild,
                                                             ABI14_0_0YGFlexDirectionRow,
                                                             availableInnerWidth));
            childWidthMeasureMode = ABI14_0_0YGMeasureModeExactly;

            childWidth = fminf(childWidth, availableInnerWidth);
            childHeight = childWidth / currentRelativeChild->style.aspectRatio;
          }
        }

        ABI14_0_0YGConstrainMaxSizeForMode(
            ABI14_0_0YGValueResolve(&currentRelativeChild->style.maxDimensions[ABI14_0_0YGDimensionWidth],
                           availableInnerWidth),
            &childWidthMeasureMode,
            &childWidth);
        ABI14_0_0YGConstrainMaxSizeForMode(
            ABI14_0_0YGValueResolve(&currentRelativeChild->style.maxDimensions[ABI14_0_0YGDimensionHeight],
                           availableInnerHeight),
            &childHeightMeasureMode,
            &childHeight);

        const bool requiresStretchLayout =
            !ABI14_0_0YGNodeIsStyleDimDefined(currentRelativeChild, crossAxis) &&
            ABI14_0_0YGNodeAlignItem(node, currentRelativeChild) == ABI14_0_0YGAlignStretch;

        // Recursively call the layout algorithm for this child with the updated
        // main size.
        ABI14_0_0YGLayoutNodeInternal(currentRelativeChild,
                             childWidth,
                             childHeight,
                             direction,
                             childWidthMeasureMode,
                             childHeightMeasureMode,
                             availableInnerWidth,
                             availableInnerHeight,
                             performLayout && !requiresStretchLayout,
                             "flex");

        currentRelativeChild = currentRelativeChild->nextChild;
      }
    }

    remainingFreeSpace = originalRemainingFreeSpace + deltaFreeSpace;

    // STEP 6: MAIN-AXIS JUSTIFICATION & CROSS-AXIS SIZE DETERMINATION

    // At this point, all the children have their dimensions set in the main
    // axis.
    // Their dimensions are also set in the cross axis with the exception of
    // items
    // that are aligned "stretch". We need to compute these stretch values and
    // set the final positions.

    // If we are using "at most" rules in the main axis. Calculate the remaining space when
    // constraint by the min size defined for the main axis.

    if (measureModeMainDim == ABI14_0_0YGMeasureModeAtMost && remainingFreeSpace > 0) {
      if (node->style.minDimensions[dim[mainAxis]].unit != ABI14_0_0YGUnitUndefined &&
          ABI14_0_0YGValueResolve(&node->style.minDimensions[dim[mainAxis]], mainAxisParentSize) >= 0) {
        remainingFreeSpace =
            fmaxf(0,
                  ABI14_0_0YGValueResolve(&node->style.minDimensions[dim[mainAxis]], mainAxisParentSize) -
                      (availableInnerMainDim - remainingFreeSpace));
      } else {
        remainingFreeSpace = 0;
      }
    }

    switch (justifyContent) {
      case ABI14_0_0YGJustifyCenter:
        leadingMainDim = remainingFreeSpace / 2;
        break;
      case ABI14_0_0YGJustifyFlexEnd:
        leadingMainDim = remainingFreeSpace;
        break;
      case ABI14_0_0YGJustifySpaceBetween:
        if (itemsOnLine > 1) {
          betweenMainDim = fmaxf(remainingFreeSpace, 0) / (itemsOnLine - 1);
        } else {
          betweenMainDim = 0;
        }
        break;
      case ABI14_0_0YGJustifySpaceAround:
        // Space on the edges is half of the space between elements
        betweenMainDim = remainingFreeSpace / itemsOnLine;
        leadingMainDim = betweenMainDim / 2;
        break;
      case ABI14_0_0YGJustifyFlexStart:
        break;
    }

    float mainDim = leadingPaddingAndBorderMain + leadingMainDim;
    float crossDim = 0;

    for (uint32_t i = startOfLineIndex; i < endOfLineIndex; i++) {
      const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeListGet(node->children, i);

      if (child->style.positionType == ABI14_0_0YGPositionTypeAbsolute &&
          ABI14_0_0YGNodeIsLeadingPosDefined(child, mainAxis)) {
        if (performLayout) {
          // In case the child is position absolute and has left/top being
          // defined, we override the position to whatever the user said
          // (and margin/border).
          child->layout.position[pos[mainAxis]] =
              ABI14_0_0YGNodeLeadingPosition(child, mainAxis, availableInnerMainDim) +
              ABI14_0_0YGNodeLeadingBorder(node, mainAxis) +
              ABI14_0_0YGNodeLeadingMargin(child, mainAxis, availableInnerWidth);
        }
      } else {
        // Now that we placed the element, we need to update the variables.
        // We need to do that only for relative elements. Absolute elements
        // do not take part in that phase.
        if (child->style.positionType == ABI14_0_0YGPositionTypeRelative) {
          if (performLayout) {
            child->layout.position[pos[mainAxis]] += mainDim;
          }

          if (canSkipFlex) {
            // If we skipped the flex step, then we can't rely on the
            // measuredDims because
            // they weren't computed. This means we can't call ABI14_0_0YGNodeDimWithMargin.
            mainDim += betweenMainDim + ABI14_0_0YGNodeMarginForAxis(child, mainAxis, availableInnerWidth) +
                       child->layout.computedFlexBasis;
            crossDim = availableInnerCrossDim;
          } else {
            // The main dimension is the sum of all the elements dimension plus
            // the spacing.
            mainDim += betweenMainDim + ABI14_0_0YGNodeDimWithMargin(child, mainAxis, availableInnerWidth);

            // The cross dimension is the max of the elements dimension since
            // there
            // can only be one element in that cross dimension.
            crossDim = fmaxf(crossDim, ABI14_0_0YGNodeDimWithMargin(child, crossAxis, availableInnerWidth));
          }
        } else if (performLayout) {
          child->layout.position[pos[mainAxis]] +=
              ABI14_0_0YGNodeLeadingBorder(node, mainAxis) + leadingMainDim;
        }
      }
    }

    mainDim += trailingPaddingAndBorderMain;

    float containerCrossAxis = availableInnerCrossDim;
    if (measureModeCrossDim == ABI14_0_0YGMeasureModeUndefined ||
        measureModeCrossDim == ABI14_0_0YGMeasureModeAtMost) {
      // Compute the cross axis from the max cross dimension of the children.
      containerCrossAxis = ABI14_0_0YGNodeBoundAxis(node,
                                           crossAxis,
                                           crossDim + paddingAndBorderAxisCross,
                                           crossAxisParentSize,
                                           parentWidth) -
                           paddingAndBorderAxisCross;

      if (measureModeCrossDim == ABI14_0_0YGMeasureModeAtMost) {
        containerCrossAxis = fminf(containerCrossAxis, availableInnerCrossDim);
      }
    }

    // If there's no flex wrap, the cross dimension is defined by the container.
    if (!isNodeFlexWrap && measureModeCrossDim == ABI14_0_0YGMeasureModeExactly) {
      crossDim = availableInnerCrossDim;
    }

    // Clamp to the min/max size specified on the container.
    crossDim = ABI14_0_0YGNodeBoundAxis(node,
                               crossAxis,
                               crossDim + paddingAndBorderAxisCross,
                               crossAxisParentSize,
                               parentWidth) -
               paddingAndBorderAxisCross;

    // STEP 7: CROSS-AXIS ALIGNMENT
    // We can skip child alignment if we're just measuring the container.
    if (performLayout) {
      for (uint32_t i = startOfLineIndex; i < endOfLineIndex; i++) {
        const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeListGet(node->children, i);

        if (child->style.positionType == ABI14_0_0YGPositionTypeAbsolute) {
          // If the child is absolutely positioned and has a
          // top/left/bottom/right
          // set, override all the previously computed positions to set it
          // correctly.
          if (ABI14_0_0YGNodeIsLeadingPosDefined(child, crossAxis)) {
            child->layout.position[pos[crossAxis]] =
                ABI14_0_0YGNodeLeadingPosition(child, crossAxis, availableInnerCrossDim) +
                ABI14_0_0YGNodeLeadingBorder(node, crossAxis) +
                ABI14_0_0YGNodeLeadingMargin(child, crossAxis, availableInnerWidth);
          } else {
            child->layout.position[pos[crossAxis]] =
                ABI14_0_0YGNodeLeadingBorder(node, crossAxis) +
                ABI14_0_0YGNodeLeadingMargin(child, crossAxis, availableInnerWidth);
          }
        } else {
          float leadingCrossDim = leadingPaddingAndBorderCross;

          // For a relative children, we're either using alignItems (parent) or
          // alignSelf (child) in order to determine the position in the cross
          // axis
          const ABI14_0_0YGAlign alignItem = ABI14_0_0YGNodeAlignItem(node, child);

          // If the child uses align stretch, we need to lay it out one more
          // time, this time
          // forcing the cross-axis size to be the computed cross size for the
          // current line.
          if (alignItem == ABI14_0_0YGAlignStretch) {
            const bool isCrossSizeDefinite =
                (isMainAxisRow && ABI14_0_0YGNodeIsStyleDimDefined(child, ABI14_0_0YGFlexDirectionColumn)) ||
                (!isMainAxisRow && ABI14_0_0YGNodeIsStyleDimDefined(child, ABI14_0_0YGFlexDirectionRow));

            float childWidth;
            float childHeight;
            ABI14_0_0YGMeasureMode childWidthMeasureMode = ABI14_0_0YGMeasureModeExactly;
            ABI14_0_0YGMeasureMode childHeightMeasureMode = ABI14_0_0YGMeasureModeExactly;

            if (isMainAxisRow) {
              childWidth = child->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] +
                           ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionRow, availableInnerWidth);

              if (!ABI14_0_0YGFloatIsUndefined(child->style.aspectRatio)) {
                childHeight = childWidth / child->style.aspectRatio;
              } else {
                childHeight = crossDim;
              }
            } else {
              childHeight = child->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] +
                            ABI14_0_0YGNodeMarginForAxis(child, ABI14_0_0YGFlexDirectionColumn, availableInnerWidth);

              if (!ABI14_0_0YGFloatIsUndefined(child->style.aspectRatio)) {
                childWidth = childHeight * child->style.aspectRatio;
              } else {
                childWidth = crossDim;
              }
            }

            ABI14_0_0YGConstrainMaxSizeForMode(ABI14_0_0YGValueResolve(&child->style.maxDimensions[ABI14_0_0YGDimensionWidth],
                                                     availableInnerWidth),
                                      &childWidthMeasureMode,
                                      &childWidth);
            ABI14_0_0YGConstrainMaxSizeForMode(ABI14_0_0YGValueResolve(&child->style.maxDimensions[ABI14_0_0YGDimensionHeight],
                                                     availableInnerHeight),
                                      &childHeightMeasureMode,
                                      &childHeight);

            // If the child defines a definite size for its cross axis, there's
            // no need to stretch.
            if (!isCrossSizeDefinite) {
              childWidthMeasureMode =
                  ABI14_0_0YGFloatIsUndefined(childWidth) ? ABI14_0_0YGMeasureModeUndefined : ABI14_0_0YGMeasureModeExactly;
              childHeightMeasureMode =
                  ABI14_0_0YGFloatIsUndefined(childHeight) ? ABI14_0_0YGMeasureModeUndefined : ABI14_0_0YGMeasureModeExactly;

              ABI14_0_0YGLayoutNodeInternal(child,
                                   childWidth,
                                   childHeight,
                                   direction,
                                   childWidthMeasureMode,
                                   childHeightMeasureMode,
                                   availableInnerWidth,
                                   availableInnerHeight,
                                   true,
                                   "stretch");
            }
          } else if (alignItem != ABI14_0_0YGAlignFlexStart) {
            const float remainingCrossDim =
                containerCrossAxis - ABI14_0_0YGNodeDimWithMargin(child, crossAxis, availableInnerWidth);

            if (alignItem == ABI14_0_0YGAlignCenter) {
              leadingCrossDim += remainingCrossDim / 2;
            } else { // ABI14_0_0YGAlignFlexEnd
              leadingCrossDim += remainingCrossDim;
            }
          }

          // And we apply the position
          child->layout.position[pos[crossAxis]] += totalLineCrossDim + leadingCrossDim;
        }
      }
    }

    totalLineCrossDim += crossDim;
    maxLineMainDim = fmaxf(maxLineMainDim, mainDim);
  }

  // STEP 8: MULTI-LINE CONTENT ALIGNMENT
  if (lineCount > 1 && performLayout && !ABI14_0_0YGFloatIsUndefined(availableInnerCrossDim)) {
    const float remainingAlignContentDim = availableInnerCrossDim - totalLineCrossDim;

    float crossDimLead = 0;
    float currentLead = leadingPaddingAndBorderCross;

    switch (node->style.alignContent) {
      case ABI14_0_0YGAlignFlexEnd:
        currentLead += remainingAlignContentDim;
        break;
      case ABI14_0_0YGAlignCenter:
        currentLead += remainingAlignContentDim / 2;
        break;
      case ABI14_0_0YGAlignStretch:
        if (availableInnerCrossDim > totalLineCrossDim) {
          crossDimLead = (remainingAlignContentDim / lineCount);
        }
        break;
      case ABI14_0_0YGAlignAuto:
      case ABI14_0_0YGAlignFlexStart:
        break;
    }

    uint32_t endIndex = 0;
    for (uint32_t i = 0; i < lineCount; i++) {
      uint32_t startIndex = endIndex;
      uint32_t ii;

      // compute the line's height and find the endIndex
      float lineHeight = 0;
      for (ii = startIndex; ii < childCount; ii++) {
        const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeListGet(node->children, ii);

        if (child->style.positionType == ABI14_0_0YGPositionTypeRelative) {
          if (child->lineIndex != i) {
            break;
          }

          if (ABI14_0_0YGNodeIsLayoutDimDefined(child, crossAxis)) {
            lineHeight = fmaxf(lineHeight,
                               child->layout.measuredDimensions[dim[crossAxis]] +
                                   ABI14_0_0YGNodeMarginForAxis(child, crossAxis, availableInnerWidth));
          }
        }
      }
      endIndex = ii;
      lineHeight += crossDimLead;

      if (performLayout) {
        for (ii = startIndex; ii < endIndex; ii++) {
          const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeListGet(node->children, ii);

          if (child->style.positionType == ABI14_0_0YGPositionTypeRelative) {
            switch (ABI14_0_0YGNodeAlignItem(node, child)) {
              case ABI14_0_0YGAlignFlexStart: {
                child->layout.position[pos[crossAxis]] =
                    currentLead + ABI14_0_0YGNodeLeadingMargin(child, crossAxis, availableInnerWidth);
                break;
              }
              case ABI14_0_0YGAlignFlexEnd: {
                child->layout.position[pos[crossAxis]] =
                    currentLead + lineHeight -
                    ABI14_0_0YGNodeTrailingMargin(child, crossAxis, availableInnerWidth) -
                    child->layout.measuredDimensions[dim[crossAxis]];
                break;
              }
              case ABI14_0_0YGAlignCenter: {
                float childHeight = child->layout.measuredDimensions[dim[crossAxis]];
                child->layout.position[pos[crossAxis]] =
                    currentLead + (lineHeight - childHeight) / 2;
                break;
              }
              case ABI14_0_0YGAlignStretch: {
                child->layout.position[pos[crossAxis]] =
                    currentLead + ABI14_0_0YGNodeLeadingMargin(child, crossAxis, availableInnerWidth);
                // TODO(prenaux): Correctly set the height of items with indefinite
                //                (auto) crossAxis dimension.
                break;
              }
              case ABI14_0_0YGAlignAuto:
                break;
            }
          }
        }
      }

      currentLead += lineHeight;
    }
  }

  // STEP 9: COMPUTING FINAL DIMENSIONS
  node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth] = ABI14_0_0YGNodeBoundAxis(
      node, ABI14_0_0YGFlexDirectionRow, availableWidth - marginAxisRow, parentWidth, parentWidth);
  node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight] = ABI14_0_0YGNodeBoundAxis(
      node, ABI14_0_0YGFlexDirectionColumn, availableHeight - marginAxisColumn, parentHeight, parentWidth);

  // If the user didn't specify a width or height for the node, set the
  // dimensions based on the children.
  if (measureModeMainDim == ABI14_0_0YGMeasureModeUndefined ||
      (node->style.overflow != ABI14_0_0YGOverflowScroll && measureModeMainDim == ABI14_0_0YGMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.
    node->layout.measuredDimensions[dim[mainAxis]] =
        ABI14_0_0YGNodeBoundAxis(node, mainAxis, maxLineMainDim, mainAxisParentSize, parentWidth);
  } else if (measureModeMainDim == ABI14_0_0YGMeasureModeAtMost &&
             node->style.overflow == ABI14_0_0YGOverflowScroll) {
    node->layout.measuredDimensions[dim[mainAxis]] = fmaxf(
        fminf(availableInnerMainDim + paddingAndBorderAxisMain,
              ABI14_0_0YGNodeBoundAxisWithinMinAndMax(node, mainAxis, maxLineMainDim, mainAxisParentSize)),
        paddingAndBorderAxisMain);
  }

  if (measureModeCrossDim == ABI14_0_0YGMeasureModeUndefined ||
      (node->style.overflow != ABI14_0_0YGOverflowScroll && measureModeCrossDim == ABI14_0_0YGMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.
    node->layout.measuredDimensions[dim[crossAxis]] =
        ABI14_0_0YGNodeBoundAxis(node,
                        crossAxis,
                        totalLineCrossDim + paddingAndBorderAxisCross,
                        crossAxisParentSize,
                        parentWidth);
  } else if (measureModeCrossDim == ABI14_0_0YGMeasureModeAtMost &&
             node->style.overflow == ABI14_0_0YGOverflowScroll) {
    node->layout.measuredDimensions[dim[crossAxis]] =
        fmaxf(fminf(availableInnerCrossDim + paddingAndBorderAxisCross,
                    ABI14_0_0YGNodeBoundAxisWithinMinAndMax(node,
                                                   crossAxis,
                                                   totalLineCrossDim + paddingAndBorderAxisCross,
                                                   crossAxisParentSize)),
              paddingAndBorderAxisCross);
  }

  if (performLayout) {
    // STEP 10: SIZING AND POSITIONING ABSOLUTE CHILDREN
    for (currentAbsoluteChild = firstAbsoluteChild; currentAbsoluteChild != NULL;
         currentAbsoluteChild = currentAbsoluteChild->nextChild) {
      ABI14_0_0YGNodeAbsoluteLayoutChild(node,
                                currentAbsoluteChild,
                                availableInnerWidth,
                                widthMeasureMode,
                                availableInnerHeight,
                                direction);
    }

    // STEP 11: SETTING TRAILING POSITIONS FOR CHILDREN
    const bool needsMainTrailingPos =
        mainAxis == ABI14_0_0YGFlexDirectionRowReverse || mainAxis == ABI14_0_0YGFlexDirectionColumnReverse;
    const bool needsCrossTrailingPos =
        crossAxis == ABI14_0_0YGFlexDirectionRowReverse || crossAxis == ABI14_0_0YGFlexDirectionColumnReverse;

    // Set trailing position if necessary.
    if (needsMainTrailingPos || needsCrossTrailingPos) {
      for (uint32_t i = 0; i < childCount; i++) {
        const ABI14_0_0YGNodeRef child = ABI14_0_0YGNodeListGet(node->children, i);

        if (needsMainTrailingPos) {
          ABI14_0_0YGNodeSetChildTrailingPosition(node, child, mainAxis);
        }

        if (needsCrossTrailingPos) {
          ABI14_0_0YGNodeSetChildTrailingPosition(node, child, crossAxis);
        }
      }
    }
  }
}

uint32_t gDepth = 0;
bool gPrintTree = false;
bool gPrintChanges = false;
bool gPrintSkips = false;

static const char *spacer = "                                                            ";

static const char *ABI14_0_0YGSpacer(const unsigned long level) {
  const size_t spacerLen = strlen(spacer);
  if (level > spacerLen) {
    return &spacer[0];
  } else {
    return &spacer[spacerLen - level];
  }
}

static const char *ABI14_0_0YGMeasureModeName(const ABI14_0_0YGMeasureMode mode, const bool performLayout) {
  const char *kMeasureModeNames[ABI14_0_0YGMeasureModeCount] = {"UNDEFINED", "ABI14_0_0EXACTLY", "AT_MOST"};
  const char *kLayoutModeNames[ABI14_0_0YGMeasureModeCount] = {"LAY_UNDEFINED",
                                                      "LAY_EXACTLY",
                                                      "LAY_AT_"
                                                      "MOST"};

  if (mode >= ABI14_0_0YGMeasureModeCount) {
    return "";
  }

  return performLayout ? kLayoutModeNames[mode] : kMeasureModeNames[mode];
}

static inline bool ABI14_0_0YGMeasureModeSizeIsExactAndMatchesOldMeasuredSize(ABI14_0_0YGMeasureMode sizeMode,
                                                                     float size,
                                                                     float lastComputedSize) {
  return sizeMode == ABI14_0_0YGMeasureModeExactly && ABI14_0_0YGFloatsEqual(size, lastComputedSize);
}

static inline bool ABI14_0_0YGMeasureModeOldSizeIsUnspecifiedAndStillFits(ABI14_0_0YGMeasureMode sizeMode,
                                                                 float size,
                                                                 ABI14_0_0YGMeasureMode lastSizeMode,
                                                                 float lastComputedSize) {
  return sizeMode == ABI14_0_0YGMeasureModeAtMost && lastSizeMode == ABI14_0_0YGMeasureModeUndefined &&
         size >= lastComputedSize;
}

static inline bool ABI14_0_0YGMeasureModeNewMeasureSizeIsStricterAndStillValid(ABI14_0_0YGMeasureMode sizeMode,
                                                                      float size,
                                                                      ABI14_0_0YGMeasureMode lastSizeMode,
                                                                      float lastSize,
                                                                      float lastComputedSize) {
  return lastSizeMode == ABI14_0_0YGMeasureModeAtMost && sizeMode == ABI14_0_0YGMeasureModeAtMost &&
         lastSize > size && lastComputedSize <= size;
}

bool ABI14_0_0YGNodeCanUseCachedMeasurement(const ABI14_0_0YGMeasureMode widthMode,
                                   const float width,
                                   const ABI14_0_0YGMeasureMode heightMode,
                                   const float height,
                                   const ABI14_0_0YGMeasureMode lastWidthMode,
                                   const float lastWidth,
                                   const ABI14_0_0YGMeasureMode lastHeightMode,
                                   const float lastHeight,
                                   const float lastComputedWidth,
                                   const float lastComputedHeight,
                                   const float marginRow,
                                   const float marginColumn) {
  if (lastComputedHeight < 0 || lastComputedWidth < 0) {
    return false;
  }

  const bool hasSameWidthSpec = lastWidthMode == widthMode && ABI14_0_0YGFloatsEqual(lastWidth, width);
  const bool hasSameHeightSpec = lastHeightMode == heightMode && ABI14_0_0YGFloatsEqual(lastHeight, height);

  const bool widthIsCompatible =
      hasSameWidthSpec || ABI14_0_0YGMeasureModeSizeIsExactAndMatchesOldMeasuredSize(widthMode,
                                                                            width - marginRow,
                                                                            lastComputedWidth) ||
      ABI14_0_0YGMeasureModeOldSizeIsUnspecifiedAndStillFits(widthMode,
                                                    width - marginRow,
                                                    lastWidthMode,
                                                    lastComputedWidth) ||
      ABI14_0_0YGMeasureModeNewMeasureSizeIsStricterAndStillValid(
          widthMode, width - marginRow, lastWidthMode, lastWidth, lastComputedWidth);

  const bool heightIsCompatible =
      hasSameHeightSpec || ABI14_0_0YGMeasureModeSizeIsExactAndMatchesOldMeasuredSize(heightMode,
                                                                             height - marginColumn,
                                                                             lastComputedHeight) ||
      ABI14_0_0YGMeasureModeOldSizeIsUnspecifiedAndStillFits(heightMode,
                                                    height - marginColumn,
                                                    lastHeightMode,
                                                    lastComputedHeight) ||
      ABI14_0_0YGMeasureModeNewMeasureSizeIsStricterAndStillValid(
          heightMode, height - marginColumn, lastHeightMode, lastHeight, lastComputedHeight);

  return widthIsCompatible && heightIsCompatible;
}

//
// This is a wrapper around the ABI14_0_0YGNodelayoutImpl function. It determines
// whether the layout request is redundant and can be skipped.
//
// Parameters:
//  Input parameters are the same as ABI14_0_0YGNodelayoutImpl (see above)
//  Return parameter is true if layout was performed, false if skipped
//
bool ABI14_0_0YGLayoutNodeInternal(const ABI14_0_0YGNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const ABI14_0_0YGDirection parentDirection,
                          const ABI14_0_0YGMeasureMode widthMeasureMode,
                          const ABI14_0_0YGMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason) {
  ABI14_0_0YGLayout *layout = &node->layout;

  gDepth++;

  const bool needToVisitNode =
      (node->isDirty && layout->generationCount != gCurrentGenerationCount) ||
      layout->lastParentDirection != parentDirection;

  if (needToVisitNode) {
    // Invalidate the cached results.
    layout->nextCachedMeasurementsIndex = 0;
    layout->cachedLayout.widthMeasureMode = (ABI14_0_0YGMeasureMode) -1;
    layout->cachedLayout.heightMeasureMode = (ABI14_0_0YGMeasureMode) -1;
    layout->cachedLayout.computedWidth = -1;
    layout->cachedLayout.computedHeight = -1;
  }

  ABI14_0_0YGCachedMeasurement *cachedResults = NULL;

  // Determine whether the results are already cached. We maintain a separate
  // cache for layouts and measurements. A layout operation modifies the
  // positions
  // and dimensions for nodes in the subtree. The algorithm assumes that each
  // node
  // gets layed out a maximum of one time per tree layout, but multiple
  // measurements
  // may be required to resolve all of the flex dimensions.
  // We handle nodes with measure functions specially here because they are the
  // most
  // expensive to measure, so it's worth avoiding redundant measurements if at
  // all possible.
  if (node->measure) {
    const float marginAxisRow = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionRow, parentWidth);
    const float marginAxisColumn = ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionColumn, parentWidth);

    // First, try to use the layout cache.
    if (ABI14_0_0YGNodeCanUseCachedMeasurement(widthMeasureMode,
                                      availableWidth,
                                      heightMeasureMode,
                                      availableHeight,
                                      layout->cachedLayout.widthMeasureMode,
                                      layout->cachedLayout.availableWidth,
                                      layout->cachedLayout.heightMeasureMode,
                                      layout->cachedLayout.availableHeight,
                                      layout->cachedLayout.computedWidth,
                                      layout->cachedLayout.computedHeight,
                                      marginAxisRow,
                                      marginAxisColumn)) {
      cachedResults = &layout->cachedLayout;
    } else {
      // Try to use the measurement cache.
      for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
        if (ABI14_0_0YGNodeCanUseCachedMeasurement(widthMeasureMode,
                                          availableWidth,
                                          heightMeasureMode,
                                          availableHeight,
                                          layout->cachedMeasurements[i].widthMeasureMode,
                                          layout->cachedMeasurements[i].availableWidth,
                                          layout->cachedMeasurements[i].heightMeasureMode,
                                          layout->cachedMeasurements[i].availableHeight,
                                          layout->cachedMeasurements[i].computedWidth,
                                          layout->cachedMeasurements[i].computedHeight,
                                          marginAxisRow,
                                          marginAxisColumn)) {
          cachedResults = &layout->cachedMeasurements[i];
          break;
        }
      }
    }
  } else if (performLayout) {
    if (ABI14_0_0YGFloatsEqual(layout->cachedLayout.availableWidth, availableWidth) &&
        ABI14_0_0YGFloatsEqual(layout->cachedLayout.availableHeight, availableHeight) &&
        layout->cachedLayout.widthMeasureMode == widthMeasureMode &&
        layout->cachedLayout.heightMeasureMode == heightMeasureMode) {
      cachedResults = &layout->cachedLayout;
    }
  } else {
    for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
      if (ABI14_0_0YGFloatsEqual(layout->cachedMeasurements[i].availableWidth, availableWidth) &&
          ABI14_0_0YGFloatsEqual(layout->cachedMeasurements[i].availableHeight, availableHeight) &&
          layout->cachedMeasurements[i].widthMeasureMode == widthMeasureMode &&
          layout->cachedMeasurements[i].heightMeasureMode == heightMeasureMode) {
        cachedResults = &layout->cachedMeasurements[i];
        break;
      }
    }
  }

  if (!needToVisitNode && cachedResults != NULL) {
    layout->measuredDimensions[ABI14_0_0YGDimensionWidth] = cachedResults->computedWidth;
    layout->measuredDimensions[ABI14_0_0YGDimensionHeight] = cachedResults->computedHeight;

    if (gPrintChanges && gPrintSkips) {
      printf("%s%d.{[skipped] ", ABI14_0_0YGSpacer(gDepth), gDepth);
      if (node->print) {
        node->print(node);
      }
      printf("wm: %s, hm: %s, aw: %f ah: %f => d: (%f, %f) %s\n",
             ABI14_0_0YGMeasureModeName(widthMeasureMode, performLayout),
             ABI14_0_0YGMeasureModeName(heightMeasureMode, performLayout),
             availableWidth,
             availableHeight,
             cachedResults->computedWidth,
             cachedResults->computedHeight,
             reason);
    }
  } else {
    if (gPrintChanges) {
      printf("%s%d.{%s", ABI14_0_0YGSpacer(gDepth), gDepth, needToVisitNode ? "*" : "");
      if (node->print) {
        node->print(node);
      }
      printf("wm: %s, hm: %s, aw: %f ah: %f %s\n",
             ABI14_0_0YGMeasureModeName(widthMeasureMode, performLayout),
             ABI14_0_0YGMeasureModeName(heightMeasureMode, performLayout),
             availableWidth,
             availableHeight,
             reason);
    }

    ABI14_0_0YGNodelayoutImpl(node,
                     availableWidth,
                     availableHeight,
                     parentDirection,
                     widthMeasureMode,
                     heightMeasureMode,
                     parentWidth,
                     parentHeight,
                     performLayout);

    if (gPrintChanges) {
      printf("%s%d.}%s", ABI14_0_0YGSpacer(gDepth), gDepth, needToVisitNode ? "*" : "");
      if (node->print) {
        node->print(node);
      }
      printf("wm: %s, hm: %s, d: (%f, %f) %s\n",
             ABI14_0_0YGMeasureModeName(widthMeasureMode, performLayout),
             ABI14_0_0YGMeasureModeName(heightMeasureMode, performLayout),
             layout->measuredDimensions[ABI14_0_0YGDimensionWidth],
             layout->measuredDimensions[ABI14_0_0YGDimensionHeight],
             reason);
    }

    layout->lastParentDirection = parentDirection;

    if (cachedResults == NULL) {
      if (layout->nextCachedMeasurementsIndex == ABI14_0_0YG_MAX_CACHED_RESULT_COUNT) {
        if (gPrintChanges) {
          printf("Out of cache entries!\n");
        }
        layout->nextCachedMeasurementsIndex = 0;
      }

      ABI14_0_0YGCachedMeasurement *newCacheEntry;
      if (performLayout) {
        // Use the single layout cache entry.
        newCacheEntry = &layout->cachedLayout;
      } else {
        // Allocate a new measurement cache entry.
        newCacheEntry = &layout->cachedMeasurements[layout->nextCachedMeasurementsIndex];
        layout->nextCachedMeasurementsIndex++;
      }

      newCacheEntry->availableWidth = availableWidth;
      newCacheEntry->availableHeight = availableHeight;
      newCacheEntry->widthMeasureMode = widthMeasureMode;
      newCacheEntry->heightMeasureMode = heightMeasureMode;
      newCacheEntry->computedWidth = layout->measuredDimensions[ABI14_0_0YGDimensionWidth];
      newCacheEntry->computedHeight = layout->measuredDimensions[ABI14_0_0YGDimensionHeight];
    }
  }

  if (performLayout) {
    node->layout.dimensions[ABI14_0_0YGDimensionWidth] = node->layout.measuredDimensions[ABI14_0_0YGDimensionWidth];
    node->layout.dimensions[ABI14_0_0YGDimensionHeight] = node->layout.measuredDimensions[ABI14_0_0YGDimensionHeight];
    node->hasNewLayout = true;
    node->isDirty = false;
  }

  gDepth--;
  layout->generationCount = gCurrentGenerationCount;
  return (needToVisitNode || cachedResults == NULL);
}

static void roundToPixelGrid(const ABI14_0_0YGNodeRef node) {
  const float fractialLeft =
      node->layout.position[ABI14_0_0YGEdgeLeft] - floorf(node->layout.position[ABI14_0_0YGEdgeLeft]);
  const float fractialTop =
      node->layout.position[ABI14_0_0YGEdgeTop] - floorf(node->layout.position[ABI14_0_0YGEdgeTop]);
  node->layout.dimensions[ABI14_0_0YGDimensionWidth] =
      roundf(fractialLeft + node->layout.dimensions[ABI14_0_0YGDimensionWidth]) - roundf(fractialLeft);
  node->layout.dimensions[ABI14_0_0YGDimensionHeight] =
      roundf(fractialTop + node->layout.dimensions[ABI14_0_0YGDimensionHeight]) - roundf(fractialTop);

  node->layout.position[ABI14_0_0YGEdgeLeft] = roundf(node->layout.position[ABI14_0_0YGEdgeLeft]);
  node->layout.position[ABI14_0_0YGEdgeTop] = roundf(node->layout.position[ABI14_0_0YGEdgeTop]);

  const uint32_t childCount = ABI14_0_0YGNodeListCount(node->children);
  for (uint32_t i = 0; i < childCount; i++) {
    roundToPixelGrid(ABI14_0_0YGNodeGetChild(node, i));
  }
}

void ABI14_0_0YGNodeCalculateLayout(const ABI14_0_0YGNodeRef node,
                           const float availableWidth,
                           const float availableHeight,
                           const ABI14_0_0YGDirection parentDirection) {
  // Increment the generation count. This will force the recursive routine to
  // visit
  // all dirty nodes at least once. Subsequent visits will be skipped if the
  // input
  // parameters don't change.
  gCurrentGenerationCount++;

  float width = availableWidth;
  float height = availableHeight;
  ABI14_0_0YGMeasureMode widthMeasureMode = ABI14_0_0YGMeasureModeUndefined;
  ABI14_0_0YGMeasureMode heightMeasureMode = ABI14_0_0YGMeasureModeUndefined;

  if (!ABI14_0_0YGFloatIsUndefined(width)) {
    widthMeasureMode = ABI14_0_0YGMeasureModeExactly;
  } else if (ABI14_0_0YGNodeIsStyleDimDefined(node, ABI14_0_0YGFlexDirectionRow)) {
    width = ABI14_0_0YGValueResolve(&node->style.dimensions[dim[ABI14_0_0YGFlexDirectionRow]], availableWidth) +
            ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionRow, availableWidth);
    widthMeasureMode = ABI14_0_0YGMeasureModeExactly;
  } else if (ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionWidth], availableWidth) >= 0.0f) {
    width = ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionWidth], availableWidth);
    widthMeasureMode = ABI14_0_0YGMeasureModeAtMost;
  }

  if (!ABI14_0_0YGFloatIsUndefined(height)) {
    heightMeasureMode = ABI14_0_0YGMeasureModeExactly;
  } else if (ABI14_0_0YGNodeIsStyleDimDefined(node, ABI14_0_0YGFlexDirectionColumn)) {
    height = ABI14_0_0YGValueResolve(&node->style.dimensions[dim[ABI14_0_0YGFlexDirectionColumn]], availableHeight) +
             ABI14_0_0YGNodeMarginForAxis(node, ABI14_0_0YGFlexDirectionColumn, availableWidth);
    heightMeasureMode = ABI14_0_0YGMeasureModeExactly;
  } else if (ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionHeight], availableHeight) >=
             0.0f) {
    height = ABI14_0_0YGValueResolve(&node->style.maxDimensions[ABI14_0_0YGDimensionHeight], availableHeight);
    heightMeasureMode = ABI14_0_0YGMeasureModeAtMost;
  }

  if (ABI14_0_0YGLayoutNodeInternal(node,
                           width,
                           height,
                           parentDirection,
                           widthMeasureMode,
                           heightMeasureMode,
                           availableWidth,
                           availableHeight,
                           true,
                           "initia"
                           "l")) {
    ABI14_0_0YGNodeSetPosition(node, node->layout.direction, availableWidth, availableHeight, availableWidth);

    if (ABI14_0_0YGIsExperimentalFeatureEnabled(ABI14_0_0YGExperimentalFeatureRounding)) {
      roundToPixelGrid(node);
    }

    if (gPrintTree) {
      ABI14_0_0YGNodePrint(node, ABI14_0_0YGPrintOptionsLayout | ABI14_0_0YGPrintOptionsChildren | ABI14_0_0YGPrintOptionsStyle);
    }
  }
}

void ABI14_0_0YGSetLogger(ABI14_0_0YGLogger logger) {
  gLogger = logger;
}

void ABI14_0_0YGLog(ABI14_0_0YGLogLevel level, const char *format, ...) {
  va_list args;
  va_start(args, format);
  gLogger(level, format, args);
  va_end(args);
}

static bool experimentalFeatures[ABI14_0_0YGExperimentalFeatureCount + 1];

void ABI14_0_0YGSetExperimentalFeatureEnabled(ABI14_0_0YGExperimentalFeature feature, bool enabled) {
  experimentalFeatures[feature] = enabled;
}

inline bool ABI14_0_0YGIsExperimentalFeatureEnabled(ABI14_0_0YGExperimentalFeature feature) {
  return experimentalFeatures[feature];
}

void ABI14_0_0YGSetMemoryFuncs(ABI14_0_0YGMalloc ygmalloc, ABI14_0_0YGCalloc yccalloc, ABI14_0_0YGRealloc ygrealloc, ABI14_0_0YGFree ygfree) {
  ABI14_0_0YG_ASSERT(gNodeInstanceCount == 0, "Cannot set memory functions: all node must be freed first");
  ABI14_0_0YG_ASSERT((ygmalloc == NULL && yccalloc == NULL && ygrealloc == NULL && ygfree == NULL) ||
                (ygmalloc != NULL && yccalloc != NULL && ygrealloc != NULL && ygfree != NULL),
            "Cannot set memory functions: functions must be all NULL or Non-NULL");

  if (ygmalloc == NULL || yccalloc == NULL || ygrealloc == NULL || ygfree == NULL) {
    gABI14_0_0YGMalloc = &malloc;
    gABI14_0_0YGCalloc = &calloc;
    gABI14_0_0YGRealloc = &realloc;
    gABI14_0_0YGFree = &free;
  } else {
    gABI14_0_0YGMalloc = ygmalloc;
    gABI14_0_0YGCalloc = yccalloc;
    gABI14_0_0YGRealloc = ygrealloc;
    gABI14_0_0YGFree = ygfree;
  }
}
