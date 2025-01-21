#include <stdio.h>
#include <string.h>

#include "yyjson.h"
#include "utils.h"

int json_to_question(const char *json_str, Question *q) {
    // Parse the JSON string
    yyjson_doc *doc = yyjson_read(json_str, strlen(json_str), 0);
    if (!doc) {
        printf("Failed to parse JSON.\n");
        return -1;
    }

    yyjson_val *root = yyjson_doc_get_root(doc);

    // Extract the "question" 
    yyjson_val *prompt_val = yyjson_obj_get(root, "question");
    if (!yyjson_is_str(prompt_val)) {
        printf("Missing or invalid 'question' field.\n");
        yyjson_doc_free(doc);
        return -1;
    }
    strncpy(q->prompt, yyjson_get_str(prompt_val), QUESTION_BUFFER_SIZE - 1);

    // Extract the "correct_answer" field
    yyjson_val *correct_answer_val = yyjson_obj_get(root, "correct_answer");
    if (!yyjson_is_int(correct_answer_val)) {
        printf("Missing or invalid 'correct_answer' field.\n");
        yyjson_doc_free(doc);
        return -1;
    }
    q->correct_answer = (int)yyjson_get_int(correct_answer_val);

    // Extract the "choices" array
    yyjson_val *choices_val = yyjson_obj_get(root, "choices");
    if (!yyjson_is_arr(choices_val)) {
        printf("Missing or invalid 'choices' field.\n");
        yyjson_doc_free(doc);
        return -1;
    }

    size_t idx, max = yyjson_arr_size(choices_val);
    if (max > NUM_CHOICES) max = NUM_CHOICES;  // Limit to NUM_CHOICES

    yyjson_val *choice_val;
    yyjson_arr_foreach(choices_val, idx, max, choice_val) {
        if (!yyjson_is_str(choice_val)) {
            printf("Invalid choice at index %zu.\n", idx);
            continue;
        }
        strncpy(q->choices[idx], yyjson_get_str(choice_val), CHOICES_BUFFER_SIZE - 1);
    }

    // Free the JSON document
    yyjson_doc_free(doc);
    return 0;
}