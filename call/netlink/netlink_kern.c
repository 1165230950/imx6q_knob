//kernel module: netlink-exam-kern.c
//#include <linux/config.h>

#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/export.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/user_namespace.h>
#include <linux/socket.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <net/net_namespace.h>
#include <linux/mutex.h>

#define BUF_SIZE 16384
static struct sock *netlink_exam_sock;
static int exit_flag = 0;
static DECLARE_COMPLETION(exit_completion);
static struct sk_buff *fly_skb;
DEFINE_MUTEX(net_mutex);

typedef struct test{
	int a;
	char *b;
} s_test;


static void recv_handler(struct sk_buff *skb)
{
//	struct nlmsghdr *nlh=NULL;
//	void *data = NULL;
	mutex_lock(&net_mutex);
	fly_skb = skb;
	mutex_unlock(&net_mutex);
	wake_up(sk_sleep(netlink_exam_sock));

/*	printk(KERN_CRIT "SKB->len : %u\n",skb->len);
	if(skb->len >= nlmsg_total_size(0))
	{
		nlh=nlmsg_hdr(skb);
		data = NLMSG_DATA(nlh);
		if(data)
		{
			printk(KERN_CRIT "kernel receive data is : %s \n",(int8_t *)data);
		}
	}
	printk(KERN_CRIT "FUNC : %s LINE : %d \n",__func__,__LINE__);
*/	
}

static int process_message_thread(void * data)
{
	struct sk_buff * skb = NULL;
	struct nlmsghdr * nlhdr = NULL;
	s_test *st;
	//int len;
	DEFINE_WAIT(wait);

	// daemonize("mynetlink");
	printk(KERN_CRIT "func: %s LINE : %d \n data : %s",__func__,__LINE__,(char *)data);
	while (exit_flag == 0) {
		prepare_to_wait(sk_sleep(netlink_exam_sock), &wait, TASK_INTERRUPTIBLE);
		schedule();
		finish_wait(sk_sleep(netlink_exam_sock), &wait); 
		printk(KERN_CRIT "FUNC : %s LINE : %d \n",__func__,__LINE__);
		while ((skb = fly_skb) != NULL) {
			printk(KERN_CRIT "FUNC : %s LINE : %d \n",__func__,__LINE__);
			fly_skb = NULL;
			printk(KERN_CRIT "skb->len : %d nlmsg_total_size(0) : %d \n",skb->len,nlmsg_total_size(0));
			if(skb->len >= nlmsg_total_size(0))
			{
				nlhdr=nlmsg_hdr(skb);
				
				printk(KERN_CRIT "Kernel receive data is : %s \n",(char *)NLMSG_DATA(nlhdr));
				
			}
	
			//nlhdr->nlmsg_pid = 0;
			//nlhdr->nlmsg_flags = 0;
			NETLINK_CB(skb).porpid = 0;
			NETLINK_CB(skb).dst_group = 1;
			netlink_broadcast(netlink_exam_sock, skb, 0, 0, GFP_KERNEL);
		}
	}
	complete(&exit_completion);
	return 0;
}

/*static int netlink_exam_readproc(char *page, char **start, off_t off,
  int count, int *eof, void *data)
  {
  int len;

  if (off >= buffer_tail) {
 * eof = 1;
 return 0;
 }
 else {
 len = count;
 if (count > PAGE_SIZE) {
 len = PAGE_SIZE;
 }
 if (len > buffer_tail - off) {
 len = buffer_tail - off;
 }
 memcpy(page, buffer + off, len);
 *start = page;
 return len;
 }

 }*/

static int __init netlink_exam_init(void)
{
	struct netlink_kernel_cfg cfg = {
		.groups = 0,
		.input = recv_handler,
	};
	netlink_exam_sock = netlink_kernel_create(&init_net, NETLINK_FLY,&cfg);
	if (!netlink_exam_sock) {
		printk(KERN_CRIT "Fail to create netlink socket.\n");
		return 1;
	}
	printk(KERN_CRIT "[CKS] SUCCESS TO CREAT SOCKET\n");
	kthread_run(process_message_thread, NULL, "message");
	//create_proc_read_entry("netlink_exam_buffer", 0444, NULL, netlink_exam_readproc, 0);
	return 0;
}

static void __exit netlink_exam_exit(void)
{
	exit_flag = 1;
	wake_up(sk_sleep(netlink_exam_sock));
	wait_for_completion(&exit_completion);
	sock_release(netlink_exam_sock->sk_socket);
}

module_init(netlink_exam_init);
module_exit(netlink_exam_exit);
MODULE_LICENSE("GPL");
