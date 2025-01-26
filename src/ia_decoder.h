/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Simulator IA Instruction Decoder Header
 *
 * Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef _SKI_IA_DECODER_H
#define _SKI_IA_DECODER_H

#include "ia_types.h"

EaInfoPtr ea16Info(ModRM modrm);
EaInfoPtr ea32Info(ModRM modrm);
BOOL ia_decode(ADDR4 eip, IAinstInfoPtr info);
BOOL iadas_decode(ADDR4 eip, IAinstInfoPtr info);

#endif	/* _SKI_IA_DECODER_H */
