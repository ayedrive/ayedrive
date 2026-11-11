// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
#include <aidl/com/example/dpc/BnDpcProtocol.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "device_protocol.h"

namespace aidl::com::example::dpc {

class DpcProtocolService final : public BnDpcProtocol {
public:
	ndk::ScopedAStatus exchange(
		const std::vector<std::uint8_t>& request,
		std::vector<std::uint8_t>* response) override
	{
		std::array<std::uint8_t, DPC_HEADER_SIZE + DPC_MAX_PAYLOAD_SIZE> buffer{};
		dpc_size_t response_length = 0;
		const auto status = dpc_process_message(
			request.data(), request.size(), buffer.data(), buffer.size(), &response_length);

		if (response == nullptr || status != DPC_CORE_SUCCESS) {
			return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
		}
		response->assign(buffer.begin(), buffer.begin() + response_length);
		return ndk::ScopedAStatus::ok();
	}
};

} // namespace aidl::com::example::dpc

int main()
{
	constexpr const char* service_name = "com.example.dpc.IDpcProtocol/default";
	ABinderProcess_setThreadPoolMaxThreadCount(1);
	auto service = ndk::SharedRefBase::make<aidl::com::example::dpc::DpcProtocolService>();
	const auto binder = service->asBinder();
	if (AServiceManager_addService(binder.get(), service_name) != STATUS_OK) {
		return 1;
	}
	ABinderProcess_joinThreadPool();
	return 1;
}
