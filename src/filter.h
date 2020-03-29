/*
 * 
 *  _   _   _        __   _   _   _                                         
 * | | (_) | |__    / _| (_) | | | |_    ___   _ __           _ __     __ _ 
 * | | | | | '_ \  | |_  | | | | | __|  / _ \ | '__|  _____  | '_ \   / _` |
 * | | | | | |_) | |  _| | | | | | |_  |  __/ | |    |_____| | | | | | (_| |
 * |_| |_| |_.__/  |_|   |_| |_|  \__|  \___| |_|            |_| |_|  \__, |
 *                                                                    |___/ 
 *
 * A self contained, header only library providing a set of filters 
 * written in C++ with efficiency in mind
 *
 * Version: 1.0.0
 * URL: https://github.com/ekondayan/libfilter-ng.git
 *
 * Copyright (c) 2019,2020 Emil Kondayan
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FILTER_H
#define FILTER_H

#include "movingaverage.h"
#include "movingaverageexp.h"
#include "movingaveragekaufman.h"
#include "movingaverageweighted.h"
#include "movingmedian.h"
#include "movingmostfrequentoccurance.h"
#include "movingmiddle.h"
#include "lowpass.h"
#include "hipass.h"
#include "interpolation.h"
#include "intervalaverage.h"
#include "intervalmedian.h"

#endif // FILTER_H
