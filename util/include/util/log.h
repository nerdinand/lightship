#ifndef LIGHTSHIP_UTIL_LOG_H
#define LIGHTSHIP_UTIL_LOG_H

#include "util/config.h"
#include "util/pstdint.h"
#include "util/event_api.h"

typedef enum log_level_t
{
    LOG_INFO = 0,
    LOG_WARNING = 1, 
    LOG_ERROR = 2,
    LOG_FATAL = 3,
    LOG_USER = 4,
    LOG_NONE = 5
} log_level_t;

struct log_t
{
    log_level_t level;
    char* message;
};

/*!
 * @brief Initialises the log. Must be called before using any other log related
 * functions.
 */
LIGHTSHIP_PUBLIC_API void
llog_init(void);

/*!
 * @brief Opens an indentation level of the log.
 * 
 * This causes every succeeding call to llog() to be indented by a certain
 * amount, until llog_unindent() is called.
 * @param[in] indent_name The name of the new indentation level.
 */
LIGHTSHIP_PUBLIC_API void
llog_indent(const char* indent_name);

/*!
 * @brief Closes one indentation level of the log.
 * 
 * This causes every succeeding call to llog() to be indented one level less
 * than before. If the indent level is at 0, then nothing happens.
 */
LIGHTSHIP_PUBLIC_API void
llog_unindent(void);

/*!
 * @brief Fires a log event with the specified information.
 * @param[in] level The severity of the message being logged. Possible
 * parameters are *LOG_INFO*, *LOG_WARNING*, *LOG_ERROR*, *LOG_FATAL*,
 * *LOG_USER*, and *LOG_NONE*. If the parameter is not *LOG_NONE*, then the
 * message being logged will be prefixed by a respective indicator of severity.
 * @param[in] num_strs The number of strings that are going to be passed to this
 * function. This is required for varargs to know how to concatenate the strings
 * together.
 * @param[in] strs... The strings to concatenate and send to the log.
 */
LIGHTSHIP_PUBLIC_API void
llog(log_level_t level, uint32_t num_strs, ...);

EVENT_LISTENER1(on_llog_indent, const char* str);
EVENT_LISTENER1(on_llog, struct log_t* arg);

#endif /* LIGHTSHIP_UTIL_LOG_H */
