#include <stdio.h>
#include "erl_driver.h"
#include "ei.h"
#include "gen_q_work.h"

typedef struct {
    ErlDrvPort port;
} GenQData;

static ErlDrvData gen_q_drv_start(ErlDrvPort port, char* buff) {
    GenQData* d = (GenQData*)driver_alloc(sizeof(GenQData));
    d->port = port;
    return (ErlDrvData)d;
}

static void gen_q_drv_stop(ErlDrvData handle) {
    driver_free((char*)handle);
}

static void gen_q_drv_output(ErlDrvData handle, char *buff,
        ErlDrvSizeT bufflen) {
    GenQData *d = (GenQData*)handle;
    driver_async(d->port, NULL,
            genq_work,
            genq_malloc_work(buff, bufflen),
            genq_free_work);
}

static void ready_async(ErlDrvData handle, ErlDrvThreadData async) {
    GenQData* d = (GenQData*)handle;

    ei_x_buff x;
    ei_x_new_with_version(&x);
    ei_x_encode_long(&x, 1);
    driver_output(d->port, x.buff, x.index);
    ei_x_free(&x);
}

ErlDrvEntry gen_q_drv_entry = {
    NULL,           /* F_PTR init, called when driver is loaded */
    gen_q_drv_start,      /* L_PTR start, called when port is opened */
    gen_q_drv_stop,       /* F_PTR stop, called when port is closed */
    gen_q_drv_output,     /* F_PTR output, called when erlang has sent */
    NULL,           /* F_PTR ready_input, called when input descriptor ready */
    NULL,           /* F_PTR ready_output, called when output descriptor ready */
    "gen_q_drv",      /* char *driver_name, the argument to open_port */
    NULL,           /* F_PTR finish, called when unloaded */
    NULL,                       /* void *handle, Reserved by VM */
    NULL,           /* F_PTR control, port_command callback */
    NULL,           /* F_PTR timeout, reserved */
    NULL,           /* F_PTR outputv, reserved */
    ready_async,                /* F_PTR ready_async, only for async drivers */
    NULL,                       /* F_PTR flush, called when port is about
                                   to be closed, but there is data in driver
                                   queue */
    NULL,                       /* F_PTR call, much like control, sync call
                                   to driver */
    NULL,                       /* F_PTR event, called when an event selected
                                   by driver_event() occurs. */
    ERL_DRV_EXTENDED_MARKER,    /* int extended marker, Should always be
                                   set to indicate driver versioning */
    ERL_DRV_EXTENDED_MAJOR_VERSION, /* int major_version, should always be
                                       set to this value */
    ERL_DRV_EXTENDED_MINOR_VERSION, /* int minor_version, should always be
                                       set to this value */
    0,                          /* int driver_flags, see documentation */
    NULL,                       /* void *handle2, reserved for VM use */
    NULL,                       /* F_PTR process_exit, called when a
                                   monitored process dies */
    NULL                        /* F_PTR stop_select, called to close an
                                   event object */
};

DRIVER_INIT(gen_q_drv) {
    return &gen_q_drv_entry;
}
