#include <stdlib.h>
#include <stdio.h>
#include "../Unity/src/unity.h"
#include "../src/project.h"

void setUp(void) {}
void tearDown(void) {}

void test_readTable_returnsLinkedList() {
    FILE *f = fopen("../src/grades.csv", "r");
    node *head = readTable(f, NULL);

    TEST_ASSERT_NOT_NULL(head);
    TEST_ASSERT_EQUAL_INT(267, head->id);
    TEST_ASSERT_EQUAL_INT(348, head->course);
    TEST_ASSERT_EQUAL_INT(84, head->grades);
    TEST_ASSERT_NOT_NULL(head->next);

    free(head);
    fclose(f);
}

void test_writeTable() {
    FILE *f = fopen("../src/grades.csv", "r");
    node *head = readTable(f, NULL);
    fclose(f);

    FILE *f2 = fopen("../src/test.csv", "w");
    writeTable(f2, head);
    fclose(f2);

    FILE *f3 = fopen("../src/test.csv", "r");
    node *head2 = readTable(f3, NULL);

    TEST_ASSERT_NOT_NULL(head2);
    TEST_ASSERT_EQUAL_INT(267, head2->id);
    TEST_ASSERT_EQUAL_INT(348, head2->course);
    TEST_ASSERT_EQUAL_INT(84, head2->grades);
    TEST_ASSERT_NOT_NULL(head2->next);

    free(head);
    free(head2);
    fclose(f3);
}

int main(void) {
  UNITY_BEGIN();
  
  RUN_TEST(test_readTable_returnsLinkedList);
  RUN_TEST(test_writeTable);

  return UNITY_END();
}
