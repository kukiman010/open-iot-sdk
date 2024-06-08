/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SAMPLE_AZURE_IOT_CREDENTIALS_H
#define SAMPLE_AZURE_IOT_CREDENTIALS_H

/* This sample uses Symmetric key (SAS) to connect to IoT Hub */

/* These values can be picked from device connection string which is of format :
   HostName=<host1>;DeviceId=<device1>;SharedAccessKey=<key1> HOST_NAME can be set to <host1>, DEVICE_ID can be set to
   <device1>, DEVICE_SYMMETRIC_KEY can be set to <key1>.  */
#ifndef HOST_NAME
#define HOST_NAME ""
#endif /* HOST_NAME */

#ifndef DEVICE_ID
#define DEVICE_ID ""
#endif /* DEVICE_ID */

/* Optional SYMMETRIC KEY.  */
#ifndef DEVICE_SYMMETRIC_KEY
#define DEVICE_SYMMETRIC_KEY ""
#endif /* DEVICE_SYMMETRIC_KEY */

/* Optional module ID.  */
#ifndef MODULE_ID
#define MODULE_ID ""
#endif /* MODULE_ID */

#endif /* SAMPLE_AZURE_IOT_CREDENTIALS_H */
