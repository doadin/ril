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

#ifndef __CRESPO_IPC_H__
#define __CRESPO_IPC_H__

#define BOOTCORE_VERSION        0xf0
#define PSI_MAGIC               0x30
#define PSI_DATA_LEN            0x5000
#define RADIO_IMG_SIZE          0xd80000

#define MAX_MODEM_DATA_SIZE     0x50000

#define GPRS_IFACE_PREFIX       "rmnet"
#define GPRS_IFACE_COUNT        3

extern struct ipc_handlers crespo_ipc_default_handlers;

#endif

// vim:ts=4:sw=4:expandtab
