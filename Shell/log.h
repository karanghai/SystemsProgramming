/** @file log.h */

#ifndef __LOG_H_
#define __LOG_H_


/*the above code encases each element in log_t */

typedef struct _log_t {
	const char* item;
	struct _log_t * next;
	struct _log_t * prev;
	int logsize;
	
} log_t;

void log_init(log_t *l);
void log_destroy(log_t* l);
void log_push(log_t* l, const char *item);
char *log_search(log_t* l, const char *prefix);
/**ADDED**/
void log_print(log_t* l);

#endif
