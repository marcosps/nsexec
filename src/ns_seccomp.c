#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <seccomp.h>

static bool resolve_add_rule(scmp_filter_ctx ctx, const char *sys_name)
{
	int sysc;

	sysc = seccomp_syscall_resolve_name(sys_name);
	if (sysc == __NR_SCMP_ERROR) {
		fprintf(stderr, "Invalid syscall: %s\n", sys_name);
		return false;
	}

	if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, sysc, 0) < 0) {
		fprintf(stderr, "Couldn't add rule for syscall %s\n", sys_name);
		return false;
	}

	return true;
}

bool default_whitelist(void)
{
	int *i;
	int syscall_whitelist[] = {
		SCMP_SYS(access),
		SCMP_SYS(accept4),
		SCMP_SYS(alarm),
		SCMP_SYS(arch_prctl),
		SCMP_SYS(bind),
		SCMP_SYS(bpf),
		SCMP_SYS(brk),
		SCMP_SYS(capget),
		SCMP_SYS(capset),
		SCMP_SYS(chdir),
		SCMP_SYS(chmod),
		SCMP_SYS(chown),
		SCMP_SYS(clock_getres),
		SCMP_SYS(clock_gettime),
		SCMP_SYS(clock_nanosleep),
		SCMP_SYS(clone),
		SCMP_SYS(close),
		SCMP_SYS(connect),
		SCMP_SYS(creat),
		SCMP_SYS(dup),
		SCMP_SYS(dup2),
		SCMP_SYS(dup3),
		SCMP_SYS(epoll_create),
		SCMP_SYS(epoll_create1),
		SCMP_SYS(epoll_ctl),
		SCMP_SYS(epoll_wait),
		SCMP_SYS(epoll_pwait),
		SCMP_SYS(eventfd),
		SCMP_SYS(eventfd2),
		SCMP_SYS(execve),
		SCMP_SYS(exit),
		SCMP_SYS(exit_group),
		SCMP_SYS(inotify_add_watch),
		SCMP_SYS(inotify_init1),
		SCMP_SYS(inotify_rm_watch),
		SCMP_SYS(ioctl),
		SCMP_SYS(ioprio_get),
		SCMP_SYS(ioprio_set),
		SCMP_SYS(io_setup),
		SCMP_SYS(faccessat),
		SCMP_SYS(faccessat2),
		SCMP_SYS(fadvise64),
		SCMP_SYS(fallocate), /* firefox */
		SCMP_SYS(fchdir),
		SCMP_SYS(fchmod),
		SCMP_SYS(fchmodat),
		SCMP_SYS(fchown),
		SCMP_SYS(fchownat),
		SCMP_SYS(fcntl),
		SCMP_SYS(fcntl64),
		SCMP_SYS(fdatasync),
		SCMP_SYS(fgetxattr),
		SCMP_SYS(flistxattr),
		SCMP_SYS(flock),
		SCMP_SYS(fremovexattr),
		SCMP_SYS(fsetxattr),
		SCMP_SYS(fstat),
		SCMP_SYS(fstatfs),
		SCMP_SYS(fsync),
		SCMP_SYS(futex),
		SCMP_SYS(ftruncate),
		SCMP_SYS(ftruncate64),
		SCMP_SYS(getcwd),
		SCMP_SYS(getdents),
		SCMP_SYS(getdents64),
		SCMP_SYS(getegid),
		SCMP_SYS(geteuid),
		SCMP_SYS(getgid),
		SCMP_SYS(getgroups),
		SCMP_SYS(getpeername),
		SCMP_SYS(getpgrp),
		SCMP_SYS(getpid),
		SCMP_SYS(getpgid),
		SCMP_SYS(getppid),
		SCMP_SYS(getpriority),
		SCMP_SYS(getrandom),
		SCMP_SYS(getresgid),
		SCMP_SYS(getresuid),
		SCMP_SYS(getrlimit),
		SCMP_SYS(getrusage),
		SCMP_SYS(getsid),
		SCMP_SYS(getsockname),
		SCMP_SYS(getsockopt),
		SCMP_SYS(gettid),
		SCMP_SYS(getuid),
		SCMP_SYS(getxattr),
		SCMP_SYS(lchown),
		SCMP_SYS(lgetxattr),
		SCMP_SYS(link),
		SCMP_SYS(listen),
		SCMP_SYS(llistxattr),
		SCMP_SYS(lseek),
		SCMP_SYS(lstat),
		SCMP_SYS(kill),
		SCMP_SYS(madvise), /* XXX there are CVE's related to madvise */
		SCMP_SYS(memfd_create),
		SCMP_SYS(mkdir),
		SCMP_SYS(mkdirat),
		SCMP_SYS(mmap),
		SCMP_SYS(mprotect),
		SCMP_SYS(mremap),
		SCMP_SYS(msgget),
		SCMP_SYS(msync),
		SCMP_SYS(munlock),
		SCMP_SYS(munmap),
		SCMP_SYS(nanosleep),
		SCMP_SYS(name_to_handle_at),
		SCMP_SYS(newfstatat),
		SCMP_SYS(open),
		SCMP_SYS(openat),
		SCMP_SYS(pause),
		SCMP_SYS(personality), /* firefox */
		SCMP_SYS(pipe),
		SCMP_SYS(pipe2),
		SCMP_SYS(poll),
		SCMP_SYS(ppoll),
		SCMP_SYS(prctl),
		SCMP_SYS(pread64),
		SCMP_SYS(prlimit64),
		SCMP_SYS(process_vm_writev),
		SCMP_SYS(process_vm_readv),
		SCMP_SYS(pselect6),
		SCMP_SYS(ptrace),
		SCMP_SYS(pwrite64),
		SCMP_SYS(quotactl),
		SCMP_SYS(read),
		SCMP_SYS(readahead),
		SCMP_SYS(readlink),
		SCMP_SYS(readlinkat),
		SCMP_SYS(readv),
		SCMP_SYS(recvfrom),
		SCMP_SYS(recvmsg),
		SCMP_SYS(removexattr),
		SCMP_SYS(rename),
		SCMP_SYS(renameat),
		SCMP_SYS(renameat2),
		SCMP_SYS(rmdir),
		SCMP_SYS(restart_syscall),
		SCMP_SYS(rseq),
		SCMP_SYS(rt_sigaction),
		SCMP_SYS(rt_sigpending),
		SCMP_SYS(rt_sigprocmask),
		SCMP_SYS(rt_sigqueueinfo),
		SCMP_SYS(rt_sigreturn),
		SCMP_SYS(seccomp), /* firefox */
		SCMP_SYS(sched_getaffinity),
		SCMP_SYS(sched_getscheduler),
		SCMP_SYS(sched_setscheduler),
		SCMP_SYS(sched_setparam),
		SCMP_SYS(sched_yield),
		SCMP_SYS(select),
		SCMP_SYS(sendmsg),
		SCMP_SYS(sendmmsg),
		SCMP_SYS(sendto),
		SCMP_SYS(set_robust_list),
		SCMP_SYS(set_tid_address),
		SCMP_SYS(setfsuid),
		SCMP_SYS(setfsgid),
		SCMP_SYS(setgid),
		SCMP_SYS(setgroups),
		SCMP_SYS(setpgid),
		SCMP_SYS(setpriority),
		SCMP_SYS(setregid),
		SCMP_SYS(setreuid),
		SCMP_SYS(setresgid),
		SCMP_SYS(setresuid),
		SCMP_SYS(setrlimit),
		SCMP_SYS(setsid),
		SCMP_SYS(setsockopt),
		SCMP_SYS(setuid),
		SCMP_SYS(setxattr),
		SCMP_SYS(shmat),
		SCMP_SYS(shmctl),
		SCMP_SYS(shmdt),
		SCMP_SYS(shmget),
		SCMP_SYS(shutdown), /* socket related, see man 2 shutdown */
		SCMP_SYS(signal),
		SCMP_SYS(sigaltstack),
		SCMP_SYS(socket),
		SCMP_SYS(socketpair),
		SCMP_SYS(splice),
		SCMP_SYS(stat),
		SCMP_SYS(statfs),
		SCMP_SYS(statx),
		SCMP_SYS(syscall),
		SCMP_SYS(syslog),
		SCMP_SYS(sysinfo),
		SCMP_SYS(symlink), /* firefox */
		SCMP_SYS(symlinkat),
		SCMP_SYS(sync_file_range),
		SCMP_SYS(tkill),
		SCMP_SYS(tgkill), /* firefox */
		SCMP_SYS(umask),
		SCMP_SYS(uname),
		SCMP_SYS(unlink),
		SCMP_SYS(unlinkat),
		SCMP_SYS(unshare), /* firefox */
		SCMP_SYS(utime),
		SCMP_SYS(utimes),
		SCMP_SYS(utimensat),
		SCMP_SYS(vfork),
		SCMP_SYS(wait4),
		SCMP_SYS(waitid),
		SCMP_SYS(write),
		SCMP_SYS(writev),
		-1
	};

	scmp_filter_ctx ctx = NULL;
	/* default behavior is to kill the process */
	ctx = seccomp_init(SCMP_ACT_KILL);
	if (!ctx) {
		fprintf(stderr, "Could not init the seccomp ctx\n");
		return false;
	}

	for (i = syscall_whitelist; *i != -1; i++) {
		if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, *i, 0) < 0) {
			fprintf(stderr, "Couldn't add rule for syscall %d\n", *i);
			return false;
		}
	}

	if (seccomp_load(ctx)) {
		fprintf(stderr, "Could not load the ctx\n");
		return false;
	}

	return true;
}

bool install_seccomp_filter(char *filter)
{
	scmp_filter_ctx ctx = NULL;
	char *token, *saveptr;

	/* default behavior is to kill the process */
	ctx = seccomp_init(SCMP_ACT_KILL);
	if (!ctx) {
		fprintf(stderr, "Could not init the seccomp ctx\n");
		return false;
	}

	/* if no filter is passed, install default whitelist */
	if (!filter)
		return default_whitelist();

	token = strtok_r(filter, ",", &saveptr);
	/* only one syscall */
	if (!token) {
		if (!resolve_add_rule(ctx, filter))
			return false;
	} else {
		while (token) {
			if (!resolve_add_rule(ctx, token))
				return false;

			token = strtok_r(NULL, ",", &saveptr);
		}
	}

	if (seccomp_load(ctx)) {
		fprintf(stderr, "Could not load the ctx\n");
		return false;
	}

	return true;
}
