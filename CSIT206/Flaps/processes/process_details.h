void get_process_info(int pid, FILE *stream);
void list_all_processes();
void process_info(int pid);
void group_processes(int pid);
void user_processes(uid_t uid, int is_effective);