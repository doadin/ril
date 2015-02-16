/**
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2011 Paul Kocialkowski <contact@paulk.fr>
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <openssl/md5.h>

#include <radio.h>

#include "ipc_private.h"

void md5hash2string(char *out, uint8_t *in)
{
    int i;

    for (i=0 ; i < MD5_DIGEST_LENGTH ; i++)
    {
        /* After the first iteration, we override \0. */
        if (*in < 0x10)
            sprintf(out, "0%x", *in);
        else
            sprintf(out, "%x", *in);
        in++;
        out+=2;
    }
}

char *nv_data_path(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_data_path == NULL)
        return NV_DATA_PATH_DEFAULT;

    return client->nv_data_specs->nv_data_path;
}

char *nv_data_md5_path(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_data_md5_path == NULL)
        return NV_DATA_MD5_PATH_DEFAULT;

    return client->nv_data_specs->nv_data_md5_path;
}

char *nv_data_bak_path(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_data_bak_path == NULL)
        return NV_DATA_BAK_PATH_DEFAULT;

    return client->nv_data_specs->nv_data_bak_path;
}

char *nv_data_md5_bak_path(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_data_md5_bak_path == NULL)
        return NV_DATA_MD5_BAK_PATH_DEFAULT;

    return client->nv_data_specs->nv_data_md5_bak_path;
}

char *nv_state_path(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_state_path == NULL)
        return NV_STATE_PATH_DEFAULT;

    return client->nv_data_specs->nv_state_path;
}

char *nv_data_secret(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_data_secret == NULL)
        return NV_DATA_SECRET_DEFAULT;

    return client->nv_data_specs->nv_data_secret;
}

int nv_data_size(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_data_size == 0)
        return NV_DATA_SIZE_DEFAULT;

    return client->nv_data_specs->nv_data_size;
}

int nv_data_chunk_size(struct ipc_client *client)
{
    if (client == NULL ||
        client->nv_data_specs == NULL ||
        client->nv_data_specs->nv_data_chunk_size == 0)
        return NV_DATA_CHUNK_SIZE_DEFAULT;

    return client->nv_data_specs->nv_data_chunk_size;
}

void nv_data_generate(struct ipc_client *client)
{
    ipc_client_log(client, "This feature isn't present yet\n");

//  nv_data_backup_create();
}

void nv_data_md5_compute(void *data_p, int size, char *secret, void *hash)
{
    MD5_CTX ctx;

//  MD5((unsigned char *)nv_data_p, nv_data_stat.st_size, nv_data_md5_hash);

    MD5_Init(&ctx);
    MD5_Update(&ctx, data_p, size);
    MD5_Update(&ctx, secret, strlen(secret));
    MD5_Final(hash, &ctx);
}

void nv_data_md5_generate(struct ipc_client *client)
{
    uint8_t nv_data_md5_hash[MD5_DIGEST_LENGTH];
    char *nv_data_md5_hash_string = NULL;
    void *nv_data_p = NULL;
    int fd;
    int rc;

    ipc_client_log(client, "nv_data_md5_generate: enter\n");

    ipc_client_log(client, "nv_data_md5_generate: generating MD5 hash\n");
    nv_data_p=ipc_client_file_read(client, nv_data_path(client),
        nv_data_size(client), nv_data_chunk_size(client));
    nv_data_md5_compute(nv_data_p, nv_data_size(client), nv_data_secret(client), nv_data_md5_hash);
    free(nv_data_p);

    /* Alloc the memory for the md5 hash string. */
    nv_data_md5_hash_string = malloc(MD5_STRING_SIZE);
    memset(nv_data_md5_hash_string, 0, MD5_STRING_SIZE);

    md5hash2string(nv_data_md5_hash_string, nv_data_md5_hash);

    ipc_client_log(client, "nv_data_md5_generate: new MD5 hash is %s\n", nv_data_md5_hash_string);

    ipc_client_log(client, "nv_data_md5_generate: writing MD5 hash\n");

    /* Write the MD5 hash in nv_data.bin.md5. */
    fd = open(nv_data_md5_path(client), O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        ipc_client_log(client, "nv_data_md5_generate: fd open failed\n");
        goto exit;
    }

    rc = write(fd, nv_data_md5_hash_string, MD5_STRING_SIZE);
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_md5_generate: failed to write MD5 hash to file\n");
        close(fd);
        goto exit;
    }

    close(fd);

exit:
    if (nv_data_md5_hash_string != NULL)
        free(nv_data_md5_hash_string);

    ipc_client_log(client, "nv_data_md5_generate: exit\n");
}

void nv_data_backup_create(struct ipc_client *client)
{
    uint8_t nv_data_md5_hash[MD5_DIGEST_LENGTH];
    char *nv_data_md5_hash_string = NULL;
    char *nv_data_md5_hash_read = NULL;
    int nv_data_write_tries = 0;

    struct stat nv_stat;
    void *nv_data_p = NULL;
    void *nv_data_bak_p = NULL;
    uint8_t data;

    int fd;
    int rc;
    int i;

    ipc_client_log(client, "nv_data_backup_create: enter\n");

    if (stat(nv_data_path(client), &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: nv_data.bin missing\n");
        nv_data_generate(client);
    }

    if (nv_stat.st_size != nv_data_size(client))
    {
        ipc_client_log(client, "nv_data_backup_create: wrong nv_data.bin size\n");
        nv_data_generate(client);
        return;
    }

    if (stat(nv_data_md5_path(client), &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: nv_data.bin.md5 missing\n");
        nv_data_generate(client);
        return;
    }

    /* Alloc the memory for the md5 hashes strings. */
    nv_data_md5_hash_string=malloc(MD5_STRING_SIZE);
    nv_data_md5_hash_read=malloc(MD5_STRING_SIZE);

    memset(nv_data_md5_hash_read, 0, MD5_STRING_SIZE);
    memset(nv_data_md5_hash_string, 0, MD5_STRING_SIZE);

    /* Read the content of the backup file. */
    nv_data_p=ipc_client_file_read(client, nv_data_path(client),
        nv_data_size(client), nv_data_chunk_size(client));

    /* Compute the backup file MD5 hash. */
    nv_data_md5_compute(nv_data_p, nv_data_size(client), nv_data_secret(client), nv_data_md5_hash);
    md5hash2string(nv_data_md5_hash_string, nv_data_md5_hash);

    /* Read the stored backup file MD5 hash. */
    fd = open(nv_data_md5_path(client), O_RDONLY);
    if (fd < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: failed to openstored backup file with MD5 hash\n");
        goto exit;
    }

    rc = read(fd, nv_data_md5_hash_read, MD5_STRING_SIZE);
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: failed to read MD5 hash from backup file\n");
        close(fd);
        goto exit;
    }

    close(fd);

    /* Add 0x0 to end the string: not sure this is always part of the file. */
    nv_data_md5_hash_read[MD5_STRING_SIZE - 1]='\0';

    ipc_client_log(client, "nv_data_backup_create: backup file computed MD5: %s read MD5: %s\n",
        nv_data_md5_hash_string, nv_data_md5_hash_read);

    if (strcmp(nv_data_md5_hash_string, nv_data_md5_hash_read) != 0)
    {
        ipc_client_log(client, "nv_data_backup_create: MD5 hash mismatch on backup file\n");
        ipc_client_log(client, "nv_data_backup_create: Consider the computed one as correct\n");

        fd = open(nv_data_md5_path(client), O_WRONLY);
        if (fd < 0)
        {
            ipc_client_log(client, "nv_data_backup_create: failed to open file with MD5 hash of data file\n");
            goto exit;
        }

        rc = read(fd, nv_data_md5_hash_string, MD5_STRING_SIZE);
        if (rc < 0)
        {
            ipc_client_log(client, "nv_data_backup_create: failed to read MD5 hash for data file from file\n");
            goto exit;
        }

        close(fd);

        /*
        nv_data_backup_generate(client);
        nv_data_backup_create(client);
        return;
        */
    }

    /* Assume the read string is the computated one */
    memcpy(nv_data_md5_hash_read, nv_data_md5_hash_string, MD5_STRING_SIZE);
    memset(nv_data_md5_hash_string, 0, MD5_STRING_SIZE);

nv_data_backup_create_write:
   while (nv_data_write_tries < 5)
    {
        ipc_client_log(client, "nv_data_backup_create: .nv_data.bak write try #%d\n", nv_data_write_tries + 1);

        fd = open(nv_data_bak_path(client), O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            ipc_client_log(client, "nv_data_backup_create: negative fd while opening /efs/.nv_data.bak, error: %s\n", strerror(errno));
            nv_data_write_tries++;
            continue;
        }

        rc = write(fd, nv_data_p, nv_data_size(client));
        if (rc < nv_data_size(client))
        {
            ipc_client_log(client, "nv_data_backup_create: wrote less (%d) than what we expected (%d) on /efs/.nv_data.bak, error: %s\n", strerror(errno));
            close(fd);
            nv_data_write_tries++;
            continue;
        }

        close(fd);
        break;
    }

    if (nv_data_write_tries == 5)
    {
        ipc_client_log(client, "nv_data_backup_create: writing nv_data.bin to .nv_data.bak failed too many times\n");
        unlink(nv_data_bak_path(client));
        goto exit;
    }

    /* Read the newly-written .nv_data.bak. */
    nv_data_bak_p = ipc_client_file_read(client, nv_data_bak_path(client), 
        nv_data_size(client), nv_data_chunk_size(client));

    /* Compute the MD5 hash for nv_data.bin. */
    nv_data_md5_compute(nv_data_bak_p, nv_data_size(client), nv_data_secret(client), nv_data_md5_hash);
    md5hash2string(nv_data_md5_hash_string, nv_data_md5_hash);

    if (nv_data_bak_p != NULL)
        free(nv_data_bak_p);

    ipc_client_log(client, "nv_data_backup_create: written file computed MD5: %s read MD5: %s\n",
        nv_data_md5_hash_string, nv_data_md5_hash_read);

    /* Make sure both hashes are the same. */
    if (strcmp(nv_data_md5_hash_string, nv_data_md5_hash_read) != 0)
    {
        ipc_client_log(client, "nv_data_backup_create: MD5 hash mismatch on written file\n");
        ipc_client_log(client, "nv_data_backup_create: Writing again\n");

        goto nv_data_backup_create_write;
    }

    /* Write the MD5 hash in .nv_data.bak.md5. */
    fd = open(nv_data_md5_bak_path(client), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: failed to open MD5 hash file\n");
        goto exit;
    }

    rc = write(fd, nv_data_md5_hash_read, MD5_STRING_SIZE);
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: failed to write MD5 hash to file\n");
        close(fd);
        goto exit;
    }
    close(fd);

    /* Write the correct .nv_state. */
    fd = open(nv_state_path(client), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: failed to open NV state file\n");
        goto exit;
    }

    data='1';
    rc = write(fd, &data, sizeof(data));
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_backup_create: failed to write state of NV data\n");
        close(fd);
        goto exit;
    }

    close(fd);

exit:
    if (nv_data_p != NULL)
        free(nv_data_p);
    if (nv_data_md5_hash_string != NULL)
        free(nv_data_md5_hash_string);
    if (nv_data_md5_hash_read)
        free(nv_data_md5_hash_read);

    ipc_client_log(client, "nv_data_backup_create: exit\n");
}

void nv_data_backup_restore(struct ipc_client *client)
{
    uint8_t nv_data_md5_hash[MD5_DIGEST_LENGTH];
    char *nv_data_md5_hash_string = NULL;
    char *nv_data_md5_hash_read = NULL;
    int nv_data_write_tries = 0;

    struct stat nv_stat;
    void *nv_data_p = NULL;
    void *nv_data_bak_p = NULL;
    uint8_t data;

    int fd;
    int rc;
    int i;

    ipc_client_log(client, "nv_data_backup_restore: enter\n");

    if (stat(nv_data_bak_path(client), &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: .nv_data.bak missing\n");
        nv_data_generate(client);
        nv_data_backup_create(client);
        return;
    }

    if (nv_stat.st_size != nv_data_size(client))
    {
        ipc_client_log(client, "nv_data_backup_restore: wrong .nv_data.bak size\n");
        nv_data_generate(client);
        nv_data_backup_create(client);
        return;
    }

    if (stat(nv_data_md5_bak_path(client), &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: .nv_data.bak.md5 missing\n");
        nv_data_generate(client);
        nv_data_backup_create(client);
        return;
    }

    /* Alloc the memory for the md5 hashes strings. */
    nv_data_md5_hash_string=malloc(MD5_STRING_SIZE);
    nv_data_md5_hash_read=malloc(MD5_STRING_SIZE);

    memset(nv_data_md5_hash_read, 0, MD5_STRING_SIZE);
    memset(nv_data_md5_hash_string, 0, MD5_STRING_SIZE);

    /* Read the content of the backup file. */
    nv_data_bak_p=ipc_client_file_read(client, nv_data_bak_path(client),
        nv_data_size(client), nv_data_chunk_size(client));

    /* Compute the backup file MD5 hash. */
    nv_data_md5_compute(nv_data_bak_p, nv_data_size(client), nv_data_secret(client), nv_data_md5_hash);
    md5hash2string(nv_data_md5_hash_string, nv_data_md5_hash);

    /* Read the stored backup file MD5 hash. */
    fd=open(nv_data_md5_bak_path(client), O_RDONLY);
    rc = read(fd, nv_data_md5_hash_read, MD5_STRING_SIZE);
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: Failed to read md5 hash for stored back file\n");
        close(fd);
        goto exit;
    }

    close(fd);

    /* Add 0x0 to end the string: not sure this is always part of the file. */
    nv_data_md5_hash_read[MD5_STRING_SIZE - 1]='\0';

    ipc_client_log(client, "nv_data_backup_restore: backup file computed MD5: %s read MD5: %s\n",
        nv_data_md5_hash_string, nv_data_md5_hash_read);

    if (strcmp(nv_data_md5_hash_string, nv_data_md5_hash_read) != 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: MD5 hash mismatch on backup file\n");
        ipc_client_log(client, "nv_data_backup_restore: Consider the computed one as correct\n");

        fd = open(nv_data_md5_bak_path(client), O_WRONLY);
        if (fd < 0)
        {
            ipc_client_log(client, "nv_data_backup_restore: failed to open MD5 hash backup file\n");
            goto exit;
        }

        rc = read(fd, nv_data_md5_hash_string, MD5_STRING_SIZE);
        if (rc < 0)
        {
            ipc_client_log(client, "nv_data_backup_restore: failed to read MD5 hash from backup file\n");
            close(fd);
            goto exit;
        }

        close(fd);

        /*
        nv_data_backup_generate(client);
        nv_data_backup_create(client);
        return;
        */
    }

    /* Assume the read string is the computated one */
    memcpy(nv_data_md5_hash_read, nv_data_md5_hash_string, MD5_STRING_SIZE);
    memset(nv_data_md5_hash_string, 0, MD5_STRING_SIZE);

nv_data_backup_restore_write:
   while (nv_data_write_tries < 5)
    {
        ipc_client_log(client, "nv_data_backup_restore: nv_data.bin write try #%d\n", nv_data_write_tries + 1);

        fd=open(nv_data_path(client), O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            ipc_client_log(client, "nv_data_backup_restore: negative fd while opening /efs/nv_data.bin, error: %s\n", strerror(errno));
            nv_data_write_tries++;
            continue;
        }

        rc = write(fd, nv_data_bak_p, nv_data_size(client));
        if (rc < nv_data_size(client))
        {
            ipc_client_log(client, "nv_data_backup_restore: wrote less (%d) than what we expected (%d) on /efs/nv_data.bin, error: %s\n", strerror(errno));
            close(fd);
            nv_data_write_tries++;
            continue;
        }

        close(fd);
        break;
    }

    if (nv_data_write_tries == 5)
    {
        ipc_client_log(client, "nv_data_backup_restore: writing the backup to nv_data.bin failed too many times\n");
        unlink(nv_data_path(client));
        goto exit;
    }

    /* Read the newly-written nv_data.bin. */
    nv_data_p=ipc_client_file_read(client, nv_data_path(client),
        nv_data_size(client), nv_data_chunk_size(client));

    /* Compute the MD5 hash for nv_data.bin. */
    nv_data_md5_compute(nv_data_p, nv_data_size(client), nv_data_secret(client), nv_data_md5_hash);
    md5hash2string(nv_data_md5_hash_string, nv_data_md5_hash);

    if (nv_data_p != NULL)
    {
        free(nv_data_p);
        nv_data_p = NULL;
    }

    ipc_client_log(client, "nv_data_backup_restore: written file computed MD5: %s read MD5: %s\n",
        nv_data_md5_hash_string, nv_data_md5_hash_read);

    /* Make sure both hashes are the same. */
    if (strcmp(nv_data_md5_hash_string, nv_data_md5_hash_read) != 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: MD5 hash mismatch on written file\n");
        ipc_client_log(client, "nv_data_backup_restore: Writing again\n");

        goto nv_data_backup_restore_write;
    }

    /* Write the MD5 hash in nv_data.bin.md5. */
    fd = open(nv_data_md5_path(client), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: failed to open file with MD5 hash\n");
        goto exit;
    }

    rc = write(fd, nv_data_md5_hash_read, MD5_STRING_SIZE);
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: failed to write MD5 hash to file\n");
        close(fd);
        goto exit;
    }
    close(fd);

    /* Write the correct .nv_state. */
    fd = open(nv_state_path(client), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        ipc_client_log(client, "nv_data_backup_restore: failed to open NV state file\n");
        goto exit;
    }

    data='1';
    rc = write(fd, &data, sizeof(data));
    if (rc <  0)
    {
        ipc_client_log(client, "nv_data_backup_restore: failed to write state to file\n");
        close(fd);
        goto exit;
    }

    close(fd);

exit:
    if (nv_data_bak_p != NULL)
        free(nv_data_bak_p);
    if (nv_data_md5_hash_string != NULL)
        free(nv_data_md5_hash_string);
    if (nv_data_md5_hash_read != NULL)
        free(nv_data_md5_hash_read);

    ipc_client_log(client, "nv_data_backup_restore: exit\n");
}

int nv_data_check(struct ipc_client *client)
{
    struct stat nv_stat;
    int nv_state_fd=-1;
    int nv_state=0;
    int rc;

    ipc_client_log(client, "nv_data_check: enter\n");

    if (stat(nv_data_path(client), &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_check: nv_data.bin missing\n");
        nv_data_backup_restore(client);
        stat(nv_data_path(client), &nv_stat);
    }

    if (nv_stat.st_size != nv_data_size(client))
    {
        ipc_client_log(client, "nv_data_check: wrong nv_data.bin size\n");
        nv_data_backup_restore(client);
    }

    if (stat(nv_data_md5_path(client), &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_check: nv_data.bin.md5 missing\n");
        nv_data_backup_restore(client);
    }

    if (stat(nv_data_bak_path(client), &nv_stat) < 0 || stat(nv_data_md5_bak_path(client), &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_check: .nv_data.bak or .nv_data.bak.md5 missing\n");
        nv_data_backup_create(client);
    }

    nv_state_fd=open(nv_state_path(client), O_RDONLY);

    if (nv_state_fd < 0 || fstat(nv_state_fd, &nv_stat) < 0)
    {
        ipc_client_log(client, "nv_data_check: .nv_state missing\n");
        nv_data_backup_restore(client);
    }

    rc = read(nv_state_fd, &nv_state, sizeof(nv_state));
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_check: couldn't read state of NV item from file\n");
        return -1;
    }

    close(nv_state_fd);

    if (nv_state != '1')
    {
        ipc_client_log(client, "nv_data_check: bad nv_state\n");
        nv_data_backup_restore(client);
    }

    ipc_client_log(client, "nv_data_check: everything should be alright\n");
    ipc_client_log(client, "nv_data_check: exit\n");

    return 0;
}

int nv_data_md5_check(struct ipc_client *client)
{
    struct stat nv_stat;
    uint8_t nv_data_md5_hash[MD5_DIGEST_LENGTH];
    char *nv_data_md5_hash_string = NULL;
    char *nv_data_md5_hash_read = NULL;
    void *nv_data_p = NULL;
    int fd;
    int rc;
    uint8_t *data_p;

    ipc_client_log(client, "nv_data_md5_check: enter\n");

    nv_data_md5_hash_string=malloc(MD5_STRING_SIZE);
    nv_data_md5_hash_read=malloc(MD5_STRING_SIZE);

    memset(nv_data_md5_hash_read, 0, MD5_STRING_SIZE);
    memset(nv_data_md5_hash_string, 0, MD5_STRING_SIZE);

    nv_data_p=ipc_client_file_read(client, nv_data_path(client),
        nv_data_size(client), nv_data_chunk_size(client));
    data_p=nv_data_p;

    nv_data_md5_compute(data_p, nv_data_size(client), nv_data_secret(client), nv_data_md5_hash);

    md5hash2string(nv_data_md5_hash_string, nv_data_md5_hash);

    free(nv_data_p);

    fd=open(nv_data_md5_path(client), O_RDONLY);

    /* Read the md5 stored in the file. */
    rc = read(fd, nv_data_md5_hash_read, MD5_STRING_SIZE);
    if (rc < 0)
    {
        ipc_client_log(client, "nv_data_md5_check: Can't read md5 hash from file\n");
        return -1;
    }

    /* Add 0x0 to end the string: not sure this is part of the file. */
    nv_data_md5_hash_read[MD5_STRING_SIZE - 1]='\0';

    ipc_client_log(client, "nv_data_md5_check: computed MD5: %s read MD5: %s\n", 
        nv_data_md5_hash_string, nv_data_md5_hash_read);

    if (strcmp(nv_data_md5_hash_string, nv_data_md5_hash_read) != 0)
    {
        ipc_client_log(client, "nv_data_md5_check: MD5 hash mismatch\n");
        nv_data_backup_restore(client);
    }

    if (nv_data_md5_hash_string != NULL)
        free(nv_data_md5_hash_string);
    if (nv_data_md5_hash_read != NULL)
        free(nv_data_md5_hash_read);

    ipc_client_log(client, "nv_data_md5_check: exit\n");

    return 0;
}

int nv_data_read(struct ipc_client *client, int offset, int length, char *buf)
{
    int fd;
    int rc;

    ipc_client_log(client, "nv_data_read: enter\n");

    if(offset < 0 || length <= 0) {
        ipc_client_log(client, "nv_data_read: offset < 0 or length <= 0\n");
        return -1;
    }

    if (buf == NULL) {
        ipc_client_log(client, "nv_data_read: provided output buf is NULL\n");
        return -1;
    }

    if (nv_data_check(client) < 0)
        return -1;

    fd = open(nv_data_path(client), O_RDONLY);

    if (fd < 0) {
        ipc_client_log(client, "nv_data_read: nv_data file fd is negative\n");
        return -1;
    }

    lseek(fd, offset, SEEK_SET);

    rc = read(fd, buf, length);

    if (rc < length) {
        ipc_client_log(client, "nv_data_read: read less than what we expected\n");
        return -1;
    }

    ipc_client_log(client, "nv_data_read: exit\n");

    return 0;
}

int nv_data_write(struct ipc_client *client, int offset, int length, char *buf)
{
    int fd;
    int rc;

    ipc_client_log(client, "nv_data_write: enter\n");

    if(offset < 0 || length <= 0) {
        ipc_client_log(client, "nv_data_write: offset or length <= 0\n");
        return -1;
    }

    if (buf == NULL) {
        ipc_client_log(client, "nv_data_write: provided input buf is NULL\n");
        return -1;
    }

    if (nv_data_check(client) < 0)
        return -1;

    fd = open(nv_data_path(client), O_WRONLY);

    if (fd < 0) {
        ipc_client_log(client, "nv_data_write: nv_data file fd is negative\n");
        return -1;
    }

    lseek(fd, offset, SEEK_SET);

    rc = write(fd, buf, length);

    close(fd);

    if (rc < length) {
        ipc_client_log(client, "nv_data_write: wrote less (%d) than what we expected (%d), error: %s, restoring backup\n", rc, length, strerror(errno));
        nv_data_backup_restore(client);
        return -1;
    }

    ipc_client_log(client, "nv_data_write: writing new md5sum\n");
    nv_data_md5_generate(client);

    ipc_client_log(client, "nv_data_write: exit\n");

    return 0;
}

void ipc_rfs_send_io_confirm_for_nv_read_item(struct ipc_client *client, struct ipc_message_info *info)
{
    struct ipc_rfs_io *rfs_io = (struct ipc_rfs_io *) info->data;
    struct ipc_rfs_io_confirm *rfs_io_conf;
    void *rfs_data;
    int rc;

    if (rfs_io == NULL)
    {
        ipc_client_log(client, "ERROR: Request message is invalid: aseq = %i", info->aseq);
        return;
    }

    rfs_io_conf = malloc(rfs_io->length + sizeof(struct ipc_rfs_io_confirm));
    memset(rfs_io_conf, 0, rfs_io->length + sizeof(struct ipc_rfs_io_confirm));
    rfs_data = rfs_io_conf + sizeof(struct ipc_rfs_io_confirm);

    ipc_client_log(client, "Asked to read 0x%x bytes at offset 0x%x", rfs_io->length, rfs_io->offset);
    rc = nv_data_read(client, rfs_io->offset, rfs_io->length, rfs_data);

#ifdef DEBUG
    ipc_client_log(client, "Read rfs_data dump:");
    ipc_client_hex_dump(client, rfs_data, rfs_io->length);
#endif

    ipc_client_log(client, "Preparing RFS IO Confirm message (rc is %d)", rc);
    rfs_io_conf->confirm = rc < 0 ? 0 : 1;
    rfs_io_conf->offset = rfs_io->offset;
    rfs_io_conf->length = rfs_io->length;

    ipc_client_send(client, IPC_RFS_NV_READ_ITEM, 0, (unsigned char*) rfs_io_conf,
                    rfs_io->length + sizeof(struct ipc_rfs_io_confirm), info->aseq);
    free(rfs_io_conf);
}

void ipc_rfs_send_io_confirm_for_nv_write_item(struct ipc_client *client, struct ipc_message_info *info)
{
    struct ipc_rfs_io *rfs_io = (struct ipc_rfs_io *) info->data;
    struct ipc_rfs_io_confirm *rfs_io_conf;
    void *rfs_data;
    int rc;

    if (rfs_io == NULL)
    {
        ipc_client_log(client, "ERROR: Request message is invalid: aseq = %i", info->aseq);
        return;
    }

    rfs_data = info->data + sizeof(struct ipc_rfs_io);

#ifdef DEBUG
    ipc_client_log(client, "Write rfs_data dump:");
    ipc_client_hex_dump(client, rfs_data, rfs_io->length);
#endif

    ipc_client_log(client, "Asked to write 0x%x bytes at offset 0x%x", rfs_io->length, rfs_io->offset);
    rc = nv_data_write(client, rfs_io->offset, rfs_io->length, rfs_data);

    ipc_client_log(client, "Sending RFS IO Confirm message (rc is %d)", rc);
    rfs_io_conf = (struct ipc_rfs_io_confirm*) malloc(sizeof(struct ipc_rfs_io_confirm));
    rfs_io_conf->confirm = rc < 0 ? 0 : 1;
    rfs_io_conf->offset = rfs_io->offset;
    rfs_io_conf->length = rfs_io->length;

    ipc_client_send(client, IPC_RFS_NV_WRITE_ITEM, 0, (unsigned char*) rfs_io_conf,
                    sizeof(struct ipc_rfs_io_confirm), info->aseq);
    free(rfs_io_conf);
}

// vim:ts=4:sw=4:expandtab
