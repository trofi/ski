/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * IA-64 Linux syscall numbers and inline-functions.
 *
 * Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef _SKI_SYSCALL_LINUX_H
#define _SKI_SYSCALL_LINUX_H

#define LIA64_ni_syscall		1024
#define LIA64_exit			1025
#define LIA64_read			1026
#define LIA64_write			1027
#define LIA64_open			1028
#define LIA64_close			1029
#define LIA64_creat			1030
#define LIA64_link			1031
#define LIA64_unlink			1032
#define LIA64_execve			1033
#define LIA64_chdir			1034
#define LIA64_fchdir			1035
#define LIA64_utimes			1036
#define LIA64_mknod			1037
#define LIA64_chmod			1038
#define LIA64_chown			1039
#define LIA64_lseek			1040
#define LIA64_getpid			1041
#define LIA64_getppid			1042
#define LIA64_mount			1043
#define LIA64_umount			1044
#define LIA64_setuid			1045
#define LIA64_getuid			1046
#define LIA64_geteuid			1047
#define LIA64_ptrace			1048
#define LIA64_access			1049
#define LIA64_sync			1050
#define LIA64_fsync			1051
#define LIA64_fdatasync			1052
#define LIA64_kill			1053
#define LIA64_rename			1054
#define LIA64_mkdir			1055
#define LIA64_rmdir			1056
#define LIA64_dup			1057
#define LIA64_pipe			1058
#define LIA64_times			1059
#define LIA64_brk			1060
#define LIA64_setgid			1061
#define LIA64_getgid			1062
#define LIA64_getegid			1063
#define LIA64_acct			1064
#define LIA64_ioctl			1065
#define LIA64_fcntl			1066
#define LIA64_umask			1067
#define LIA64_chroot			1068
#define LIA64_ustat			1069
#define LIA64_dup2			1070
#define LIA64_setreuid			1071
#define LIA64_setregid			1072
#define LIA64_getresuid			1073
#define LIA64_setresuid			1074
#define LIA64_getresgid			1075
#define LIA64_setresgid			1076
#define LIA64_getgroups			1077
#define LIA64_setgroups			1078
#define LIA64_getpgid			1079
#define LIA64_setpgid			1080
#define LIA64_setsid			1081
#define LIA64_getsid			1082
#define LIA64_sethostname		1083
#define LIA64_setrlimit			1084
#define LIA64_getrlimit			1085
#define LIA64_getrusage			1086
#define LIA64_gettimeofday		1087
#define LIA64_settimeofday		1088
#define LIA64_select			1089
#define LIA64_poll			1090
#define LIA64_symlink			1091
#define LIA64_readlink			1092
#define LIA64_uselib			1093
#define LIA64_swapon			1094
#define LIA64_swapoff			1095
#define LIA64_reboot			1096
#define LIA64_truncate			1097
#define LIA64_ftruncate			1098
#define LIA64_fchmod			1099
#define LIA64_fchown			1100
#define LIA64_getpriority		1101
#define LIA64_setpriority		1102
#define LIA64_statfs			1103
#define LIA64_fstatfs			1104
/* unused; used to be LIA64_ioperm */
#define LIA64_semget			1106
#define LIA64_semop			1107
#define LIA64_semctl			1108
#define LIA64_msgget			1109
#define LIA64_msgsnd			1110
#define LIA64_msgrcv			1111
#define LIA64_msgctl			1112
#define LIA64_shmget			1113
#define LIA64_shmat			1114
#define LIA64_shmdt			1115
#define LIA64_shmctl			1116
/* also known as klogctl() in GNU libc: */
#define LIA64_syslog			1117
#define LIA64_setitimer			1118
#define LIA64_getitimer			1119
#define LIA64_old_stat			1120
#define LIA64_old_lstat			1121
#define LIA64_old_fstat			1122
#define LIA64_vhangup			1123
#define LIA64_lchown			1124
#define LIA64_vm86			1125
#define LIA64_wait4			1126
#define LIA64_sysinfo			1127
#define LIA64_clone			1128
#define LIA64_setdomainname		1129
#define LIA64_uname			1130
#define LIA64_adjtimex			1131
#define LIA64_create_module		1132
#define LIA64_init_module		1133
#define LIA64_delete_module		1134
#define LIA64_get_kernel_syms		1135
#define LIA64_query_module		1136
#define LIA64_quotactl			1137
#define LIA64_bdflush			1138
#define LIA64_sysfs			1139
#define LIA64_personality		1140
#define LIA64_afs_syscall		1141
#define LIA64_setfsuid			1142
#define LIA64_setfsgid			1143
#define LIA64_getdents			1144
#define LIA64_flock			1145
#define LIA64_readv			1146
#define LIA64_writev			1147
#define LIA64_pread			1148
#define LIA64_pwrite			1149
#define LIA64__sysctl			1150
#define LIA64_mmap			1151
#define LIA64_munmap			1152
#define LIA64_mlock			1153
#define LIA64_mlockall			1154
#define LIA64_mprotect			1155
#define LIA64_mremap			1156
#define LIA64_msync			1157
#define LIA64_munlock			1158
#define LIA64_munlockall		1159
#define LIA64_sched_getparam		1160
#define LIA64_sched_setparam		1161
#define LIA64_sched_getscheduler	1162
#define LIA64_sched_setscheduler	1163
#define LIA64_sched_yield		1164
#define LIA64_sched_get_priority_max	1165
#define LIA64_sched_get_priority_min	1166
#define LIA64_sched_rr_get_interval	1167
#define LIA64_nanosleep			1168
#define LIA64_nfsservctl		1169
#define LIA64_prctl			1170
/* 1171 is reserved for backwards compatibility with old LIA64_getpagesize */
#define LIA64_mmap2			1172
#define LIA64_pciconfig_read		1173
#define LIA64_pciconfig_write		1174
#define LIA64_perfmonctl		1175
#define LIA64_sigaltstack		1176
#define LIA64_rt_sigaction		1177
#define LIA64_rt_sigpending		1178
#define LIA64_rt_sigprocmask		1179
#define LIA64_rt_sigqueueinfo		1180
#define LIA64_rt_sigreturn		1181
#define LIA64_rt_sigsuspend		1182
#define LIA64_rt_sigtimedwait		1183
#define LIA64_getcwd			1184
#define LIA64_capget			1185
#define LIA64_capset			1186
#define LIA64_sendfile			1187
#define LIA64_getpmsg			1188
#define LIA64_putpmsg			1189
#define LIA64_socket			1190
#define LIA64_bind			1191
#define LIA64_connect			1192
#define LIA64_listen			1193
#define LIA64_accept			1194
#define LIA64_getsockname		1195
#define LIA64_getpeername		1196
#define LIA64_socketpair		1197
#define LIA64_send			1198
#define LIA64_sendto			1199
#define LIA64_recv			1200
#define LIA64_recvfrom			1201
#define LIA64_shutdown			1202
#define LIA64_setsockopt		1203
#define LIA64_getsockopt		1204
#define LIA64_sendmsg			1205
#define LIA64_recvmsg			1206
#define LIA64_pivot_root		1207
#define LIA64_mincore			1208
#define LIA64_madvise			1209
#define LIA64_stat			1210
#define LIA64_lstat			1211
#define LIA64_fstat			1212
#define LIA64_clone2			1213
#define LIA64_getdents64		1214
/* 1215? */
/* 1216? */
/* 1217? */
/* 1218? */
/* 1219? */
/* 1220? */
/* 1221? */
/* 1222? */
/* 1223? */
/* 1224? */
/* 1225? */
/* 1226? */
/* 1227? */
/* 1228? */
/* 1229? */
/* 1230? */
/* 1230? */
/* 1231? */
/* 1232? */
/* 1233? */
/* 1234? */
/* 1235? */
#define LIA64_exit_group		1236   /* same as LIA64_exit? */

#endif /* _SKI_SYSCALL_LINUX_H */
