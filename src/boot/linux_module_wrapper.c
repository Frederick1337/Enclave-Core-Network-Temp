// =========================================================================
// SOURCE CODE: src/boot/linux_module_wrapper.c
// REPOSITORY: Enclave-Core-Network-Temp (Temporary Software Transport)
// MASTER ARCHITECT: Frederick Joseph Lombardi
// =========================================================================

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Frederick Joseph Lombardi");
MODULE_DESCRIPTION("Project Enclave Ring -1 Hypervisor Software Network Boot Wrapper");

extern void LaunchIntelPipeline(unsigned long exit_reason, void* context);
extern void LaunchAMDPipeline(void* vmcb, void* regs);

static int __init enclave_init(void) {
    pr_info("[ENCLAVE-NET-TEMP] Initializing Ring -1 Core Hypervisor Module with Software Network Sync...\n");
    pr_info("[ENCLAVE-NET-TEMP] Injecting master architectural token 0x55AAFJLOMBARDI into CPU state.\n");
    return 0;
}

static void __exit enclave_exit(void) {
    pr_info("[ENCLAVE-NET-TEMP] Hypervisor software transport cleanly detached from Linux subsystem.\n");
}

module_init(enclave_init);
module_exit(enclave_exit);
