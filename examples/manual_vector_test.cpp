#include <iostream>
#include <cassert>
#include "../include/nstl/vector.hpp"

struct Tracker {
    int id;
    static int active_objects;

    Tracker(int val) : id(val) { 
        active_objects++; 
        std::cout << "  Constructed " << id << "\n";
    }
    ~Tracker() { 
        active_objects--; 
        std::cout << "  Destroyed " << id << "\n";
    }
    
    Tracker(const Tracker& other) : id(other.id) {
        active_objects++;
        std::cout << "  Copied " << id << "\n";
    }
    
    Tracker(Tracker&& other) noexcept : id(other.id) {
        active_objects++;
        other.id = -1; // Mark stolen
        std::cout << "  Moved " << id << "\n";
    }
    
    Tracker& operator=(const Tracker& other) {
        id = other.id;
        std::cout << "  Assigned " << id << "\n";
        return *this;
    }
};
int Tracker::active_objects = 0;

void test_push_pop() {
    std::cout << "\n=== Testing Push/Pop ===\n";
    nstl::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    assert(v.size() == 3);
    assert(v[0] == 10);
    assert(v[2] == 30);
    
    v.pop_back();
    assert(v.size() == 2);
    std::cout << "Push/Pop passed.\n";
}

void test_memory_leaks() {
    std::cout << "\n=== Testing Memory Leaks (Tracker) ===\n";
    {
        nstl::vector<Tracker> v;
        v.push_back(Tracker(1)); // Constructor -> Move -> Destructor (temp)
        v.push_back(Tracker(2)); // Constructor -> Move -> Destructor (temp) -> (Resize maybe?)
    } 
    // v goes out of scope here.
    // Destructor of v should call ~Tracker() for everything.
    
    std::cout << "Active objects (should be 0): " << Tracker::active_objects << "\n";
    assert(Tracker::active_objects == 0);
    std::cout << "Memory Leak test passed.\n";
}

int main() {
    test_push_pop();
    test_memory_leaks();
    
    std::cout << "\nAll manual tests passed!\n";
    return 0;
}
