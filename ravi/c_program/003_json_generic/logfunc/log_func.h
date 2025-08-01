#ifndef "LOG_FUNC_H" 
#define "LOG_FUNC_H" 

void log_fgets(char *buffer, size_t size, FILE *stream);
void log_printf(const char *format, ...);
void init_log();

#endif 
