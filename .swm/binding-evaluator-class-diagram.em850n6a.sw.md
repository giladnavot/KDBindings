---
title: Binding Evaluator class Diagram
---
# Introduction

This document will walk you through the design and implementation of the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> class in the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="22:7:7" line-data=" * when a KDBindings::Binding is reevaluated.">`KDBindings`</SwmToken> library. The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> is a crucial component for managing when bindings are reevaluated, providing control over the timing of these evaluations.

We will cover:

1. Why the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> class was designed to manage binding reevaluation.
2. The decision to use a private implementation (pimpl) pattern.
3. The rationale behind the copy and move semantics of the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken>.
4. The purpose of the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="123:17:17" line-data=" * Any Binding that is constructed with an ImmediateBindingEvaluator will not wait">`ImmediateBindingEvaluator`</SwmToken> subclass.

# <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> class overview

<SwmSnippet path="/src/kdbindings/binding_evaluator.h" line="20">

---

The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> class is designed to control the exact timing of when a <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="22:7:9" line-data=" * when a KDBindings::Binding is reevaluated.">`KDBindings::Binding`</SwmToken> is reevaluated. This is particularly useful in scenarios where performance is a concern, as it allows bindings to be reevaluated only when necessary.

```
/**
 * @brief A BindingEvaluator provides a mechanism to control the exact time
 * when a KDBindings::Binding is reevaluated.
 *
 * A BindingEvaluator represents a collection of Binding instances that can be
 * selectively reevaluated.
 *
 * If a Binding is created using KDBindings::makeBoundProperty with a BindingEvaluator,
 * the Binding will only be evaluated if BindingEvaluator::evaluateAll is called
 * on the given evaluator.
 *
 * Note that instances of BindingEvaluator internally wrap their collection of
 * Bindings in such a way that copying a BindingEvaluator does not actually
 * copy the collection of Bindings. Therefore adding a Binding to a copy of a
 * BindingEvaluator will also add it to the original.
 * This is done for ease of use, so evaluators can be passed around easily throughout
 * the codebase.
 *
 * Examples:
 * - @ref 06-lazy-property-bindings/main.cpp
 */
class BindingEvaluator
{
    // We use pimpl here so that we can pass evaluators around by value (copies)
    // yet each copy refers to the same set of data
    struct Private {
        // TODO: Use std::vector here?
        std::map<int, std::function<void()>> m_bindingEvalFunctions;
        int m_currentId;
    };
```

---

</SwmSnippet>

# Private implementation pattern

<SwmSnippet path="/src/kdbindings/binding_evaluator.h" line="20">

---

The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> uses the pimpl pattern to manage its internal state. This allows the evaluator to be passed around by value while maintaining a shared collection of bindings. This design choice simplifies the use of evaluators across the codebase.

```
/**
 * @brief A BindingEvaluator provides a mechanism to control the exact time
 * when a KDBindings::Binding is reevaluated.
 *
 * A BindingEvaluator represents a collection of Binding instances that can be
 * selectively reevaluated.
 *
 * If a Binding is created using KDBindings::makeBoundProperty with a BindingEvaluator,
 * the Binding will only be evaluated if BindingEvaluator::evaluateAll is called
 * on the given evaluator.
 *
 * Note that instances of BindingEvaluator internally wrap their collection of
 * Bindings in such a way that copying a BindingEvaluator does not actually
 * copy the collection of Bindings. Therefore adding a Binding to a copy of a
 * BindingEvaluator will also add it to the original.
 * This is done for ease of use, so evaluators can be passed around easily throughout
 * the codebase.
 *
 * Examples:
 * - @ref 06-lazy-property-bindings/main.cpp
 */
class BindingEvaluator
{
    // We use pimpl here so that we can pass evaluators around by value (copies)
    // yet each copy refers to the same set of data
    struct Private {
        // TODO: Use std::vector here?
        std::map<int, std::function<void()>> m_bindingEvalFunctions;
        int m_currentId;
    };
```

---

</SwmSnippet>

# Copy and move semantics

<SwmSnippet path="/src/kdbindings/binding_evaluator.h" line="51">

---

The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="52:5:5" line-data="    /** A BindingEvaluator can be default constructed */">`BindingEvaluator`</SwmToken> can be copy constructed and assigned, but it cannot be move constructed or assigned. This ensures that all copies of an evaluator refer to the same collection of bindings, preventing accidental duplication of state.

```
public:
    /** A BindingEvaluator can be default constructed */
    BindingEvaluator() = default;

    /**
     * A BindingEvaluator can be copy constructed.
     *
     * Note that copying the evaluator will NOT create a new collection of
     * Binding instances, but the new evaluator will refer to the same collection,
     * so creating a new Binding with this evaluator will also modify the previous one.
     */
    BindingEvaluator(const BindingEvaluator &) noexcept = default;

    /**
     * A BindingEvaluator can be copy assigned.
     *
     * Note that copying the evaluator will NOT create a new collection of
     * Binding instances, but the new evaluator will refer to the same collection,
     * so creating a new Binding with this evaluator will also modify the previous one.
     */
    BindingEvaluator &operator=(const BindingEvaluator &) noexcept = default;

    /**
     * A BindingEvaluator can not be move constructed.
     */
    BindingEvaluator(BindingEvaluator &&other) noexcept = delete;

    /**
     * A BindingEvaluator can not be move assigned.
     */
    BindingEvaluator &operator=(BindingEvaluator &&other) noexcept = delete;
```

---

</SwmSnippet>

# Evaluating bindings

<SwmSnippet path="/src/kdbindings/binding_evaluator.h" line="83">

---

The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="89:3:3" line-data="    void evaluateAll() const">`evaluateAll`</SwmToken> function is a key method in the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> class. It evaluates all bindings associated with the evaluator in the order they were inserted. This ensures that dependencies are resolved correctly.

```
    /**
     * This function evaluates all Binding instances that were constructed with this
     * evaluator, in the order they were inserted.
     *
     * It will therefore update the associated Property instances as well.
     */
    void evaluateAll() const
    {
        // a std::map's ordering is deterministic, so the bindings are evaluated
        // in the order they were inserted, ensuring correct transitive dependency
        // evaluation.
        for (auto &[id, func] : m_d->m_bindingEvalFunctions)
            func();
    }
```

---

</SwmSnippet>

# Managing bindings

<SwmSnippet path="/src/kdbindings/binding_evaluator.h" line="98">

---

Bindings are managed internally using a map of functions. The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="100:3:3" line-data="    int insert(BindingType *binding)">`insert`</SwmToken> method adds a new binding, while the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="107:3:3" line-data="    void remove(int id)">`remove`</SwmToken> method deletes a binding by its identifier. This allows for dynamic management of bindings within the evaluator.

```
private:
    template<typename BindingType>
    int insert(BindingType *binding)
    {
        m_d->m_bindingEvalFunctions.insert({ ++(m_d->m_currentId),
                                             [=]() { binding->evaluate(); } });
        return m_d->m_currentId;
    }

    void remove(int id)
    {
        m_d->m_bindingEvalFunctions.erase(id);
    }
```

---

</SwmSnippet>

# <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="123:17:17" line-data=" * Any Binding that is constructed with an ImmediateBindingEvaluator will not wait">`ImmediateBindingEvaluator`</SwmToken> subclass

<SwmSnippet path="/src/kdbindings/binding_evaluator.h" line="118">

---

The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="123:17:17" line-data=" * Any Binding that is constructed with an ImmediateBindingEvaluator will not wait">`ImmediateBindingEvaluator`</SwmToken> is a subclass of <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="119:9:9" line-data=" * This subclass of BindingEvaluator doesn&#39;t do anything special on its own.">`BindingEvaluator`</SwmToken> that provides immediate mode bindings. This means that bindings are evaluated as soon as their properties change, rather than waiting for an explicit call to <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="124:13:13" line-data=" * for the evaluator to call evaluateAll, but rather evaluate the Binding immediately">`evaluateAll`</SwmToken>.

```
/**
 * This subclass of BindingEvaluator doesn't do anything special on its own.
 * It is used together with a template specialization of Binding to provide
 * old-school, immediate mode Bindings.
 *
 * Any Binding that is constructed with an ImmediateBindingEvaluator will not wait
 * for the evaluator to call evaluateAll, but rather evaluate the Binding immediately
 * when any of its bindables (i.e. Property instances) change.
 * This can lead to a Property Binding being evaluated many
 * times before the result is ever used in a typical GUI application.
 */
class ImmediateBindingEvaluator final : public BindingEvaluator
{
public:
    static inline ImmediateBindingEvaluator instance()
    {
        static ImmediateBindingEvaluator evaluator;
        return evaluator;
    }
};
```

---

</SwmSnippet>

# Conclusion

The <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="21:8:8" line-data=" * @brief A BindingEvaluator provides a mechanism to control the exact time">`BindingEvaluator`</SwmToken> class is a powerful tool for managing the timing of binding reevaluations in the <SwmToken path="/src/kdbindings/binding_evaluator.h" pos="22:7:7" line-data=" * when a KDBindings::Binding is reevaluated.">`KDBindings`</SwmToken> library. Its design allows for efficient and flexible use of bindings, reducing unnecessary computations and improving performance.

<SwmMeta version="3.0.0" repo-id="Z2l0aHViJTNBJTNBS0RCaW5kaW5ncyUzQSUzQUxvaXBmaW5nZXJN" repo-name="KDBindings"><sup>Powered by [Swimm](https://app.swimm.io/)</sup></SwmMeta>
