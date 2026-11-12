// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
#include <linux/compat.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "dpc_linux_uapi.h"
#include "../../Shared/core/device_protocol.c"

static long dpc_reference_ioctl(struct file *file, unsigned int command, unsigned long argument)
{
	dpc_linux_exchange *exchange;
	dpc_size_t response_length = 0;
	dpc_core_status core_status;
	long result;

	(void)file;
	if (command != DPC_LINUX_IOCTL_EXCHANGE) {
		return -ENOTTY;
	}

	exchange = kzalloc(sizeof(*exchange), GFP_KERNEL);
	if (exchange == NULL) {
		return -ENOMEM;
	}
	if (copy_from_user(exchange, (void __user *)argument, sizeof(*exchange)) != 0) {
		result = -EFAULT;
		goto done;
	}
	if (exchange->input_length > DPC_LINUX_FRAME_CAPACITY ||
		exchange->output_capacity > DPC_LINUX_FRAME_CAPACITY) {
		result = -EINVAL;
		goto done;
	}

	core_status = dpc_process_message(
		exchange->input,
		exchange->input_length,
		exchange->output,
		exchange->output_capacity,
		&response_length);
	if (core_status == DPC_CORE_BUFFER_TOO_SMALL) {
		result = -EMSGSIZE;
		goto done;
	}
	if (core_status == DPC_CORE_UNSUPPORTED_VERSION) {
		result = -EPROTONOSUPPORT;
		goto done;
	}
	if (core_status != DPC_CORE_SUCCESS) {
		result = -EINVAL;
		goto done;
	}

	exchange->output_length = (dpc_u32)response_length;
	if (copy_to_user((void __user *)argument, exchange, sizeof(*exchange)) != 0) {
		result = -EFAULT;
		goto done;
	}
	result = 0;

done:
	kfree(exchange);
	return result;
}

#ifdef CONFIG_COMPAT
static long dpc_reference_compat_ioctl(struct file *file, unsigned int command, unsigned long argument)
{
	return dpc_reference_ioctl(file, command, (unsigned long)compat_ptr(argument));
}
#endif

static const struct file_operations dpc_reference_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = dpc_reference_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = dpc_reference_compat_ioctl,
#endif
};

static struct miscdevice dpc_reference_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "dpc-reference",
	.fops = &dpc_reference_fops,
	.mode = 0600,
};

static int __init dpc_reference_init(void)
{
	return misc_register(&dpc_reference_device);
}

static void __exit dpc_reference_exit(void)
{
	misc_deregister(&dpc_reference_device);
}

module_init(dpc_reference_init);
module_exit(dpc_reference_exit);
MODULE_DESCRIPTION("GPL test-only reference protocol device");
MODULE_LICENSE("GPL");
