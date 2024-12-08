#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//represent a row in the table
typedef struct {
    int id;
    int course;
    int grade;
} Row;

//check query validity
int validate_query(const char *query) {
    //validate SELECT, FROM, WHERE
    if (!strstr(query, "SELECT") || !strstr(query, "FROM")) {
        printf("Invalid query: Missing SELECT or FROM clause.\n");
        return 0;
    }
    return 1;
}

//handle SELECT queries
void execute_select(const char *query) {
    char attributes[100], table[100], condition[100];
    int has_where = sscanf(query, "SELECT %[^ ] FROM %[^ ] WHERE %[^\n]", attributes, table, condition);

    //validate table
    if (strcmp(table, "grades") != 0) {
        printf("Invalid query: Table %s does not exist.\n", table);
        return;
    }

    //parse attributes
    int select_id = strstr(attributes, "id") != NULL;
    int select_course = strstr(attributes, "course") != NULL;
    int select_grade = strstr(attributes, "grade") != NULL;

    //parse condition
    int filter_col = -1, filter_val = 0, op = 0;
    if (has_where == 3) {
        if (strstr(condition, "grade >") != NULL) {
            filter_col = 2;
            sscanf(condition, "grade > %d", &filter_val);
            op = 1;
        }
        //add more conditions if needed
    }

    //execute query
    printf("Query result:\n");
    for (int i = 0; i < num_rows; i++) {
        int match = 1;
        if (filter_col == 2 && op == 1)
            match = grades[i].grade > filter_val;

        if (match) {
            if (select_id) printf("%d ", grades[i].id);
            if (select_course) printf("%d ", grades[i].course);
            if (select_grade) printf("%d ", grades[i].grade);
            printf("\n");
        }
    }
}

//handle INSERT queries
void execute_insert(const char *query) {
    int id, course, grade;
    if (sscanf(query, "INSERT INTO grades (id, course, grade) VALUES (%d, %d, %d)", &id, &course, &grade) == 3) {
        grades[num_rows++] = (Row){id, course, grade};
        printf("Row inserted successfully.\n");
    } else {
        printf("Invalid INSERT query.\n");
    }
}

//handle DELETE queries
void execute_delete(const char *query) {
    char condition[100];
    if (sscanf(query, "DELETE FROM grades WHERE %[^\n]", condition) == 1) {
        int filter_col = -1, filter_val = 0, op = 0;
        if (strstr(condition, "grade >") != NULL) {
            filter_col = 2;
            sscanf(condition, "grade > %d", &filter_val);
            op = 1;
        }
        //deletion
        for (int i = 0; i < num_rows; i++) {
            if (filter_col == 2 && op == 1 && grades[i].grade > filter_val) {
                for (int j = i; j < num_rows - 1; j++) {
                    grades[j] = grades[j + 1];
                }
                num_rows--;
                i--; //adjust index after deletion
            }
        }
        printf("Rows deleted successfully.\n");
    } else {
        printf("Invalid DELETE query.\n");
    }
}