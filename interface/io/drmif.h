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
 * frontend-id
 *      Values:         <string representing a positive integer>
 *
 *      Domain ID of the sound frontend.
 *
 * drv_idx
 *      Values:         <string representing a positive integer>
 *
 *      Zero based index of the virtualized sound driver instance in
 *      this domain. Multiple PV drivers are allowed in the domain
 *      at the same time.
 *
 * card_idx
 *      Values:         <string representing a positive integer>
 *
 *      Zero based index of the card within the driver.
 *
 * conn_id
 *      Values:         <string representing a positive integer>
 *
 *      Zero based index of the connector within the card.
 *
 *----------------------------- Connector settings -----------------------------
 * type
 *      Values:         <char[32]>
 *
 *      Type of the connector.
 *
 * id
 *      Values:         <string representing a positive integer>
 *
 *      Unique (within given card instance) ID.
 *      Doesn't have to be zero based and/or be contiguous.
 *
 * resolution
 *      Values:         <width x height>
 *
 *      Width and height for the connector in pixels separated by
 *      XENDRM_LIST_SEPARATOR.
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
 *      Values:         <string representing a positive integer>
 *
 *      The identifier of the Xen connector's control event channel
 *      used to signal activity in the ring buffer.
 *
 * ctrl-ring-ref
 *      Values:         <string representing a positive integer>
 *
 *      The Xen grant reference granting permission for the backend to map
 *      a sole page in a single page sized connector's control ring buffer.
 *
 * event-channel
 *      Values:         <string representing a positive integer>
 *
 *      The identifier of the Xen connector's event channel
 *      used to signal activity in the ring buffer.
 *
 * event-ring-ref
 *      Values:         <string representing a positive integer>
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
 */

/*
 * REQUEST CODES.
 */
#define XENDRM_OP_OPEN                  0
#define XENDRM_OP_CLOSE                 1
#define XENDRM_OP_PG_FLIP               2
#define XENDRM_OP_FB_DESTROY            3
#define XENDRM_OP_FB_CREATE             4

/*
 * EVENT CODES.
 */
#define XENDRM_EVT_PG_FLIP              0

/*
 * XENSTORE FIELD AND PATH NAME STRINGS, HELPERS.
 */
#define XENDRM_DRIVER_NAME                   "vdrm"

#define XENDRM_LIST_SEPARATOR                "x"
/* Field names */
#define XENDRM_FIELD_CTRL_RING_REF           "ctrl-ring-ref"
#define XENDRM_FIELD_CTRL_CHANNEL            "ctrl-channel"
#define XENDRM_FIELD_EVT_RING_REF            "event-ring-ref"
#define XENDRM_FIELD_EVT_CHANNEL             "event-channel"
#define XENSND_FIELD_TYPE                    "type"
#define XENSND_FIELD_ID                      "id"
#define XENSND_FIELD_RESOLUTION              "resolution"

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
 *
 * All request packets have the same length (16 octets)
 *
 *
 * Request open - open a XXX
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     stream_idx  |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                pcm_rate                               |
 * +-----------------+-----------------+-----------------+-----------------+
 * |  pcm_format     |  pcm_channels   |             reserved              |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                         gref_directory_start                          |
 * +-----------------+-----------------+-----------------+-----------------+

 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_OPEN
 * stream_idx - uint8_t, index of the stream ("streams_idx" XenStore entry
 *   of the stream)
 * pcm_rate - uint32_t, stream data rate, Hz
 * pcm_format - uint8_t, XENDRM_PCM_FORMAT_XXX value
 * pcm_channels - uint8_t, number of channels of this stream
 * gref_directory_start - grant_ref_t, a reference to the first shared page
 *   describing shared buffer references. At least one page exists. If shared
 *   buffer size exceeds what can be addressed by this single page, then
 *   reference to the next page must be supplied (gref_dir_next_page below
 *   is not NULL)
 *
 * Shared page for XENDRM_OP_OPEN buffer descriptor (gref_directory in the
 *   request) employs a list of pages, describing all pages of the shared data
 *   buffer:
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
 * +/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/|
 * +-----------------+-----------------+-----------------+-----------------+
 * |                                gref[N -1]                             |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * gref_dir_next_page - grant_ref_t, reference to the next page describing
 *   page directory
 * num_grefs - number of grefs in this page
 * gref[i] - grant_ref_t, reference to a shared page of the buffer
 *   allocated at XENDRM_OP_OPEN
 *
 * Request close - close an opened pcm stream:
 *          0                 1                  2                3        octet
 * +-----------------+-----------------+-----------------+-----------------+
 * |                 id                |    operation    |     stream_idx  |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 * |                               reserved                                |
 * +-----------------+-----------------+-----------------+-----------------+
 *
 * id - uint16_t, private guest value, echoed in response
 * operation - uint8_t, XENDRM_OP_CLOSE
 * stream_idx - uint8_t, index of the stream ("streams_idx" XenStore entry
 *   of the stream)
 *
 */

struct xendrm_request {
	uint8_t raw[16];
};

struct xendrm_response {
	uint8_t raw[16];
};

struct xendrm_event {
	uint8_t raw[16];
};

struct xendrm_event_page {
	union {
		uint8_t pad[16];
		struct {
			uint32_t in_cons;
			uint32_t in_prod;
		} ring;
	} u;
};

#endif /* __XEN_PUBLIC_IO_XENDRM_H__ */
