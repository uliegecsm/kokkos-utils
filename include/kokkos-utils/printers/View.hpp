#ifndef KOKKOS_UTILS_PRINTERS_VIEW_HPP
#define KOKKOS_UTILS_PRINTERS_VIEW_HPP

#include "Kokkos_View.hpp"

#include "kokkos-utils/concepts/View.hpp"
#include "kokkos-utils/view/slice.hpp"

namespace Kokkos::utils::printers
{

//! Print a @c Kokkos::View.
template <concepts::View ViewType>
std::ostream& operator<<(std::ostream& out, const ViewType& view)
{
    //! Create a host mirror view.
    const auto view_h = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace{}, view);

    std::ostringstream oss;
    oss.copyfmt(out);

    oss << "[";
    if constexpr (ViewType::rank() == 0) {
        oss << view_h();
    } else
    {
        for (size_t idx = 0; idx < view_h.extent(0); ++idx)
        {
            if (idx > 0) oss << ", ";
            if constexpr (ViewType::rank() == 1) {
                oss << view_h(idx);
            } else {
                oss << view::slice<ViewType::rank()>(view_h, idx);
            }
        }
    }
    oss << "]";

    return out << oss.str();
}

} // namespace Kokkos::utils::printers

#endif // KOKKOS_UTILS_PRINTERS_VIEW_HPP
