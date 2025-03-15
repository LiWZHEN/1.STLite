#include <iostream>
#include <stdexcept>
#include <vector>
#include <climits>
#include "priority_queue.hpp"

int A = 325, B = 2336, last = 233, mod = 1000007;

int Rand(){
    return last = (A * last + B) % mod;
}

struct TestData {
    int value;
    TestData(int v) : value(v) {}
    TestData() : value(0) {}
};

bool operator==(const TestData &a, const TestData &b) {
    return a.value == b.value;
}

const int TRIGGER_VALUE = 100; // Specific value to trigger exception

bool force_exception = false; // Global flag, if true, FaultyCompare unconditionally throws exception

// Faulty comparator: throws exception when any value to compare equals TRIGGER_VALUE, or global flag force_exception is true
struct FaultyCompare {
    bool operator()(const TestData &a, const TestData &b) const {
        if (force_exception || a.value == TRIGGER_VALUE || b.value == TRIGGER_VALUE)
            throw sjtu::runtime_error();
        return a.value < b.value;
    }
};

std::vector<int> copyQueueState(sjtu::priority_queue<TestData, FaultyCompare> pq) {
    std::vector<int> state;
    while (!pq.empty()) {
        state.push_back(pq.top().value);
        pq.pop();
    }
    return state;
}

bool test1() {
    // Test 1: Basic push/pop test (all normal, no exceptions triggered)
    sjtu::priority_queue<TestData, FaultyCompare> pq;

    try {
        for (int i = 0; i < 10; i++) {
            int val = Rand() % 90 + 1;
            pq.push(TestData(val));
        }

        int prev = mod;
        while (!pq.empty()) {
            int curr = pq.top().value;
            if (curr > prev) {
                std::cout << "test1: Heap property violated!" << std::endl;
                return false;
            }
            prev = curr;
            pq.pop();
        }

        return true;
    } catch (const std::exception& e) {
        std::cout << "test1: Unexpected exception: " << e.what() << std::endl;
        return false;
    }
}

bool test2() {
    // Test 2: Test push when Compare throws exception
    sjtu::priority_queue<TestData, FaultyCompare> pq;

    try {
        for (int i = 0; i < 100; i++) {
            int val = Rand() % 90 + 1;
            pq.push(TestData(val));
        }
    } catch (const std::exception& e) {
        std::cout << "test2: Unexpected exception during setup: " << e.what() << std::endl;
        return false;
    }

    std::vector<int> originalState = copyQueueState(pq);

    sjtu::priority_queue<TestData, FaultyCompare> pq_rebuilt = pq;

    bool exceptionCaught = false;
    try {
        pq_rebuilt.push(TestData(TRIGGER_VALUE));
    } catch (const sjtu::runtime_error& e) {
        exceptionCaught = true;
    }

    if (!exceptionCaught) {
        std::cout << "test2: Exception not thrown when expected" << std::endl;
        return false;
    }

    std::vector<int> newState = copyQueueState(pq_rebuilt);
    if (originalState != newState) {
        std::cout << "test2: Queue state changed after exception" << std::endl;
        return false;
    }

    return true;
}

bool test3() {
    // Test 3: Test pop when Compare throws exception
    sjtu::priority_queue<TestData, FaultyCompare> pq;

    try {
        for (int i = 0; i < 100; i++) {
            int val = Rand() % 90 + 1;
            pq.push(TestData(val));
        }
    } catch (const std::exception& e) {
        std::cout << "test3: Unexpected exception during setup: " << e.what() << std::endl;
        return false;
    }

    std::vector<int> originalState = copyQueueState(pq);

    sjtu::priority_queue<TestData, FaultyCompare> pq_rebuilt = pq;

    bool exceptionCaught = false;
    try {
        force_exception = true;
        pq_rebuilt.pop();
        force_exception = false;
    } catch (const sjtu::runtime_error& e) {
        exceptionCaught = true;
        force_exception = false;
    }

    if (!exceptionCaught) {
        std::cout << "test3: Exception not thrown when expected" << std::endl;
        return false;
    }

    std::vector<int> newState = copyQueueState(pq_rebuilt);
    if (originalState != newState) {
        std::cout << "test3: Queue state changed after exception" << std::endl;
        return false;
    }

    return true;
}

bool test4() {
    // Test 4: Test merge operation when exception is triggered
    sjtu::priority_queue<TestData, FaultyCompare> pq1, pq2;

    try {
        for (int i = 0; i < 100; i++) {
            int val = Rand() % 90 + 1;
            pq1.push(TestData(val));
        }
        for (int i = 0; i < 100; i++) {
            int val = Rand() % 90 + 1;
            pq2.push(TestData(val));
        }
    } catch (const std::exception& e) {
        std::cout << "test4: Unexpected exception during setup: " << e.what() << std::endl;
        return false;
    }

    std::vector<int> originalState1 = copyQueueState(pq1);
    std::vector<int> originalState2 = copyQueueState(pq2);

    sjtu::priority_queue<TestData, FaultyCompare> pq1_rebuilt = pq1;
    sjtu::priority_queue<TestData, FaultyCompare> pq2_rebuilt = pq2;

    bool exceptionCaught = false;
    try {
        force_exception = true;
        pq1_rebuilt.merge(pq2_rebuilt);
        force_exception = false;
    } catch (const sjtu::runtime_error& e) {
        exceptionCaught = true;
        force_exception = false;
    }

    if (!exceptionCaught) {
        std::cout << "test4: Exception not thrown when expected" << std::endl;
        return false;
    }

    std::vector<int> newState1 = copyQueueState(pq1_rebuilt);
    std::vector<int> newState2 = copyQueueState(pq2_rebuilt);

    if (originalState1 != newState1) {
        std::cout << "test4: First queue state changed after exception" << std::endl;
        return false;
    }

    if (originalState2 != newState2) {
        std::cout << "test4: Second queue state changed after exception" << std::endl;
        return false;
    }

    return true;
}

bool test5() {
    // Test 5: Verify that after exceptions, queue can perform normal push/pop operations
    sjtu::priority_queue<TestData, FaultyCompare> pq;

    try {
        for (int i = 0; i < 100; i++) {
            int val = Rand() % 90 + 1;
            pq.push(TestData(val));
        }
    } catch (const std::exception& e) {
        std::cout << "test5: Unexpected exception during setup: " << e.what() << std::endl;
        return false;
    }

    sjtu::priority_queue<TestData, FaultyCompare> pq_rebuilt = pq;

    try {
        pq_rebuilt.push(TestData(TRIGGER_VALUE));
    } catch (const sjtu::runtime_error& e) {
        // Expected exception, continue
    }

    try {
        for (int i = 0; i < 100; i++) {
            int val = Rand() % 90 + 1;
            pq_rebuilt.push(TestData(val));
        }

        int prev = mod;
        while (!pq_rebuilt.empty()) {
            int curr = pq_rebuilt.top().value;
            if (curr > prev) {
                std::cout << "test5: Heap property violated after recovery!" << std::endl;
                return false;
            }
            prev = curr;
            pq_rebuilt.pop();
        }

        return true;
    } catch (const std::exception& e) {
        std::cout << "test5: Exception during normal operations after recovery: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    int score = 0;
    if (test1()
        && test2()
        && test3()
        && test4()
        && test5()) {
        score = 1;
    }
    std::cout << score << std::endl;
    return 0;
}