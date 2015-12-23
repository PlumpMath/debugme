#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


// TODO: support configuring a different path
static const char * const SCRIPT = "Debugger/debugme.sh";
static const char * const ARGS[] = {"debugme.sh", NULL};
static const char * const ENVIRON[] = {NULL};


static volatile atomic_int g_debugme_cnt = 0;


static void debugme_alert(const char *msg) 
{
    write(2, msg, strlen(msg));
}


// We can only call signal-safe functions and system calls while in a signal
// handler.  This means no locks or memory allocations.
// See signal(7)
static void debugme_signal_handler(int sig) 
{
    // In case multiple threads crash at once, only start the debugger once.
    // Multiple threads do NOT mask each other out while one is handling a
    // thread-directed signal.  
    if (atomic_fetch_add(&g_debugme_cnt, 1) != 0) {
        //debugme_alert("debugme: already tripped\n");
        // don't return, otherwise it just sends SEGV again.
        while (1) { sleep(1); }
    }

    debugme_alert("debugme: launching debugger\n");

    int pid = fork();
    if (pid == -1) {
        debugme_alert("error: fork failed\n");
        _exit(30);
    }

    if (pid == 0) {
        // child - launch debugger

        // Don't leak FDs, bad form and in exceptional cases can cause hangs.
        // Leave 0,1,2 (in,out,err)
        for (int i = 3; i < 10000; i++) {
            close(i);
        }

        // Should not return
        int ret = execve(SCRIPT, ARGS, ENVIRON);
        debugme_alert("error: debugger exec failed\n");
        _exit(1);
    } else {
        // parent - ourself
        waitpid(pid, NULL, 0);

        // If we got here, debugger failed to launch, or it failed to kill us
        // when done.
        debugme_alert("error: still alive after running debugger\n");
        _exit(31);
    }
}


void debugme_install() 
{
    signal(SIGILL, debugme_signal_handler);
    signal(SIGSEGV, debugme_signal_handler);
}
