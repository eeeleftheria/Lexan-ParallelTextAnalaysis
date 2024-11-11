#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

int main(){
struct rlimit myrlimit;

// RLIMIT_AS: maximum size of process’s virtual memory in bytes
getrlimit(RLIMIT_AS, &myrlimit);
printf("Maximum address space = %lu and current = %lu\n",
	myrlimit.rlim_max,  myrlimit.rlim_cur);

// RLIMIT_CORE:  Maximum  size  of core file
getrlimit(RLIMIT_CORE, &myrlimit);
printf("Maximum core file size = %lu and current = %lu\n",
	myrlimit.rlim_max,  myrlimit.rlim_cur);

// RLIMIT_DATA: maximum size of files that the process may create
getrlimit(RLIMIT_DATA, &myrlimit);
printf("Maximum data+heap size = %lu and current = %lu\n",
	myrlimit.rlim_max,  myrlimit.rlim_cur);

// RLIMIT_FSIZE: maximum size of files that the process may create
getrlimit(RLIMIT_FSIZE, &myrlimit);
printf("Maximum file size = %lu and current = %lu\n",
	myrlimit.rlim_max,  myrlimit.rlim_cur);

// RLIMIT_STACK: maximum size of the process stack, in bytes.
getrlimit(RLIMIT_STACK, &myrlimit);
printf("Maximum stack size = %lu and current = %lu\n",
	myrlimit.rlim_max,  myrlimit.rlim_cur);


return 1;
}
