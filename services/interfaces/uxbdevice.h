
/*
 * UXB device interface
 *
 * Copyright (C) 2017, Marek Koza, qyx@krtko.org
 *
 * This file is part of uMesh node firmware (http://qyx.krtko.org/projects/umesh)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Do not edit this file directly, it is autogenerated. */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "interface.h"


#include "interfaces/uxbslot.h"
typedef enum {
	IUXBDEVICE_RET_OK,
	IUXBDEVICE_RET_FAILED,
} iuxbdevice_ret_t;

struct iuxbdevice_vmt {
	iuxbdevice_ret_t (*add_slot)(void *context, IUxbSlot ** slot);
	iuxbdevice_ret_t (*remove_slot)(void *context, IUxbSlot * slot);
	iuxbdevice_ret_t (*set_address)(void *context, uint8_t * local_address, uint8_t * remote_address);
	iuxbdevice_ret_t (*get_firmware_version)(void *context, char ** buf);
	iuxbdevice_ret_t (*read_descriptor)(void *context, uint8_t line, char * key, size_t key_size, char * value, size_t value_size);
	iuxbdevice_ret_t (*get_hardware_version)(void *context, char ** buf);
	iuxbdevice_ret_t (*get_name)(void *context, char ** buf);
	iuxbdevice_ret_t (*get_id)(void *context, uint8_t * id);
	void *context;
};

typedef struct {
	Interface interface;
	struct iuxbdevice_vmt vmt;
} IUxbDevice;


iuxbdevice_ret_t iuxbdevice_add_slot(IUxbDevice *self, IUxbSlot ** slot);
iuxbdevice_ret_t iuxbdevice_remove_slot(IUxbDevice *self, IUxbSlot * slot);
iuxbdevice_ret_t iuxbdevice_set_address(IUxbDevice *self, uint8_t * local_address, uint8_t * remote_address);
iuxbdevice_ret_t iuxbdevice_init(IUxbDevice *self);
iuxbdevice_ret_t iuxbdevice_get_firmware_version(IUxbDevice *self, char ** buf);
iuxbdevice_ret_t iuxbdevice_read_descriptor(IUxbDevice *self, uint8_t line, char * key, size_t key_size, char * value, size_t value_size);
iuxbdevice_ret_t iuxbdevice_get_hardware_version(IUxbDevice *self, char ** buf);
iuxbdevice_ret_t iuxbdevice_get_name(IUxbDevice *self, char ** buf);
iuxbdevice_ret_t iuxbdevice_get_id(IUxbDevice *self, uint8_t * id);
iuxbdevice_ret_t iuxbdevice_free(IUxbDevice *self);
