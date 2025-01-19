#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define DB_PATH "data/trains.db"

int main(void) {

    sqlite3 *db;

    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(
            stderr, 
            "Cannot open database: %s\n",
            sqlite3_errmsg(db)
        );
        return EXIT_FAILURE;
    }

    sqlite3_close(db);
    return EXIT_SUCCESS;

}
