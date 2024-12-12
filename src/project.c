#include "project.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Helper function to create a new linked list node
node *createNode(int id, int course, int grades) {
    node *newNode = (node *) malloc(sizeof(node));
    if (newNode == NULL) {
        return NULL;
    }

    newNode->id = id;
    newNode->course = course;
    newNode->grades = grades;
    newNode->next = NULL;

    return newNode;
}

// Helper function to free a linked list
void freeLinkedList(node *head) {
    node *current = head;

    while (current != NULL) {
        node *nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

// Reads the table from grades.csv and returns a pointer to results linked list
node *readTable(FILE *f, node *head) {
    char line[11];
    head = NULL;
    node *current = NULL;

    // While there are lines to read
    while (fgets(line, sizeof(line), f)) {
        int id, course, grades;

        // Parse the line
        if (sscanf(line, "%d,%d,%d", &id, &course, &grades) != 3) {
            continue;
        }

        // Create a new node
        node *newNode = createNode(id, course, grades);
        if (newNode == NULL) {
            return NULL;
        }

        // Add the node to the linked list
        if (head == NULL) {
            head = newNode;
            current = head;
        } else {
            current->next = newNode;
            current = current->next;
        }
    }

    return head;
}

// Writes the linked list to a .csv file
void writeTable(FILE *f, node *head) {
    node *current = head;

    // While there are nodes to write
    while (current != NULL) {
        // Write the node to the file
        fprintf(f, "%d,%d,%d\n", current->id, current->course, current->grades);
        current = current->next;
    }
}

// !!!! Prints all columns regardless of the select clause !!!!
// Prints the results linked list as tab-separated values
void printResults(node *head, const char **attributeNames, int attrCount) {
    node *current = head;

    // Print the column names
    for (int i = 0; i < attrCount; i++) {
        printf("%s", attributeNames[i]);

        // Print a comma if not the last column
        if (i < attrCount - 1) {
            printf("\t");
        }
    }
    printf("\n");

    // Print the results
    while (current != NULL) {
        printf("%d\t%d\t%d\n", current->id, current->course, current->grades);
        current = current->next;
    }
}

// Trim leading and trailing whitespace from a string
char *trim(char *str) {
    if (str == NULL) {
        return NULL;
    }

    // Allocate a buffer to store the trimmed string
    size_t len = strlen(str);
    char *buffer = malloc((len + 1) * sizeof(char));
    if (buffer == NULL) {
        return NULL;
    }

    // Trim leading space
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        buffer[0] = '\0';
        return buffer;
    }

    // Trim trailing space
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    // Copy the trimmed string into the buffer
    size_t trimmed_len = end - str + 1;
    strncpy(buffer, str, trimmed_len);
    buffer[trimmed_len] = '\0';

    return buffer;
}

void parseQuery(char *query, char *selectClause, char *fromClause, char *whereClause) {
    // Convert the query to lowercase
    char *lowerQuery = strdup(query);
    for (int i = 0; lowerQuery[i]; i++) {
        lowerQuery[i] = tolower(lowerQuery[i]);
    }

    // Find the select clause
    char *selectStart = strstr(lowerQuery, "select");
    if (selectStart == NULL) {
        selectClause[0] = '\0';
        fromClause[0] = '\0';
        whereClause[0] = '\0';
        free(lowerQuery);
        return;
    }

    // Move past 'select '
    selectStart += 7;

    // Find the from clause
    char *fromStart = strstr(selectStart, "from");
    if (fromStart == NULL) {
        selectClause[0] = '\0';
        fromClause[0] = '\0';
        whereClause[0] = '\0';
        free(lowerQuery);
        return;
    }

    // Extract the column names
    strncpy(selectClause, query + (selectStart - lowerQuery), fromStart - selectStart);
    selectClause[fromStart - selectStart] = '\0';
    strcpy(selectClause, trim(selectClause));

    // Move past 'from '
    fromStart += 5;

    // Find the where clause
    char *whereStart = strstr(fromStart, "where");
    if (whereStart != NULL) {
        // Extract the table name
        strncpy(fromClause, query + (fromStart - lowerQuery), whereStart - fromStart);
        fromClause[whereStart - fromStart] = '\0';
        strcpy(fromClause, trim(fromClause));

        // Move past 'where '
        whereStart += 6;

        // Extract the where clause
        strcpy(whereClause, query + (whereStart - lowerQuery));
        strcpy(whereClause, trim(whereClause));
    } else {
        // Extract the table name
        strcpy(fromClause, query + (fromStart - lowerQuery));
        strcpy(fromClause, trim(fromClause));

        whereClause[0] = '\0';
    }

    free(lowerQuery);
}

int parseSelect(char *selectClause, char **attributeNames) {
    // Copy select clause into a new string
    char *s = strdup(selectClause);
    int i = 0;

    // Tokenize the string
    char *token = strtok(s, ", ");
    while (token != NULL) {
        attributeNames[i] = (char *) malloc(100 * sizeof(char));
        strcpy(attributeNames[i], token);
        i++;
        token = strtok(NULL, ", ");
    }

    free(s);
    return i;
}

void parseFrom(char *fromClause, char *tableName) {
    // Trim leading and trailing whitespace
    char *trimmed = trim(fromClause);
    // Copy table name into new string
    strcpy(tableName, trimmed);
}

void parseWhere(char *whereClause, char *conditionCol, char *conditionOp, int *conditionVal) {
    // Extract the column, operator, and value from the where clause
    if (sscanf(whereClause, "%s %s %d", conditionCol, conditionOp, conditionVal) != 3) {
        // Handle invalid condition
        conditionCol[0] = '\0';
        conditionOp[0] = '\0';
        *conditionVal = 0;
        return;
    }

    // Validate the condition value
    char *valueStr = strstr(whereClause, conditionOp) + strlen(conditionOp);
    while (isspace(*valueStr)) valueStr++;
    char *endPtr;
    long val = strtol(valueStr, &endPtr, 10);

    if (*endPtr != '\0' && !isspace(*endPtr)) {
        // Handle invalid integer value
        conditionCol[0] = '\0';
        conditionOp[0] = '\0';
        *conditionVal = 0;
    } else {
        *conditionVal = (int) val;
    }
}

int parseSchema(char *schema, char *tableName, char **attributeNames) {
    // Find the end of the table name
    char *tableEnd = strstr(schema, "(");
    char *columnsEnd = strstr(schema, ")");
    if (tableEnd + 1 == columnsEnd) {
        return 0;
    }
    if (tableEnd == NULL || columnsEnd == NULL || tableEnd > columnsEnd) {
        return -1;
    }

    // Extract the table name
    strncpy(tableName, schema, tableEnd - schema);
    tableName[tableEnd - schema] = '\0';

    // Move past '('
    char *columnsStart = strdup(tableEnd + 1);

    // Tokenize the column names
    int i = 0;
    char *token = strtok(columnsStart, ", ");
    while (token != NULL) {
        attributeNames[i] = (char *) malloc(100 * sizeof(char));
        strcpy(attributeNames[i], token);
        i++;
        token = strtok(NULL, ", ");
    }

    // Remove the ')' from the last column name
    char *lastAttribute = attributeNames[i - 1];
    char *closingParen = strchr(lastAttribute, ')');
    if (closingParen != NULL) {
        *closingParen = '\0';
    }

    free(columnsStart);
    return i;
}

int isValidQuery(char *query, char *schema) {
    // Convert the query to lowercase
    char *lowerQuery = strdup(query);
    for (int i = 0; lowerQuery[i]; i++) {
        lowerQuery[i] = tolower(lowerQuery[i]);
    }

    // Check if the query has a select clause
    if (strstr(lowerQuery, "select") == NULL) {
        free(lowerQuery);
        return 0;
    }

    // Check if the query has a from clause
    if (strstr(lowerQuery, "from") == NULL) {
        free(lowerQuery);
        return 0;
    }

    // Parse the query
    char *selectClause = malloc(100 * sizeof(char));
    char *fromClause = malloc(100 * sizeof(char));
    char *whereClause = malloc(100 * sizeof(char));
    parseQuery(query, selectClause, fromClause, whereClause);

    // Get column names
    char **attributeNames = malloc(100 * sizeof(char *));
    int numAttributes = parseSelect(selectClause, attributeNames);

    // Get table name
    char *tableName = malloc(100 * sizeof(char));
    parseFrom(fromClause, tableName);

    // Parse the schema
    char *tableName2 = malloc(100 * sizeof(char));
    char **attributeNames2 = malloc(100 * sizeof(char *));
    int numAttributes2 = parseSchema(schema, tableName2, attributeNames2);

    // Check if table names match
    if (strcmp(tableName, tableName2) != 0) {
        free(selectClause);
        free(fromClause);
        free(whereClause);
        free(attributeNames);
        free(attributeNames2);
        free(tableName);
        free(tableName2);
        free(lowerQuery);
        return 0;
    }

    // Check if column names match
    for (int i = 0; i < numAttributes; i++) {
        if (strcmp(attributeNames[i], attributeNames2[i]) != 0) {
            free(selectClause);
            free(fromClause);
            free(whereClause);
            free(attributeNames);
            free(attributeNames2);
            free(tableName);
            free(tableName2);
            free(lowerQuery);
            return 0;
        }
    }

    free(selectClause);
    free(fromClause);
    free(whereClause);
    free(attributeNames);
    free(attributeNames2);
    free(tableName);
    free(tableName2);
    free(lowerQuery);
    return 1;
}

int **runQuery(char *query, char *schema, int **table, int nrows, int ncols) {
    // Check if the query is valid
    if (!isValidQuery(query, schema)) {
        return NULL;
    }

    // Allocate memory for the result
    int **result = malloc(nrows * sizeof(int *));
    for (int i = 0; i < nrows; i++) {
        result[i] = malloc(ncols * sizeof(int));
    }

    // Parse the query
    char *selectClause = malloc(100 * sizeof(char));
    char *fromClause = malloc(100 * sizeof(char));
    char *whereClause = malloc(100 * sizeof(char));
    parseQuery(query, selectClause, fromClause, whereClause);

    // Parse the select clause to ge the column names
    char **selectColumns = malloc(100 * sizeof(char *));
    int numSelectColumns = parseSelect(selectClause, selectColumns);

    // Parse the from clause to get the table name
    char *tableName = malloc(100 * sizeof(char));
    parseFrom(fromClause, tableName);

    // Parse the where clause to get the condition column, operator, and value
    char *conditionCol = malloc(100 * sizeof(char));
    char *conditionOp = malloc(100 * sizeof(char));
    int *conditionVal = malloc(sizeof(int));
    parseWhere(whereClause, conditionCol, conditionOp, conditionVal);

    char **schemaColumns = malloc(100 * sizeof(char *));
    int numSchemaColumns = parseSchema(schema, tableName, schemaColumns);

    // Find the index of the condition column
    int conditionColIndex = -1;
    for (int i = 0; i < numSchemaColumns; i++) {
        if (strcmp(conditionCol, schemaColumns[i]) == 0) {
            conditionColIndex = i;
            break;
        }
    }

    int resultRows = 0;
    // For each row in the input table
    for (int i = 0; i < nrows; i++) {
        // Check if the operator is '>' and the value is greater than the condition value
        if ((strcmp(conditionOp, ">") == 0 && table[i][conditionColIndex] > *conditionVal) ||
            (strcmp(conditionOp, "<") == 0 && table[i][conditionColIndex] < *conditionVal) ||
            (strcmp(conditionOp, "=") == 0 && table[i][conditionColIndex] == *conditionVal) ||
            (strcmp(conditionOp, ">=") == 0 && table[i][conditionColIndex] >= *conditionVal) ||
            (strcmp(conditionOp, "<=") == 0 && table[i][conditionColIndex] <= *conditionVal) ||
            (strcmp(conditionOp, "!=") == 0 && table[i][conditionColIndex] != *conditionVal)) {
            // Copy the selected columns into the result
            for (int j = 0; j < numSelectColumns; j++) {
                for (int k = 0; k < numSchemaColumns; k++) {
                    if (strcmp(selectColumns[j], schemaColumns[k]) == 0) {
                        result[resultRows][j] = table[i][k];
                    }
                }
            }

            resultRows++;
        }
    }

    // Reallocate the result to the correct size
    result = realloc(result, resultRows * sizeof(int *));
    for (int i = 0; i < resultRows; i++) {
        result[i] = realloc(result[i], numSelectColumns * sizeof(int));
    }

    free(selectClause);
    free(fromClause);
    free(whereClause);
    for (int i = 0; i < numSelectColumns; i++) {
        free(selectColumns[i]);
    }
    free(selectColumns);
    for (int i = 0; i < numSchemaColumns; i++) {
        free(schemaColumns[i]);
    }
    free(schemaColumns);
    free(tableName);
    free(conditionCol);
    free(conditionOp);
    free(conditionVal);

    return result;
}

// Helper function to check if a node matches the condition in the where clause
int matchesCondition(node *current, const char *conditionCol, const char *conditionOp, int conditionVal) {
    int value;

    // Determine which column to check
    if (strcmp(conditionCol, "id") == 0) {
        value = current->id;
    } else if (strcmp(conditionCol, "course") == 0) {
        value = current->course;
    } else if (strcmp(conditionCol, "grade") == 0) {
        value = current->grades;
    } else {
        return 0; // Invalid column name
    }

    // Compare the value with the condition
    if (strcmp(conditionOp, ">") == 0) {
        return value > conditionVal;
    } else if (strcmp(conditionOp, "<") == 0) {
        return value < conditionVal;
    } else if (strcmp(conditionOp, "=") == 0) {
        return value == conditionVal;
    } else if (strcmp(conditionOp, ">=") == 0) {
        return value >= conditionVal;
    } else if (strcmp(conditionOp, "<=") == 0) {
        return value <= conditionVal;
    } else if (strcmp(conditionOp, "!=") == 0) {
        return value != conditionVal;
    } else {
        return 0; // Invalid operator
    }
}

node *selectData(char *query, char *schema, node *head, FILE *f) {
    // Check if the query is valid
    if (!isValidQuery(query, schema)) {
        return NULL;
    }

    // Parse the query
    char *selectClause = malloc(100 * sizeof(char));
    char *fromClause = malloc(100 * sizeof(char));
    char *whereClause = malloc(100 * sizeof(char));
    parseQuery(query, selectClause, fromClause, whereClause);

    // Parse the select clause to get the column names
    char **selectColumns = malloc(100 * sizeof(char *));
    int numSelectColumns = parseSelect(selectClause, selectColumns);

    // Parse the from clause to get the table name
    char *tableName = malloc(100 * sizeof(char));
    parseFrom(fromClause, tableName);

    char *conditionCol = malloc(100 * sizeof(char));
    char *conditionOp = malloc(100 * sizeof(char));
    int *conditionVal = malloc(sizeof(int));

    // Parse the where clause to get the condition column, operator, and value
    if (whereClause != NULL) {
        parseWhere(whereClause, conditionCol, conditionOp, conditionVal);
    }

    // Read the table
    head = readTable(f, NULL);


    // !!!! ADD FUNCTIONALITY FOR ORDER BY !!!!
    node *current = head;
    node *resultHead = NULL;
    node *resultCurrent = NULL;
    if (whereClause != NULL) {
        while (current != NULL) {
            // Check if the current node matches the where clause
            if (!matchesCondition(current, conditionCol, conditionOp, *conditionVal)) {
                current = current->next;
                continue;
            }

            // Create a new node
            node *newNode = (node *) malloc(sizeof(node));
            newNode->id, newNode->course, newNode->grades = 0;
            if (newNode == NULL) {
                return NULL;
            }

            // Copy the selected columns into the new node
            for (int i = 0; i < numSelectColumns; i++) {
                if (strcmp(selectColumns[i], "id") == 0) {
                    newNode->id = current->id;
                } else if (strcmp(selectColumns[i], "course") == 0) {
                    newNode->course = current->course;
                } else if (strcmp(selectColumns[i], "grades") == 0) {
                    newNode->grades = current->grades;
                }
            }
            newNode->next = NULL;

            // Add the node to the result linked list
            if (resultHead == NULL) {
                resultHead = newNode;
                resultCurrent = resultHead;
            } else {
                resultCurrent->next = newNode;
                resultCurrent = resultCurrent->next;
            }

            current = current->next;
        }
    }

    free(selectClause);
    free(fromClause);
    free(whereClause);
    for (int i = 0; i < numSelectColumns; i++) {
        free(selectColumns[i]);
    }
    free(selectColumns);
    free(tableName);
    free(conditionCol);
    free(conditionOp);
    free(conditionVal);

    if (whereClause != NULL) {
        freeLinkedList(head);
        return resultHead;
    }

    return head;
}

