/*
 *
 *  D-Bus++ - C++ bindings for D-Bus
 *
 *  Copyright (C) 2005-2007  Paolo Durante <shackan@gmail.com>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dbus-c++/debug.h>

#include <stdarg.h>
#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static void _debug_log_default(const char *format, ...)
{
//#ifdef DEBUG

	static int debug_env = getenv("DBUSXX_VERBOSE") ? 1 : 0;

	if (debug_env)
	{
	        va_list args;
	        va_start(args, format);

	        fprintf(stderr, "%i dbus-c++: ", getpid());
	        vfprintf(stderr, format, args);
	        fprintf(stderr, "\n");

	        va_end(args);
	}

//#endif//DEBUG
}

DBus::LogFunction DBus::debug_log = _debug_log_default;

