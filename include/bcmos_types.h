/*
<:copyright-BRCM:2016:DUAL/GPL:standard

   Broadcom Proprietary and Confidential.(c) 2016 Broadcom
   All Rights Reserved

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>
 */

#ifndef BCMOS_TYPES_H_
#define BCMOS_TYPES_H_

#ifndef BCMOS_SYSTEM_H_
#error Please do not include bcmos_types.h directly. Include bcmos_system.h
#endif

#include "bcmos_pack.h"

/** \defgroup system_types Generic types
 * @{
 */

/*
 * Limits of integer types.
 */

/* Minimum of signed integer types.  */
#ifndef INT8_MIN
#define INT8_MIN       (0x80)
#endif
#ifndef INT16_MIN
#define INT16_MIN      (0x8000)
#endif
#ifndef INT32_MIN
#define INT32_MIN      (0x80000000)
#endif
#ifndef INT64_MIN
#define INT64_MIN      (0x8000000000000000)
#endif

/* Maximum of signed integer types.  */
#ifndef INT8_MAX
#define INT8_MAX       (0x7F)
#endif
#ifndef INT16_MAX
#define INT16_MAX      (0x7FFF)
#endif
#ifndef INT32_MAX
#define INT32_MAX      (0x7FFFFFFF)
#endif
#ifndef INT64_MAX
#define INT64_MAX      (0x7FFFFFFFFFFFFFFF)
#endif

/* Maximum of unsigned integer types.  */
#ifndef UINT8_MAX
#define UINT8_MAX      (0xFF)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX     (0xFFFF)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX     (0xFFFFFFFF)
#endif
#ifndef UINT64_MAX
#define UINT64_MAX     (0xFFFFFFFFFFFFFFFF)
#endif


/** Endianness */
#define BCMOS_ENDIAN_BIG        0
#define BCMOS_ENDIAN_LITTLE     1

typedef enum
{
    BCMOS_ENDIAN_BIG_E          = BCMOS_ENDIAN_BIG,
    BCMOS_ENDIAN_LITTLE_E       = BCMOS_ENDIAN_LITTLE,
} bcmos_endian;

/* If endianness is not set explicitly, try to autodetect it.
 * Modern gcc versions (over 4.8) define __BYTE_ORDER__
 * see "gcc -E -dM - < /dev/null | grep ENDIAN"
 */
#ifndef BCM_CPU_ENDIAN

#ifdef __BYTE_ORDER__

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BCM_CPU_ENDIAN BCMOS_ENDIAN_LITTLE
#else
#define BCM_CPU_ENDIAN BCMOS_ENDIAN_BIG
#endif

#else

/* We are dealing with compiler that doesn't set __BYTE_ORDER__.
 * If it is simulation build than it must be LE x86.
 * Otherwise, no way to tell.
 */
#ifdef SIMULATION_BUILD
#define BCM_CPU_ENDIAN BCMOS_ENDIAN_LITTLE
#endif

#endif /* #ifdef __BYTE_ORDER */

#endif /* #ifndef BCM_CPU_ENDIAN */

#include <bcm_config.h>

/** 24-bit unsigned integer */
typedef union
{
    uint8_t u8[3];
    struct __PACKED_ATTR_START__
    {
#if (BCM_CPU_ENDIAN == BCMOS_ENDIAN_BIG)
        uint8_t hi;
        uint8_t mid;
        uint8_t low;
#elif (BCM_CPU_ENDIAN == BCMOS_ENDIAN_LITTLE)
        uint8_t low;
        uint8_t mid;
        uint8_t hi;
#else
#error BCM_CPU_ENDIAN must be BCMOS_ENDIAN_BIG or _LITTLE
#endif
    } __PACKED_ATTR_END__ low_hi;
} uint24_t;

static inline uint32_t uint24_to_32(uint24_t u24)
{
    return (u24.low_hi.hi << 16) | (u24.low_hi.mid << 8) | u24.low_hi.low;
}

static inline uint24_t uint32_to_24(uint32_t u32)
{
    uint24_t u24;
    u24.low_hi.hi = (u32 >> 16) & 0xff;
    u24.low_hi.mid = (u32 >> 8) & 0xff;
    u24.low_hi.low = u32 & 0xff;
    return u24;
}

/** VLAN tag (CoS/CFI/VID) */
typedef uint16_t bcmos_vlan_tag;

#define BCMOS_ETH_ALEN   6

/** MAC address */
typedef struct
{
    uint8_t u8[BCMOS_ETH_ALEN];
} bcmos_mac_address;

/** IPv4 address. It is stored in network byte order. */
typedef union
{
    uint32_t u32;
    uint8_t u8[4];
} bcmos_ipv4_address;

/** IPv6 address */
typedef struct
{
    uint8_t u8[16];
} bcmos_ipv6_address;

typedef enum
{
    BCMOS_IP_VERSION_UNKNOWN    = 0,
    BCMOS_IP_VERSION_4          = 4,
    BCMOS_IP_VERSION_6          = 6
} bcmos_ip_version;

typedef struct
{
    bcmos_ip_version version;
    union
    {
        bcmos_ipv4_address ipv4;
        bcmos_ipv6_address ipv6;
    };
} bcmos_ip_address;

static inline void bcmos_mac_address_init(bcmos_mac_address *mac)
{
    memset(mac, 0, sizeof(*mac));
}

static inline void bcmos_ipv4_address_init(bcmos_ipv4_address *ip)
{
    memset(ip, 0, sizeof(*ip));
}

/* Convert IPv4/IPv6 address from internal presentation to string */
bcmos_errno bcmos_ip_to_str(const bcmos_ip_address *ip, char *ip_str, int32_t ip_str_size);

/* Convert IPv4/IPv6 address from string to internal presentation */
bcmos_errno bcmos_str_to_ip(const char *ip_str, bcmos_ip_address *ip);

#ifndef CFE_BUILD
static inline char *bcmos_inet_ntoa(bcmos_ipv4_address *ip, char *ip_str)
{
    snprintf(ip_str, 16, "%u.%u.%u.%u", ip->u8[0], ip->u8[1], ip->u8[2], ip->u8[3]);

    return ip_str;
}
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({            \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (long)__mptr - offsetof(type,member) );})
#endif

/** @} */

#endif /* BCMOS_TYPES_H_ */
