#ifndef __TODO_LIST_H__
#define __TODO_LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef enum {
  OPENED,
  IN_PROGRESS,
  CLOSED,
}State;

typedef struct task_node {
  size_t id;
  State state;
  time_t created_at;
  time_t due_date;
  char name[100];
  char description[500];
  struct task_node *next;
}task_node;

typedef struct task_record {
  size_t id;
  State state;
  time_t created_at;
  time_t due_date;
  char name[100];
  char description[500];
} task_record;

typedef struct todo_list{
  task_node *head;
  task_node *tail;
  size_t size;
  size_t next_id;
} todo_list;

todo_list *init_todo_list();
void add_task(todo_list *, task_node *);
task_node *create_task(char *, char *, time_t);
void load_todo_list(todo_list *, const char *);
void save_todo_list(todo_list *, const char *);
void print_tasks(todo_list *);
const char *get_state(int);
void delete_task(todo_list *, size_t);
#endif





