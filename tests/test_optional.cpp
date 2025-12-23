#include <gtest/gtest.h>
#include <type_traits>
#include <optional>
#include <string>
#include <nstl/optional.hpp>

namespace {

struct Tracked {
    static int ctor_count;
    static int dtor_count;
    int value;

    explicit Tracked(int v) : value(v) { ++ctor_count; }
    Tracked(const Tracked& other) : value(other.value) { ++ctor_count; }
    Tracked(Tracked&& other) noexcept : value(other.value) { ++ctor_count; }
    ~Tracked() { ++dtor_count; }
};

int Tracked::ctor_count = 0;
int Tracked::dtor_count = 0;

}

// ---- Basic type traits ----

TEST(OptionalBasic, TrivialTraits_Int) {
    using NOpt = nstl::optional<int>;
    using SOpt = std::optional<int>;

    EXPECT_TRUE(std::is_copy_constructible_v<NOpt>);
    EXPECT_TRUE(std::is_move_constructible_v<NOpt>);
    EXPECT_TRUE(std::is_copy_assignable_v<NOpt>);
    EXPECT_TRUE(std::is_move_assignable_v<NOpt>);

    EXPECT_EQ(sizeof(NOpt) >= sizeof(SOpt), true);
}

// ---- Default construction and nullopt ----

TEST(OptionalBasic, DefaultAndNulloptConstruction) {
    nstl::optional<int> n1;
    nstl::optional<int> n2(nstl::nullopt);

    std::optional<int> s1;
    std::optional<int> s2(std::nullopt);

    EXPECT_FALSE(n1.has_value());
    EXPECT_FALSE(n2.has_value());
    EXPECT_FALSE(s1.has_value());
    EXPECT_FALSE(s2.has_value());
}

// ---- Value construction and access ----

TEST(OptionalBasic, ValueConstructionAndAccess) {
    nstl::optional<int> n1(5);
    nstl::optional<int> n2 = 6;

    std::optional<int> s1(5);
    std::optional<int> s2 = 6;

    EXPECT_TRUE(n1.has_value());
    EXPECT_TRUE(n2.has_value());
    EXPECT_TRUE(s1.has_value());
    EXPECT_TRUE(s2.has_value());

    EXPECT_EQ(*n1, *s1);
    EXPECT_EQ(n2.value(), s2.value());

    // operator-> with non-trivial type
    nstl::optional<std::string> nstr(std::string("hello"));
    std::optional<std::string> sstr(std::string("hello"));

    EXPECT_EQ(nstr->size(), sstr->size());
    EXPECT_EQ(*nstr, *sstr);
}

// ---- Copy and move construction ----

TEST(OptionalBasic, CopyAndMoveConstruction_Int) {
    nstl::optional<int> nsrc(10);
    std::optional<int> ssrc(10);

    nstl::optional<int> ncopy(nsrc);
    std::optional<int> scopy(ssrc);

    EXPECT_TRUE(ncopy.has_value());
    EXPECT_EQ(ncopy.value(), scopy.value());

    nstl::optional<int> nmove(std::move(nsrc));
    std::optional<int> smove(std::move(ssrc));

    EXPECT_TRUE(nmove.has_value());
    EXPECT_TRUE(smove.has_value());
    EXPECT_EQ(nmove.value(), smove.value());
}

// ---- Copy and move assignment ----

TEST(OptionalBasic, CopyAndMoveAssignment_Int) {
    nstl::optional<int> n1(1);
    nstl::optional<int> n2(2);

    std::optional<int> s1(1);
    std::optional<int> s2(2);

    // copy assign engaged -> engaged
    n2 = n1;
    s2 = s1;

    EXPECT_TRUE(n2.has_value());
    EXPECT_TRUE(s2.has_value());
    EXPECT_EQ(n2.value(), s2.value());

    // move assign
    nstl::optional<int> n3(3);
    std::optional<int> s3(3);

    n3 = n2;
    s3 = s2;

    EXPECT_TRUE(n3.has_value());
    EXPECT_TRUE(s3.has_value());
    EXPECT_EQ(n3.value(), s3.value());
}

// ---- Assignment from value and nullopt ----

TEST(OptionalBasic, AssignFromValueAndNullopt) {
    nstl::optional<int> n;
    std::optional<int> s;

    n = 42;
    s = 42;

    EXPECT_TRUE(n.has_value());
    EXPECT_TRUE(s.has_value());
    EXPECT_EQ(n.value(), s.value());

    n = nstl::nullopt;
    s = std::nullopt;

    EXPECT_FALSE(n.has_value());
    EXPECT_FALSE(s.has_value());
}

// ---- reset and emplace ----

TEST(OptionalModifiers, ResetAndEmplace) {
    Tracked::ctor_count = 0;
    Tracked::dtor_count = 0;

    nstl::optional<Tracked> n;
    std::optional<Tracked> s;

    EXPECT_FALSE(n.has_value());
    EXPECT_FALSE(s.has_value());

    n.emplace(10);
    s.emplace(10);

    EXPECT_TRUE(n.has_value());
    EXPECT_TRUE(s.has_value());
    EXPECT_EQ(n->value, s->value);
    EXPECT_EQ(Tracked::ctor_count >= 2, true); // at least two constructions

    n.reset();
    s.reset();

    EXPECT_FALSE(n.has_value());
    EXPECT_FALSE(s.has_value());
    EXPECT_EQ(Tracked::dtor_count >= 2, true); // both destroyed
}

// ---- swap behavior ----

TEST(OptionalModifiers, SwapEngagedAndEmpty) {
    nstl::optional<int> n1(5);
    nstl::optional<int> n2;

    std::optional<int> s1(5);
    std::optional<int> s2;

    n1.swap(n2);
    s1.swap(s2);

    EXPECT_FALSE(n1.has_value());
    EXPECT_FALSE(s1.has_value());
    EXPECT_TRUE(n2.has_value());
    EXPECT_TRUE(s2.has_value());
    EXPECT_EQ(n2.value(), s2.value());
}

TEST(OptionalModifiers, SwapTwoEngaged) {
    nstl::optional<int> n1(1);
    nstl::optional<int> n2(2);

    std::optional<int> s1(1);
    std::optional<int> s2(2);

    n1.swap(n2);
    s1.swap(s2);

    EXPECT_TRUE(n1.has_value());
    EXPECT_TRUE(n2.has_value());
    EXPECT_TRUE(s1.has_value());
    EXPECT_TRUE(s2.has_value());

    EXPECT_EQ(n1.value(), s1.value());
    EXPECT_EQ(n2.value(), s2.value());
}

// ---- value_or ----

TEST(OptionalObservers, ValueOr) {
    nstl::optional<int> n1(5);
    nstl::optional<int> n2;

    std::optional<int> s1(5);
    std::optional<int> s2;

    EXPECT_EQ(n1.value_or(10), s1.value_or(10));
    EXPECT_EQ(n2.value_or(10), s2.value_or(10));

    nstl::optional<std::string> nstr;
    std::optional<std::string> sstr;

    EXPECT_EQ(nstr.value_or("fallback"), sstr.value_or("fallback"));
}

// ---- bool / has_value / operator* and operator-> ----

TEST(OptionalObservers, BoolAndDereference) {
    nstl::optional<std::string> nstr;
    std::optional<std::string> sstr;

    EXPECT_FALSE(nstr);
    EXPECT_FALSE(sstr);

    nstr = std::string("test");
    sstr = std::string("test");

    EXPECT_TRUE(nstr);
    EXPECT_TRUE(sstr);

    EXPECT_EQ((*nstr).size(), (*sstr).size());
    EXPECT_EQ(nstr->at(1), sstr->at(1));
}

// ---- Exception from value() when empty ----

TEST(OptionalObservers, ValueThrowsWhenEmpty) {
    nstl::optional<int> n;
    std::optional<int> s;

    EXPECT_FALSE(n.has_value());
    EXPECT_FALSE(s.has_value());

    EXPECT_THROW(n.value(), std::bad_optional_access);
    EXPECT_THROW(s.value(), std::bad_optional_access);
}

// ---- Tracked lifetime with engaged/disengaged ----

TEST(OptionalComplex, TrackedLifetime) {
    Tracked::ctor_count = 0;
    Tracked::dtor_count = 0;

    {
        nstl::optional<Tracked> n;
        std::optional<Tracked> s;

        n.emplace(7);
        s.emplace(7);

        EXPECT_TRUE(n.has_value());
        EXPECT_TRUE(s.has_value());
        EXPECT_EQ(n->value, s->value);

        n.reset();
        s.reset();

        EXPECT_FALSE(n.has_value());
        EXPECT_FALSE(s.has_value());
    }

    EXPECT_GE(Tracked::ctor_count, 2);
    EXPECT_GE(Tracked::dtor_count, 2);
}
