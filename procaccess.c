#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/syscalls.h>
#include<linux/slab.h>
#include<asm/page.h>
#include<linux/mm_types.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");
int pid;
module_param(pid, int, 0);
unsigned long long timer_interval_ns = 5e9;
static struct hrtimer hr_timer;



static void function(void){
	
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *ptep;
	struct mm_struct *mm;
	struct task_struct *task;
	struct vm_area_struct *mmap;
	unsigned long start = 0;
	unsigned long end = 0;
	int referenced=0;
	int found = 0;
	for_each_process(task){
		if(task->pid==pid){
			found=1;
			break;
		}
	}
	if(found==1){
		mm=task->mm;//set local mm
		mmap= mm->mmap;
		while(mmap){
			start = mmap->vm_start;//address
			end = mmap->vm_end;//end
			while(start<end){
				pgd=pgd_offset(mm, start);
				p4d=p4d_offset(pgd, start);
				pud=pud_offset(p4d, start);
				pmd=pmd_offset(pud, start);
				ptep=pte_offset_map(pmd, start);			
				if(pte_young(*ptep)){//if accessed> then add, and clear refernced bit
					referenced=referenced+1;
					pte_mkold(*ptep);
				}
				start=start+PAGE_SIZE;
			}
		mmap=mmap->vm_next;
		}//end of page table walking
	printk("[%d]:[%d]",pid,referenced);
	}
}

enum hrtimer_restart timer_callback( struct hrtimer *timer_for_restart )
{

    ktime_t currtime , interval;
    currtime  = ktime_get();
    interval = ktime_set(0,timer_interval_ns);
    hrtimer_forward(timer_for_restart, currtime , interval);
    function();
    return HRTIMER_RESTART;
}
static int __init procaccess_init(void){
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *ptep;
	struct mm_struct *mm;
	struct task_struct *task;
	struct vm_area_struct *mmap;
	ktime_t ktime;
	unsigned long start = 0;
	unsigned long end = 0;
	int found = 0;
	for_each_process(task){
		if(task->pid==pid){
			found=1;
			break;
		}
	}
	if(task && found==1){//we found the process
		mm=task->mm;//set local mm
		mmap= mm->mmap;
		while(mmap){//first run (just clearning bits) so dont add referenced bits yet
			start = mmap->vm_start;//address
			end = mmap->vm_end;//end
			while(start<end){
				pgd=pgd_offset(mm, start);
				p4d=p4d_offset(pgd, start);
				pud=pud_offset(p4d, start);
				pmd=pmd_offset(pud, start);
				ptep=pte_offset_map(pmd, start);				
				if(pte_young(*ptep)){//if accessed> clear refernced bit
					pte_mkold(*ptep);
				}
				start=start+PAGE_SIZE;
			}
			mmap=mmap->vm_next;
		}//end of page table walking. Start prep

		mm=task->mm;//reset pointer
		
		mmap=mm->mmap;//reset pointer
		

		////////////////START TIMER///////////
		
		
    	ktime = ktime_set( 0, timer_interval_ns );
    	hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
    	hr_timer.function = &timer_callback;
    	hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );//start start
		
		////////////////////////////////////////
	}
	return 1;
}

static void __exit procaccess_exit(void){
	int ret;
    ret = hrtimer_cancel( &hr_timer );
    if (ret) printk("Timer was still in use!\n");
    printk("HR Timer removed\n");
}


module_init(procaccess_init);
module_exit(procaccess_exit);
