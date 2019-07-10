/****************************************************************************
 * Copyright (c) 2016 Free Software Foundation, Inc.                        *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 *  Author: Thomas E. Dickey                                                *
 ****************************************************************************/

#define USE_LIBTINFO
#include <tty_settings.h>

#include <fcntl.h>

MODULE_ID("$Id: tty_settings.c,v 1.2 2016/12/24 19:31:11 tom Exp $")

static int my_fd;
static TTY original_settings;
static bool can_restore = FALSE;

static void
exit_error(void)
{
    restore_tty_settings();
    (void) fprintf(stderr, "\n");
    ExitProgram(EXIT_FAILURE);
    /* NOTREACHED */
}

static void
failed(const char *msg)
{
    char temp[BUFSIZ];

    _nc_STRCPY(temp, _nc_progname, sizeof(temp));
    _nc_STRCAT(temp, ": ", sizeof(temp));
    _nc_STRNCAT(temp, msg, sizeof(temp), sizeof(temp) - strlen(temp) - 2);
    perror(temp);
    exit_error();
    /* NOTREACHED */
}

static bool
get_tty_settings(int fd, TTY * tty_settings)
{
    bool success = TRUE;
    my_fd = fd;
    if (fd < 0 || GET_TTY(my_fd, tty_settings) < 0) {
	success = FALSE;
    }
    return success;
}

/*
 * Open a file descriptor on the current terminal, to obtain its settings.
 * stderr is less likely to be redirected than stdout; try that first.
 */
int
save_tty_settings(TTY * tty_settings)
{
    if (!get_tty_settings(STDERR_FILENO, tty_settings) &&
	!get_tty_settings(STDOUT_FILENO, tty_settings) &&
	!get_tty_settings(STDIN_FILENO, tty_settings) &&
	!get_tty_settings(open("/dev/tty", O_RDWR), tty_settings)) {
	failed("terminal attributes");
    }
    can_restore = TRUE;
    original_settings = *tty_settings;
    return my_fd;
}

void
restore_tty_settings(void)
{
    if (can_restore)
	SET_TTY(my_fd, &original_settings);
}

/* Set the modes if they've changed. */
void
update_tty_settings(TTY * old_settings, TTY * new_settings)
{
    if (memcmp(new_settings, old_settings, sizeof(TTY))) {
	SET_TTY(my_fd, new_settings);
    }
}
