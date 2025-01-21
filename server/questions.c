#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "questions.h"
#include "types.h"
#include "utils.h"



Question default_question() {
    return (Question){
        .prompt="Which line is the best one ?",
        .choices={"Yamanote", "Oedo", "Chuo-Sobu", "Ginza"},
        .correct_answer=2
    };
}

Question generate_question(sqlite3 *db) {
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