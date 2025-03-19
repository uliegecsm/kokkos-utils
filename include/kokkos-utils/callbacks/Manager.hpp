#ifndef KOKKOS_UTILS_CALLBACKS_MANAGER_HPP
#define KOKKOS_UTILS_CALLBACKS_MANAGER_HPP

#include <list>
#include <ranges>

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/callbacks/Listener.hpp"

namespace Kokkos::utils::callbacks
{

namespace impl
{

/**
 * @brief Helper structures used by @ref Kokkos::utils::callbacks::Manager to store and call registered listeners.
 *
 * The implementation follows a type-erasure pattern:
 *     - The type-erasure concept is a structure that declares abstractly the interface of a listener.
 *     - The type-erasure model is a templated wrapper structure that stores a callable object and implements
 *       the abstract interface by forwarding the call methods to the stored callable object.
 *
 * The implementation uses the CRTP to allow the call operators to access the stored callable object.
 */
///@{
struct ListenerConceptBase
{
    virtual ~ListenerConceptBase() = default;
};

template <Event EventType>
struct ListenerConceptCallOperator
{
    virtual ~ListenerConceptCallOperator() = default;

    virtual void operator()(const EventType& event) const = 0;
};

template <typename...>
struct ListenerModel;

template <typename Callable>
struct ListenerModelBase : public ListenerConceptBase
{
    template <typename T> requires std::same_as<std::remove_cvref_t<T>, Callable>
    explicit ListenerModelBase(T&& callable_) : callable(std::forward<T>(callable_)) {}

    Callable callable;
};

template <Event EventType, typename ListenerModel>
struct ListenerModelCallOperator : public ListenerConceptCallOperator<EventType>
{
    void operator()(const EventType& event) const override
    {
        // Distinguish between a callable object that is a function object and one that is a shared pointer.
        if constexpr (std::is_invocable_v<decltype(static_cast<const ListenerModel*>(this)->callable), const EventType&>) {
            static_cast<const ListenerModel*>(this)->callable.operator()(event);
        } else {
            static_cast<const ListenerModel*>(this)->callable->operator()(event);
        }
    };
};

template <typename Callable, Event... EventTypes>
struct ListenerModel<Callable, EventTypes...>
  : public ListenerModelCallOperator<EventTypes, ListenerModel<Callable, EventTypes...>>...,
    public ListenerModelBase<Callable>
{
    using event_type_list_t = Kokkos::Impl::type_list<EventTypes...>;

    using ListenerModelBase<Callable>::ListenerModelBase;
};

template <typename Callable, Event... EventTypes>
struct ListenerModel<Callable, Kokkos::Impl::type_list<EventTypes...>>
  : public ListenerModel<Callable, EventTypes...>
{
    using event_type_list_t = Kokkos::Impl::type_list<EventTypes...>;

    using ListenerModel<Callable, EventTypes...>::ListenerModel;
};

//! Helper struct needed for the implementation of @ref Kokkos::utils::callbacks::impl::listener_call_opr_list_tuple_t.
template <Event EventType>
struct ListOfListenerCallOprPerEventTypeTransformer
{
    using type = std::list<const ListenerConceptCallOperator<EventType>*>;
};

/**
 * @brief Type of container used by @ref Kokkos::utils::callbacks::Manager to store pointers
 *        to the registered listeners per event type.
 */
using listener_call_opr_list_tuple_t = Kokkos::utils::impl::type_list_to_tuple_t<
    Kokkos::utils::impl::transform_type_list_t<ListOfListenerCallOprPerEventTypeTransformer, EventTypeList>
>;
///@}

} // namespace impl

/**
 * @brief Class to manage @ref Kokkos profiling callback calls.
 *
 * This class allows the registration of callable objects as listeners for events representing
 * @ref Kokkos profiling callback calls.
 *
 * On initialization, this class uses @c Kokkos::Tools::Experimental::get_callbacks to retrieve
 * the callback function pointers already set within @ref Kokkos. Note that this class refers to
 * these callback functions as the "context callbacks".
 *
 * On registering one or more callable objects as listeners for events, this class stores these
 * callable objects by using a type-erasure pattern. Thus, on registering a callable object, this
 * callable object is wrapped into a @ref impl::ListenerModel that implements the abstract interface
 * composed of @ref impl::ListenerConceptBase and one or more @ref impl::ListenerConceptCallOperator
 * instances. The wrapped callable object is then stored in a list of unique pointers to the abstract
 * base class.
 *
 * In order to efficiently dispatch events to the corresponding call operators of the callable
 * objects, this class stores for each event type a list of pointers to the listener models wrapping
 * the callable objects that are registered as listeners for this event type. Note that this class
 * refers to the call operators of these registered callable objects as the "registered callbacks".
 *
 * This class uses the setters @c Kokkos::Tools::Experimental::set_<event_type_name>_callback
 * to set the @ref Kokkos callback function pointers to this class's dispatching functions for the
 * event types for which there are registered callbacks. When a @ref Kokkos callback call is made,
 * these dispatching functions call the context callback first and then sequentially the registered
 * callbacks for the event type.
 *
 * On finalization, this class clears the list of registered callable objects and restores the
 * @ref Kokkos callback function pointers to the values they had prior to initializing this class.
 *
 * @note The profiling callback calls are implemented in @ref Kokkos in terms of C function pointers.
 *       Class member functions cannot be converted to such C function pointers, unless they are
 *       static. This class is thus implemented as a singleton, and the dispatching functions are
 *       static member functions.
 */
class Manager
{
public:
    using listener_list_t                = std::list<std::unique_ptr<impl::ListenerConceptBase>>;
    using listener_list_const_iter_t     = typename listener_list_t::const_iterator;
    using listener_call_opr_list_tuple_t = impl::listener_call_opr_list_tuple_t;

public:
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    ~Manager() { reset_context_callbacks(); }

protected:
    /**
     * @brief Constructor. Retrieves and stores the @c Kokkos::Tools::Experimental::EventSet containing
     *        the @ref Kokkos callback function pointers.
     */
    Manager() : context_callbacks(Kokkos::Tools::Experimental::get_callbacks()) {}

public:
    static void initialize() { singleton = std::unique_ptr<Manager>(new Manager()); }
    static void finalize()   { singleton.reset(); }

    static Manager& get_instance() { return *singleton; }

    /**
     * @brief Register a callable object, passed as a shared pointer, as a listener.
     *
     * @todo Add the possibility to select only a subset of the event types.
     */
    template <Listener Callable>
    static listener_list_const_iter_t register_listener(std::shared_ptr<Callable> callable)
    {
        using event_type_list_t = listener_event_type_list_t<Callable>;
        using listener_model_t  = impl::ListenerModel<std::shared_ptr<Callable>, event_type_list_t>;
        return register_listener_impl<listener_model_t>(std::move(callable));
    }

    /**
     * @overload
     *
     * This overload registers a new callable object that is copy or move constructed from the passed @p callable.
     * This overload can be useful for instance to register a lambda as a listener.
     */
    template <typename T> requires Listener<std::remove_cvref_t<T>>
    static listener_list_const_iter_t register_listener(T&& callable)
    {
        using event_type_list_t = listener_event_type_list_t<std::remove_cvref_t<T>>;
        using listener_model_t  = impl::ListenerModel<std::remove_cvref_t<T>, event_type_list_t>;
        return register_listener_impl<listener_model_t>(std::forward<T>(callable));
    }

    //! Unregister a callable object as a listener.
    template <Listener Callable>
    static void unregister_listener(const Callable* const callable)
    {
        using event_type_list_t = listener_event_type_list_t<Callable>;

        using listener_model_t = impl::ListenerModel<std::shared_ptr<Callable>, event_type_list_t>;

        auto& listeners = get_instance().listeners;

        const auto iter = std::ranges::find_if(
            listeners,
            [&] (const std::unique_ptr<impl::ListenerConceptBase>& listener)
            {
                auto* const listener_ptr = dynamic_cast<listener_model_t*>(listener.get());
                return listener_ptr == nullptr ? false : listener_ptr->callable.get() == callable;
            }
        );

        Kokkos::utils::impl::for_each<event_type_list_t>([&] <Event EventType>() {
            get<Kokkos::utils::impl::type_list_index_v<EventType, EventTypeList>>(get_instance().registered_callbacks).remove(
                dynamic_cast<const impl::ListenerConceptCallOperator<EventType>*>(iter->get())
            );
        });

        listeners.erase(iter);

        get_instance().set_dispatching_callbacks();
    }

    static void unregister_listener(const listener_list_const_iter_t& iter)
    {
        Kokkos::utils::impl::for_each<EventTypeList>([&] <Event EventType>() {
            get<Kokkos::utils::impl::type_list_index_v<EventType, EventTypeList>>(get_instance().registered_callbacks).remove(
                dynamic_cast<const impl::ListenerConceptCallOperator<EventType>*>(iter->get())
            );
        });

        get_instance().listeners.erase(iter);

        get_instance().set_dispatching_callbacks();
    }

private:
    /**
     * @brief Uses @c Kokkos::Tools::Experimental::set_callbacks to restore the @ref Kokkos callback
     *        function pointers to those retrieved on this class's initialization.
     */
    void reset_context_callbacks() const {
        Kokkos::Tools::Experimental::set_callbacks(context_callbacks);
    }

    template <typename ListenerModelType, typename CallableType>
    static listener_list_const_iter_t register_listener_impl(CallableType&& callable)
    {
        using event_type_list_t = typename ListenerModelType::event_type_list_t;

        auto& listeners = get_instance().listeners;

        //! Wrap the callable object into a @ref impl::ListenerModel and store it in the list of listeners.
        const auto iter = listeners.insert(
            listeners.end(), std::make_unique<ListenerModelType>(std::forward<CallableType>(callable))
        );

        /// For each event type that the callable object can be invoked with, store the raw pointer
        /// to the listener model in the list of listeners for this event type.
        Kokkos::utils::impl::for_each<event_type_list_t>([&] <Event EventType>() {
            get<Kokkos::utils::impl::type_list_index_v<EventType, EventTypeList>>(get_instance().registered_callbacks).push_back(
                dynamic_cast<const impl::ListenerConceptCallOperator<EventType>*>(iter->get())
            );
        });

        get_instance().set_dispatching_callbacks();

        return iter;
    }

    /**
     * @brief Uses the setters @c Kokkos::Tools::Experimental::set_<event_type_name>_callback
     *        to set the @ref Kokkos callback function pointers to this class's dispatching functions
     *        for the event types for which there are registered callbacks.
     */
    void set_dispatching_callbacks() const
    {
        Kokkos::utils::impl::for_each<EventTypeList>([&] <Event EventType>() {
            set_dispatching_callback_impl<EventType>();
        });
    }

    template <Event EventType>
    void set_dispatching_callback_impl() const
    {
        get_callback_setter<EventType>() (
            get<Kokkos::utils::impl::type_list_index_v<EventType, EventTypeList>>(registered_callbacks).empty()
                ? get_callback_from_eventset<EventType>(context_callbacks)
                : create_dispatching_callback_for_event_type_impl<EventType>()
        );
    }

    /// @note We don't forward in the templated lambda because the signature must be the one
    ///       of the @ref Kokkos callback function pointers which pass by value.
    template <Event EventType>
    static auto create_dispatching_callback_for_event_type_impl()
    {
        return [] <typename... Args>(Args... args) -> void {
            EventType event{args...};
            get_instance().dispatch(event);
        };
    }

    template <BeginEvent EventType>
    static auto create_dispatching_callback_for_event_type_impl()
    {
        return [] (const char* name, const uint32_t dev_id, uint64_t* event_id) -> void
        {
            EventType event{ .name = name, .dev_id = dev_id, .event_id = *event_id };
            get_instance().dispatch(event);
            *event_id = event.event_id;
        };
    }

    template <DataEvent EventType>
    static auto create_dispatching_callback_for_event_type_impl()
    {
        return [] (Kokkos_Profiling_SpaceHandle kpsh, const char* name,  const void* ptr, uint64_t size) -> void
        {
            EventType event{ .alloc = { .kpsh = kpsh, .name = name, .ptr = ptr, .size = size } };
            get_instance().dispatch(event);
        };
    }

    template <Event EventType> requires std::same_as<EventType, BeginDeepCopyEvent>
    static auto create_dispatching_callback_for_event_type_impl()
    {
        return [] (Kokkos_Profiling_SpaceHandle dst_kpsh, const char* dst_name,  const void* dst_ptr,
                   Kokkos_Profiling_SpaceHandle src_kpsh, const char* src_name,  const void* src_ptr, uint64_t size) -> void
        {
            EventType event{
                .dst = { .kpsh = dst_kpsh, .name = dst_name, .ptr = dst_ptr, .size = size },
                .src = { .kpsh = src_kpsh, .name = src_name, .ptr = src_ptr, .size = size }
            };
            get_instance().dispatch(event);
        };
    }

    template <Event EventType> requires std::same_as<EventType, CreateProfileSectionEvent>
    static auto create_dispatching_callback_for_event_type_impl()
    {
        return [] (const char* name, uint32_t* section_id) -> void
        {
            EventType event{ .name = name, .section_id = *section_id };
            get_instance().dispatch(event);
            *section_id = event.section_id;
        };
    }

    /**
     * @brief Dispatch first the context callback, if any is set in @ref Kokkos, and then sequentially
     *        the registered callbacks for the event type.
     */
    template <Event EventType>
    void dispatch(EventType& event)
    {
        dispatch_context_callback(event);

        dispatch_registered_callbacks(event);
    }

    //! @brief Dispatch the event to the context callback, if any is set in @ref Kokkos.
    template <Event EventType>
    void dispatch_context_callback(EventType& event)
    {
        auto* context_callback = get_callback_from_eventset<EventType>(context_callbacks);

        if (context_callback) {
            dispatch_context_callback_for_event_type_impl(context_callback, event);
        } else {
            increment_id_if_needed_for_event_type_impl(event);
        }
    }

    template <typename ContextCallbackType, BeginEvent EventType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, EventType& event) {
        context_callback(event.name.c_str(), event.dev_id, &event.event_id);
    }

    template <typename ContextCallbackType, EndEvent EventType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const EventType& event) {
        context_callback(event.event_id);
    }

    template <typename ContextCallbackType, DataEvent EventType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const EventType& event) {
        context_callback(event.alloc.kpsh, event.alloc.name.c_str(), event.alloc.ptr, event.alloc.size);
    }

    template <typename ContextCallbackType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const BeginDeepCopyEvent& event) {
        context_callback(event.dst.kpsh, event.dst.name.c_str(), event.dst.ptr,
                         event.src.kpsh, event.src.name.c_str(), event.src.ptr, event.src.size);
    }

    template <typename ContextCallbackType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const EndDeepCopyEvent& /* event */) {
        context_callback();
    }

    template <typename ContextCallbackType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, CreateProfileSectionEvent& event) {
        context_callback(event.name.c_str(), &event.section_id);
    }

    template <typename ContextCallbackType, ProfileSectionManipulationEvent EventType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const EventType& event) {
        context_callback(event.section_id);
    }

    template <typename ContextCallbackType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const PushRegionEvent& event) {
        context_callback(event.name.c_str());
    }

    template <typename ContextCallbackType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const PopRegionEvent& /* event */) {
        context_callback();
    }

    template <typename ContextCallbackType>
    static void dispatch_context_callback_for_event_type_impl(ContextCallbackType* context_callback, const ProfileEvent& event) {
        context_callback(event.name.c_str());
    }

    template <Event EventType>
    void increment_id_if_needed_for_event_type_impl(EventType& /* event */) {}

    template <BeginEvent EventType>
    void increment_id_if_needed_for_event_type_impl(EventType& event) {
        event.event_id = next_event_id++;
    }

    void increment_id_if_needed_for_event_type_impl(CreateProfileSectionEvent& event) {
        event.section_id = next_section_id++;
    }

    template <Event EventType>
    void dispatch_registered_callbacks(const EventType& event) const
    {
        for (const auto& listener : get<Kokkos::utils::impl::type_list_index_v<EventType, EventTypeList>>(registered_callbacks)) {
            listener->operator()(event);
        }
    }

private:
    listener_call_opr_list_tuple_t registered_callbacks {};

    listener_list_t listeners {};

    static inline std::unique_ptr<Manager> singleton = nullptr;

    Kokkos::Tools::Experimental::EventSet context_callbacks {};

    uint64_t next_event_id   = 0;
    uint32_t next_section_id = 0;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_MANAGER_HPP
