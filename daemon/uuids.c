/* libguestfs - the guestfsd daemon
 * Copyright (C) 2013 Red Hat Inc.
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

#include "daemon.h"
#include "actions.h"
#include "optgroups.h"

GUESTFSD_EXT_CMD(str_swaplabel, swaplabel);

static int
e2uuid (const char *device, const char *uuid)
{
  /* Don't allow the magic values here.  If callers want to do this
   * we'll add alternate set_uuid_* calls.
   */
  if (STREQ (uuid, "clear") || STREQ (uuid, "random") ||
      STREQ (uuid, "time")) {
    reply_with_error ("e2: invalid new UUID");
    return -1;
  }

  return do_set_e2uuid (device, uuid);
}

static int
xfsuuid (const char *device, const char *uuid)
{
  /* Don't allow special values. */
  if (STREQ (uuid, "nil") || STREQ (uuid, "generate")) {
    reply_with_error ("xfs: invalid new UUID");
    return -1;
  }

  return xfs_set_uuid (device, uuid);
}

static int
swapuuid (const char *device, const char *uuid)
{
  int r;
  CLEANUP_FREE char *err = NULL;

  r = command (NULL, &err, str_swaplabel, "-U", uuid, device, NULL);
  if (r == -1) {
    reply_with_error ("%s", err);
    return -1;
  }

  return 0;
}

int
do_set_uuid (const char *device, const char *uuid)
{
  int r;

  /* How we set the UUID depends on the filesystem type. */
  CLEANUP_FREE char *vfs_type = get_blkid_tag (device, "TYPE");
  if (vfs_type == NULL)
    return -1;

  if (fstype_is_extfs (vfs_type))
    r = e2uuid (device, uuid);

  else if (STREQ (vfs_type, "xfs"))
    r = xfsuuid (device, uuid);

  else if (STREQ (vfs_type, "swap"))
    r = swapuuid (device, uuid);

  else if (STREQ (vfs_type, "btrfs")) {
    reply_with_error ("btrfs filesystems' UUID cannot be changed");
    r = -1;
  }

  else {
    reply_with_error ("don't know how to set the UUID for '%s' filesystems",
                      vfs_type);
    r = -1;
  }

  return r;
}
