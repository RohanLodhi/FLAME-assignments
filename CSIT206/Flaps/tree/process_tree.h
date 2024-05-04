typedef struct ProcessNode {
    int pid;
    int ppid;
    char name[256];
    struct ProcessNode* next;  // Next process in the flat list
    struct ProcessNode* child; // First child process
    struct ProcessNode* sibling; // Next sibling process
} ProcessNode;

typedef struct Process {
    int pid;
    int ppid;
    int pgid;
} Process;

ProcessNode* add_process(int pid, int ppid, const char* name);
void build_tree();
void print_tree(ProcessNode* node, int depth);
ProcessNode* find_process_by_pid(int pid);
void read_proc();