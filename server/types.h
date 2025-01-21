#pragma once

#define QUESTION_BUFFER_SIZE 1024
#define CHOICES_BUFFER_SIZE 512
#define NUM_CHOICES 4 

typedef struct {
    char prompt[QUESTION_BUFFER_SIZE];
    char choices[NUM_CHOICES][CHOICES_BUFFER_SIZE];
    int correct_answer;
} Question;
