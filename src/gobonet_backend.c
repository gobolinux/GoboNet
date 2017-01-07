
#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"

#define quiet_run(cmd, ...) run_backend(true, cmd, ##__VA_ARGS__)
#define run(cmd, ...) run_backend(false, cmd, ##__VA_ARGS__)

/* Dummy marker for mutable (i.e. non-const) variables */
#define mut

static int const run_backend(bool const quiet, char const *const cmd, ...) {
   int mut argc = 1;
   va_list ap;

   va_start(ap, cmd);
   for(;;) {
      char const *const arg = va_arg(ap, char const *const);
      if (!arg) {
         break;
      }
      argc++;
   }
   va_end(ap);

   char const *mut *const argv = malloc(sizeof(char*) * (argc + 1));
   if (!argv) {
      return -1;
   }
   argv[0] = (char*) cmd;

   va_start(ap, cmd);
   for (int mut i = 1; i <= argc; i++) {
      argv[i] = va_arg(ap, char const *const);
   }
   va_end(ap);

   pid_t const pid = fork();
   if (pid == 0) {
      if (quiet) {
         close(1);
         close(2);
      }
      char const *const *const envp = { NULL };
      execve(cmd, (char mut *const *const) argv, (char mut *const *const) envp);
      perror("exec");
      return -1;
   } else if (pid > 0) {
      int mut ok;
      if (wait(&ok) != pid) {
         perror("wait");
         return 1;
      }
      return ok;
   }
   free(argv);
   perror("fork");
   return -1;
}

static char const *const base_name(char const *const path) {
   char const *const p = strrchr(path, '/');
   if (!p) return NULL;
   return p+1;
}

static int const ifconfig(char const *const interface, char const *const mode) {
   if (mode) {
      return quiet_run(GOBONET_IFCONFIG, interface, mode, NULL);
   } else {
      return quiet_run(GOBONET_IFCONFIG, interface, NULL);
   }
}

static int const disconnect(char const *const interface) {
   if (ifconfig(interface, NULL) != 0) return 1;
   run(GOBONET_RFKILL, "unblock", "all", NULL);
   char const *const wpa_supplicant = base_name(GOBONET_WPA_SUPPLICANT);
   char const *const dhcpcd = base_name(GOBONET_DHCPCD);
   if ((!wpa_supplicant) || (!dhcpcd)) {
      return -1;
   }
   run(GOBONET_KILLALL, wpa_supplicant, NULL);
   run(GOBONET_KILLALL, dhcpcd, NULL);
   return ifconfig(interface, "down");
}

static int const connect(char const *const config, char const *const interface) {
   if (disconnect(interface) != 0) return 1;
   if (ifconfig(interface, "up") != 0) return 1;
   if (run(GOBONET_WPA_SUPPLICANT, "-Dnl80211,wext", "-c", config, "-i", interface, "-B", NULL) != 0) return 1;
   if (run(GOBONET_DHCPCD, "-C", "wpa_supplicant", interface, NULL) != 0) return 1;
   return 0;
}

static int const scan() {
   return run(GOBONET_IWLIST, "scan", NULL);
}

static int const scan_command(int const argc, char const *const *const argv, bool const is_quick_scan) {
   if (argc <= 2) {
      fprintf(stderr, "usage: %s %s <interface>\n", argv[0], argv[1]);
      return 1;
   }
   char const *const interface = argv[2];
   if (strlen(interface) > 64) return 1;

   uid_t const uid = getuid();
   setuid(0);
   run(GOBONET_RFKILL, "unblock", "all", NULL);
   if (ifconfig(interface, "up") != 0) return 1;
   if (is_quick_scan) {
      setuid(uid); /* drop privileges */
   }
   if (scan() != 0) return 1;
   return 0;
}

int const main(int const argc, char const *const *const argv) {

   setlocale(LC_ALL, "C");

   if (argc < 2) {
      return 1;
   }
   
   if (strcmp(argv[1], "connect") == 0) {
      if (argc <= 3) {
         fprintf(stderr, "usage: %s connect <config> <interface>\n", argv[0]);
         return 1;
      }
      char const *const config = argv[2];
      if (strlen(config) > 1024) return 1;
      char const *const interface = argv[3];
      if (strlen(interface) > 64) return 1;

      setuid(0);
      if (connect(config, interface) != 0) return 1;

   } else if (strcmp(argv[1], "disconnect") == 0) {
      if (argc <= 2) {
         fprintf(stderr, "usage: %s disconnect <interface>\n", argv[0]);
         return 1;
      }
      char const *const interface = argv[2];
      if (strlen(interface) > 64) return 1;

      setuid(0);
      if (disconnect(interface) != 0) return 1;
      
   } else if (strcmp(argv[1], "quick-scan") == 0) {
      return scan_command(argc, argv, true);
   } else if (strcmp(argv[1], "full-scan") == 0) {
      return scan_command(argc, argv, false);
   }
   return 0;
}
