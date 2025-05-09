#ifndef KOKKOS_UTILS_CALLBACKS_EVENTS_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTS_HPP

#include <iomanip>

#include "impl/Kokkos_Profiling.hpp"

#include "kokkos-utils/impl/type_list.hpp"

namespace Kokkos::utils::callbacks
{

//! Equality comparison for @c Kokkos_Profiling_SpaceHandle.
bool operator==(const Kokkos_Profiling_SpaceHandle& fst, const Kokkos_Profiling_SpaceHandle& snd) {
    return strcmp(fst.name, snd.name) == 0;
};

/**
 * @name Event types.
 *
 * Each event is an aggregate holding the argument values of a corresponding @ref Kokkos profiling function
 * associated with a @ref Kokkos profiling callback.
 *
 * See also:
 *     - https://github.com/kokkos/kokkos/blob/4c4fa17dccbd01106c0fa8e03fd23fe70896dc1c/core/src/impl/Kokkos_Profiling_C_Interface.h#L47-L113
 */
///@{
//! Begin-parallel-for event associated with @c Kokkos::Tools::Experimental::EventSet::begin_parallel_for.
struct BeginParallelForEvent
{
    std::string name {};
    uint32_t dev_id = 0;
    uint64_t event_id = 0;

    bool operator==(const BeginParallelForEvent&) const = default;
};

//! End-parallel-for event associated with @c Kokkos::Tools::Experimental::EventSet::end_parallel_for.
struct EndParallelForEvent
{
    uint64_t event_id = 0;

    bool operator==(const EndParallelForEvent&) const = default;
};

//! Begin-parallel-reduce event associated with @c Kokkos::Tools::Experimental::EventSet::begin_parallel_reduce.
struct BeginParallelReduceEvent
{
    std::string name {};
    uint32_t dev_id = 0;
    uint64_t event_id = 0;

    bool operator==(const BeginParallelReduceEvent&) const = default;
};

//! End-parallel-reduce event associated with @c Kokkos::Tools::Experimental::EventSet::end_parallel_reduce.
struct EndParallelReduceEvent
{
    uint64_t event_id = 0;

    bool operator==(const EndParallelReduceEvent&) const = default;
};

//! Begin-parallel-scan event associated with @c Kokkos::Tools::Experimental::EventSet::begin_parallel_scan.
struct BeginParallelScanEvent
{
    std::string name {};
    uint32_t dev_id = 0;
    uint64_t event_id = 0;

    bool operator==(const BeginParallelScanEvent&) const = default;
};

//! End-parallel-scan event associated with @c Kokkos::Tools::Experimental::EventSet::end_parallel_scan.
struct EndParallelScanEvent
{
    uint64_t event_id = 0;

    bool operator==(const EndParallelScanEvent&) const = default;
};

//! Begin-fence event associated with @c Kokkos::Tools::Experimental::EventSet::begin_fence.
struct BeginFenceEvent
{
    std::string name {};
    uint32_t dev_id = 0;
    uint64_t event_id = 0;

    bool operator==(const BeginFenceEvent&) const = default;
};

//! End-fence event associated with @c Kokkos::Tools::Experimental::EventSet::end_fence.
struct EndFenceEvent
{
    uint64_t event_id = 0;

    bool operator==(const EndFenceEvent&) const = default;
};

//! Helper struct to hold descriptors of a data allocation.
struct AllocDescriptor
{
    Kokkos_Profiling_SpaceHandle kpsh {};
    std::string name {};
    const void* ptr = nullptr;
    uint64_t size = 0;

    bool operator==(const AllocDescriptor&) const = default;
};

//! Allocate-data event associated with @c Kokkos::Tools::Experimental::EventSet::allocate_data.
struct AllocateDataEvent
{
    AllocDescriptor alloc {};

    bool operator==(const AllocateDataEvent&) const = default;
};

//! Deallocate-data event associated with @c Kokkos::Tools::Experimental::EventSet::deallocate_data.
struct DeallocateDataEvent
{
    AllocDescriptor alloc {};

    bool operator==(const DeallocateDataEvent&) const = default;
};

//! Begin-deep-copy event associated with @c Kokkos::Tools::Experimental::EventSet::begin_deep_copy.
struct BeginDeepCopyEvent
{
    AllocDescriptor dst {};
    AllocDescriptor src {};

    bool operator==(const BeginDeepCopyEvent&) const = default;
};

//! End-deep-copy event associated with @c Kokkos::Tools::Experimental::EventSet::end_deep_copy.
struct EndDeepCopyEvent
{
    bool operator==(const EndDeepCopyEvent&) const = default;
};

//! Create-profile-section event associated with @c Kokkos::Tools::Experimental::EventSet::create_profile_section.
struct CreateProfileSectionEvent
{
    std::string name {};
    uint32_t section_id = 0;

    bool operator==(const CreateProfileSectionEvent&) const = default;
};

//! Start-profile-section event associated with @c Kokkos::Tools::Experimental::EventSet::start_profile_section.
struct StartProfileSectionEvent
{
    uint32_t section_id = 0;

    bool operator==(const StartProfileSectionEvent&) const = default;
};

//! Stop-profile-section event associated with @c Kokkos::Tools::Experimental::EventSet::stop_profile_section.
struct StopProfileSectionEvent
{
    uint32_t section_id = 0;

    bool operator==(const StopProfileSectionEvent&) const = default;
};

//! Destroy-profile-section event associated with @c Kokkos::Tools::Experimental::EventSet::destroy_profile_section.
struct DestroyProfileSectionEvent
{
    uint32_t section_id = 0;

    bool operator==(const DestroyProfileSectionEvent&) const = default;
};

//! Push-region event associated with @c Kokkos::Tools::Experimental::EventSet::push_region.
struct PushRegionEvent
{
    std::string name {};

    bool operator==(const PushRegionEvent&) const = default;
};

//! Pop-region event associated with @c Kokkos::Tools::Experimental::EventSet::pop_region.
struct PopRegionEvent
{
    bool operator==(const PopRegionEvent&) const = default;
};

//! Profile event associated with @c Kokkos::Tools::Experimental::EventSet::profile_event.
struct ProfileEvent
{
    std::string name {};

    bool operator==(const ProfileEvent&) const = default;
};
///@}

//! Type list holding all event types.
using EventTypeList = Kokkos::Impl::type_list<
    BeginParallelForEvent,     EndParallelForEvent,
    BeginParallelReduceEvent,  EndParallelReduceEvent,
    BeginParallelScanEvent,    EndParallelScanEvent,
    BeginFenceEvent,           EndFenceEvent,
    AllocateDataEvent,         DeallocateDataEvent,
    BeginDeepCopyEvent,        EndDeepCopyEvent,
    CreateProfileSectionEvent, DestroyProfileSectionEvent,
    StartProfileSectionEvent,  StopProfileSectionEvent,
    PushRegionEvent,           PopRegionEvent,
    ProfileEvent
>;

//! @name Concepts to constrain event types of similar nature.
///@{
//! Concept to constrain any event type in @ref Kokkos::utils::callbacks::EventTypeList.
template <typename T>
concept Event = impl::type_list_contains_v<T, EventTypeList>;

//! Concept to constrain any begin event type.
template <typename EventType>
concept BeginEvent =
    Event<EventType> &&
    std::constructible_from<EventType, const char*, uint32_t, uint64_t> &&
    requires (EventType event) {
        { event.name }     -> std::same_as<std::string&>;
        { event.dev_id }   -> std::same_as<uint32_t&>;
        { event.event_id } -> std::same_as<uint64_t&>;
    };

//! Concept to constrain any end event type.
template <typename EventType>
concept EndEvent =
    Event<EventType> && (! BeginEvent<EventType>) &&
    std::constructible_from<EventType, uint64_t> &&
    requires (EventType event) {
        { event.event_id } -> std::same_as<uint64_t&>;
    };

//! Concept to constrain any data event type.
template <typename EventType>
concept DataEvent =
    Event<EventType> &&
    std::constructible_from<EventType, AllocDescriptor> &&
    requires (EventType event) {
        { event.alloc } -> std::same_as<AllocDescriptor&>;
    };

//! Concept to constrain any profile section manipulation event type.
template <typename EventType>
concept ProfileSectionManipulationEvent =
    Event<EventType> &&
    std::constructible_from<EventType, uint32_t> &&
    requires (EventType event) {
        { event.section_id } -> std::same_as<uint32_t&>;
    };

//! Concept to constrain any event type that has a @c name field.
template <typename EventType>
concept NamedEvent =
    Event<EventType> &&
    requires (EventType event) {
        { event.name } -> std::same_as<std::string&>;
    };

//! Concept to constrain any event type that is one of the given event types.
template <typename T, typename... EventTypes>
concept EventOneOf = impl::type_list_contains_v<T, EventTypes...>;

//! Concept to constraint any event that can happens on a given @c dev_id.
template <typename EventType>
concept OnDeviceEvent = Event<EventType> && requires (EventType& event) {
    {event.dev_id} -> std::same_as<uint32_t&>;
};
///@}

//! @name Stream operators.
///@{
//! Helper function related to the implementation of the stream operators.
template <Event EventType>
constexpr auto get_name() {
    return Kokkos::Impl::TypeInfo<EventType>::name().substr(26); // erase "Kokkos::utils::callbacks::"
}

template <Event EventType>
std::ostream& operator<<(std::ostream &out, const EventType& /* event */) {
    return out << get_name<EventType>() << ": "
               << "{}";
}

template <BeginEvent EventType>
std::ostream& operator<<(std::ostream &out, const EventType& event) {
    return out << get_name<EventType>() << ": "
               << "{name = " << std::quoted(event.name) << ", dev_id = " << event.dev_id << ", event_id = " << event.event_id  << "}";
}

template <EndEvent EventType>
std::ostream& operator<<(std::ostream &out, const EventType& event) {
    return out << get_name<EventType>() << ": "
               << "{event_id = " << event.event_id  << "}";
}

template <DataEvent EventType>
std::ostream& operator<<(std::ostream &out, const EventType& event) {
    const auto& alloc = event.alloc;
    return out << get_name<EventType>() << ": "
               << "{name = " << std::quoted(alloc.name) << ", space = " << std::quoted(alloc.kpsh.name) << ", ptr = " << alloc.ptr << ", size = " << alloc.size << "}";
}

inline std::ostream& operator<<(std::ostream &out, const BeginDeepCopyEvent& event) {
    const auto& dst = event.dst;
    const auto& src = event.src;
    return out << get_name<BeginDeepCopyEvent>() << ": "
               << "{src = " << std::quoted(src.name) << " (" << src.kpsh.name << ", " << src.ptr << ") -> "
               << "dst = "  << std::quoted(dst.name) << " (" << dst.kpsh.name << ", " << dst.ptr << ") of size " << src.size << "}";
}

inline std::ostream& operator<<(std::ostream &out, const CreateProfileSectionEvent& event) {
    return out << get_name<CreateProfileSectionEvent>() << ": "
               << "{name = " << std::quoted(event.name) << ", section_id = " << event.section_id << "}";
}

template <ProfileSectionManipulationEvent EventType>
std::ostream& operator<<(std::ostream &out, const EventType& event) {
    return out << get_name<EventType>() << ": "
               << "{section_id = " << event.section_id << "}";
}

inline std::ostream& operator<<(std::ostream &out, const PushRegionEvent& event) {
    return out << get_name<PushRegionEvent>() << ": "
               << "{name = " << std::quoted(event.name) << "}";
}

inline std::ostream& operator<<(std::ostream &out, const ProfileEvent& event) {
    return out << get_name<ProfileEvent>() << ": "
               << "{name = " << std::quoted(event.name) << "}";
}
///@}

//! @name Helpers providing the @c Kokkos::Tools getters and setters corresponding to the event types.
///@{
//! Get the member of a @c Kokkos::Tools::Experimental::EventSet corresponding to @p EventType.
template <Event EventType>
auto get_callback_from_eventset(const Kokkos::Tools::Experimental::EventSet& event_set);

//! Helper macro related to the implementation of @ref Kokkos::utils::callbacks::get_callback_from_eventset.
#define GET_CALLBACK_FROM_EVENTSET(__eventtype__, __callback__) \
    template <> \
    inline auto get_callback_from_eventset<__eventtype__>(const Kokkos::Tools::Experimental::EventSet& event_set) { return event_set.__callback__; }

GET_CALLBACK_FROM_EVENTSET(BeginParallelForEvent,      begin_parallel_for)
GET_CALLBACK_FROM_EVENTSET(EndParallelForEvent,        end_parallel_for)
GET_CALLBACK_FROM_EVENTSET(BeginParallelReduceEvent,   begin_parallel_reduce)
GET_CALLBACK_FROM_EVENTSET(EndParallelReduceEvent,     end_parallel_reduce)
GET_CALLBACK_FROM_EVENTSET(BeginParallelScanEvent,     begin_parallel_scan)
GET_CALLBACK_FROM_EVENTSET(EndParallelScanEvent,       end_parallel_scan)
GET_CALLBACK_FROM_EVENTSET(BeginFenceEvent,            begin_fence)
GET_CALLBACK_FROM_EVENTSET(EndFenceEvent,              end_fence)
GET_CALLBACK_FROM_EVENTSET(AllocateDataEvent,          allocate_data)
GET_CALLBACK_FROM_EVENTSET(DeallocateDataEvent,        deallocate_data)
GET_CALLBACK_FROM_EVENTSET(BeginDeepCopyEvent,         begin_deep_copy)
GET_CALLBACK_FROM_EVENTSET(EndDeepCopyEvent,           end_deep_copy)
GET_CALLBACK_FROM_EVENTSET(CreateProfileSectionEvent,  create_profile_section)
GET_CALLBACK_FROM_EVENTSET(StartProfileSectionEvent,   start_profile_section)
GET_CALLBACK_FROM_EVENTSET(StopProfileSectionEvent,    stop_profile_section)
GET_CALLBACK_FROM_EVENTSET(DestroyProfileSectionEvent, destroy_profile_section)
GET_CALLBACK_FROM_EVENTSET(PushRegionEvent,            push_region)
GET_CALLBACK_FROM_EVENTSET(PopRegionEvent,             pop_region)
GET_CALLBACK_FROM_EVENTSET(ProfileEvent,               profile_event)

//! Get the setter function of a @ref Kokkos profiling callback corresponding to @p EventType.
template <Event EventType>
auto get_callback_setter();

//! Helper macro related to the implementation of @ref Kokkos::utils::callbacks::get_callback_setter.
#define GET_CALLBACK_SETTER(__eventtype__, __callback_setter__) \
    template <> \
    inline auto get_callback_setter<__eventtype__>() { return &Kokkos::Tools::Experimental::__callback_setter__; }

GET_CALLBACK_SETTER(BeginParallelForEvent,      set_begin_parallel_for_callback)
GET_CALLBACK_SETTER(EndParallelForEvent,        set_end_parallel_for_callback)
GET_CALLBACK_SETTER(BeginParallelReduceEvent,   set_begin_parallel_reduce_callback)
GET_CALLBACK_SETTER(EndParallelReduceEvent,     set_end_parallel_reduce_callback)
GET_CALLBACK_SETTER(BeginParallelScanEvent,     set_begin_parallel_scan_callback)
GET_CALLBACK_SETTER(EndParallelScanEvent,       set_end_parallel_scan_callback)
GET_CALLBACK_SETTER(BeginFenceEvent,            set_begin_fence_callback)
GET_CALLBACK_SETTER(EndFenceEvent,              set_end_fence_callback)
GET_CALLBACK_SETTER(AllocateDataEvent,          set_allocate_data_callback)
GET_CALLBACK_SETTER(DeallocateDataEvent,        set_deallocate_data_callback)
GET_CALLBACK_SETTER(BeginDeepCopyEvent,         set_begin_deep_copy_callback)
GET_CALLBACK_SETTER(EndDeepCopyEvent,           set_end_deep_copy_callback)
GET_CALLBACK_SETTER(CreateProfileSectionEvent,  set_create_profile_section_callback)
GET_CALLBACK_SETTER(StartProfileSectionEvent,   set_start_profile_section_callback)
GET_CALLBACK_SETTER(StopProfileSectionEvent,    set_stop_profile_section_callback)
GET_CALLBACK_SETTER(DestroyProfileSectionEvent, set_destroy_profile_section_callback)
GET_CALLBACK_SETTER(PushRegionEvent,            set_push_region_callback)
GET_CALLBACK_SETTER(PopRegionEvent,             set_pop_region_callback)
GET_CALLBACK_SETTER(ProfileEvent,               set_profile_event_callback)
///@}

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTS_HPP
