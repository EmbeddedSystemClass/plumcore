/*
 * uMeshFw Command Line Interface service
 *
 * Copyright (C) 2015, Marek Koza, qyx@krtko.org
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
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "u_assert.h"
#include "u_log.h"
#include "hal_module.h"
#include "port.h"
#include "config.h"

#include "interface_stream.h"
#include "cli.h"

#include "system_cli_tree.h"

/* For output logging. */
#include "port.h"
#include "sffs.h"


int32_t module_cli_output(const char *s, void *ctx) {
	ServiceCli *cli = (ServiceCli *)ctx;

	interface_stream_write(cli->stream, (const uint8_t *)s, strlen(s));
	if (cli->log_file_opened) {
		sffs_write(&cli->log_file, s, strlen(s));
	}
	return 0;
}


static void cli_task(void *p) {
	ServiceCli *cli = (ServiceCli *)p;

	while (1) {
		uint8_t buf[10];
		int32_t ret = interface_stream_read(cli->stream, buf, sizeof(buf));
		if (ret <= 0) {
			vTaskDelay(100);
		} else {
			for (int32_t i = 0; i < ret; i++) {
				treecli_shell_keypress(&(cli->sh), buf[i]);
			}
		}
	}

}


service_cli_ret_t service_cli_init(ServiceCli *self, struct interface_stream *stream, const struct treecli_node *root) {
	if (u_assert(self != NULL) ||
	    u_assert(stream != NULL) ||
	    u_assert(root != NULL)) {
		return SERVICE_CLI_RET_FAILED;
	}

	memset(self, 0, sizeof(ServiceCli));
	self->stream = stream;

	treecli_shell_init(&self->sh, root);
	treecli_shell_set_print_handler(&self->sh, module_cli_output, (void *)self);
	treecli_shell_set_parser_context(&self->sh, (void *)self);

	return SERVICE_CLI_RET_OK;
}


service_cli_ret_t service_cli_start(ServiceCli *self) {
	if (u_assert(self != NULL)) {
		return SERVICE_CLI_RET_FAILED;
	}

	xTaskCreate(cli_task, "service-cli", configMINIMAL_STACK_SIZE + 768, (void *)self, 1, NULL);

	return SERVICE_CLI_RET_OK;
}


service_cli_ret_t service_cli_stop(ServiceCli *self) {
	/** @todo */

	return SERVICE_CLI_RET_OK;
}


service_cli_ret_t service_cli_free(ServiceCli *self) {
	/** @todo */

	return SERVICE_CLI_RET_OK;
}


service_cli_ret_t service_cli_start_out_logging(ServiceCli *self, const char *filename) {
	if (u_assert(self != NULL) ||
	    u_assert(filename != NULL)) {
		return SERVICE_CLI_RET_FAILED;
	}

	if (sffs_open(&fs, &self->log_file, filename, SFFS_OVERWRITE) != SFFS_OPEN_OK) {
		return SERVICE_CLI_RET_FAILED;
	}
	self->log_file_opened = true;

	return SERVICE_CLI_RET_OK;
}


service_cli_ret_t service_cli_stop_out_logging(ServiceCli *self) {
	if (u_assert(self != NULL)) {
		return SERVICE_CLI_RET_FAILED;
	}

	self->log_file_opened = false;
	sffs_close(&self->log_file);

	return SERVICE_CLI_RET_OK;
}


service_cli_ret_t service_cli_load_file(ServiceCli *self, const char *filename) {
	if (u_assert(self != NULL)) {
		return SERVICE_CLI_RET_FAILED;
	}

	struct sffs_file f;
	if (sffs_open(&fs, &f, filename, SFFS_READ) != SFFS_OPEN_OK) {
		return 1;
	}

	while (1) {
		uint8_t buf[256];
		size_t read = sffs_read(&f, buf, sizeof(buf));
		if (read == 0) {
			break;
		}
		for (size_t i = 0; i < read; i++) {
			treecli_shell_keypress(&self->sh, buf[i]);
		}
	}
	sffs_close(&f);
	treecli_shell_keypress(&self->sh, '/');
	treecli_shell_keypress(&self->sh, '\n');


	return SERVICE_CLI_RET_OK;
}
