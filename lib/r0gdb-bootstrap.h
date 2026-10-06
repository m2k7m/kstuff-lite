#ifndef R0GDB_BOOTSTRAP_H
#define R0GDB_BOOTSTRAP_H

#include <stdint.h>

#define R0GDB_BOOTSTRAP_MAGIC UINT64_C(0x5230474442504950)
#define KSTUFF_DYNLIB_RESOLVER_MAGIC UINT64_C(0x4b53544650524731)

/*
 * Loader -> payload boot configuration.
 *
 * The loader is the only side that can read /data, so it resolves the runtime
 * switch files and passes the result to the ring-0 payload as one extra
 * argument to elf_main(). The payload stores it in kstuff_boot_config before
 * main() runs; see lib/crt-elf-c.c.
 *
 * Low 32 bits are independent on/off flags, high 32 bits are the ShellCore
 * patch-group skip mask. A boot_config of 0 means "everything enabled", which
 * is exactly the behaviour of a build with no switch files present.
 */
#define KSTUFF_BOOT_CFG_NO_FPKG_HOOK (UINT64_C(1) << 0)
#define KSTUFF_BOOT_CFG_NO_SHELLCORE_PATCHES (UINT64_C(1) << 1)

#define KSTUFF_BOOT_CFG_PATCH_MASK_SHIFT 32

static inline uint64_t kstuff_boot_cfg_flags(uint64_t boot_config)
{
    return boot_config & UINT64_C(0xffffffff);
}

static inline uint32_t kstuff_boot_cfg_patch_mask(uint64_t boot_config)
{
    return (uint32_t)(boot_config >> KSTUFF_BOOT_CFG_PATCH_MASK_SHIFT);
}

/*
 * ShellCore patch groups.
 *
 * struct shellcore_patch::group is a bitmask of these. A patch with group 0 is
 * unconditional and is never skipped, which is what every pre-existing
 * firmware table relies on: they omit the field entirely and C zero-fills it.
 * Only tables that have been annotated (currently 13_42) can be masked, so any
 * firmware without verified group tags behaves exactly as before.
 */
#define KSTUFF_PATCH_GROUP_NONE 0x0000
#define KSTUFF_PATCH_GROUP_SYSVER 0x0001 /* fw version-check cluster            */
#define KSTUFF_PATCH_GROUP_UNK1 0x0002 /* three unconditional "eb 04"         */
#define KSTUFF_PATCH_GROUP_UNK2 0x0004 /* four misc 1-4 byte patches          */
#define KSTUFF_PATCH_GROUP_PS4MINI 0x0008 /* SELF lazy load / ps4_nongame_mini */
#define KSTUFF_PATCH_GROUP_RIF 0x0010 /* sceRifManagerRegisterActivation...  */
#define KSTUFF_PATCH_GROUP_VR 0x0020 /* VR + VR2 update bypass              */
#define KSTUFF_PATCH_GROUP_SYSVPATH 0x0040 /* getSceSysDirPath debug path       */
#define KSTUFF_PATCH_GROUP_TROPHY 0x0080 /* trophy unlock fix                 */
#define KSTUFF_PATCH_GROUP_ERRMSG 0x0100 /* suppress game error message       */
#define KSTUFF_PATCH_GROUP_CATEGORY 0x0200 /* preLaunch/category checks         */
#define KSTUFF_PATCH_GROUP_DISCINST 0x0400 /* PS4/PS5 disc installer bypass     */
#define KSTUFF_PATCH_GROUP_PKGINST 0x0800 /* PS4/PS5 pkg installer bypass      */

#define KSTUFF_PATCH_GROUP_ALL 0x0fff

struct r0gdb_bootstrap {
    uint64_t magic;
    int rwpipe[2];
    uint64_t kpipe_addr;
};

#endif // R0GDB_BOOTSTRAP_H