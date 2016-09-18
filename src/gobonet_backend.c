
#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"

static int run(const char* cmd, ...) {
   int size = 20;
   char** argv = malloc(sizeof(char*) * size);
   if (!argv) {
      return -1;
   }
   argv[0] = (char*) cmd;
   
   va_list ap;
   va_start(ap, cmd);
   int n = 1;
   for(;;) {
      char* arg = va_arg(ap, char*);
      argv[n] = arg;
      n++;
      if (n == size) {
         size *= 2;
         argv = realloc(argv, size);
         if (!argv) {
            return -1;
         }
      }
      if (!arg) {
         break;
      }
   }
   va_end(ap);

   pid_t pid = fork();
   int ok;
   if (pid == 0) {
      char* const envp[] = { NULL };
      execve(cmd, argv, envp);
      perror("exec");
      return -1;
   } else if (pid > 0) {
      if (wait(&ok) != pid) {
         perror("wait");
         return 1;
      }
   } else {
      free(argv);
      perror("fork");
      return -1;
   }
   return ok;
}

static char* base_name(char* path) {
   char* p = strrchr(path, '/');
   if (!p) return NULL;
   return strdup(p+1);
}


static int ifconfig(const char* interface, const char* mode) {
   if (mode) {
      return run(GOBONET_IFCONFIG, interface, mode, NULL);
   } else {
      return run(GOBONET_IFCONFIG, interface, NULL);
   }
}

static int disconnect(const char* interface) {
   if (ifconfig(interface, NULL) != 0) return 1;
   char* wpa_supplicant = base_name(GOBONET_WPA_SUPPLICANT);
   char* dhcpcd = base_name(GOBONET_DHCPCD);
   if ((!wpa_supplicant) || (!dhcpcd)) {
      return -1;
   }
   run(GOBONET_KILLALL, wpa_supplicant, NULL);
   run(GOBONET_KILLALL, dhcpcd, NULL);
   free(wpa_supplicant);
   free(dhcpcd);
   return ifconfig(interface, "down");
}

static int connect(const char* config, const char* interface) {
   if (disconnect(interface) != 0) return 1;
   if (ifconfig(interface, "up") != 0) return 1;
   if (run(GOBONET_WPA_SUPPLICANT, "-Dnl80211,wext", "-c", config, "-i", interface, "-B", NULL) != 0) return 1;
   if (run(GOBONET_DHCPCD, "-C", "wpa_supplicant", interface, NULL) != 0) return 1;
   return 0;
}

static int scan() {
   return run(GOBONET_IWLIST, "scan", NULL);
}

int main(int argc, char** argv) {

   if (argc < 2) {
      return 1;
   }
   
   if (strcmp(argv[1], "connect") == 0) {
      if (argc <= 3) {
         fprintf(stderr, "usage: %s connect <config> <interface>\n", argv[0]);
         return 1;
      }
      char* config = argv[2];
      char* interface = argv[3];
      if (strlen(config) > 1024) return 1;
      if (strlen(interface) > 64) return 1;

      setuid(0);
      if (connect(config, interface) != 0) return 1;

   } else if (strcmp(argv[1], "disconnect") == 0) {
      if (argc <= 2) {
         fprintf(stderr, "usage: %s disconnect <interface>\n", argv[0]);
         return 1;
      }
      char* interface = argv[2];
      if (strlen(interface) > 64) return 1;

      setuid(0);
      if (disconnect(interface) != 0) return 1;
      
   } else if (strcmp(argv[1], "quick-scan") == 0) {
      if (argc <= 2) {
         fprintf(stderr, "usage: %s quick-scan <interface>\n", argv[0]);
         return 1;
      }
      char* interface = argv[2];
      if (strlen(interface) > 64) return 1;

      uid_t uid = getuid();
      setuid(0);
      if (ifconfig(interface, "up") != 0) return 1;
      setuid(uid); /* drop privileges to run quick scan */
      if (scan() != 0) return 1;
   } else if (strcmp(argv[1], "full-scan") == 0) {
      if (argc <= 2) {
         fprintf(stderr, "usage: %s full-scan <interface>\n", argv[0]);
         return 1;
      }
      char* interface = argv[2];
      if (strlen(interface) > 64) return 1;

      setuid(0);
      if (ifconfig(interface, "up") != 0) return 1;
      if (scan() != 0) return 1;
   }
   return 0;
}
