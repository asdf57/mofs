#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "logger.h"

const size_t ISO_FMT_SIZE = 20;

const char* get_str_type(const TYPE type) {
   if (type == INFO) {
      return "INFO";
   } else if (type == WARNING) {
      return "WARNING";
   }

   return "ERROR";
}

void logger(TYPE type, const char* message, ...) {
   va_list    args;
   time_t     current_time;
   struct tm  *time_info;
   char       time_string[ISO_FMT_SIZE];

   va_start(args, message);

   time(&current_time);
   time_info = localtime(&current_time);
   strftime(time_string, sizeof(time_string), "%Y-%m-%dT%H:%M:%S", time_info);

   if (type == ERROR) {
      fprintf(stderr, "%s [%s]: ", time_string, get_str_type(type));
      vfprintf(stderr, message, args);

   } else {
      printf("%s [%s]: ", time_string, get_str_type(type));
      vprintf(message, args);
   }

   va_end(args);
}
