/*
 *  Unified DRM-device I/O interface for Xen guest OSes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Copyright (C) 2016 EPAM Systems Inc.
 */

#ifndef __XEN_PUBLIC_IO_XENSND_LINUX_H__
#define __XEN_PUBLIC_IO_XENSND_LINUX_H__

#include <xen/interface/io/ring.h>
#include <xen/interface/io/drmif.h>
#include <xen/interface/grant_table.h>

struct xendrm_open_req {
	grant_ref_t gref_directory_start;
} __packed;

struct xendrm_page_directory {
	grant_ref_t gref_dir_next_page;
	uint32_t num_grefs;
	grant_ref_t gref[0];
} __packed;

struct xendrm_close_req {
} __packed;

struct xendrm_req {
	union {
		struct xendrm_request raw;
		struct {
			uint16_t id;
			uint8_t operation;
			union {
				struct xendrm_open_req open;
				struct xendrm_close_req close;
			} op;
		} data;
	} u;
};

struct xendrm_resp {
	union {
		struct xendrm_response raw;
		struct {
			uint16_t id;
			uint8_t operation;
			int8_t status;
		} data;
	} u;
};

DEFINE_RING_TYPES(xen_drmif, struct xendrm_req,
		struct xendrm_resp);

#endif /* __XEN_PUBLIC_IO_XENDRM_LINUX_H__ */
