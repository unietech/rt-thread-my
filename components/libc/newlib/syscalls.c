#include <reent.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <rtthread.h>

#ifdef RT_USING_DFS
#include <dfs_posix.h>
#endif

#ifdef RT_USING_PTHREADS 
#include <pthread.h>
#endif

/* Reentrant versions of system calls.  */

int
_close_r(struct _reent *ptr, int fd)
{
#ifndef RT_USING_DFS
	return 0;
#else
	return close(fd);
#endif
}

int
_execve_r(struct _reent *ptr, const char * name, char *const *argv, char *const *env)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_fork_r(struct _reent *ptr)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
#ifndef RT_USING_DFS
	return 0;
#else
	int rc;

	rc = fstat(fd, pstat);
	return rc;
#endif
}

int
_getpid_r(struct _reent *ptr)
{
	return 0;
}

int
_isatty_r(struct _reent *ptr, int fd)
{
	if (fd >=0 && fd < 3) return 1;

	return 0;
}

int
_isatty(int fd)
{
	if (fd >=0 && fd < 3) return 1;

	return 0;
}

int
_kill_r(struct _reent *ptr, int pid, int sig)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_link_r(struct _reent *ptr, const char *old, const char *new)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

_off_t
_lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
#ifndef RT_USING_DFS
	return 0;
#else
	_off_t rc;

	rc = lseek(fd, pos, whence);
	return rc;
#endif
}

int
_mkdir_r(struct _reent *ptr, const char *name, int mode)
{
#ifndef RT_USING_DFS
	return 0;
#else
	int rc;

	rc = mkdir(name, mode);
	return rc;
#endif
}

int
_open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
#ifndef RT_USING_DFS
	return 0;
#else
	int rc;

	rc = open(file, flags, mode);
	return rc;
#endif
}

_ssize_t 
_read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
#ifndef RT_USING_DFS
	return 0;
#else
	_ssize_t rc;

	rc = read(fd, buf, nbytes);
	return rc;
#endif
}

int
_rename_r(struct _reent *ptr, const char *old, const char *new)
{
#ifndef RT_USING_DFS
	return 0;
#else
	int rc;

	rc = rename(old, new);
	return rc;
#endif
}

void *
_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
	/* no use this routine to get memory */
	return RT_NULL;
}

int
_stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
#ifndef RT_USING_DFS
	return 0;
#else
	int rc;

	rc = stat(file, pstat);
	return rc;
#endif
}

_CLOCK_T_
_times_r(struct _reent *ptr, struct tms *ptms)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_unlink_r(struct _reent *ptr, const char *file)
{
#ifndef RT_USING_DFS
	return 0;
#else
	int rc;

	rc = unlink(file);
	return rc;
#endif
}

int
_wait_r(struct _reent *ptr, int *status)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

_ssize_t
_write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
#ifndef RT_USING_DFS
	return 0;
#else
	_ssize_t rc;

	rc = write(fd, buf, nbytes);
	return rc;
#endif
}

int
_gettimeofday_r(struct _reent *ptr, struct timeval *__tp, void *__tzp)
{
#ifndef RT_USING_RTC
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
#else
	static time_t sysnow = 0;
	static uint16_t sysms = 0;
	time_t nownow = time(NULL);
	uint16_t nowms = rt_tick_get() % 1000;
	if (sysnow == 0 && sysms == 0)
	{
		sysnow = nownow;
		sysms = nowms;
	}
	else
	{
		//如果毫秒走的比较快，在秒上面增加
		if ((sysnow == nownow)
				&& (nowms<sysnow))
			sysnow = nownow+1;
		else
			sysnow = nownow;
		sysms = nowms;
	}
	__tp->tv_sec = sysnow;
	__tp->tv_usec = sysms * 1000l;
	return 0;
#endif
}

int
settimeofday(const struct timeval *__tp, const struct timezone *__tzp)
{
#ifndef RT_USING_RTC
	/* return "not supported" */
	return -1;
#else
	rt_device_t device;
	device = rt_device_find("rtc");
    if (device == RT_NULL)
        return -1;
    /* update to RTC device. */
    rt_device_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, (void *)&__tp->tv_sec);
	return 0;
#endif
}
RTM_EXPORT(settimeofday);

/* Memory routine */
void *
_malloc_r (struct _reent *ptr, size_t size)
{
	void* result;

	result = (void*)rt_malloc (size);
	if (result == RT_NULL)
	{
		ptr->_errno = ENOMEM;
	}

	return result;
}

void *
_realloc_r (struct _reent *ptr, void *old, size_t newlen)
{
	void* result;

	result = (void*)rt_realloc (old, newlen);
	if (result == RT_NULL)
	{
		ptr->_errno = ENOMEM;
	}

	return result;
}

void *_calloc_r (struct _reent *ptr, size_t size, size_t len)
{
	void* result;

	result = (void*)rt_calloc (size, len);
	if (result == RT_NULL)
	{
		ptr->_errno = ENOMEM;
	}

	return result;
}

void 
_free_r (struct _reent *ptr, void *addr)
{
	rt_free (addr);
}

void
_exit (int status)
{
#ifdef RT_USING_MODULE
	rt_module_t module;

	module = rt_module_self();
	if (module != RT_NULL)
	{
		/* unload assertion module */
		rt_module_unload(module);

		/* re-schedule */
		rt_schedule();
	}
#endif
	
	rt_kprintf("thread:%s exit with %d\n", rt_thread_self()->name, status);
	RT_ASSERT(0);

	while (1);
}

void 
_system(const char *s)
{
    /* not support this call */
    return;
}

void __libc_init_array(void)
{
	/* we not use __libc init_aray to initialize C++ objects */
}

void abort(void)
{
    if (rt_thread_self())
    {
        rt_thread_t self = rt_thread_self();

        rt_kprintf("thread:%-8.*s abort!\n", RT_NAME_MAX, self->name);
        rt_thread_suspend(self);

        rt_schedule();
    }

	while (1);
}

#ifdef RT_USING_PROCESS
#include "linux-syscall.h"
extern void *rt_module_conv_ptr(rt_module_t module, rt_uint32_t ptr, rt_uint32_t size);
extern rt_uint32_t rt_module_brk(rt_module_t module, rt_uint32_t addr);
static inline int ret_err(int ret)
{
    if (ret < 0)
    {
        int err = errno;
        if (err == 0)
            err = rt_get_errno();
        return (err>0)?(-err):(err);
    }
    return ret;
}

rt_uint32_t sys_call_switch(rt_uint32_t nbr, rt_uint32_t parm1,
        rt_uint32_t parm2, rt_uint32_t parm3,
        rt_uint32_t parm4, rt_uint32_t parm5,
        rt_uint32_t parm6)
{
    rt_module_t module = rt_module_self();

    RT_ASSERT(module != RT_NULL);
    RT_ASSERT((nbr&SYS_BASE) == SYS_BASE);

    switch(nbr)
    {
    case SYS_exit:
    {
        _exit(parm1);
        return -ENOTSUP;
    }
    case SYS_brk:
    {
        return rt_module_brk(module,parm1);
    }
    case SYS_link:
    {
        rt_kprintf("syscall link %s=>%s\n",(const char *)rt_module_conv_ptr(module,parm1,0),
                (const char *)rt_module_conv_ptr(module,parm2,0));
        return -ENOTSUP;
    }
    case SYS_unlink:
    case SYS_rmdir:
    {
        errno = 0;
        return ret_err(unlink((const char *)rt_module_conv_ptr(module,parm1,0)));
    }
    case SYS_rename:
    {
        errno = 0;
        return ret_err(rename((const char *)rt_module_conv_ptr(module,parm1,0),
                (const char *)rt_module_conv_ptr(module,parm2,0)));
    }
    case SYS_mkdir:
    {
        errno = 0;
        return ret_err(mkdir((const char *)rt_module_conv_ptr(module,parm1,0),parm2));
    }
    case SYS_lseek:
    {
        errno = 0;
        return ret_err(lseek(parm1, parm2, parm3));
    }
    case SYS_stat:
    case SYS_lstat:
    {
        errno = 0;
        return ret_err(stat((const char *)rt_module_conv_ptr(module,parm1,0),
                (struct stat *)rt_module_conv_ptr(module,parm2,sizeof(struct stat))));
    }
    case SYS_fstat:
    {
        errno = 0;
        return ret_err(fstat(parm1, (struct stat *)rt_module_conv_ptr(module,parm2,sizeof(struct stat))));
    }
    case SYS_open:
    {
       errno = 0;
       return ret_err(open((const char*)rt_module_conv_ptr(module,parm1,0), parm2, parm3));
    }
    case SYS_read:
    {
       errno = 0;
       return ret_err(read(parm1, rt_module_conv_ptr(module,parm2,parm3), parm3));
    }
    case SYS_write:
    {
       errno = 0;
       return ret_err(write(parm1, rt_module_conv_ptr(module,parm2,parm3), parm3));
    }
    case SYS_close:
    {
       errno = 0;
       return ret_err(close(parm1));
    }
    case SYS_getdents:
    {
        errno = 0;
        extern int getdents(int file, struct dirent *dirp, rt_size_t nbytes);
        return ret_err(getdents(parm1, (struct dirent*)rt_module_conv_ptr(module,parm2,parm3), parm3));
    }
    case SYS_gettimeofday:
    {
        errno = 0;
        return ret_err(_gettimeofday_r(0, (struct timeval *)rt_module_conv_ptr(module,parm1,sizeof(struct timeval)), (void *)parm2));
    }
    case SYS_ioctl:
    {
        errno = 0;
        rt_kprintf("ioctl %d %x %x\n",parm1,parm2,parm3);
        return -ENOTSUP;
    }
    case 0x900000+901:
    case 0x900000+903:
    {
        rt_mutex_take(module->mod_mutex, RT_WAITING_FOREVER);
        return 0;
    }
    case 0x900000+902:
    case 0x900000+904:
    {
        rt_mutex_release(module->mod_mutex);
        return 0;
    }
    }
    rt_kprintf("syscall %d not supported\n",nbr-0x900000);
    return -ENOTSUP;
}
#endif
