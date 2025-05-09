#include "gtest/gtest.h"

#include "kokkos-utils/impl/type_list.hpp"

/**
 * @addtogroup unittests
 *
 * Type list
 * ---------
 *
 * This group of tests check the behavior of our utilities extending @c Kokkos::Impl::type_list,
 * which are found in @ref kokkos-utils/impl/type_list.hpp.
 */

namespace Kokkos::utils::tests::impl
{

using type_list_t = Kokkos::Impl::type_list<char, short, int>;

//! @test Check @ref Kokkos::utils::impl::is_type_list_v.
TEST(impl, is_type_list_v)
{
    static_assert(! Kokkos::utils::impl::is_type_list_v<int>);
    static_assert(! Kokkos::utils::impl::is_type_list_v<std::tuple<int, int>>);
    static_assert(  Kokkos::utils::impl::is_type_list_v<type_list_t>);
    static_assert(  Kokkos::utils::impl::is_type_list_v<Kokkos::Impl::type_list<>>);
    static_assert(  Kokkos::utils::impl::is_type_list_v<Kokkos::Impl::type_list<int>>);
    static_assert(  Kokkos::utils::impl::is_type_list_v<Kokkos::Impl::type_list<int, double>>);
}

//! @test Check @ref Kokkos::utils::impl::make_type_list_t.
TEST(impl, make_type_list_t)
{
    static_assert(std::same_as<Kokkos::utils::impl::make_type_list_t<int        >, Kokkos::Impl::type_list<int        >>);
    static_assert(std::same_as<Kokkos::utils::impl::make_type_list_t<int, double>, Kokkos::Impl::type_list<int, double>>);

    static_assert(std::same_as<Kokkos::utils::impl::make_type_list_t<type_list_t>, type_list_t>);
}

//! @test Check @ref Kokkos::utils::impl::type_list_size_v.
TEST(impl, type_list_size_v)
{
    using Kokkos::utils::impl::type_list_size_v;

    using type_list_empty_t = Kokkos::Impl::type_list<>;
    static_assert(type_list_size_v<type_list_empty_t> == 0);

    static_assert(type_list_size_v<type_list_t> == 3);
}

//! @test Check @ref Kokkos::utils::impl::type_list_contains_v.
TEST(impl, type_list_contains_v)
{
    using Kokkos::utils::impl::type_list_contains_v;

    static_assert(   type_list_contains_v<char,   type_list_t>);
    static_assert( ! type_list_contains_v<double, type_list_t>);

    static_assert(   type_list_contains_v<char, int, double, char>);
    static_assert( ! type_list_contains_v<char, int, double, const char>);
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

//! @test Check @ref Kokkos::utils::impl::type_list_index_v.
TEST(impl, type_list_index_v)
{
    using Kokkos::utils::impl::type_list_index_v;

    static_assert(type_list_index_v<char,  type_list_t> == 0);
    static_assert(type_list_index_v<short, type_list_t> == 1);
    static_assert(type_list_index_v<int,   type_list_t> == 2);
}

//! @test Check @ref Kokkos::utils::impl::type_list_all_v.
TEST(impl, type_list_all_v)
{
    static_assert(  Kokkos::utils::impl::type_list_all_v<std::is_floating_point, Kokkos::Impl::type_list<float, double>>);
    static_assert(! Kokkos::utils::impl::type_list_all_v<std::is_floating_point, Kokkos::Impl::type_list<float, double, int>>);

    static_assert(  Kokkos::utils::impl::type_list_all_v<std::is_floating_point, float, double>);
    static_assert(! Kokkos::utils::impl::type_list_all_v<std::is_floating_point, float, double, int>);
}

//! @test Check @ref Kokkos::utils::impl::type_list_any_v.
TEST(impl, type_list_any_v)
{
    static_assert(  Kokkos::utils::impl::type_list_any_v<std::is_floating_point, Kokkos::Impl::type_list<float, std::string, int>>);
    static_assert(  Kokkos::utils::impl::type_list_any_v<std::is_integral,       Kokkos::Impl::type_list<float, std::string, int>>);
    static_assert(! Kokkos::utils::impl::type_list_any_v<std::is_enum,           Kokkos::Impl::type_list<float, std::string, int>>);

    static_assert(  Kokkos::utils::impl::type_list_any_v<std::is_floating_point, float, std::string, int>);
    static_assert(  Kokkos::utils::impl::type_list_any_v<std::is_integral,       float, std::string, int>);
    static_assert(! Kokkos::utils::impl::type_list_any_v<std::is_enum,           float, std::string, int>);
}

//! @test Check @ref Kokkos::utils::impl::for_each.
TEST(impl, for_each)
{
    using Kokkos::utils::impl::for_each;

    static_assert([]() {
        size_t sum_of_sizes = 0;

        for_each<type_list_t>([&] <typename T>() constexpr {
            sum_of_sizes += sizeof(T);
        });

        return sum_of_sizes == sizeof(char) + sizeof(short) + sizeof(int);
    }());
}

} // namespace Kokkos::utils::tests::impl
