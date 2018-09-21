/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Bundle Parts Functions
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


#include <_decoder.h>

BundleParts bundle_parts(const BundlePtr bundle)
{
    BundleParts parts;
    Bundle swappedBundle;

    swapBundle(swappedBundle, *bundle);
    parts.slot[0] = SLOT0(swappedBundle);
    parts.slot[1] = SLOT1(swappedBundle);
    parts.slot[2] = SLOT2(swappedBundle);
    parts.templSB = TEMPL_SB(swappedBundle);
    return parts;
}

Bundle bundle_from_parts(const BundlePartsPtr parts)
{
    Bundle bundle;

    bundle.right =
	BUNDLE_RIGHT(parts->templSB, parts->slot[0], parts->slot[1]);
    bundle.left = BUNDLE_LEFT(parts->slot[1], parts->slot[2]);
    swapBundle(bundle, bundle);
    return bundle;
}
