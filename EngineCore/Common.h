/*
 *     Copyright (C) 2013-2018 Sumandeep Banerjee
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
 * File:   
 * Author: sumandeep
 * Email:  sumandeep.banerjee@gmail.com
*/

// Common.h - common definitions / log function declarations
#pragma once

// used globally for error logging
int BeginDataLog ();
void EndDataLog ();
void LogData (const char *szFormat, ...);
#define error_log LogData

// if set, all data files are output as text, else as binary data
#define _TEXT_DATA

// infinity
#define INF 1.0E10

// infinitesimal
#define EPSILON	1E-6

// defines
#define MAX_STRLEN			256

#define TEMP_FOLDER			"temp"
#define LOG_FILE			"error.log"
