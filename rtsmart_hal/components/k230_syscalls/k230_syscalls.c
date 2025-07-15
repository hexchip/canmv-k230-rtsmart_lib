/* Copyright (c) 2025, Canaan Bright Sight Co., Ltd
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/statfs.h>
#include <sys/statvfs.h>

#include "hal_syscall.h"

///////////////////////////////////////////////////////////////////////////////
// Syscalls for statfs and statvfs ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int statfs(const char* path, struct statfs* buf)
{
    *buf = (struct statfs) { 0 };
    return syscall(_NRSYS_statfs, path, buf);
}

static void fixup(struct statvfs* out, const struct statfs* in)
{
    *out = (struct statvfs) { 0 };

    out->f_bsize   = in->f_bsize;
    out->f_frsize  = in->f_frsize ? in->f_frsize : in->f_bsize;
    out->f_blocks  = in->f_blocks;
    out->f_bfree   = in->f_bfree;
    out->f_bavail  = in->f_bavail;
    out->f_files   = in->f_files;
    out->f_ffree   = in->f_ffree;
    out->f_favail  = in->f_ffree;
    out->f_fsid    = in->f_fsid.__val[0];
    out->f_flag    = in->f_flags;
    out->f_namemax = in->f_namelen;
}

int statvfs(const char* restrict path, struct statvfs* restrict buf)
{
    struct statfs kbuf;
    if (statfs(path, &kbuf) < 0)
        return -1;
    fixup(buf, &kbuf);
    return 0;
}
