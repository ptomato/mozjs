/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: set ts=8 sts=2 et sw=2 tw=80:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef js_TracingAPI_h
#define js_TracingAPI_h

#include "js/GCTypeMacros.h"
#include "js/HeapAPI.h"
#include "js/TraceKind.h"

class JS_PUBLIC_API JSTracer;

namespace JS {
class JS_PUBLIC_API CallbackTracer;
template <typename T>
class Heap;
template <typename T>
class TenuredHeap;

/** Returns a static string equivalent of |kind|. */
JS_PUBLIC_API const char* GCTraceKindToAscii(JS::TraceKind kind);

/** Returns the base size in bytes of the GC thing of kind |kind|. */
JS_PUBLIC_API size_t GCTraceKindSize(JS::TraceKind kind);

// Kinds of JSTracer.
enum class TracerKind {
  // Marking path: a tracer used only for marking liveness of cells, not
  // for moving them.
  Marking,

  // Generic tracers: Internal tracers that have a different virtual method
  // called for each edge kind.
  //
  // Order is important. All generic kinds must follow this one.
  Generic,

  // Specific kinds of generic tracer.
  Tenuring,
  Moving,
  GrayBuffering,
  ClearEdges,
  Sweeping,
  MinorSweeping,
  Barrier,

  // Callback tracers: General-purpose tracers that have a single virtual
  // method called on every edge.
  //
  // Order is important. All callback kinds must follow this one.
  Callback,

  // Specific kinds of callback tracer.
  UnmarkGray,
  VerifyTraceProtoAndIface,
  CompartmentCheck,
};

enum class WeakMapTraceAction {
  /**
   * Do not trace into weak map keys or values during traversal. Users must
   * handle weak maps manually.
   */
  Skip,

  /**
   * Do true ephemeron marking with a weak key lookup marking phase. This is
   * the default for GCMarker.
   */
  Expand,

  /**
   * Trace through to all values, irrespective of whether the keys are live
   * or not. Used for non-marking tracers.
   */
  TraceValues,

  /**
   * Trace through to all keys and values, irrespective of whether the keys
   * are live or not. Used for non-marking tracers.
   */
  TraceKeysAndValues
};

// Whether a tracer should trace weak edges. GCMarker sets this to Skip.
enum class WeakEdgeTraceAction { Skip, Trace };

struct TraceOptions {
  JS::WeakMapTraceAction weakMapAction = WeakMapTraceAction::TraceValues;
  JS::WeakEdgeTraceAction weakEdgeAction = WeakEdgeTraceAction::Trace;

  TraceOptions() = default;
  TraceOptions(JS::WeakMapTraceAction weakMapActionArg,
               JS::WeakEdgeTraceAction weakEdgeActionArg)
      : weakMapAction(weakMapActionArg), weakEdgeAction(weakEdgeActionArg) {}
  MOZ_IMPLICIT TraceOptions(JS::WeakMapTraceAction weakMapActionArg)
      : weakMapAction(weakMapActionArg) {}
  MOZ_IMPLICIT TraceOptions(JS::WeakEdgeTraceAction weakEdgeActionArg)
      : weakEdgeAction(weakEdgeActionArg) {}
};

class AutoTracingName;
class AutoTracingIndex;

// Optional context information that can be used to construct human readable
// descriptions of what is being traced.
class TracingContext {
 public:
  // Access to the tracing context: When tracing with a JS::CallbackTracer, we
  // invoke the callback with the edge location and the type of target. This is
  // useful for operating on the edge in the abstract or on the target thing,
  // satisfying most common use cases.  However, some tracers need additional
  // detail about the specific edge that is being traced in order to be
  // useful. Unfortunately, the raw pointer to the edge that we provide is not
  // enough information to infer much of anything useful about that edge.
  //
  // In order to better support use cases that care in particular about edges --
  // as opposed to the target thing -- tracing implementations are responsible
  // for providing extra context information about each edge they trace, as it
  // is traced. This contains, at a minimum, an edge name and, when tracing an
  // array, the index. Further specialization can be achieved (with some
  // complexity), by associating a functor with the tracer so that, when
  // requested, the user can generate totally custom edge descriptions.

  // Returns the current edge's name. It is only valid to call this when
  // inside the trace callback, however, the edge name will always be set.
  const char* name() const {
    MOZ_ASSERT(name_);
    return name_;
  }

  // Returns the current edge's index, if marked as part of an array of edges.
  // This must be called only inside the trace callback. When not tracing an
  // array, the value will be InvalidIndex.
  constexpr static size_t InvalidIndex = size_t(-1);
  size_t index() const { return index_; }

  // Build a description of this edge in the heap graph. This call may invoke
  // the context functor, if set, which may inspect arbitrary areas of the
  // heap. On the other hand, the description provided by this method may be
  // substantially more accurate and useful than those provided by only the
  // name and index.
  void getEdgeName(char* buffer, size_t bufferSize);

  // The trace implementation may associate a callback with one or more edges
  // using AutoTracingDetails. This functor is called by getEdgeName and
  // is responsible for providing a textual representation of the edge currently
  // being traced. The callback has access to the full heap, including the
  // currently set tracing context.
  class Functor {
   public:
    virtual void operator()(TracingContext* tcx, char* buf, size_t bufsize) = 0;
  };

 private:
  friend class AutoTracingName;
  const char* name_ = nullptr;

  friend class AutoTracingIndex;
  size_t index_ = InvalidIndex;

  friend class AutoTracingDetails;
  Functor* functor_ = nullptr;
};

}  // namespace JS

namespace js {
class GenericTracer;
}  // namespace js

class JS_PUBLIC_API JSTracer {
 public:
  // Return the runtime set on the tracer.
  JSRuntime* runtime() const { return runtime_; }

  JS::TracerKind kind() const { return kind_; }
  bool isMarkingTracer() const { return kind_ == JS::TracerKind::Marking; }
  bool isTenuringTracer() const { return kind_ == JS::TracerKind::Tenuring; }
  bool isGenericTracer() const { return kind_ >= JS::TracerKind::Generic; }
  bool isCallbackTracer() const { return kind_ >= JS::TracerKind::Callback; }

  inline js::GenericTracer* asGenericTracer();
  inline JS::CallbackTracer* asCallbackTracer();

  JS::WeakMapTraceAction weakMapAction() const {
    return options_.weakMapAction;
  }
  bool traceWeakEdges() const {
    return options_.weakEdgeAction == JS::WeakEdgeTraceAction::Trace;
  }

  JS::TracingContext& context() { return context_; }

  // Get the current GC number. Only call this method if |isMarkingTracer()|
  // is true.
  uint32_t gcNumberForMarking() const;

 protected:
  JSTracer(JSRuntime* rt, JS::TracerKind kind,
           JS::TraceOptions options = JS::TraceOptions())
      : runtime_(rt), kind_(kind), options_(options) {}

 private:
  JSRuntime* const runtime_;
  const JS::TracerKind kind_;
  const JS::TraceOptions options_;
  JS::TracingContext context_;
};

namespace js {

class GenericTracer : public JSTracer {
 public:
  GenericTracer(JSRuntime* rt, JS::TracerKind kind = JS::TracerKind::Generic,
                JS::TraceOptions options = JS::TraceOptions())
      : JSTracer(rt, kind, options) {
    MOZ_ASSERT(isGenericTracer());
  }

  // These methods are called when the tracer encounters an edge. Clients should
  // override them to receive notifications when an edge of each type is
  // visited.
  //
  // The caller updates the edge with the return value (if different).
  //
  // In C++, overriding a method hides all methods in the base class with that
  // name, not just methods with that signature. Thus, the typed edge methods
  // have to have distinct names to allow us to override them individually,
  // which is freqently useful if, for example, we only want to process one type
  // of edge.
  virtual JSObject* onObjectEdge(JSObject* obj) = 0;
  virtual JSString* onStringEdge(JSString* str) = 0;
  virtual JS::Symbol* onSymbolEdge(JS::Symbol* sym) = 0;
  virtual JS::BigInt* onBigIntEdge(JS::BigInt* bi) = 0;
  virtual js::BaseScript* onScriptEdge(js::BaseScript* script) = 0;
  virtual js::Shape* onShapeEdge(js::Shape* shape) = 0;
  virtual js::RegExpShared* onRegExpSharedEdge(js::RegExpShared* shared) = 0;
  virtual js::GetterSetter* onGetterSetterEdge(js::GetterSetter* gs) = 0;
  virtual js::PropMap* onPropMapEdge(js::PropMap* map) = 0;
  virtual js::BaseShape* onBaseShapeEdge(js::BaseShape* base) = 0;
  virtual js::jit::JitCode* onJitCodeEdge(js::jit::JitCode* code) = 0;
  virtual js::Scope* onScopeEdge(js::Scope* scope) = 0;
};

// A helper class that implements a GenericTracer by calling template method
// on a derived type for each edge kind.
template <typename T>
class GenericTracerImpl : public GenericTracer {
 public:
  GenericTracerImpl(JSRuntime* rt, JS::TracerKind kind,
                    JS::TraceOptions options)
      : GenericTracer(rt, kind, options) {}

 private:
  T* derived() { return static_cast<T*>(this); }

  JSObject* onObjectEdge(JSObject* obj) override {
    return derived()->onEdge(obj);
  }
  Shape* onShapeEdge(Shape* shape) override { return derived()->onEdge(shape); }
  JSString* onStringEdge(JSString* string) override {
    return derived()->onEdge(string);
  }
  BaseScript* onScriptEdge(BaseScript* script) override {
    return derived()->onEdge(script);
  }
  BaseShape* onBaseShapeEdge(BaseShape* base) override {
    return derived()->onEdge(base);
  }
  GetterSetter* onGetterSetterEdge(GetterSetter* gs) override {
    return derived()->onEdge(gs);
  }
  PropMap* onPropMapEdge(PropMap* map) override {
    return derived()->onEdge(map);
  }
  Scope* onScopeEdge(Scope* scope) override { return derived()->onEdge(scope); }
  RegExpShared* onRegExpSharedEdge(RegExpShared* shared) override {
    return derived()->onEdge(shared);
  }
  JS::BigInt* onBigIntEdge(JS::BigInt* bi) override {
    return derived()->onEdge(bi);
  }
  JS::Symbol* onSymbolEdge(JS::Symbol* sym) override {
    return derived()->onEdge(sym);
  }
  jit::JitCode* onJitCodeEdge(jit::JitCode* jit) override {
    return derived()->onEdge(jit);
  }
};

}  // namespace js

namespace JS {

class JS_PUBLIC_API CallbackTracer
    : public js::GenericTracerImpl<CallbackTracer> {
 public:
  CallbackTracer(JSRuntime* rt, JS::TracerKind kind = JS::TracerKind::Callback,
                 JS::TraceOptions options = JS::TraceOptions())
      : GenericTracerImpl(rt, kind, options) {
    MOZ_ASSERT(isCallbackTracer());
  }
  CallbackTracer(JSContext* cx, JS::TracerKind kind = JS::TracerKind::Callback,
                 JS::TraceOptions options = JS::TraceOptions());

  // Override this method to receive notification when a node in the GC
  // heap graph is visited.
  virtual void onChild(JS::GCCellPtr thing) = 0;

 private:
  template <typename T>
  T* onEdge(T* thing) {
    onChild(JS::GCCellPtr(thing));
    return thing;
  }
  friend class js::GenericTracerImpl<CallbackTracer>;
};

// Set the name portion of the tracer's context for the current edge.
class MOZ_RAII AutoTracingName {
  JSTracer* trc_;

 public:
  AutoTracingName(JSTracer* trc, const char* name) : trc_(trc) {
    MOZ_ASSERT(name);
    MOZ_ASSERT(!trc_->context().name_);
    trc_->context().name_ = name;
  }
  ~AutoTracingName() {
    MOZ_ASSERT(trc_->context().name_);
    trc_->context().name_ = nullptr;
  }
};

// Set the index portion of the tracer's context for the current range.
class MOZ_RAII AutoTracingIndex {
  JSTracer* trc_;

 public:
  explicit AutoTracingIndex(JSTracer* trc, size_t initial = 0) : trc_(trc) {
    MOZ_ASSERT(trc_->context().index_ == TracingContext::InvalidIndex);
    trc_->context().index_ = initial;
  }
  ~AutoTracingIndex() {
    MOZ_ASSERT(trc_->context().index_ != TracingContext::InvalidIndex);
    trc_->context().index_ = TracingContext::InvalidIndex;
  }

  void operator++() {
    MOZ_ASSERT(trc_->context().index_ != TracingContext::InvalidIndex);
    ++trc_->context().index_;
  }
};

// Set a context callback for the trace callback to use, if it needs a detailed
// edge description.
class MOZ_RAII AutoTracingDetails {
  JSTracer* trc_;

 public:
  AutoTracingDetails(JSTracer* trc, TracingContext::Functor& func) : trc_(trc) {
    MOZ_ASSERT(trc_->context().functor_ == nullptr);
    trc_->context().functor_ = &func;
  }
  ~AutoTracingDetails() {
    MOZ_ASSERT(trc_->context().functor_);
    trc_->context().functor_ = nullptr;
  }
};

// Save and clear tracing context when performing nested tracing.
class MOZ_RAII AutoClearTracingContext {
  JSTracer* trc_;
  TracingContext prev_;

 public:
  explicit AutoClearTracingContext(JSTracer* trc)
      : trc_(trc), prev_(trc->context()) {
    trc_->context() = TracingContext();
  }

  ~AutoClearTracingContext() { trc_->context() = prev_; }
};

}  // namespace JS

js::GenericTracer* JSTracer::asGenericTracer() {
  MOZ_ASSERT(isGenericTracer());
  return static_cast<js::GenericTracer*>(this);
}

JS::CallbackTracer* JSTracer::asCallbackTracer() {
  MOZ_ASSERT(isCallbackTracer());
  return static_cast<JS::CallbackTracer*>(this);
}

namespace js {

class AbstractGeneratorObject;
class SavedFrame;

namespace gc {

#define JS_DECLARE_TRACE_EXTERNAL_EDGE(type)                               \
  extern JS_PUBLIC_API void TraceExternalEdge(JSTracer* trc, type* thingp, \
                                              const char* name);

// Declare edge-tracing function overloads for public GC pointer types.
JS_FOR_EACH_PUBLIC_GC_POINTER_TYPE(JS_DECLARE_TRACE_EXTERNAL_EDGE)
JS_FOR_EACH_PUBLIC_TAGGED_GC_POINTER_TYPE(JS_DECLARE_TRACE_EXTERNAL_EDGE)

#undef JS_DECLARE_TRACE_EXTERNAL_EDGE

}  // namespace gc
}  // namespace js

namespace JS {

// The JS::TraceEdge family of functions traces the given GC thing reference.
// This performs the tracing action configured on the given JSTracer: typically
// calling the JSTracer::callback or marking the thing as live.
//
// The argument to JS::TraceEdge is an in-out param: when the function returns,
// the garbage collector might have moved the GC thing. In this case, the
// reference passed to JS::TraceEdge will be updated to the thing's new
// location. Callers of this method are responsible for updating any state that
// is dependent on the object's address. For example, if the object's address
// is used as a key in a hashtable, then the object must be removed and
// re-inserted with the correct hash.
//
// Note that while |edgep| must never be null, it is fine for |*edgep| to be
// nullptr.

template <typename T>
inline void TraceEdge(JSTracer* trc, JS::Heap<T>* thingp, const char* name) {
  MOZ_ASSERT(thingp);
  if (*thingp) {
    js::gc::TraceExternalEdge(trc, thingp->unsafeGet(), name);
  }
}

template <typename T>
inline void TraceEdge(JSTracer* trc, JS::TenuredHeap<T>* thingp,
                      const char* name) {
  MOZ_ASSERT(thingp);
  if (T ptr = thingp->unbarrieredGetPtr()) {
    js::gc::TraceExternalEdge(trc, &ptr, name);
    thingp->setPtr(ptr);
  }
}

// Edges that are always traced as part of root marking do not require
// incremental barriers. |JS::TraceRoot| overloads allow for marking
// non-barriered pointers but assert that this happens during root marking.
//
// Note that while |edgep| must never be null, it is fine for |*edgep| to be
// nullptr.
#define JS_DECLARE_TRACE_ROOT(type)                               \
  extern JS_PUBLIC_API void TraceRoot(JSTracer* trc, type* edgep, \
                                      const char* name);

// Declare edge-tracing function overloads for public GC pointer types.
JS_FOR_EACH_PUBLIC_GC_POINTER_TYPE(JS_DECLARE_TRACE_ROOT)
JS_FOR_EACH_PUBLIC_TAGGED_GC_POINTER_TYPE(JS_DECLARE_TRACE_ROOT)

// We also require overloads for these purely-internal types.  These overloads
// ought not be in public headers, and they should use a different name in order
// to not be *actual* overloads, but for the moment we still declare them here.
JS_DECLARE_TRACE_ROOT(js::AbstractGeneratorObject*)
JS_DECLARE_TRACE_ROOT(js::SavedFrame*)

#undef JS_DECLARE_TRACE_ROOT

extern JS_PUBLIC_API void TraceChildren(JSTracer* trc, GCCellPtr thing);

}  // namespace JS

namespace js {

inline bool IsTracerKind(JSTracer* trc, JS::TracerKind kind) {
  return trc->kind() == kind;
}

// Trace an edge that is not a GC root and is not wrapped in a barriered
// wrapper for some reason.
//
// This method does not check if |*edgep| is non-null before tracing through
// it, so callers must check any nullable pointer before calling this method.
extern JS_PUBLIC_API void UnsafeTraceManuallyBarrieredEdge(JSTracer* trc,
                                                           JSObject** edgep,
                                                           const char* name);

namespace gc {

// Return true if the given edge is not live and is about to be swept.
template <typename T>
extern JS_PUBLIC_API bool TraceWeakEdge(JSTracer* trc, JS::Heap<T>* thingp);

}  // namespace gc

#ifdef DEBUG
/*
 * Return whether the runtime is currently being destroyed, for use in
 * assertions.
 */
extern JS_PUBLIC_API bool RuntimeIsBeingDestroyed();
#endif

}  // namespace js

#endif /* js_TracingAPI_h */
