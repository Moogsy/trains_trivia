#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "questions.h"
#include "types.h"
#include "utils.h"

typedef Question (*GenerateQuestionFunc)(sqlite3 *db);

Question default_question() {
    return (Question){
        .prompt="We couldn't generate a question, so how was your day ?",
        .choices={"Very bad", "Bad", "Good", "Very good"},
        .correct_answer=3
    };
}

static Question genrate_station_number_question(sqlite3* db) {
    const char *sql = 
        "SELECT p.company_name, p.line_name, p.prefix, s.station_number, s.station_name_en "
        "FROM Line AS p "
        "INNER JOIN Station_Line AS s "
        "ON p.line_name = s.line_name "
        "ORDER BY RANDOM() "
        "LIMIT ?;";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to generate station number question: %s", sqlite3_errmsg(db));
        return default_question();
    }
    sqlite3_bind_int(stmt, 1, NUM_CHOICES);

    int correct_answer = random_bound(NUM_CHOICES);
    Question question = {.correct_answer=correct_answer};

    for (int i = 0; i < NUM_CHOICES; i++) {
        int rc = sqlite3_step(stmt);

        if (rc != SQLITE_ROW) {
            return default_question();
        }

        const char *prefix = (const char*)sqlite3_column_text(stmt, 2);
        int station_number = sqlite3_column_int(stmt, 3);
        snprintf(question.choices[i], CHOICES_BUFFER_SIZE, "%s%d", prefix, station_number);

        if (i == correct_answer) {
            const char *company_name = (const char*)sqlite3_column_text(stmt, 0);
            const char *line_name = (const char*)sqlite3_column_text(stmt, 1);
            const char *station_name_en = (const char*)sqlite3_column_text(stmt, 4);

            snprintf(
                question.prompt, 
                QUESTION_BUFFER_SIZE, 
                "On %s's %s line, what is the station number of %s ?",
                company_name,
                line_name,
                station_name_en
            );
        }
    }

    return question;
}

static Question generate_station_name_question(sqlite3 *db) {
    const char *sql = "SELECT * FROM Station ORDER BY RANDOM() LIMIT ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to generate question: %s\n", sqlite3_errmsg(db));
        return default_question();
    }

    sqlite3_bind_int(stmt, 1, NUM_CHOICES);

    int correct_answer = random_bound(NUM_CHOICES);
    Question question = {.correct_answer=correct_answer};

    for (int i = 0; i < NUM_CHOICES; i++) {
        int rc = sqlite3_step(stmt);

        if (rc != SQLITE_ROW) {
            return default_question();
        }
        const char *name_en = (const char*)sqlite3_column_text(stmt, 0);
        strncpy(question.choices[i], name_en, CHOICES_BUFFER_SIZE);

        if (i == correct_answer) {
            const char *name_jp = (const char*)sqlite3_column_text(stmt, 1);
            snprintf(
                question.prompt, 
                QUESTION_BUFFER_SIZE, 
                "What is the english name for %s ?",
                name_jp
            );
        }
    }

    return question;
}

Question generate_question(sqlite3 *db) {

    GenerateQuestionFunc qfuncs[] = {
        generate_station_name_question,
        genrate_station_number_question
    };

    int num_funcs = sizeof(qfuncs) / sizeof(qfuncs[0]);

    return qfuncs[random_bound(num_funcs)](db);
}
