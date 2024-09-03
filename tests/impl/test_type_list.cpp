#include "gtest/gtest.h"

#include "kokkos-utils/impl/type_list.hpp"

/**
 * @addtogroup unittests
 *
 * **Type list**
 *
 * This group of tests check the behavior of our utilities extending @c Kokkos::Impl::type_list,
 * which are found in @ref kokkos-utils/impl/type_list.hpp.
 */

namespace Kokkos::utils::tests::impl
{

using type_list_t = Kokkos::Impl::type_list<char, short, int>;

//! @test Check @ref Kokkos::utils::impl::type_list_size_v.
TEST(impl, type_list_size_v)
{
    using Kokkos::utils::impl::type_list_size_v;

    using type_list_empty_t = Kokkos::Impl::type_list<>;
    static_assert(type_list_size_v<type_list_empty_t> == 0);

    static_assert(type_list_size_v<type_list_t> == 3);
}

//! @test Check @ref Kokkos::utils::impl::type_list_at_t.
TEST(impl, type_list_at_t)
{
    using Kokkos::utils::impl::type_list_at_t;

    static_assert(std::same_as<type_list_at_t<0, type_list_t>, char>);
    static_assert(std::same_as<type_list_at_t<1, type_list_t>, short>);
    static_assert(std::same_as<type_list_at_t<2, type_list_t>, int>);
}

//! Helper struct for the test of @ref Kokkos::utils::impl::transform_type_list_t.
template <typename T>
struct VectorOfTransformer
{
    using type = std::vector<T>;
};

//! @test Check @ref Kokkos::utils::impl::transform_type_list_t.
TEST(impl, transform_type_list_t)
{
    using Kokkos::utils::impl::transform_type_list_t;

    using transformed_type_list_t = transform_type_list_t<VectorOfTransformer, type_list_t>;

    using expt_transformed_type_list_t = Kokkos::Impl::type_list<std::vector<char>, std::vector<short>, std::vector<int>>;

    static_assert(std::same_as<transformed_type_list_t, expt_transformed_type_list_t>);
}

//! @test Check @ref Kokkos::utils::impl::type_list_to_tuple_t.
TEST(impl, type_list_to_tuple)
{
    using Kokkos::utils::impl::type_list_to_tuple_t;

    using expt_tuple_t = std::tuple<char, short, int>;

    static_assert(std::same_as<type_list_to_tuple_t<type_list_t>, expt_tuple_t>);
}

} // namespace Kokkos::utils::tests::impl
