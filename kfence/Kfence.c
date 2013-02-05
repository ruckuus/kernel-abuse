/*
 * kfence provides kernel protection against basic exploiting techniques
 * 
 * Method:
 * 
 * It patches /dev/kmem and redirects system_call
 * to our own code that first tests if the 
 * eip of the caller is in wrong memory regions
 * (between 0xbf000000 and 0xbfffffff
 * or current->mm->start_data and current->mm->end_data
 * or current->mm->start_brk and current->mm->brk).
 * If the code is executed from those regions it exits 
 * otherwise it proceeds to the normal system_call
 * 
 * Tested on linux kernels 2.4.18-14 and 2.4.7-10
 * 
 * Coded by ins1der 
 * 2003
 * trixterjack@yahoo.com
 */
  
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <asm/unistd.h>

#define ulong unsigned long

struct _idt
{
  unsigned short offset_low,segment_sel;
  unsigned char reserved,flags;
  unsigned short offset_high;
};

char shellc[]=
                                /*start:*/
"\x83\xf8\x21"			/*cmp 	 $0x21,%eax*/
"\x74\x39" 			/*je 	 exitme*/	
"\x55"				/*push 	 %ebp*/
"\x89\xe5"			/*mov 	 %esp,%ebp*/
"\x60"				/*pusha*/
"\x8b\x55\x04"			/*mov 	 0x4(%ebp),%edx*/
"\x81\xfa\xfe\xff\xff\xbf"	/*cmp    $0xbffffffe,%edx*/
"\x77\x08"			/*ja 	 test1*/
"\x81\xfa\x00\x00\x00\xbf"	/*cmp    $0xbf000000,%edx*/
"\x77\x22"			/*ja 	 exitme*/
				/*test1:*/
"\xb8\x00\xe0\xff\xff"		/*mov    $0xffffe000,%eax*/
"\x21\xe0"			/*and 	 %esp,%eax*/
"\x8b\x40\x58"			/*mov    0x58(%eax),%eax*/
"\x3b\x50\x44"			/*cmp    0x44(%eax),%edx*/
"\x73\x05"			/*jae	 test2*/
"\x3b\x50\x40"			/*cmp 	 0x40(%eax),%edx*/
"\x77\x0e"			/*ja	 exitme*/
				/*test2:*/
"\x3b\x50\x3c"			/*cmp	 0x3c(%eax),%edx*/
"\x73\x05"			/*jae	 endme*/
"\x3b\x50\x38"			/*cmp	 0x38(%eax),%edx*/
"\x77\x04"			/*ja	 exitme*/
				/*endme:*/
"\x61"				/*popa*/
"\x5d"				/*pop 	 %ebp*/
"\xeb\x06"			/*jmp 	 realend*/
				/*exitme:*/
"\x31\xc0"			/*xor	 %eax,%eax*/
"\xfe\xc0"			/*inc 	 %al*/
"\xcd\x80"			/*int	 $0x80*/
				/*realend:*/
;

/* read or write to /dev/kmem */

int kmemrw(void *rwbuf, ulong offset, size_t size, int type)
{
  int f;
  int ret;
  f=open("/dev/kmem",O_RDWR);
  if (f<0) 
    {
	  fprintf(stderr,"unable to open /dev/kmem in rw mode\n");
	  _exit(0);
    }
  lseek(f,offset,SEEK_SET);
  if (type==0)
    ret=read(f,rwbuf,size);
  else
    ret=write(f,rwbuf,size);
  close(f);
  return ret;
}

void usage(char *argv)
{
  fprintf(stderr,"***\nkfence \nins1der 2003 (trixterjack@yahoo.com)\n***\n");
  fprintf(stderr,"Usage : %s [command]\n",argv);
  fprintf(stderr,"Commands:\n");
  fprintf(stderr,"  r	remove the kernel patch\n");
  fprintf(stderr,"  i 	install kfence	 	\n");
  _exit(0);
}


int main(int argc,char *argv[])
{
  
  unsigned char idtr[6],shell[100],mm_dist,start_data;
  struct _idt idt;
  ulong olduname,setgid,sct,len,system_call,readbytes;
  char *p;
  
  if ((argc!=2)||(argv[1][0]!='r'&&argv[1][0]!='i')) usage(argv[0]);
  
  printf("***\nkfence \nins1der 2003 (trixterjack@yahoo.com)\n***\n");
  
  /*get the interrupt descriptor table*/
  
  asm("sidt %0" : "=m" (idtr));
  
  /*get the address of system_call*/
  
  kmemrw(&idt,*((ulong*)&idtr[2])+8*0x80,sizeof(idt),0);
  system_call=(idt.offset_high<<16)|idt.offset_low;
  printf("# system_call at 0x%x\n",(int)system_call);
  
  /*read the first 4 bytes of system_call*/
  
  kmemrw(&readbytes,system_call,4,0);
  
  
  if (argv[1][0]=='r') 
    
    {
      
      if (readbytes==0x1e06fc50)

	/*if the first bytes of system_call are normal kfence is not installed*/
	
	printf("kfence is not up. nothing to remove.\n");
      
      else
	
	{	
	
	  /*remove kfence by writing the usual bytes back in system_call*/
	  
	  printf("# removing kfence:\n");
	  kmemrw("\x50\xfc\x06\x1e\x50\x55",system_call,6,1);
	  printf("# Done.\n");
	
	}
      
      return 0;
  
    }
  
  
  if (readbytes!=0x1e06fc50)
    {
    
      fprintf(stderr,"Something is wrong with system_call\n");
      fprintf(stderr,"kfence probably installed\n");
      fprintf(stderr,"Exiting.\n");
      _exit(0);
  
    }
  
  /*get sys_call_table*/
  
  kmemrw(shell,system_call,100,0);
  p=(char*)memmem(shell,100,"\xff\x14\x85",3);
  if (!p) 
    {
      fprintf(stderr,"sys_call_table not found!\n");
      return 0;
    }
  p+=3;
  sct=*(ulong*)p;
  printf("# sys_call_table  0x%x\n",(int)sct);
  
  /*get the address of sys_olduname*/
  
  kmemrw(&olduname,sct+4*__NR_olduname,4,0);
  printf("# olduname at 0x%x\n",(int)olduname);
  
  /*get the address of sys_setgid32*/
  
  kmemrw(&setgid,sct+4*__NR_setgid32,4,0);
  printf("# setgid at 0x%x\n",(int)setgid);
  
  /*
    search sys_setgid32 for something like: 
    
    mov    $0xffffe000,%eax
    and    %esp,%eax
    mov    0x58(%eax),%eax
    andb   $0xfe,0x74(%eax)
    
    we extract from the code above the distance of mm in task_struct (0x58) 
    and the distance of dumpable in mm_struct (0x74)
  */
  
  kmemrw(shell,setgid,100,0);
  p=(char*)memmem(shell,100,"\xb8\x00\xe0\xff\xff\x21\xe0\x8b\x40",9);
  if (!p) 
    {
    fprintf(stderr,"couldn't get needed structures!\n");
    return 0;
    }
  p+=9;
  mm_dist=*p;
  printf("# mm distance in task_struct = 0x%x\n",mm_dist);
  
  /*we asume that start_data is before dumpabale in mm_struct at a distance of 15 ulongs*/
  
  start_data=*(p+3)-4*15;
  printf("# start_data distance in mm_struct = 0x%x\n",start_data);
  
  printf("# If everything seems fine, press enter.\n");
  getc(stdin);
  
  /*prepare our code*/
  
  len=sizeof(shellc)-1;
  shellc[2]=__NR_olduname;
  shellc[37]=mm_dist;
  shellc[55]=start_data;
  shellc[50]=start_data+4;
  shellc[45]=start_data+8;
  shellc[40]=start_data+12;
  memcpy(shell,shellc,len);
  kmemrw(shell+len,system_call,6,0);
  *(char*)(shell+len+6)=0x68;
  *(int*)(shell+len+7)=(int)(system_call+6);
  *(char*)(shell+len+11)=0xc3;
  
  /*write the code in sys_olduname*/
  
  kmemrw(shell,olduname,len+12,1);
  shell[0]=0x68;
  *(int*)(shell+1)=olduname;
  shell[5]=0xc3;
  
  /*write a push ret at the begining system_call to jump to sys_olduname*/
  
  kmemrw(shell,system_call,6,1);
  
  printf("# Done. kfence is installed\n");
  
  return 1;
}
