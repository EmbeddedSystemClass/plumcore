/*
 * UXB bus device auto-discovery
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "u_assert.h"
#include "u_log.h"

#include "port.h"
#include "services/interfaces/servicelocator.h"

#include "module.h"
#include "uhal/interfaces/uxbbus.h"
#include "uhal/interfaces/uxbdevice.h"
#include "uhal/interfaces/uxbslot.h"

#include "puxb_discovery.h"

#ifdef MODULE_NAME
#undef MODULE_NAME
#endif
#define MODULE_NAME "puxb-discovery"


static void id_to_str(uint8_t id[8], char *s) {
	sprintf(s, "%02x%02x.%02x%02x.%02x%02x.%02x%02x", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
}


static PUxbDiscoveryDevice *get_device_by_id(PUxbDiscovery *self, uint8_t id[8]) {
	PUxbDiscoveryDevice *i = self->devices;
	while (i) {
		if (!memcmp(i->id, id, 8)) {
			return i;
		}
		i = i->next;
	}
	return NULL;
}


/* Read the device descriptor, up to PXUB_DISCOVERY_DESCRIPTOR_LENGTH lines. */
static void read_device_descriptor(PUxbDiscoveryDevice *self) {
	char key[16];
	char value[48];

	for (uint8_t i = 1; i < PXUB_DISCOVERY_DSECRIPTOR_LENGTH; i++) {
		if (iuxbdevice_read_descriptor(self->device, i, key, 16, value, 48) != IUXBDEVICE_RET_OK) {
			break;
		}
	}
	char *name = "";
	iuxbdevice_get_name(self->device, &name);
	u_log(system_log, LOG_TYPE_INFO, U_LOG_MODULE_PREFIX("%s: done reading device descriptors"), name);
}


static void add_new_device(PUxbDiscovery *self, uint8_t id[8]) {
	uint8_t local[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	PUxbDiscoveryDevice *device = malloc(sizeof(PUxbDiscoveryDevice));
	if (device != NULL) {
		memset(device, 0, sizeof(PUxbDiscoveryDevice));

		if (iuxbbus_add_device(self->bus, &device->device) == IUXBBUS_RET_OK) {
			iuxbdevice_set_address(device->device, local, id);
			memcpy(device->id, id, 8);
			device->parent = self;

			/* Append it to the list. */
			device->next = self->devices;
			self->devices = device;

			char s[25];
			id_to_str(id, s);
			u_log(system_log, LOG_TYPE_INFO, U_LOG_MODULE_PREFIX("new device discovered addr = %s"), s);

			read_device_descriptor(device);

			char *name = NULL;
			iuxbdevice_get_name(device->device, &name);
			if (name != NULL) {
				iservicelocator_add(locator, ISERVICELOCATOR_TYPE_UXBDEVICE, &device->device->interface, name);
			}
		}
	}
}


static void discover_new_devices(PUxbDiscovery *self) {
	for (size_t i = 0; i < 16; i++) {
		/* Select the next ID interval. */
		uint8_t id[8] = {0, 0, 0, 0, 0, 0, 0, i};

		bool present = false;
		iuxbbus_probe_device(self->bus, id, &present);
		PUxbDiscoveryDevice *device = get_device_by_id(self, id);

		/* If the device is present and it is not in the list yet, add it. */
		if (present && (device == NULL)) {
			add_new_device(self, id);
		}
	}
}


static void check_active_devices(PUxbDiscovery *self) {
	PUxbDiscoveryDevice *device = self->devices;
	while (device != NULL) {
		char *name = "";
		iuxbdevice_get_name(device->device, &name);

		bool present = false;

		/** @todo fix occasional device disconnections */
		vTaskDelay(1);
		iuxbbus_probe_device(self->bus, device->id, &present);
		if (present && !device->active) {
			u_log(system_log, LOG_TYPE_INFO, U_LOG_MODULE_PREFIX("%s: device active"), name);
			device->active = present;
		}
		if (!present && device->active) {
			u_log(system_log, LOG_TYPE_INFO, U_LOG_MODULE_PREFIX("%s: device disconnected"), name);
			device->active = present;
		}

		device = device->next;
	}
}


static void discovery_task(void *p) {
	PUxbDiscovery *self = (PUxbDiscovery *)p;

	while (self->discovery_can_run) {
		discover_new_devices(self);
		check_active_devices(self);

		vTaskDelay(1000);
	}

	vTaskDelete(NULL);
}



puxb_discovery_ret_t puxb_discovery_init(PUxbDiscovery *self, IUxbBus *bus) {

	memset(self, 0, sizeof(PUxbDiscovery));

	self->bus = bus;

	/* Delay the discovery a bit. */
	vTaskDelay(100);

	self->discovery_can_run = true;
	xTaskCreate(discovery_task, "puxb-discovery", configMINIMAL_STACK_SIZE + 256, (void *)self, 1, &(self->discovery_task));
	if (self->discovery_task == NULL) {
		u_log(system_log, LOG_TYPE_ERROR, U_LOG_MODULE_PREFIX("cannot create task"));
		return PUXB_DISCOVERY_RET_FAILED;
	}

	u_log(system_log, LOG_TYPE_INFO, U_LOG_MODULE_PREFIX("module initialized"));
	return PUXB_DISCOVERY_RET_OK;
}
