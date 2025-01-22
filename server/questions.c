#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>           // For seeding random

#include "questions.h"
#include "types.h"
#include "utils.h"

typedef Question (*GenerateQuestionFunc)(sqlite3 *db);

static Question default_question() {
    return (Question){
        .prompt = "We couldn't generate a question, so how was your day ?",
        .choices = {"Very bad", "Bad", "Good", "Very good"},
        .correct_answer = 3
    };
}

static Question generate_station_name_question(sqlite3 *db) {
    const char *sql = "SELECT station_name_en, station_name_jp "
                      "FROM Station "
                      "ORDER BY RANDOM() "
                      "LIMIT ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to generate station name question: %s\n", sqlite3_errmsg(db));
        return default_question();
    }

    sqlite3_bind_int(stmt, 1, NUM_CHOICES);

    int correct_answer = random_bound(NUM_CHOICES);
    Question question = {.correct_answer = correct_answer};

    for (int i = 0; i < NUM_CHOICES; i++) {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            fprintf(stderr, "Not enough stations to generate name choices.\n");
            sqlite3_finalize(stmt);
            return default_question();
        }

        const char *name_en = (const char *)sqlite3_column_text(stmt, 0);
        const char *name_jp = (const char *)sqlite3_column_text(stmt, 1);

        if (!name_en) name_en = "Unknown(EN)";
        if (!name_jp) name_jp = "Unknown(JP)";

        strncpy(question.choices[i], name_en, CHOICES_BUFFER_SIZE);

        if (i == correct_answer) {
            snprintf(
                question.prompt,
                QUESTION_BUFFER_SIZE,
                "What is the English name for %s?",
                name_jp
            );
        }
    }

    sqlite3_finalize(stmt);

    return question;
}

static Question genrate_station_number_question(sqlite3* db) {
    const char *sql =
        "WITH RandomLine AS ("
        "    SELECT line_name "
        "    FROM Line "
        "    ORDER BY RANDOM() "
        "    LIMIT 1"
        ") "
        "SELECT p.company_name, p.line_name, p.prefix, s.station_number, s.station_name_en "
        "FROM Line AS p "
        "INNER JOIN Station_Line AS s "
        "  ON p.line_name = s.line_name "
        "WHERE p.line_name = (SELECT line_name FROM RandomLine) "
        "ORDER BY RANDOM() "
        "LIMIT ?;";

    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to generate station number question: %s\n", sqlite3_errmsg(db));
        return default_question();
    }

    sqlite3_bind_int(stmt, 1, NUM_CHOICES);

    int correct_answer = random_bound(NUM_CHOICES);
    Question question = {.correct_answer = correct_answer};

    for (int i = 0; i < NUM_CHOICES; i++) {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            fprintf(stderr, "Not enough stations to generate number choices.\n");
            sqlite3_finalize(stmt);
            return default_question();
        }

        const char *company_name     = (const char *)sqlite3_column_text(stmt, 0);
        const char *line_name        = (const char *)sqlite3_column_text(stmt, 1);
        const char *prefix           = (const char *)sqlite3_column_text(stmt, 2);
        int station_number           = sqlite3_column_int(stmt, 3);
        const char *station_name_en  = (const char *)sqlite3_column_text(stmt, 4);

        if (!company_name)    company_name    = "UnknownCompany";
        if (!line_name)       line_name       = "UnknownLine";
        if (!prefix)          prefix          = "XX";
        if (!station_name_en) station_name_en = "UnknownStation";

        snprintf(question.choices[i], CHOICES_BUFFER_SIZE, "%s-%d", prefix, station_number);

        if (i == correct_answer) {
            snprintf(
                question.prompt,
                QUESTION_BUFFER_SIZE,
                "On %s's %s line, what is the station number of %s?",
                company_name,
                line_name,
                station_name_en
            );
        }
    }

    sqlite3_finalize(stmt);

    return question;
}

// -------------------------------------------------------------------------
// generate_question
// -------------------------------------------------------------------------
Question generate_question(sqlite3 *db) {
    GenerateQuestionFunc qfuncs[] = {
        generate_station_name_question,
        genrate_station_number_question
    };
    int num_funcs = sizeof(qfuncs) / sizeof(qfuncs[0]);

    // Pick a random index
    return qfuncs[random_bound(num_funcs)](db);
}

