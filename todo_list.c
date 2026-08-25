#include "todo_list.h"
#include <time.h>
#define DB_FILE "/tmp/tasks.dat"

todo_list *init_todo_list() {
  todo_list *todo = malloc(sizeof(todo_list));
  if (todo == NULL) {
    perror("Error creating todo list\n");
    return NULL;
  }

  todo->head = NULL;
  todo->tail = NULL;
  todo->size = 0;
  todo->next_id = 1;
  return todo;
}

task_node *create_task(char *name, char *description, time_t due_date) {
  task_node *task = malloc(sizeof(task_node));
  if (task == NULL)
    return NULL;
  snprintf(task->name, sizeof(task->name), "%s", name);
  snprintf(task->description, sizeof(task->description), "%s", description);
  task->created_at = time(NULL);
  task->state = OPENED;
  task->due_date = due_date;
  task->next = NULL;
  return task;
}

void add_task(todo_list *todo, task_node *task) {
  if (todo == NULL || task == NULL)
    return;

  task->id = todo->next_id;
  todo->next_id++;

  if (todo->head == NULL) {
    todo->head = task;
    todo->tail = task;
  } else {
    todo->tail->next = task;
    todo->tail = task;
  }
  todo->size++;
}

const char *get_state(int state) {
  switch (state) {
  case OPENED:
    return "OPENED";
  case CLOSED:
    return "CLOSED";
  case IN_PROGRESS:
    return "IN PROGRESS";
  default:
    return "INVALID STATE";
  }
}

void print_tasks(todo_list *todo) {
  if (todo == NULL) {
    fprintf(stderr, "File does not exist!\n");
    return;
  }
  char due_buffer[20];
  char created_buffer[20];

  task_node *current = todo->head;
  printf("\n================================= CURRENT TASKS (%zu) "
         "===============================================\n",
         todo->size);
  printf("%-4s | %-25s | %-25s | %-8s | %-12s | %-12s\n", "ID", "Title", "Description",
         "Status", "Created Date", "Due Date");
  printf("-----------------------------------------------------------------------"
         "----------------------------\n");
  while (current != NULL) {
    struct tm *due = localtime(&current->due_date);
    if (due != NULL){
      strftime(due_buffer, sizeof(due_buffer), "%d/%m/%Y", due);
    } else {
      snprintf(due_buffer, sizeof(due_buffer), "No date!\n");
    }
    struct tm *created = localtime(&current->created_at);
    if (created != NULL) {
      strftime(created_buffer, sizeof(created_buffer), "%d/%m/%Y", created); 
    } else {
      snprintf(created_buffer, sizeof(created_buffer), "No date!\n");
    }
    printf("%-4zu | %-25s | %-25s | %-8s | %-12s | %-12s\n", current->id, current->name,
           current->description, get_state(current->state), created_buffer, due_buffer);
    current = current->next;
  }
}

void load_todo_list(todo_list *todo, const char *filename) {
  if (todo == NULL || filename == NULL)
    return;

  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    return;
  if (fread(&(todo->next_id), sizeof(size_t), 1, fp) != 1) {
    fclose(fp);
    return;
  }
  task_record record;
  while (fread(&record, sizeof(task_record), 1, fp) == 1) {
    task_node *task =
        create_task(record.name, record.description, record.due_date);
    if (task != NULL) {
      task->id = record.id;
      task->state = record.state;
      task->created_at = record.created_at;
    }
    if (todo->head == NULL) {
      todo->head = task;
      todo->tail = task;
    } else {
      todo->tail->next = task;
      todo->tail = task;
    }
    todo->size++;
  }
  fclose(fp);
}

void save_todo_list(todo_list *todo, const char *filename) {
  if (todo == NULL || filename == NULL)
    return;

  FILE *fp = fopen(filename, "wb");
  if (fp == NULL) {
    perror("Unable to create file\n");
    return;
  }

  fwrite(&(todo->next_id), sizeof(size_t), 1, fp);
  task_node *current = todo->head;
  while (current != NULL) {
    task_record record;
    record.id = current->id;
    snprintf(record.name, sizeof(record.name), "%s", current->name);
    snprintf(record.description, sizeof(record.description), "%s",
             current->description);
    record.state = current->state;
    record.created_at = current->created_at;
    record.due_date = current->due_date;

    fwrite(&record, sizeof(task_record), 1, fp);
    current = current->next;
  }
  fclose(fp);
}

void delete_task(todo_list *todo, size_t id) {
  if (todo->head->id == id) {
    task_node *tmp = todo->head;
    todo->head = todo->head->next;

    if (todo->head == NULL)
      todo->tail = NULL;

    free(tmp);

    todo->size--;
    printf("Task %zu deleted successfully\n", id);
    return;
  }
  task_node *current = todo->head;
  while (current->next != NULL && current->next->id != id)
    current = current->next;

  if (current->next == NULL) {
    printf("Task %zu not found\n", id);
    return;
  }
  task_node *tmp = current->next;
  current->next = tmp->next;
  if (tmp == todo->tail)
    todo->tail = current;
  free(tmp);
  todo->size--;
  printf("Task %zu deleted successfully\n", id);
}
