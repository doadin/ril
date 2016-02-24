/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
 *                    Joerie de Gram <j.de.gram@gmail.com>
 *                    Simon Busch <morphis@gravedo.de>
 *                    Igor Almeida <igor.contato@gmail.com>
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
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <net/if.h>
#include <asm/types.h>
#include <mtd/mtd-abi.h>
#include <assert.h>
#include <string.h>

#include <radio.h>

#include "aries_ipc.h"
#include "ipc_private.h"
#include "sipc4.h"
#include "onedram.h"
#include "phonet.h"

int phonet_iface_ifdown(void)
{
    struct ifreq ifr;
    int fd;
    int rc;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, PHONET_IFACE, IFNAMSIZ);

    fd = socket(AF_PHONET, SOCK_DGRAM, 0);

    rc = ioctl(fd, SIOCGIFFLAGS, &ifr);
    if(rc < 0)
        return -1;

    ifr.ifr_flags = (ifr.ifr_flags & (~IFF_UP));

    rc = ioctl(fd, SIOCSIFFLAGS, &ifr);
    if(rc < 0)
        return -1;

    close(fd);

    return 0;
}

int phonet_iface_ifup(void)
{
    struct ifreq ifr;
    int fd;
    int rc;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, PHONET_IFACE, IFNAMSIZ);

    fd = socket(AF_PHONET, SOCK_DGRAM, 0);

    rc = ioctl(fd, SIOCGIFFLAGS, &ifr);
    if(rc < 0)
        return -1;

    ifr.ifr_flags |= IFF_UP;

    rc = ioctl(fd, SIOCSIFFLAGS, &ifr);
    if(rc < 0)
        return -1;

    close(fd);

    return 0;
}

int aries_modem_bootstrap(struct ipc_client *client)
{
    int s3c2410_serial3_fd = -1;
    int onedram_fd = -1;

    /* Control variables. */
    int rc = 0;

    /* Boot variables */
    uint8_t *radio_img_p = NULL;
    uint32_t onedram_data = 0;
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
    void *onedram_p;

    /* General purpose variables. */
    uint8_t data;
    uint16_t data_16;
    uint8_t *data_p;
    int i;

    ipc_client_log(client, "aries_ipc_bootstrap: enter");

    /* Read the radio.img image. */
    ipc_client_log(client, "aries_ipc_bootstrap: reading radio image");
    radio_img_p = ipc_client_mtd_read(client, "/dev/block/bml12", RADIO_IMG_READ_SIZE, RADIO_IMG_READ_SIZE);
    ipc_client_log(client, "aries_ipc_bootstrap: radio image read");

    ipc_client_log(client, "aries_ipc_bootstrap: open onedram");
    onedram_fd=open("/dev/onedram", O_RDWR);
    if(onedram_fd < 0)
        goto error;

    /* Reset the modem before init to send the first part of modem.img. */
    ipc_client_log(client, "aries_ipc_bootstrap: turning %s iface down", PHONET_IFACE);
    rc = phonet_iface_ifdown();
    if(rc < 0)
        goto error;

    ipc_client_power_off(client);
    ipc_client_log(client, "aries_ipc_bootstrap: sent PHONE \"off\" command");
    usleep(1000);

    ipc_client_power_on(client);
    ipc_client_log(client, "aries_ipc_bootstrap: sent PHONE \"on\" command");
    usleep(200000);

    ipc_client_log(client, "aries_ipc_bootstrap: open s3c2410_serial3");
    s3c2410_serial3_fd=open("/dev/s3c2410_serial3", O_RDWR);
    if(s3c2410_serial3_fd < 0)
        goto error;

    /* Setup the s3c2410 serial. */
    ipc_client_log(client, "aries_ipc_bootstrap: setup s3c2410_serial3");
    tcgetattr(s3c2410_serial3_fd, &termios);

    cfmakeraw(&termios);
    cfsetispeed(&termios, B115200);
    cfsetospeed(&termios, B115200);

    tcsetattr(s3c2410_serial3_fd, TCSANOW, &termios);

    /* Send 'AT' in ASCII. */
    ipc_client_log(client, "aries_ipc_bootstrap: sending AT in ASCII");
    for(i=0 ; i < 20 ; i++)
    {
        rc = write(s3c2410_serial3_fd, "AT", 2);
        usleep(50000);
    }
    ipc_client_log(client, "aries_ipc_bootstrap: sending AT in ASCII done");

    /* Write the first part of modem.img. */
    FD_ZERO(&fds);
    FD_SET(s3c2410_serial3_fd, &fds);

    timeout.tv_sec=5;
    timeout.tv_usec=0;

    if(select(s3c2410_serial3_fd + 1, &fds, NULL, NULL, &timeout) == 0)
    {
        ipc_client_log(client, "aries_ipc_bootstrap: select timeout passed");
        goto error;
    }

    /* Get and check bootcore version. */
    read(s3c2410_serial3_fd, &bootcore_version, sizeof(bootcore_version));
    ipc_client_log(client, "aries_ipc_bootstrap: got bootcore version: 0x%x", bootcore_version);

    if(bootcore_version != BOOTCORE_VERSION)
        goto error;

    FD_ZERO(&fds);
    FD_SET(s3c2410_serial3_fd, &fds);

    timeout.tv_sec=5;
    timeout.tv_usec=0;

    if(select(s3c2410_serial3_fd + 1, &fds, NULL, NULL, &timeout) == 0)
    {
        ipc_client_log(client, "aries_ipc_bootstrap: select timeout passed");
        goto error;
    }

    /* Get info_size. */
    read(s3c2410_serial3_fd, &info_size, sizeof(info_size));
    ipc_client_log(client, "aries_ipc_bootstrap: got info_size: 0x%x", info_size);

    /* Send PSI magic. */
    data=PSI_MAGIC;
    write(s3c2410_serial3_fd, &data, sizeof(uint8_t));
    ipc_client_log(client, "aries_ipc_bootstrap: sent PSI_MAGIC (0x%x)", PSI_MAGIC);

    /* Send PSI data len. */
    data_16=PSI_DATA_LEN;
    data_p=(uint8_t *)&data_16;

    for(i=0 ; i < 2 ; i++)
    {
        write(s3c2410_serial3_fd, data_p, sizeof(uint8_t));
        data_p++;
    }
    ipc_client_log(client, "aries_ipc_bootstrap: sent PSI_DATA_LEN (0x%x)", PSI_DATA_LEN);

    timeout.tv_sec=5;
    timeout.tv_usec=0;

    data_p=radio_img_p;

    ipc_client_log(client, "aries_ipc_bootstrap: sending the first part of radio.img");

    for(i=0 ; i < PSI_DATA_LEN ; i++)
    {
        FD_ZERO(&fds);
        FD_SET(s3c2410_serial3_fd, &fds);

        if(select(s3c2410_serial3_fd + 1, NULL, &fds, NULL, &timeout) == 0)
        {
            ipc_client_log(client, "aries_ipc_bootstrap: select timeout passed");
            goto error;
        }

        write(s3c2410_serial3_fd, data_p, 1);
        crc_byte=crc_byte ^ *data_p;

        data_p++;
    }

    ipc_client_log(client, "aries_ipc_bootstrap: first part of radio.img sent; crc_byte is 0x%x", crc_byte);

    FD_ZERO(&fds);
    FD_SET(s3c2410_serial3_fd, &fds);

    if(select(s3c2410_serial3_fd + 1, NULL, &fds, NULL, &timeout) == 0)
    {
        ipc_client_log(client, "aries_ipc_bootstrap: select timeout passed");
        goto error;
    }

    write(s3c2410_serial3_fd, &crc_byte, sizeof(crc_byte));

    ipc_client_log(client, "aries_ipc_bootstrap: crc_byte sent");

    timeout.tv_sec=5;
    timeout.tv_usec=0;

    data = 0;
    for(i = 0 ; data != 0x01 ; i++)
    {
        FD_ZERO(&fds);
        FD_SET(s3c2410_serial3_fd, &fds);

        if(select(s3c2410_serial3_fd + 1, &fds, NULL, NULL, &timeout) == 0)
        {
            ipc_client_log(client, "aries_ipc_bootstrap: select timeout passed");
            goto error;
        }

        read(s3c2410_serial3_fd, &data, sizeof(data));

        if(i > 50)
        {
            ipc_client_log(client, "aries_ipc_bootstrap: fairly too much attempts to get ACK");
            goto error;
        }
    }

    ipc_client_log(client, "aries_ipc_bootstrap: close s3c2410_serial3");
    close(s3c2410_serial3_fd);

    timeout.tv_sec=3;
    timeout.tv_usec=0;

    ipc_client_log(client, "aries_ipc_bootstrap: wait for 0x%04x from onedram", ONEDRAM_INIT_READ);

    onedram_data = 0;
    while(onedram_data != ONEDRAM_INIT_READ) {
        FD_ZERO(&fds);
        FD_SET(onedram_fd, &fds);

        if(select(onedram_fd + 1, &fds, NULL, NULL, &timeout) == 0)
        {
            ipc_client_log(client, "aries_ipc_bootstrap: select timeout passed");
            goto error;
        }

        read(onedram_fd, &onedram_data, sizeof(onedram_data));

        ipc_client_log(client, "aries_ipc_bootstrap: read 0x%04x from onedram", onedram_data);
    }

    ipc_client_log(client, "aries_ipc_bootstrap: writing the rest of modem.img to onedram.");

    /* Pointer to the remaining part of radio.img. */
    data_p=radio_img_p + PSI_DATA_LEN;

    onedram_p = mmap(NULL, ONENAND_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, onedram_fd, 0);

    if(onedram_p == NULL || onedram_p < 0 || onedram_p == 0xffffffff)
    {
        ipc_client_log(client, "aries_ipc_bootstrap: could not map onedram to memory");
        goto error;
    }

    ipc_client_log(client, "aries_ipc_bootstrap: mapped onedram to 0x%x", onedram_p);

    memcpy(onedram_p, data_p, RADIO_IMG_READ_SIZE - PSI_DATA_LEN);

    free(radio_img_p);

    /* nv_data part. */

    /* Check if all the nv_data files are ok. */
    if (nv_data_check(client) < 0)
        goto error;

    /* Check if the MD5 is ok. */
    if (nv_data_md5_check(client) < 0)
        goto error;

    /* Write nv_data.bin to modem_ctl. */
    ipc_client_log(client, "aries_ipc_bootstrap: write nv_data to onedram");

    nv_data_p = ipc_client_file_read(client, nv_data_path(client), nv_data_size(client), nv_data_chunk_size(client));
    if (nv_data_p == NULL)
        goto error;
    data_p = nv_data_p;

    memcpy(onedram_p + RADIO_IMG_MAX_SIZE, data_p, nv_data_size(client));

    free(nv_data_p);

    munmap(onedram_p, ONENAND_MAP_SIZE);

    if(ioctl(onedram_fd, ONEDRAM_REL_SEM) < 0)
    {
        ipc_client_log(client, "aries_ipc_bootstrap: ONEDRAM_REL_SEM ioctl on onedram failed");
        goto error;
    }

    onedram_data = ONEDRAM_DEINIT_CMD;

    ipc_client_log(client, "aries_ipc_bootstrap: send 0x%04x", onedram_data);
    write(onedram_fd, &onedram_data, sizeof(onedram_data));

    timeout.tv_sec=3;
    timeout.tv_usec=0;

    ipc_client_log(client, "aries_ipc_bootstrap: wait for 0x%04x from onedram", ONEDRAM_DEINIT_READ);

    onedram_data = 0;
    while(onedram_data != ONEDRAM_DEINIT_READ) {
        FD_ZERO(&fds);
        FD_SET(onedram_fd, &fds);

        if(select(onedram_fd + 1, &fds, NULL, NULL, &timeout) == 0)
        {
            ipc_client_log(client, "aries_ipc_bootstrap: select timeout passed");
            goto error;
        }

        read(onedram_fd, &onedram_data, sizeof(onedram_data));

        ipc_client_log(client, "aries_ipc_bootstrap: read 0x%04x from onedram", onedram_data);
    }

    close(onedram_fd);

    rc = 0;
    goto exit;

error:
    ipc_client_log(client, "aries_ipc_bootstrap: something went wrong");
    rc = -1;

exit:
    ipc_client_log(client, "aries_ipc_bootstrap: exit");
    return rc;
}

int aries_ipc_fmt_client_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct ipc_header reqhdr;
    void *data;
    int rc = 0;

    reqhdr.mseq = request->mseq;
    reqhdr.aseq = request->aseq;
    reqhdr.group = request->group;
    reqhdr.index = request->index;
    reqhdr.type = request->type;
    reqhdr.length = (uint16_t) (request->length + sizeof(struct ipc_header));

    data = malloc(reqhdr.length);

    memcpy(data, &reqhdr, sizeof(struct ipc_header));
    memcpy((unsigned char *) (data + sizeof(struct ipc_header)), request->data, request->length);

    assert(client->handlers->write != NULL);

    ipc_client_log_send(client, request, __func__);

    rc = client->handlers->write(data, reqhdr.length, client->handlers->write_data);
    return rc;
}

int aries_ipc_rfs_client_send(struct ipc_client *client, struct ipc_message_info *request)
{
    struct rfs_hdr *rfs_hdr;
    void *data;
    int rc = 0;

    data = malloc(request->length + sizeof(struct rfs_hdr));
    memset(data, 0, request->length + sizeof(struct rfs_hdr));

    rfs_hdr = (struct rfs_hdr *) data;

    rfs_hdr->id = request->mseq;
    rfs_hdr->cmd = request->index;
    rfs_hdr->len = request->length + sizeof(struct rfs_hdr);

    memcpy((void *) (data + sizeof(struct rfs_hdr)), request->data, request->length);

    assert(client->handlers->write != NULL);

    ipc_client_log_send(client, request, __func__);

    rc = client->handlers->write((uint8_t*) data, rfs_hdr->len, client->handlers->write_data);
    return rc;
}

int aries_ipc_fmt_client_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    struct ipc_header *resphdr;
    void *data;
    int bread = 0;

    data = malloc(MAX_MODEM_DATA_SIZE);
    memset(data, 0, MAX_MODEM_DATA_SIZE);

    memset(response, 0, sizeof(struct ipc_message_info));

    assert(client->handlers->read != NULL);
    bread = client->handlers->read((uint8_t*) data, MAX_MODEM_DATA_SIZE, client->handlers->read_data);

    if (bread < 0)
    {
        ipc_client_log(client, "aries_ipc_fmt_client_recv: can't receive enough bytes from modem to process incoming response!");
        return -1;
    }

    if(data == NULL)
    {
        ipc_client_log(client, "aries_ipc_fmt_client_recv: we retrieve less (or fairly too much) bytes from the modem than we exepected!");
        return -1;
    }

    resphdr = (struct ipc_header *) data;

    response->mseq = resphdr->mseq;
    response->aseq = resphdr->aseq;
    response->group = resphdr->group;
    response->index = resphdr->index;
    response->type = resphdr->type;
    response->cmd = IPC_COMMAND(response);
    response->length = resphdr->length - sizeof(struct ipc_header);
    response->data = NULL;

    if(response->length > 0)
    {
        response->data = malloc(response->length);
        memcpy(response->data, (uint8_t *) data + sizeof(struct ipc_header), response->length);
    }

    free(data);

    ipc_client_log_recv(client, response, __func__);

    return 0;
}

int aries_ipc_rfs_client_recv(struct ipc_client *client, struct ipc_message_info *response)
{
    void *data;
    int bread = 0;
    struct rfs_hdr *rfs_hdr;

    data = malloc(MAX_MODEM_DATA_SIZE);
    memset(data, 0, MAX_MODEM_DATA_SIZE);

    memset(response, 0, sizeof(struct ipc_message_info));

    assert(client->handlers->read != NULL);
    bread = client->handlers->read((uint8_t*) data, MAX_MODEM_DATA_SIZE, client->handlers->read_data);
    if (bread < 0)
    {
        ipc_client_log(client, "aries_ipc_rfs_client_recv: can't receive enough bytes from modem to process incoming response!");
        return -1;
    }

    rfs_hdr = (struct rfs_hdr *) data;

    if(rfs_hdr->len <= 0 || rfs_hdr->len >= MAX_MODEM_DATA_SIZE || data == NULL)
    {
        ipc_client_log(client, "aries_ipc_rfs_client_recv: we retrieve less (or fairly too much) bytes from the modem than we exepected!");
        return -1;
    }

    response->mseq = 0;
    response->aseq = rfs_hdr->id;
    response->group = IPC_GROUP_RFS;
    response->index = rfs_hdr->cmd;
    response->type = 0;
    response->length = rfs_hdr->len - sizeof(struct rfs_hdr);
    response->data = NULL;

    if(response->length > 0)
    {
        response->data = malloc(response->length);
        memcpy(response->data, (uint8_t *) (data + sizeof(struct rfs_hdr)), response->length);
    }

    free(data);

    ipc_client_log_recv(client, response, __func__);

    return 0;
}

int aries_ipc_open(int type, void *io_data)
{
    struct aries_ipc_handlers_common_data *common_data;
    struct sockaddr_pn *spn;
    struct ifreq ifr;

    int reuse;
    int socket_rfs_magic;

    int fd = -1;
    int rc;

    if(io_data == NULL)
        goto error;

    common_data = (struct aries_ipc_handlers_common_data *) io_data;
    spn = common_data->spn;

    if(spn == NULL)
        goto error;

    memset(&ifr, 0, sizeof(ifr));
    memset(ifr.ifr_name, 0, IFNAMSIZ);
    memset(spn, 0, sizeof(struct sockaddr_pn));

    strncpy(ifr.ifr_name, PHONET_IFACE, IFNAMSIZ);

    spn->spn_family = AF_PHONET;
    spn->spn_dev = 0;

    switch(type)
    {
        case IPC_CLIENT_TYPE_FMT:
            spn->spn_resource = PHONET_SPN_RES_FMT;
            break;
        case IPC_CLIENT_TYPE_RFS:
            spn->spn_resource = PHONET_SPN_RES_RFS;
            break;
        default:
            break;
    }

    fd = socket(AF_PHONET, SOCK_DGRAM, 0);
    if(fd < 0)
        return -1;

    rc = setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifr.ifr_name, IFNAMSIZ);
    if(rc < 0)
        goto error;

    rc = ioctl(fd, SIOCGIFINDEX, &ifr);
    if(rc < 0)
        goto error;

    reuse = 1;
    rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if(rc < 0)
        goto error;

    rc = bind(fd, spn, sizeof(struct sockaddr_pn));
    if(rc < 0)
        goto error;

    common_data->fd = fd;

    if(type == IPC_CLIENT_TYPE_RFS)
    {
        socket_rfs_magic = SOCKET_RFS_MAGIC;
        rc = setsockopt(fd, SOL_SOCKET, SO_RFSMAGIC, &socket_rfs_magic, sizeof(socket_rfs_magic));
        if(rc < 0)
            goto error;
    }

    rc = phonet_iface_ifup();
    if(rc < 0)
        goto error;

    goto end;

error:
    return -1;

end:
    return 0;
}

int aries_ipc_close(void *io_data)
{
    struct aries_ipc_handlers_common_data *common_data;
    int fd = -1;

    if(io_data == NULL)
        return -1;

    common_data = (struct aries_ipc_handlers_common_data *) io_data;
    fd = common_data->fd;

    if(fd < 0)
        return -1;

    close(fd);

    return 0;
}

int aries_ipc_read(void *data, unsigned int size, void *io_data)
{
    struct aries_ipc_handlers_common_data *common_data;
    int spn_len;
    int fd = -1;
    int rc;

    if(io_data == NULL)
        return -1;

    if(data == NULL)
        return -1;

    common_data = (struct aries_ipc_handlers_common_data *) io_data;
    fd = common_data->fd;

    if(fd < 0)
        return -1;

    spn_len = sizeof(struct sockaddr_pn);
    rc = recvfrom(fd, data, size, 0, common_data->spn, &spn_len);

    if(rc < 0)
        return -1;

    return 0;
}

int aries_ipc_write(void *data, unsigned int size, void *io_data)
{
    struct aries_ipc_handlers_common_data *common_data;
    int spn_len;
    int fd = -1;
    int rc;

    if(io_data == NULL)
        return -1;

    if(data == NULL)
        return -1;

    common_data = (struct aries_ipc_handlers_common_data *) io_data;
    fd = common_data->fd;

    if(fd < 0)
        return -1;

    spn_len = sizeof(struct sockaddr_pn);

    rc = sendto(fd, data, size, 0, common_data->spn, spn_len);

    if(rc < 0)
        return -1;

    return 0;
}

int aries_ipc_power_on(void *data)
{
    int fd = open("/sys/class/modemctl/xmm/status", O_RDONLY);
    char status[1] = { 0 };
    char power_data[4] = "on";
    int rc;

    if(fd < 0)
        return -1;

    rc = read(fd, status, 1);

    close(fd);

    if(rc < 0)
        return -1;

    // it's already on
    if(status[0] == '1')
        return 0;

    fd = open("/sys/class/modemctl/xmm/control", O_RDWR);
    if(fd < 0)
        return -1;

    rc = write(fd, power_data, 2);

    close(fd);

    if(rc < 0)
        return -1;

    return 0;
}

int aries_ipc_power_off(void *data)
{
    int fd = open("/sys/class/modemctl/xmm/status", O_RDONLY);
    char status[1] = { 0 };
    char power_data[5] = "off";
    int rc;

    if(fd < 0)
        return -1;

    rc = read(fd, status, 1);

    close(fd);

    if(rc < 0)
        return -1;

    // it's already off
    if(status[0] == '0')
        return 0;

    fd = open("/sys/class/modemctl/xmm/control", O_RDWR);
    if(fd < 0)
        return -1;

    rc = write(fd, power_data, 3);

    close(fd);

    if(rc < 0)
        return -1;

    return 0;
}

//TODO: there are also suspend/resume nodes

int aries_ipc_gprs_activate(void *data, int cid)
{
    int fd = open("/sys/class/net/svnet0/pdp/activate", O_RDWR);
    char *activate_data = NULL;
    int rc;

    if(fd < 0)
        return -1;

    asprintf(&activate_data, "%d\n", cid);
    if(activate_data == NULL)
        return -1;

    rc = write(fd, activate_data, strlen(activate_data));

    free(activate_data);
    close(fd);

    if(rc < 0)
        return -1;

    return 0;

}

int aries_ipc_gprs_deactivate(void *data, int cid)
{
    int fd = open("/sys/class/net/svnet0/pdp/deactivate", O_RDWR);
    char *deactivate_data = NULL;
    int rc;

    if(fd < 0)
        return -1;

    asprintf(&deactivate_data, "%d\n", cid);
    if(deactivate_data == NULL)
        return -1;

    rc = write(fd, deactivate_data, strlen(deactivate_data));

    free(deactivate_data);
    close(fd);

    if(rc < 0)
        return -1;

    return 0;
}

char *aries_ipc_gprs_get_iface(int cid)
{
    struct ifreq ifr;
    char *iface = NULL;
    int fd;
    int rc;
    int i;

    memset(&ifr, 0, sizeof(ifr));

    fd = socket(AF_PHONET, SOCK_DGRAM, 0);


    for(i=GPRS_IFACE_COUNT-1 ; i >= 0 ; i--) {
        sprintf(ifr.ifr_name, "%s%d", GPRS_IFACE_PREFIX, i);
        rc = ioctl(fd, SIOCGIFFLAGS, &ifr);
        if(rc < 0 || ifr.ifr_flags & IFF_UP) {
            continue;
        } else {
            asprintf(&iface, "%s%d", GPRS_IFACE_PREFIX, i);
            return iface;
        }
    }

    return NULL;
}

int aries_ipc_gprs_get_capabilities(struct ipc_client_gprs_capabilities *cap)
{
    if (cap == NULL)
        return -1;

    cap->port_list = 1;
    cap->cid_max = GPRS_IFACE_COUNT;

    return 0;
}

void *aries_ipc_common_data_create(void)
{
    struct aries_ipc_handlers_common_data *common_data;
    void *io_data;
    int io_data_len;
    int spn_len;

    io_data_len = sizeof(struct aries_ipc_handlers_common_data);
    io_data = malloc(io_data_len);

    if(io_data == NULL)
        return NULL;

    memset(io_data, 0, io_data_len);

    common_data = (struct aries_ipc_handlers_common_data *) io_data;

    spn_len = sizeof(struct sockaddr_pn);
    common_data->spn = malloc(spn_len);

    if(common_data == NULL)
        return NULL;

    memset(common_data->spn, 0, spn_len);

    return io_data;
}

int aries_ipc_common_data_destroy(void *io_data)
{
    struct aries_ipc_handlers_common_data *common_data;

    // This was already done, not an error but we need to return
    if(io_data == NULL)
        return 0;

    common_data = (struct aries_ipc_handlers_common_data *) io_data;

    if(common_data->spn != NULL)
        free(common_data->spn);

    free(io_data);

    return 0;
}

int aries_ipc_common_data_set_fd(void *io_data, int fd)
{
    struct aries_ipc_handlers_common_data *common_data;

    if(io_data == NULL)
        return -1;

    common_data = (struct aries_ipc_handlers_common_data *) io_data;
    common_data->fd = fd;

    return 0;
}

int aries_ipc_common_data_get_fd(void *io_data)
{
    struct aries_ipc_handlers_common_data *common_data;

    if(io_data == NULL)
        return -1;

    common_data = (struct aries_ipc_handlers_common_data *) io_data;

    return common_data->fd;
}

struct ipc_ops aries_fmt_ops = {
    .send = aries_ipc_fmt_client_send,
    .recv = aries_ipc_fmt_client_recv,
    .bootstrap = aries_modem_bootstrap,
};

struct ipc_ops aries_rfs_ops = {
    .send = aries_ipc_rfs_client_send,
    .recv = aries_ipc_rfs_client_recv,
    .bootstrap = NULL,
};

struct ipc_handlers aries_default_handlers = {
    .read = aries_ipc_read,
    .write = aries_ipc_write,
    .open = aries_ipc_open,
    .close = aries_ipc_close,
    .power_on = aries_ipc_power_on,
    .power_off = aries_ipc_power_off,
    .gprs_activate = aries_ipc_gprs_activate,
    .gprs_deactivate = aries_ipc_gprs_deactivate,
    .common_data = NULL,
    .common_data_create = aries_ipc_common_data_create,
    .common_data_destroy = aries_ipc_common_data_destroy,
    .common_data_set_fd = aries_ipc_common_data_set_fd,
    .common_data_get_fd = aries_ipc_common_data_get_fd,
};

struct ipc_gprs_specs aries_gprs_specs = {
    .gprs_get_iface = aries_ipc_gprs_get_iface,
    .gprs_get_capabilities = aries_ipc_gprs_get_capabilities,
};

// vim:ts=4:sw=4:expandtab
