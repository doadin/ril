/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 *                    Joerie de Gram <j.de.gram@gmail.com>
 *                    Simon Busch <morphis@gravedo.de>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <mtd/mtd-abi.h>
#include <assert.h>

#include <radio.h>
#include <wakelock.h>

#include "crespo_modem_ctl.h"
#include "crespo_ipc.h"
#include "ipc_private.h"

int crespo_modem_bootstrap(struct ipc_client *client)
{
    int s3c2410_serial3_fd = -1;
    int modem_ctl_fd = -1;

    /* Control variables. */
    int rc = 0;

    /* Boot variables */
    uint8_t *radio_img_p = NULL;
    uint8_t bootcore_version = 0;
    uint8_t info_size = 0;
    uint8_t crc_byte = 0;
    int block_size = 0;

    /* s3c2410 serial setup variables. */
    struct termios termios;
    int serial;

    /* fds maniplation variables */
    struct timeval timeout;
    fd_set fds;

    /* nv_data variables */
    void *nv_data_p;

    /* General purpose variables. */
    uint8_t data;
    uint16_t data_16;
    uint8_t *data_p;
    int i;

    ipc_client_log(client, "crespo_ipc_bootstrap: enter");

    /* Read the radio.img image. */
    ipc_client_log(client, "crespo_ipc_bootstrap: reading radio image");
    radio_img_p = ipc_client_mtd_read(client, "/dev/mtd/mtd5ro", RADIO_IMG_SIZE, 0x1000);
    if (radio_img_p == NULL) {
        radio_img_p = ipc_client_mtd_read(client, "/dev/mtd5ro", RADIO_IMG_SIZE, 0x1000);
        if (radio_img_p == NULL)
            goto error;
    }
    ipc_client_log(client, "crespo_ipc_bootstrap: radio image read");

    ipc_client_log(client, "crespo_ipc_bootstrap: open modem_ctl");
    modem_ctl_fd=open("/dev/modem_ctl", O_RDWR | O_NDELAY);
    if(modem_ctl_fd < 0)
        goto error;

    /* Reset the modem before init to send the first part of modem.img. */
    rc = ioctl(modem_ctl_fd, IOCTL_MODEM_RESET);
    if (rc < 0)
        goto error;

    usleep(400000);

    ipc_client_log(client, "crespo_ipc_bootstrap: open s3c2410_serial3");
    s3c2410_serial3_fd=open("/dev/s3c2410_serial3", O_RDWR | O_NDELAY);
    if(s3c2410_serial3_fd < 0)
        goto error;

    /* Setup the s3c2410 serial. */
    ipc_client_log(client, "crespo_ipc_bootstrap: setup s3c2410_serial3");
    tcgetattr(s3c2410_serial3_fd, &termios);

    cfmakeraw(&termios);
    cfsetispeed(&termios, B115200);
    cfsetospeed(&termios, B115200);

    tcsetattr(s3c2410_serial3_fd, TCSANOW, &termios);

    /* Send 'AT' in ASCII. */
    ipc_client_log(client, "crespo_ipc_bootstrap: sending AT in ASCII");
    for(i=0 ; i < 20 ; i++)
    {
        rc = write(s3c2410_serial3_fd, "AT", 2);
        usleep(50000);
    }
    ipc_client_log(client, "crespo_ipc_bootstrap: sending AT in ASCII done");

    usleep(50000); //FIXME

    /* Get and check bootcore version. */
    rc = read(s3c2410_serial3_fd, &bootcore_version, sizeof(bootcore_version));
    if (rc < 0)
        goto error;

    ipc_client_log(client, "crespo_ipc_bootstrap: got bootcore version: 0x%x", bootcore_version);

    if(bootcore_version != BOOTCORE_VERSION)
        goto error;

    /* Get info_size. */
    rc = read(s3c2410_serial3_fd, &info_size, sizeof(info_size));
    if (rc < 0)
        goto error;

    ipc_client_log(client, "crespo_ipc_bootstrap: got info_size: 0x%x", info_size);

    /* Send PSI magic. */
    data = PSI_MAGIC;
    rc = write(s3c2410_serial3_fd, &data, sizeof(data));
    if (rc < 0)
        goto error;

    ipc_client_log(client, "crespo_ipc_bootstrap: sent PSI_MAGIC (0x%x)", PSI_MAGIC);

    /* Send PSI data len. */
    data_16 = PSI_DATA_LEN;
    data_p = (uint8_t *)&data_16;

    for(i=0 ; i < 2 ; i++)
    {
        rc = write(s3c2410_serial3_fd, data_p, 1);
        if (rc < 0)
            goto error;

        data_p++;
    }
    ipc_client_log(client, "crespo_ipc_bootstrap: sent PSI_DATA_LEN (0x%x)", PSI_DATA_LEN);

    /* Write the first part of modem.img. */
    FD_ZERO(&fds);
    FD_SET(s3c2410_serial3_fd, &fds);

    timeout.tv_sec = 4;
    timeout.tv_usec = 0;

    data_p = radio_img_p;

    ipc_client_log(client, "crespo_ipc_bootstrap: sending the first part of radio.img");

    for (i=0 ; i < PSI_DATA_LEN ; i++)
    {
        if (select(FD_SETSIZE, NULL, &fds, NULL, &timeout) == 0)
        {
            ipc_client_log(client, "crespo_ipc_bootstrap: select timeout passed");
            goto error;
        }

        rc = write(s3c2410_serial3_fd, data_p, 1);
        if (rc < 0)
            goto error;

        crc_byte = crc_byte ^ *data_p;
        data_p++;
    }

    ipc_client_log(client, "crespo_ipc_bootstrap: first part of radio.img sent; crc_byte is 0x%x", crc_byte);

    if (select(FD_SETSIZE, NULL, &fds, NULL, &timeout) == 0)
    {
        ipc_client_log(client, "crespo_ipc_bootstrap: select timeout passed");
        goto error;
    }

    rc = write(s3c2410_serial3_fd, &crc_byte, sizeof(crc_byte));
    if (rc < 0)
        goto error;

    ipc_client_log(client, "crespo_ipc_bootstrap: crc_byte sent");

    data = 0;
    for (i = 0 ; data != 0x01 ; i++)
    {
        if (select(FD_SETSIZE, &fds, NULL, NULL, &timeout) == 0)
        {
            ipc_client_log(client, "crespo_ipc_bootstrap: select timeout passed");
            goto error;
        }

        rc = read(s3c2410_serial3_fd, &data, sizeof(data));
        if (rc < 0)
            goto error;

        if(i > 50)
        {
            ipc_client_log(client, "crespo_ipc_bootstrap: fairly too much attempts to get ACK");
            goto error;
        }
    }

    ipc_client_log(client, "crespo_ipc_bootstrap: close s3c2410_serial3");
    close(s3c2410_serial3_fd);

    ipc_client_log(client, "crespo_ipc_bootstrap: writing the rest of radio.img to modem_ctl.");
    /* Seek to the begining of modem_ctl_fd (should already be so). */
    lseek(modem_ctl_fd, 0, SEEK_SET);

    /* Pointer to the remaining part of radio.img. */
    data_p = radio_img_p + PSI_DATA_LEN;

    FD_ZERO(&fds);
    FD_SET(modem_ctl_fd, &fds);

    block_size = 0x100000;

    for (i=0 ; i < (RADIO_IMG_SIZE - PSI_DATA_LEN) / block_size ; i++)
    {
        if (select(FD_SETSIZE, NULL, &fds, NULL, &timeout) == 0)
        {
            ipc_client_log(client, "crespo_ipc_bootstrap: select timeout passed");
            goto error;
        }

        rc = write(modem_ctl_fd, data_p, block_size);
        if (rc < 0)
            goto error;

        data_p += block_size;
    }

    free(radio_img_p);

    /* nv_data part. */

    /* Check if all the nv_data files are ok. */
    if (nv_data_check(client) < 0)
        goto error;

    /* Check if the MD5 is ok. */
    if (nv_data_md5_check(client) < 0)
        goto error;

    /* Write nv_data.bin to modem_ctl. */
    ipc_client_log(client, "crespo_ipc_bootstrap: write nv_data to modem_ctl");

    nv_data_p = ipc_client_file_read(client, nv_data_path(client), nv_data_size(client), nv_data_chunk_size(client));
    if (nv_data_p == NULL)
        goto error;

    data_p = nv_data_p;
    lseek(modem_ctl_fd, RADIO_IMG_SIZE, SEEK_SET);

    for(i=0 ; i < 2 ; i++)
    {
        rc = write(modem_ctl_fd, data_p, nv_data_size(client) / 2);
        if (rc < 0)
            goto error;

        data_p += nv_data_size(client) / 2;
    }

    free(nv_data_p);
    close(modem_ctl_fd);

    rc = 0;
    goto exit;

error:
    ipc_client_log(client, "%s: something went wrong", __func__);
    rc = -1;

exit:
    ipc_client_log(client, "crespo_ipc_bootstrap: exit");
    return rc;
}

int crespo_ipc_fmt_client_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct modem_io modem_data;
    struct ipc_header reqhdr;
    int rc = 0;

    memset(&modem_data, 0, sizeof(struct modem_io));
    modem_data.size = request->length + sizeof(struct ipc_header);

    reqhdr.mseq = request->mseq;
    reqhdr.aseq = request->aseq;
    reqhdr.group = request->group;
    reqhdr.index = request->index;
    reqhdr.type = request->type;
    reqhdr.length = (uint16_t) (request->length + sizeof(struct ipc_header));

    modem_data.data = malloc(reqhdr.length);

    memcpy(modem_data.data, &reqhdr, sizeof(struct ipc_header));
    memcpy((unsigned char *) (modem_data.data + sizeof(struct ipc_header)), request->data, request->length);

    assert(client->handlers->write != NULL);

    ipc_client_log_send(client, request, __func__);

    rc = client->handlers->write((uint8_t*) &modem_data, sizeof(struct modem_io), client->handlers->write_data);
    return rc;
}

int crespo_ipc_rfs_client_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct modem_io modem_data;
    int rc = 0;

    memset(&modem_data, 0, sizeof(struct modem_io));

    modem_data.id = request->mseq;
    modem_data.cmd = request->index;

    modem_data.size = request->length;
    modem_data.data = malloc(request->length);

    memcpy(modem_data.data, request->data, request->length);

    assert(client->handlers->write != NULL);

    ipc_client_log_send(client, request, __func__);

    rc = client->handlers->write((uint8_t*) &modem_data, sizeof(struct modem_io), client->handlers->write_data);
    return rc;
}

int crespo_ipc_fmt_client_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    struct modem_io modem_data;
    struct ipc_header *resphdr;
    int bread = 0;

    memset(&modem_data, 0, sizeof(struct modem_io));
    modem_data.data = malloc(MAX_MODEM_DATA_SIZE);
    modem_data.size = MAX_MODEM_DATA_SIZE;

    memset(response, 0, sizeof(struct ipc_message_info));

    wake_lock("secril_fmt-interface");

    assert(client->handlers->read != NULL);
    bread = client->handlers->read((uint8_t*) &modem_data, sizeof(struct modem_io) + MAX_MODEM_DATA_SIZE, client->handlers->read_data);
    if (bread < 0)
    {
        ipc_client_log(client, "crespo_ipc_fmt_client_recv: can't receive enough bytes from modem to process incoming response!");
        return -1;
    }

    if(modem_data.size <= 0 || modem_data.size >= MAX_MODEM_DATA_SIZE || modem_data.data == NULL)
    {
        ipc_client_log(client, "crespo_ipc_fmt_client_recv: we retrieve less (or fairly too much) bytes from the modem than we exepected!");
        return -1;
    }

    resphdr = (struct ipc_header *) modem_data.data;

    response->mseq = resphdr->mseq;
    response->aseq = resphdr->aseq;
    response->group = resphdr->group;
    response->index = resphdr->index;
    response->type = resphdr->type;
    response->cmd = IPC_COMMAND(response);
    response->length = modem_data.size - sizeof(struct ipc_header);
    response->data = NULL;

    if(response->length > 0)
    {
        response->data = malloc(response->length);
        memcpy(response->data, (uint8_t *) modem_data.data + sizeof(struct ipc_header), response->length);
    }

    free(modem_data.data);

    ipc_client_log_recv(client, response, __func__);

    wake_unlock("secril_fmt-interface");

    return 0;
}

int crespo_ipc_rfs_client_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    struct modem_io modem_data;
    int bread = 0;

    memset(&modem_data, 0, sizeof(struct modem_io));
    modem_data.data = malloc(MAX_MODEM_DATA_SIZE);
    modem_data.size = MAX_MODEM_DATA_SIZE;

    memset(response, 0, sizeof(struct ipc_message_info));

    wake_lock("secril_rfs-interface");

    assert(client->handlers->read != NULL);
    bread = client->handlers->read((uint8_t*) &modem_data, sizeof(struct modem_io) + MAX_MODEM_DATA_SIZE, client->handlers->read_data);
    if (bread < 0)
    {
        ipc_client_log(client, "crespo_ipc_rfs_client_recv: can't receive enough bytes from modem to process incoming response!");
        return -1;
    }

    if(modem_data.size <= 0 || modem_data.size >= MAX_MODEM_DATA_SIZE || modem_data.data == NULL)
    {
        ipc_client_log(client, "crespo_ipc_rfs_client_recv: we retrieve less (or fairly too much) bytes from the modem than we exepected!");
        return -1;
    }

    response->mseq = 0;
    response->aseq = modem_data.id;
    response->group = IPC_GROUP_RFS;
    response->index = modem_data.cmd;
    response->type = 0;
    response->length = modem_data.size;
    response->data = NULL;

    if(response->length > 0)
    {
        response->data = malloc(response->length);
        memcpy(response->data, (uint8_t *) modem_data.data, response->length);
    }

    free(modem_data.data);

    ipc_client_log_recv(client, response, __func__);

    wake_unlock("secril_rfs-interface");

    return 0;
}

int crespo_ipc_open(int type, void *io_data)
{
    int fd = -1;

    switch(type)
    {
        case IPC_CLIENT_TYPE_FMT:
            fd = open("/dev/modem_fmt", O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        case IPC_CLIENT_TYPE_RFS:
            fd = open("/dev/modem_rfs", O_RDWR | O_NOCTTY | O_NONBLOCK);
            break;
        default:
            break;
    }

    if(fd < 0)
        return -1;

    if(io_data == NULL)
        return -1;

    memcpy(io_data, &fd, sizeof(int));

    return 0;
}

int crespo_ipc_close(void *io_data)
{
    int fd = -1;

    if(io_data == NULL)
        return -1;

    fd = *((int *) io_data);

    if(fd < 0)
        return -1;

    close(fd);

    return 0;
}

int crespo_ipc_read(void *data, unsigned int size, void *io_data)
{
    int fd = -1;
    int rc;

    if(io_data == NULL)
        return -1;

    if(data == NULL)
        return -1;

    fd = *((int *) io_data);

    if(fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_RECV, data);

    if(rc < 0)
        return -1;

    return 0;
}

int crespo_ipc_write(void *data, unsigned int size, void *io_data)
{
    int fd = -1;
    int rc;

    if(io_data == NULL)
        return -1;

    fd = *((int *) io_data);

    if(fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_SEND, data);

    if(rc < 0)
        return -1;

    return 0;
}

int crespo_ipc_power_on(void *io_data)
{
    int fd=open("/dev/modem_ctl", O_RDWR);
    int rc;

/*
    fd = open("/sys/devices/platform/modemctl/power_mode", O_RDWR);
    rc = write(fd, "1", 1);
*/

    if(fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_START);
    close(fd);

    if(rc < 0)
        return -1;

    return 0;
}

int crespo_ipc_power_off(void *io_data)
{
    int fd=open("/dev/modem_ctl", O_RDWR);
    int rc;

/*
    fd = open("/sys/devices/platform/modemctl/power_mode", O_RDWR);
    rc = write(fd, "0", 1);
*/

    if(fd < 0)
        return -1;

    rc = ioctl(fd, IOCTL_MODEM_OFF);
    close(fd);

    if(rc < 0)
        return -1;

    return 0;
}

char *crespo_2_6_35_ipc_gprs_get_iface(int cid)
{
    char *iface = NULL;

    asprintf(&iface, "%s%d", GPRS_IFACE_PREFIX, 0);

    return iface;
}

char *crespo_3_0_ipc_gprs_get_iface(int cid)
{
    char *iface = NULL;

    if(cid > GPRS_IFACE_COUNT)
        return NULL;

    asprintf(&iface, "%s%d", GPRS_IFACE_PREFIX, cid - 1);

    return iface;
}

int crespo_2_6_35_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *cap)
{
    if (cap == NULL)
        return -1;

    cap->port_list = 0;
    cap->cid_max = 1;

    return 0;
}

int crespo_3_0_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *cap)
{
    if (cap == NULL)
        return -1;

    cap->port_list = 0;
    cap->cid_max = GPRS_IFACE_COUNT;

    return 0;
}

void *crespo_ipc_common_data_create(void)
{
    void *io_data;
    int io_data_len;

    io_data_len = sizeof(int);
    io_data = malloc(io_data_len);

    if(io_data == NULL)
        return NULL;

    memset(io_data, 0, io_data_len);

    return io_data;
}

int crespo_ipc_common_data_destroy(void *io_data)
{
    // This was already done, not an error but we need to return
    if(io_data == NULL)
        return 0;

    free(io_data);

    return 0;
}

int crespo_ipc_common_data_set_fd(void *io_data, int fd)
{
    int *common_data;

    if(io_data == NULL)
        return -1;

    common_data = (int *) io_data;
    *common_data = fd;

    return 0;
}

int crespo_ipc_common_data_get_fd(void *io_data)
{
    int *common_data;

    if(io_data == NULL)
        return -1;

    common_data = (int *) io_data;

    return (int) *(common_data);
}

struct ipc_ops crespo_fmt_ops = {
    .send = crespo_ipc_fmt_client_send,
    .recv = crespo_ipc_fmt_client_recv,
    .bootstrap = crespo_modem_bootstrap,
};

struct ipc_ops crespo_rfs_ops = {
    .send = crespo_ipc_rfs_client_send,
    .recv = crespo_ipc_rfs_client_recv,
    .bootstrap = NULL,
};

struct ipc_handlers crespo_default_handlers = {
    .read = crespo_ipc_read,
    .write = crespo_ipc_write,
    .open = crespo_ipc_open,
    .close = crespo_ipc_close,
    .power_on = crespo_ipc_power_on,
    .power_off = crespo_ipc_power_off,
    .common_data = NULL,
    .common_data_create = crespo_ipc_common_data_create,
    .common_data_destroy = crespo_ipc_common_data_destroy,
    .common_data_set_fd = crespo_ipc_common_data_set_fd,
    .common_data_get_fd = crespo_ipc_common_data_get_fd,
};

struct ipc_gprs_specs crespo_2_6_35_gprs_specs = {
    .gprs_get_iface = crespo_2_6_35_ipc_gprs_get_iface,
    .gprs_get_capabilities = crespo_2_6_35_ipc_gprs_get_capabilities,
};

struct ipc_gprs_specs crespo_3_0_gprs_specs = {
    .gprs_get_iface = crespo_3_0_ipc_gprs_get_iface,
    .gprs_get_capabilities = crespo_3_0_ipc_gprs_get_capabilities,
};

// vim:ts=4:sw=4:expandtab
