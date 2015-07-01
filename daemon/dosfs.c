/* libguestfs - the guestfsd daemon
 * Copyright (C) 2009-2015 Red Hat Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "daemon.h"
#include "actions.h"

#define MAX_ARGS 64

GUESTFSD_EXT_CMD(str_mkfs_vfat, mkfs.vfat);

/* Takes optional arguments, consult optargs_bitmask. */
int
do_mkfs_vfat (const char *device, int fatsize)
{
  const char *argv[MAX_ARGS];
  size_t i = 0;
  int r;
  char fatsize_str[32];
  CLEANUP_FREE char *err = NULL;

  ADD_ARG (argv, i, str_mkfs_vfat);

  if (optargs_bitmask & GUESTFS_MKFS_VFAT_FATSIZE_BITMASK) {
    snprintf (fatsize_str, sizeof fatsize_str, "%d", fatsize);
    ADD_ARG (argv, i, "-F");
    ADD_ARG (argv, i, fatsize_str);
  }

  ADD_ARG (argv, i, device);
  ADD_ARG (argv, i, NULL);

  r = commandv (NULL, &err, argv);
  if (r == -1) {
    reply_with_error ("%s: %s", device, err);
    return -1;
  }

  return 0;
}
