#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/user.h>
#include <sys/reg.h>
void launch(char * argv[]) {
      ptrace(PTRACE_TRACEME, 0, 0, 0);
      execl("/bin/echo", "/bin/echo", "Hello, world!", NULL);
      perror("execl");
    }
void prot(pid_t pid)
{
        int st,ls=0,ts=0;
	int sys[400][400];
        waitpid(pid, &st, 0);
	ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
  while (!WIFEXITED(st)) 
  {
	//st=0;
    struct user_regs_struct usr;
    
    ptrace(PTRACE_SYSCALL, pid, 0, 0);
    waitpid(pid, &st, 0);
    
    
    if (WIFSTOPPED(st) && WSTOPSIG(st) & 0x80) 
	{
     	 	ptrace(PTRACE_GETREGS, pid, 0, &usr);
      		printf("SYS %lld\n", usr.orig_rax);
      		ts=usr.orig_rax;
      		if(ls!=-1)
		{
			if(sys[ls][ts]!=1)
				{
					printf("You system is under attack!");
					ptrace(PTRACE_KILL,pid,0,0);
				}
		}
     		ptrace(PTRACE_SYSCALL, pid, 0, 0);
      		waitpid(pid, &st, 0);
    	}
    
   }



}
int main(int argc, char *argv[]) {
  pid_t pid = fork();
  if (pid)
    prot(pid);
  else
    launch(argv);
  return 0;
}
