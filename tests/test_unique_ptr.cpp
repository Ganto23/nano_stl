#include <gtest/gtest.h>
#include <type_traits>
#include <nstl/unique_ptr.hpp>

namespace {

struct Tracked {
    static int ctor_count;
    static int dtor_count;
    int value;

    explicit Tracked(int v) : value(v) { ++ctor_count; }
    ~Tracked() { ++dtor_count; }
};

int Tracked::ctor_count = 0;
int Tracked::dtor_count = 0;

struct BoolDeleter {
    bool* deleted;
    void operator()(Tracked* p) const noexcept {
        delete p;
        *deleted = true;
    }
};

}


TEST(UniquePtrBasic, NotCopyable) {
    using UP = nstl::unique_ptr<int>;
    EXPECT_FALSE(std::is_copy_constructible_v<UP>);
    EXPECT_FALSE(std::is_copy_assignable_v<UP>);
}

TEST(UniquePtrBasic, MoveTraits) {
    using UP = nstl::unique_ptr<int>;
    EXPECT_TRUE(std::is_move_constructible_v<UP>);
    EXPECT_TRUE(std::is_move_assignable_v<UP>);
}

TEST(UniquePtrBasic, CreationFromRawPointer) {
    nstl::unique_ptr<int> up1(new int(5));
    EXPECT_EQ(*up1, 5);
    *up1 = 6;
    EXPECT_EQ(*up1, 6);
}

TEST(UniquePtrBasic, CreationWithMakeUnique) {
    auto up2 = nstl::make_unique<int>(6);
    EXPECT_EQ(*up2, 6);
}

TEST(UniquePtrBasic, MoveConstructionTransfersOwnership) {
    auto up1 = nstl::make_unique<int>(5);
    nstl::unique_ptr<int> up2(std::move(up1));
    EXPECT_EQ(up1.get(), nullptr);
    EXPECT_NE(up2.get(), nullptr);
    EXPECT_EQ(*up2, 5);
}

TEST(UniquePtrBasic, MoveAssignmentReleasesOld) {
    auto up1 = nstl::make_unique<int>(1);
    auto up2 = nstl::make_unique<int>(2);
    up2 = std::move(up1);
    EXPECT_EQ(up1.get(), nullptr);
    EXPECT_NE(up2.get(), nullptr);
    EXPECT_EQ(*up2, 1);
}

TEST(UniquePtrBasic, ResetAndRelease) {
    auto up = nstl::make_unique<int>(10);
    int* raw = up.release();
    EXPECT_EQ(up.get(), nullptr);
    EXPECT_EQ(*raw, 10);
    delete raw;

    up.reset(new int(20));
    EXPECT_NE(up.get(), nullptr);
    EXPECT_EQ(*up, 20);
    up.reset();
    EXPECT_EQ(up.get(), nullptr);
}

TEST(UniquePtrBasic, SizeIsReasonable) {
    using UP = nstl::unique_ptr<int>;
    EXPECT_LE(sizeof(UP), 2 * sizeof(void*));
}

TEST(UniquePtrArray, BasicUsage) {
    nstl::unique_ptr<int[]> arr(new int[3]);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(UniquePtrComplex, OutOfScopeDestroysObject) {
    Tracked::ctor_count = 0;
    Tracked::dtor_count = 0;

    {
        auto up = nstl::make_unique<Tracked>(42);
        EXPECT_EQ(Tracked::ctor_count, 1);
        EXPECT_EQ(Tracked::dtor_count, 0);
        EXPECT_EQ(up->value, 42);
    } // up goes out of scope

    EXPECT_EQ(Tracked::ctor_count, 1);
    EXPECT_EQ(Tracked::dtor_count, 1);
}

TEST(UniquePtrComplex, MoveTransfersOwnership) {
    Tracked::ctor_count = 0;
    Tracked::dtor_count = 0;

    auto up1 = nstl::make_unique<Tracked>(7);
    EXPECT_EQ(up1->value, 7);

    nstl::unique_ptr<Tracked> up2(std::move(up1));
    EXPECT_EQ(up1.get(), nullptr);
    EXPECT_NE(up2.get(), nullptr);
    EXPECT_EQ(up2->value, 7);

    EXPECT_EQ(Tracked::ctor_count, 1);
    EXPECT_EQ(Tracked::dtor_count, 0);
}

TEST(UniquePtrComplex, MoveAssignmentReleasesOld) {
    Tracked::ctor_count = 0;
    Tracked::dtor_count = 0;

    auto up1 = nstl::make_unique<Tracked>(1);
    auto up2 = nstl::make_unique<Tracked>(2);
    EXPECT_EQ(Tracked::ctor_count, 2);
    EXPECT_EQ(Tracked::dtor_count, 0);

    up2 = std::move(up1);
    EXPECT_EQ(up1.get(), nullptr);
    EXPECT_NE(up2.get(), nullptr);
    EXPECT_EQ(up2->value, 1);

    EXPECT_EQ(Tracked::ctor_count, 2);
    EXPECT_EQ(Tracked::dtor_count, 1);
}

TEST(UniquePtrComplex, CustomDeleterIsCalled) {
    bool deleted = false;
    {
        nstl::unique_ptr<Tracked, BoolDeleter> up(
            new Tracked(99),
            BoolDeleter{&deleted}
        );
        EXPECT_FALSE(deleted);
        EXPECT_EQ(up->value, 99);
    }

    EXPECT_TRUE(deleted);
}


TEST(UniquePtrBasic, NullptrBehavior) {
    nstl::unique_ptr<int> up;
    EXPECT_EQ(up.get(), nullptr);
    EXPECT_TRUE(up == nullptr);
    EXPECT_FALSE(up != nullptr);

    up.reset(new int(3));
    EXPECT_FALSE(up == nullptr);
    EXPECT_TRUE(up != nullptr);
}
