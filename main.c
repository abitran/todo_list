#include "todo_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define DB_FILE "/tmp/tasks.dat"

int main(int argc, char *argv[]) {
  int opt;
  todo_list *todo = init_todo_list();
  if (todo == NULL)
    return 1;

  load_todo_list(todo, DB_FILE);
  while ((opt = getopt(argc, argv, "c:ld:u:")) != -1) {
    switch (opt) {
    case 'c': {
      char *task_name = optarg;
      char *task_description = NULL;
      char *task_due_date = NULL;

      if (optind < argc) {
        task_description = argv[optind];
        optind++;
      }

      if (optind < argc) {
        task_due_date = argv[optind];
        optind++;
      }
      struct tm dd;
      memset(&dd, 0, sizeof(struct tm));
      int day, month, year;
      if (sscanf(task_due_date, "%d/%d/%d", &day, &month, &year) == 3) {
        dd.tm_mday = day;
        dd.tm_mon = month - 1;
        dd.tm_year = year - 1900;
        dd.tm_isdst = -1;
      } else {
        fprintf(stderr, "Invalid date format! Use dd/mm/yyyy\n");
        break;
      }
      time_t rawtime = mktime(&dd);
      task_node *new_task = create_task(task_name, task_description, rawtime);
      if (new_task != NULL) {
        add_task(todo, new_task);
        save_todo_list(todo, DB_FILE);
        printf("Task %zu, added succesfully\n", new_task->id);
      }
      break;
    }
    case 'l':
      print_tasks(todo);
      break;
    case 'd': {
      delete_task(todo, atoi(optarg));
      save_todo_list(todo, DB_FILE);
      break;
    }
    }
  }

  return EXIT_SUCCESS;
}
