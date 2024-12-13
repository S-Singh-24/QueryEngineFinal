#include <stdio.h>

typedef struct n {
    int id;
    int course;
    int grade;
    struct n *next;
} node;

node *readTable(FILE *f, node *head);

void writeTable(FILE *f, node *head);

void printResults(node *head, const char **attributeNames, int attrCount);

node *selectData(char *query, char *schema, node *head, FILE *f);

void insertData(char *query, char *schema, FILE *f);

node *deleteData(char *query, char *schema, node *head, FILE *f);

void parseQuery(char *query, char *selectClause, char *fromClause, char *whereClause);

int parseSelect(char *selectClause, char **attributeNames);

void parseFrom(char *fromClause, char *tableName);

void parseWhere(char *whereClause, char *conditionCol, char *conditionOp, int *conditionValue);

int parseSchema(char *schema, char *tableName, char **attributeNames);

int isValidQuery(char *query, char *schema);

void runQuery(char *query, char *schema, const char *filename);
