#include <stdio.h>

typedef struct n {
    int id;
    int course;
    int grades;
    struct n *next;
} node;

node *readTable(FILE *f, node *head);

void writeTable(FILE *f, node *head);

void printResults(node *head, const char **attributeNames, int attrCount);

node *seleetData(char *query, char *schema, node *head, FILE *f);

void insertData(char *query, char *schema, FILE *f);

node *deleteData(char *query, char *schema, node *head, FILE *f);

void parseQuery(char *query, char *selectClause, char *fromClause, char *whereClause);

int parseSelect(char *selectClause, char **attributeNames);

void parseFrom(char *fromClause, char *tableName);

void parseWhere(char *whereClause, char *conditionCol, char *conditionOp, int *conditionValue);

int parseSchema(char *schema, char *tableName, char **attributeNames);

int isValidQuery(char *query, char *schema);

int **runQuery(char *query, char *schema, int **table, int nrows, int ncols);
