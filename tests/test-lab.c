#include "harness/unity.h"
#include "../src/lab.h"
#include <stdlib.h>

// Global variable for the queue used in tests
static queue_t q = NULL;

// Set up before each test
void setUp(void) {

}

// Clean up after each test
void tearDown(void) {

}

// Test functions
void test_create_destroy(void) {
    TEST_ASSERT_TRUE(q != NULL);
    // No need to destroy the queue here
}

void test_queue_dequeue(void) {
    TEST_ASSERT_TRUE(q != NULL);
    int data = 1;
    enqueue(q, &data);
    TEST_ASSERT_TRUE(dequeue(q) == &data);
}

void test_queue_dequeue_multiple(void) {
    TEST_ASSERT_TRUE(q != NULL);
    int data1 = 1;
    int data2 = 2;
    int data3 = 3;
    enqueue(q, &data1);
    enqueue(q, &data2);
    enqueue(q, &data3);
    TEST_ASSERT_TRUE(dequeue(q) == &data1);
    TEST_ASSERT_TRUE(dequeue(q) == &data2);
    TEST_ASSERT_TRUE(dequeue(q) == &data3);
}

void test_queue_dequeue_shutdown(void) {
    TEST_ASSERT_TRUE(q != NULL);
    int data1 = 1;
    int data2 = 2;
    int data3 = 3;
    enqueue(q, &data1);
    enqueue(q, &data2);
    enqueue(q, &data3);
    TEST_ASSERT_TRUE(dequeue(q) == &data1);
    TEST_ASSERT_TRUE(dequeue(q) == &data2);
    queue_shutdown(q);
    TEST_ASSERT_TRUE(dequeue(q) == &data3);
    TEST_ASSERT_TRUE(is_shutdown(q));
    TEST_ASSERT_TRUE(is_empty(q));
}

// Main function to run the tests
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_destroy);
    RUN_TEST(test_queue_dequeue);
    RUN_TEST(test_queue_dequeue_multiple);
    RUN_TEST(test_queue_dequeue_shutdown);
    return UNITY_END();
}
