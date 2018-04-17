/**
 * Functions to output debugging information.
 */

#include "API.h"
#include "main.h"
#include "utilities.h"
#include "tui.h"
#include "debug.h"
#include "motor.h"
#include "liftControl.h"

#define DB_TABLE_ROWS 7
#define DB_TABLE_COLS 3
#define DB_TABLE_COL_WIDTH 32
#define DB_TABLE_VALUE_WIDTH 8

#define DB_GET_LABEL_ROW(row) ((row) + 1)
#define DB_GET_LABEL_COL(col) ((col) * DB_TABLE_COL_WIDTH + 2)
#define DB_GET_VALUE_ROW(row) ((row) + 1)
#define DB_GET_VALUE_COL(col) (((col) + 1) * DB_TABLE_COL_WIDTH - DB_TABLE_VALUE_WIDTH - 1)

#define dbTableValuePrintf(row, col, ...) mvprintf( \
        DB_GET_VALUE_ROW(row), DB_GET_VALUE_COL(col), __VA_ARGS__)

static const char *debugTable[DB_TABLE_ROWS][DB_TABLE_COLS] = {
    { "AUTONOMOUS",     "MISC",         "SENSORS" },
    { "rightError",     "liftTarget",   "liftPosition" },
    { "rightPower",     "mogoTarget",   "mogoPositoin" },
    { "leftError",      "coneTarget",   "conePosition" },
    { "leftPower",      NULL,           "gyro" },
    { "turnError",      "coneCount",    NULL},
    { "turnPower",      "grabState",    NULL },
};

static void debugPrintTable(void) {
    erase();

    /* mvhline(0, 0, '-', DB_TABLE_COLS * DB_TABLE_COL_WIDTH + 1); */
    for (int i = 0; i < DB_TABLE_COLS; i++) {
        /* mvvline(1, i * DB_TABLE_COL_WIDTH, '|', DB_TABLE_ROWS); */

        for (int j = 0; j < DB_TABLE_ROWS; j++) {
            const char *label = debugTable[j][i];
            if (label) {
                mvaddchstr(DB_GET_LABEL_ROW(j), DB_GET_LABEL_COL(i), label);
            }
        }
    }
    /* mvvline(1, DB_TABLE_COLS * DB_TABLE_COL_WIDTH, '|', DB_TABLE_ROWS); */
    /* mvhline(DB_TABLE_ROWS, 0, '-', DB_TABLE_COLS * DB_TABLE_COL_WIDTH + 1); */
}

void debugMonitor(void *parameter) {
#ifdef WINDOWS
    /* ANSI Escape codes don't work on windows even though MS says they do */
    return;
#endif

    debugPrintTable();

    while (true) {
        /* Printing the full table every frame makes it really jerky, so only do
         * it when a key (enter, in particular) is pressed.
         */
        if (!feof(stdin)) {
            while (!feof(stdin)) {
                getchar();
            }

            debugPrintTable();
        }

        dbTableValuePrintf(1, 0, "%8d", rightError);
        dbTableValuePrintf(2, 0, "%8d", rightPower);
        dbTableValuePrintf(3, 0, "%8d", leftError);
        dbTableValuePrintf(4, 0, "%8d", leftPower);
        dbTableValuePrintf(5, 0, "%8d", turnError);
        dbTableValuePrintf(6, 0, "%8d", turnPower);

        dbTableValuePrintf(1, 1, "%8d", liftTarget);
        dbTableValuePrintf(2, 1, "%8d", mogoTarget);
        dbTableValuePrintf(3, 1, "%8d", coneTarget);
        dbTableValuePrintf(5, 1, "%8d", numCones);
        dbTableValuePrintf(6, 1, "%8d", grabState);

        dbTableValuePrintf(1, 2, "%8d", liftPosition);
        dbTableValuePrintf(2, 2, "%8d", mogoPosition);
        dbTableValuePrintf(3, 2, "%8d", conePosition);
        dbTableValuePrintf(4, 2, "%8d", devgyroGet(&gyroDev));

        fflush(stdout);

        /* Have to do this because output is buffered */

        taskDelay(250);
    }
}

/* vim: set tw=80 ft=c: */
