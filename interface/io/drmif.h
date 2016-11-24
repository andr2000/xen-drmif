/******************************************************************************
 * drmif.h
 *
 * Unified DRM-device I/O interface for Xen guest OSes.
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
 *
 * Authors: Oleksandr Andrushchenko <andr2000@gmail.com>
 *          Oleksandr Grytsov <al1img@gmail.com>
 */

#ifndef __XEN_PUBLIC_IO_XENDRM_H__
#define __XEN_PUBLIC_IO_XENDRM_H__

/*
 * Front->back notifications: When enqueuing a new request, sending a
 * notification can be made conditional on req_event (i.e., the generic
 * hold-off mechanism provided by the ring macros). Backends must set
 * req_event appropriately (e.g., using RING_FINAL_CHECK_FOR_REQUESTS()).
 *
 * Back->front notifications: When enqueuing a new response, sending a
 * notification can be made conditional on rsp_event (i.e., the generic
 * hold-off mechanism provided by the ring macros). Frontends must set
 * rsp_event appropriately (e.g., using RING_FINAL_CHECK_FOR_RESPONSES()).
 */

/*
 * Feature and Parameter Negotiation
 * =================================
 * The two halves of a para-virtual DRM driver utilize nodes within the
 * XenStore to communicate capabilities and to negotiate operating parameters.
 * This section enumerates these nodes which reside in the respective front and
 * backend portions of the XenStore, following the XenBus convention.
 *
 * All data in the XenStore is stored as strings.  Nodes specifying numeric
 * values are encoded in decimal.  Integer value ranges listed below are
 * expressed as fixed sized integer types capable of storing the conversion
 * of a properly formated node string, without loss of information.
 *
 *****************************************************************************
 *                            Backend XenBus Nodes
 *****************************************************************************
 *
 *-------------------------------- Addressing ---------------------------------
 *
 * Indices used to address frontends, driver instances, cards,
 * devices and streams.
 *
 * frontend_id
 *      Values:         <uint>
 *
 *      Domain ID of the sound frontend.
 *
 * drv_idx
 *      Values:         <uint>
 *
 *      Zero based contiguous index of the virtualized DRM driver instance in
 *      this domain. Multiple PV drivers are allowed in the domain
 *      at the same time.
 *
 * conn_id
 *      Values:         <uint>
 *
 *      Zero based contiguous index of the connector within the card.
 *
 *----------------------------- Connector settings -----------------------------
 * resolution
 *      Values:         <[width]x[height]>
 *
 *      Width and height for the connector in pixels separated by
 *      XENDRM_RESOLUTION_SEPARATOR. For example,
 *      vdrm/0/connector/0/resolution = "800x600"
 *
 *
 *****************************************************************************
 *                            Frontend XenBus Nodes
 *****************************************************************************
 *
 *----------------------- Request Transport Parameters -----------------------
 *
 * These are per stream.
 *
 * ctrl-channel
 *      Values:         <uint>
 *
 *      The identifier of the Xen connector's control event channel
 *      used to signal activity in the ring buffer.
 *
 * ctrl-ring-ref
 *      Values:         <uint>
 *
 *      The Xen grant reference granting permission for the backend to map
 *      a sole page in a single page sized connector's control ring buffer.
 *
 * event-channel
 *      Values:         <uint>
 *
 *      The identifier of the Xen connector's event channel
 *      used to signal activity in the ring buffer.
 *
 * event-ring-ref
 *      Values:         <uint>
 *
 *      The Xen grant reference granting permission for the backend to map
 *      a sole page in a single page sized connector's event ring buffer.
 */

/*
 * STATE DIAGRAMS
 *
 *****************************************************************************
 *                                   Startup                                 *
 *****************************************************************************
 *
 * Tool stack creates front and back state nodes with initial state
 * XenbusStateInitialising.
 * Tool stack creates and sets up frontend sound configuration nodes per domain.
 *
 * Front                                Back
 * =================================    =====================================
 * XenbusStateInitialising              XenbusStateInitialising
 *                                       o Query backend device identification
 *                                         data.
 *                                       o Open and validate backend device.
 *                                                      |
 *                                                      |
 *                                                      V
 *                                      XenbusStateInitWait
 *
 * o Query frontend configuration
 * o Allocate and initialize
 *   event channels and buffers
 * o Publish transport parameters
 *   that will be in effect during
 *   this connection.
 *              |
 *              |
 *              V
 * XenbusStateInitialised
 *
 *                                       o Query frontend transport parameters.
 *                                       o Connect to the event channels.
 *                                                      |
 *                                                      |
 *                                                      V
 *                                      XenbusStateConnected
 *
 *  o Create and initialize OS
 *  virtual DRM as per configuration.
 *              |
 *              |
 *              V
 * XenbusStateConnected
 *
 *                                      XenbusStateUnknown
 *                                      XenbusStateClosed
 *                                      XenbusStateClosing
 * o Remove virtual sound device
 * o Remove event channels
 *              |
 *              |
 *              V
 * XenbusStateClosed
 *
 */

/*
 * REQUEST CODES.
 */
#define XENDRM_OP_DUMB_CREATE           0
#define XENDRM_OP_DUMB_DESTROY          1
#define XENDRM_OP_FB_CREATE             2
#define XENDRM_OP_FB_DESTROY            3
#define XENDRM_OP_SET_CONFIG            4
#define XENDRM_OP_PG_FLIP               5

/*
 * EVENT CODES.
 */
#define XENDRM_EVT_PG_FLIP              0

/*
 * XENSTORE FIELD AND PATH NAME STRINGS, HELPERS.
 */
#define XENDRM_DRIVER_NAME                   "vdrm"

#define XENDRM_RESOLUTION_SEPARATOR          "x"
/* Field names */
#define XENDRM_FIELD_CTRL_RING_REF           "ctrl-ring-ref"
#define XENDRM_FIELD_CTRL_CHANNEL            "ctrl-channel"
#define XENDRM_FIELD_EVT_RING_REF            "event-ring-ref"
#define XENDRM_FIELD_EVT_CHANNEL             "event-channel"
#define XENDRM_FIELD_RESOLUTION              "resolution"

/*
 * STATUS RETURN CODES.
 */
 /* Operation failed for some unspecified reason (e. g. -EIO). */
#define XENDRM_RSP_ERROR                 (-1)
 /* Operation completed successfully. */
#define XENDRM_RSP_OKAY                  0

/* Path entries */
#define XENDRM_PATH_CONNECTOR                "connector"

/*
 * Description of the protocol between frontend and backend driver.
 *
 * The two halves of a Para-virtual sound driver communicates with
 * each other using a shared page and an event channel.
 * Shared page contains a ring with request/response packets.
 *
 *****************************************************************************
 *                            Frontend to backend requests
 *****************************************************************************
 *
 * All request packets have the same length (64 octets)
 *
 *
 * Request dumb creation - request creation of a DRM dumb buffer.
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     reserved    |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         dumb_cookie low 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         dumb_cookie high 32-bit                       |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 width                                 |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 height                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                  bpp                                  |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         gref_directory_start                          |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_DUMB_CREATE
 * dumb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote dumb to local in requests
 * width - uint32_t, width in pixels
 * height - uint32_t, height in pixels
 * bpp - uint32_t, bits per pixel
 * gref_directory_start - grant_ref_t, a reference to the first shared page
 *   describing shared buffer references. At least one page exists. If shared
 *   buffer size exceeds what can be addressed by this single page, then
 *   reference to the next page must be supplied (gref_dir_next_page below
 *   is not NULL)
 *
 * Shared page for XENDRM_OP_DUMB_CREATE buffer descriptor (gref_directory in
 *   the request) employs a list of pages, describing all pages of the shared
 *   data buffer:
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          gref_dir_next_page                           |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                num_grefs                              |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                gref[0]                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                gref[1]                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                gref[N -1]                             |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * gref_dir_next_page - grant_ref_t, reference to the next page describing
 *   page directory
 * num_grefs - number of grefs in this page
 * gref[i] - grant_ref_t, reference to a shared page of the dumb buffer
 *   allocated at XENDRM_OP_DUMB_CREATE
 *
 *
 * Request dumb destruction - destroy a previously allocated dumb buffer:
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     reserved    |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         dumb_cookie low 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         dumb_cookie high 32-bit                       |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_DUMB_DESTROY
 * dumb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote dumb to local in requests
 *
 *
 * Request framebuffer creation - request creation of a DRM framebuffer.
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     reserved    |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         dumb_cookie low 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         dumb_cookie high 32-bit                       |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie low 32-bit                         |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie high 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 width                                 |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 height                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                              pixel_format                             |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_FB_CREATE
 * dumb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote dumb to local in requests
 * fb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote framebuffer to local in requests
 * width - uint32_t, width in pixels
 * height - uint32_t, height in pixels
 * pixel_format - uint32_t, pixel format of the framebuffer
 *
 *
 * Request framebuffer destruction - destroy a previously
 *   allocated framebuffer buffer:
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     reserved    |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie low 32-bit                         |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie high 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_FB_DESTROY
 * dumb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote dumb to local in requests
 *
 *
 * Request configuration set/reset - request to set or reset
 *   the configuration/mode on CRTC:
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     reserved    |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie low 32-bit                         |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie high 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                   x                                   |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                   y                                   |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 width                                 |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 height                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                  bpp                                  |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_SET_CONFIG
 * fb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote framebuffer to local in requests
 * x - uint32_t, starting position in pixels by X axis
 * y - uint32_t, starting position in pixels by Y axis
 * width - uint32_t, width in pixels
 * height - uint32_t, height in pixels
 * bpp - uint32_t, bits per pixel
 *
 *
 * Request page flip - request to flip a page identified by the framebuffer
 *   cookie:
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     reserved    |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 crtc_idx                              |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie low 32-bit                         |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie high 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_PG_FLIP
 * crtc_idx - uint32_t, private guest value, echoed in response by the back
 * fb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote framebuffer to local in requests
 *
 *****************************************************************************
 *                            Backend to frontend events
 *****************************************************************************
 *
 * All event packets have the same length (64 octets)
 * Events are sent via a shared page allocated by the front and propagated by
 *   event-channel/event-ring-ref XenStore entries
 *
 *
 * Page flip complete event - event from back to front on page flip completed:
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |      type       |     reserved    |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                 crtc_idx                              |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie low 32-bit                         |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                          fb_cookie high 32-bit                        |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, event id, may be used by front
 * type - uint8_t, XENDRM_EVT_PG_FLIP
 * crtc_idx - uint32_t, echoed value received in XENDRM_OP_PG_FLIP request
 * fb_cookie - uint64_t, unique to guest domain value used by the backend
 *   to map remote framebuffer to local in requests
 *
 */

struct xendrm_request {
	uint8_t raw[64];
};

struct xendrm_response {
	uint8_t raw[64];
};

struct xendrm_event {
	uint8_t raw[64];
};

struct xendrm_event_page {
	union {
		struct xendrm_event raw;
		struct {
			uint32_t in_cons;
			uint32_t in_prod;
		} ring;
	} u;
};

#endif /* __XEN_PUBLIC_IO_XENDRM_H__ */
