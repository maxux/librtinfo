/*
 * cpu temerature support for librtinfo
 * Copyright (C) 2012  DANIEL Maxime <root@maxux.net>
 *
 * source from: http://www.ericcarlson.org/mrtg/temp.c
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
/*
 * From original file:
 */
/*
 *  The IOKitGetPropertyAtGivenPathCopy function is taken from an e-mail message
 *  to the darwin-development mailing list from Chad Jones (chadj@mail.apple.com)
 *  on September 5, 2001. The message is available here:
 *  http://lists.apple.com/archives/darwin-development/2001/Sep/05/ioregistryharddiskhardwa.001.txt
 *  (user: archives, password: archives)
 *
 *  Information on the Dallas Semiconductor 1775 part was obtained from the
 *  publicly available specifications sheet available on the web. All knowledge
 *  of the workings of this system is based on speculation on my part, not on
 *  any information from Apple. To my knowledge the AppleCPUThermo and AppleFan
 *  kernel extensions are closed-source Apple proprietary, so I have no knowledge
 *  of their inner workings, only what I could gleam from information available
 *  in the IO Registry.
 *
 *  Author: Eric Carlson, carl0240@tc.umn.edu
 *
 *  This software is supplied 'as is', with no warranty.
 *  You are free to use and/or redistribute this source, modified or unmodified,
 *  in any form.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include "rtinfo.h"

/*
 *  Note: only AppleCPUThermo PowerPC supported yet
 */

static CFDataRef IOKitGetPropertyAtGivenPathCopy(char* PathToLookUp, char* PropertyToLookup) {
	mach_port_t MasterPort;
	kern_return_t status;
	io_registry_entry_t IOKitEntry;
	CFStringRef PropertyToLookupCFString;
	CFMutableDictionaryRef DictionaryOfIOKitEntries;
	CFDataRef DataToReturn;

	if(IOMasterPort(MACH_PORT_NULL,&MasterPort) != KERN_SUCCESS)
		return NULL;

	if((IOKitEntry = IORegistryEntryFromPath(MasterPort,PathToLookUp)) == MACH_PORT_NULL)
		return NULL;

	status = IORegistryEntryCreateCFProperties(IOKitEntry, &DictionaryOfIOKitEntries, NULL, kNilOptions);
	IOObjectRelease(IOKitEntry);

	if(status != KERN_SUCCESS)
		return NULL;

	/* Now need to convert the character string into a CFString for use in dictionary lookup. */
	if(!(PropertyToLookupCFString = CFStringCreateWithCString(NULL, PropertyToLookup, kCFStringEncodingASCII)))
		return NULL;

	/* Now pull the property we are interested in out of the Dictioary. */
	DataToReturn = CFDictionaryGetValue(DictionaryOfIOKitEntries,PropertyToLookupCFString);

	CFRetain(DataToReturn); /* need the value for later so retain now */
	CFRelease(DictionaryOfIOKitEntries);
	CFRelease(PropertyToLookupCFString);

	return DataToReturn;
}

rtinfo_temp_cpu_t * rtinfo_get_temp_cpu(rtinfo_temp_cpu_t *temp) {
	/* the path to the node with the temperature info as provided by the AppleCPUThermo kernel extension */
	char *ioRegPathTemp    = "IOService:/MacRISC2PE/uni-n/AppleUniN/i2c/PPCI2CInterface/temp-monitor/AppleCPUThermo";
	char *propertyNameTemp = "temperature";
	CFNumberRef tempCFNum;
	int tempNum;

	temp->critical    = 0;
	temp->cpu_average = 0;

	if(!(tempCFNum = (CFNumberRef) IOKitGetPropertyAtGivenPathCopy(ioRegPathTemp, propertyNameTemp)))
		return temp;

	/* get the raw temperature number */
	if(!CFNumberGetValue(tempCFNum, kCFNumberIntType, (void*) &tempNum))
		return temp;

	/* The value stored in the IO registry appears to be the raw value returned from the temperature sensor,
	 * a Dallas Semiconductor 1775 (ds1775) part. According to spec, the data is returned in two's complement form,
	 * with apparently 12 of the 16 bit positions used (the default is 12 for the chip, and this seems to be what it is set to).
	 * So, to convert to degrees C, divide by 2^4 and multiply by 0.0625 (the resolution)
	 */

	temp->cpu_average = (uint16_t)((tempNum / 16.0) * 0.0625);

	return temp;
}
