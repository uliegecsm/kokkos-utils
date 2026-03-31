#ifndef KOKKOS_UTILS_CALLBACKS_HELPERS_HPP
#define KOKKOS_UTILS_CALLBACKS_HELPERS_HPP

#include "gmock/gmock.h"

#include "kokkos-utils/callbacks/Manager.hpp"

namespace Kokkos::utils::callbacks
{

#define TEST_F_WITH_CB_MGR(__test_fixture_name__, __test_name__)        \
    class __test_fixture_name__ : public ::testing::Test,               \
                                  public scoped::callbacks::Manager {}; \
    TEST_F(__test_fixture_name__, __test_name__)

#define DEFINE_EVENT_MATCHER_IN(__namespace__, __eventtype__)                                                \
    template <typename... Matchers>                                                                          \
    auto A##__eventtype__(Matchers&&... matchers)                                                            \
    {                                                                                                        \
        using EventType = __namespace__::__eventtype__;                                                      \
        if constexpr (sizeof...(Matchers) == 0)                                                              \
            return ::testing::VariantWith<EventType>(::testing::_);                                          \
        else                                                                                                 \
            return ::testing::VariantWith<EventType>(::testing::AllOf(std::forward<Matchers>(matchers)...)); \
    }

#define DEFINE_EVENT_MATCHER(__eventtype__) DEFINE_EVENT_MATCHER_IN(Kokkos::utils::callbacks, __eventtype__)

DEFINE_EVENT_MATCHER(BeginParallelForEvent)
DEFINE_EVENT_MATCHER(EndParallelForEvent)
DEFINE_EVENT_MATCHER(BeginParallelReduceEvent)
DEFINE_EVENT_MATCHER(EndParallelReduceEvent)
DEFINE_EVENT_MATCHER(BeginParallelScanEvent)
DEFINE_EVENT_MATCHER(EndParallelScanEvent)
DEFINE_EVENT_MATCHER(BeginFenceEvent)
DEFINE_EVENT_MATCHER(EndFenceEvent)
DEFINE_EVENT_MATCHER(AllocateDataEvent)
DEFINE_EVENT_MATCHER(DeallocateDataEvent)
DEFINE_EVENT_MATCHER(BeginDeepCopyEvent)
DEFINE_EVENT_MATCHER(EndDeepCopyEvent)
DEFINE_EVENT_MATCHER(CreateProfileSectionEvent)
DEFINE_EVENT_MATCHER(DestroyProfileSectionEvent)
DEFINE_EVENT_MATCHER(StartProfileSectionEvent)
DEFINE_EVENT_MATCHER(StopProfileSectionEvent)
DEFINE_EVENT_MATCHER(PushRegionEvent)
DEFINE_EVENT_MATCHER(PopRegionEvent)
DEFINE_EVENT_MATCHER(ProfileEvent)

#define DEFINE_EVENT_WITH_NAME_MATCHER_IN(__namespace__, __eventtype__)                                               \
    template <typename Matcher>                                                                                       \
    auto A##__eventtype__##WithName(Matcher&& matcher)                                                                \
    {                                                                                                                 \
        using EventType = __namespace__::__eventtype__;                                                               \
        return ::testing::VariantWith<EventType>(::testing::Field(&EventType::name, std::forward<Matcher>(matcher))); \
    }

#define DEFINE_EVENT_WITH_NAME_MATCHER(__eventtype__) DEFINE_EVENT_WITH_NAME_MATCHER_IN(Kokkos::utils::callbacks, __eventtype__)

DEFINE_EVENT_WITH_NAME_MATCHER(BeginParallelForEvent)
DEFINE_EVENT_WITH_NAME_MATCHER(BeginParallelReduceEvent)
DEFINE_EVENT_WITH_NAME_MATCHER(BeginParallelScanEvent)
DEFINE_EVENT_WITH_NAME_MATCHER(BeginFenceEvent)
DEFINE_EVENT_WITH_NAME_MATCHER(CreateProfileSectionEvent)
DEFINE_EVENT_WITH_NAME_MATCHER(PushRegionEvent)
DEFINE_EVENT_WITH_NAME_MATCHER(ProfileEvent)

namespace impl
{

template <typename T, typename... Matchers>
class ContainsInOrderMatcher
{
public:
    template <typename... Matchers_> requires std::conjunction_v<std::is_same<std::remove_cvref_t<Matchers_>, Matchers>...>
    explicit ContainsInOrderMatcher(Matchers_&&... matchers_)
      : matchers{std::forward<Matchers_>(matchers_)...} {}

    template <typename IterableType>
    bool MatchAndExplain([[maybe_unused]]const IterableType& arg, [[maybe_unused]]::testing::MatchResultListener* const listener) const
    {
        if constexpr(sizeof...(Matchers) == 0) {
            return true;
        } else {
            return MatchAndExplainImpl<0>(arg.cbegin(), arg.cend(), listener);
        }
    }

    void DescribeTo(std::ostream* out) const
    {
        *out << "contains in order elements that match ";
        DescribeInnerMatchersImpl(out);
    }

    void DescribeNegationTo(std::ostream* out) const
    {
        *out << "does not contain in order elements that match ";
        DescribeInnerMatchersImpl(out);
    }

private:
    template <size_t Idx, typename IteratorType>
    bool MatchAndExplainImpl(const IteratorType& it_first, const IteratorType& it_last, ::testing::MatchResultListener* const listener) const
    {
        if (it_first == it_last) {;
            *listener << "does not contain in order an element that "
                      << ::testing::DescribeMatcher<T>(std::get<Idx>(matchers), false);
            return false;
        }

        if (::testing::Matches(std::get<Idx>(matchers))(*it_first)) {
            if constexpr (Idx == sizeof...(Matchers) - 1) {
                return true;
            } else {
                return MatchAndExplainImpl<Idx + 1>(std::next(it_first), it_last, listener);
            }
        } else {
            return MatchAndExplainImpl<Idx>(std::next(it_first), it_last, listener);
        }
    }

    void DescribeInnerMatchersImpl(std::ostream* out) const
    {
        *out << "(";
        if constexpr (sizeof...(Matchers) >= 2)
        {
            [&] <size_t... Idxs>(std::index_sequence<Idxs...>) {
                ((*out << ::testing::DescribeMatcher<T>(std::get<Idxs>(matchers), false) << ", "), ...);
            }(std::make_index_sequence<sizeof...(Matchers) - 1>{});
        }
        if constexpr (sizeof...(Matchers) >= 1)
            *out << ::testing::DescribeMatcher<T>(std::get<sizeof...(Matchers) - 1>(matchers), false);
        *out << ")";
    }

private:
    std::tuple<Matchers...> matchers;
};

template <typename ElementType, typename ElementMatcher>
class ElementAtMatcher
{
public:
    explicit ElementAtMatcher(const size_t index, ElementMatcher matcher)
        : m_index(index), m_matcher(std::move(matcher)) {}

    template <typename IterableType>
    bool MatchAndExplain(const IterableType& arg, ::testing::MatchResultListener* const listener) const
    {
        if (m_index >= arg.size()) {
            *listener << "index " << m_index << " is out of bounds (size " << arg.size() << ")";
            return false;
        }
        return ::testing::ExplainMatchResult(m_matcher, arg[m_index], listener);
    }

    void DescribeTo(std::ostream* out) const
    {
        *out << "element at index " << m_index << " "
             << ::testing::DescribeMatcher<ElementType>(m_matcher, false);
    }

    void DescribeNegationTo(std::ostream* out) const
    {
        *out << "element at index " << m_index << " "
             << ::testing::DescribeMatcher<ElementType>(m_matcher, true);
    }

private:
    size_t m_index;
    ElementMatcher m_matcher;
};

} // namespace impl

template <typename T, typename... Matchers>
auto ContainsInOrder(Matchers&&... matchers) {
    return ::testing::MakePolymorphicMatcher(impl::ContainsInOrderMatcher<T, std::remove_cvref_t<Matchers>...>(std::forward<Matchers>(matchers)...));
}

/**
 * @brief Check that an element at a given index matches.
 *
 * @code
 * ASSERT_THAT(iterable, ElementAt(index, matcher));
 * @endcode
 */
template <typename ElementType, typename ElementMatcher>
auto ElementAt(const size_t index, ElementMatcher&& matcher) {
    return ::testing::MakePolymorphicMatcher(
        impl::ElementAtMatcher<ElementType, std::remove_cvref_t<ElementMatcher>>{index, std::forward<ElementMatcher>(matcher)});
}

template <typename T>
struct PartialMatcher;

/**
 * @brief For an allocation, one typically wants to check the memory space, label and size of the allocation.
 *
 * Therefore, the @ref Kokkos::utils::callbacks::AllocDescriptor::ptr is not included in the comparison (as it is subject to change from one run to another).
 */
template <>
struct PartialMatcher<AllocDescriptor>
{
    decltype(auto) operator()(const AllocDescriptor& descr) const {
        return ::testing::AllOf(
            ::testing::Field(
                &AllocDescriptor::kpsh,
                ::testing::Field(&Kokkos_Profiling_SpaceHandle::name, ::testing::StrEq(descr.kpsh.name))
            ),
            ::testing::Field(
                &AllocDescriptor::name,
                ::testing::StrEq(descr.name)
            ),
            ::testing::Field(
                &AllocDescriptor::size,
                ::testing::Eq(descr.size)
            )
        );
    }
};

/**
 * Uses @ref Kokkos::utils::callbacks::PartialMatcher<AllocDescriptor> for the
 * @ref Kokkos::utils::callbacks::BeginDeepCopyEvent::dst and 
 * @ref Kokkos::utils::callbacks::BeginDeepCopyEvent::src.
 */
template <>
struct PartialMatcher<BeginDeepCopyEvent>
{
    decltype(auto) operator()(const BeginDeepCopyEvent& event) const {
        return ::testing::AllOf(
            ::testing::Field(&BeginDeepCopyEvent::dst, PartialMatcher<AllocDescriptor>{}(event.dst)),
            ::testing::Field(&BeginDeepCopyEvent::src, PartialMatcher<AllocDescriptor>{}(event.src))
        );
    }
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_HELPERS_HPP
