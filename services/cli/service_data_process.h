/*
 * Copyright (c) 2017, Marek Koza (qyx@krtko.org)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cli_table_helper.h"


#define DNODE_INDEX(p, i) p->pos.levels[p->pos.depth + i].dnode_index


/**
 * Tables for printing.
 */
extern const struct cli_table_cell service_data_process_node_table[];
extern const struct treecli_node *service_data_process_sensor_source_N;


/**
 * Informational and printing commands.
 */

int32_t service_data_process_print(struct treecli_parser *parser, void *exec_context);
int32_t service_data_process_sensor_source_print(struct treecli_parser *parser, void *exec_context);


/**
 * Configuration exporting commands.
 */

int32_t service_data_process_sensor_source_export(struct treecli_parser *parser, void *exec_context);
int32_t service_data_process_sensor_source_N_export(struct treecli_parser *parser, void *exec_context);


/**
 * Dynamic node creation functions.
 */

int32_t service_data_process_sensor_source_create(struct treecli_parser *parser, uint32_t index, struct treecli_node *node, void *ctx);
int32_t service_data_process_statistics_node_create(struct treecli_parser *parser, uint32_t index, struct treecli_node *node, void *ctx);
int32_t service_data_process_log_sink_create(struct treecli_parser *parser, uint32_t index, struct treecli_node *node, void *ctx);


/**
 * Commands for adding and removing items.
 */

int32_t service_data_process_sensor_source_add(struct treecli_parser *parser, void *exec_context);
int32_t service_data_process_log_sink_add(struct treecli_parser *parser, void *exec_context);


/**
 * Value manipulation functions.
 */

int32_t service_data_process_sensor_source_N_name_set(struct treecli_parser *parser, void *ctx, struct treecli_value *value, void *buf, size_t len);
int32_t service_data_process_sensor_source_N_enabled_set(struct treecli_parser *parser, void *ctx, struct treecli_value *value, void *buf, size_t len);
int32_t service_data_process_log_sink_N_name_set(struct treecli_parser *parser, void *ctx, struct treecli_value *value, void *buf, size_t len);
int32_t service_data_process_log_sink_N_enabled_set(struct treecli_parser *parser, void *ctx, struct treecli_value *value, void *buf, size_t len);
int32_t service_data_process_log_sink_N_input_set(struct treecli_parser *parser, void *ctx, struct treecli_value *value, void *buf, size_t len);
int32_t service_data_process_sensor_source_N_sensor_set(struct treecli_parser *parser, void *ctx, struct treecli_value *value, void *buf, size_t len);
int32_t service_data_process_sensor_source_N_interval_set(struct treecli_parser *parser, void *ctx, struct treecli_value *value, void *buf, size_t len);



