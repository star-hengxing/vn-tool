#pragma once

#include <fast_io.h>

#include <base/base.hpp>

NAMESPACE_BEGIN(os)
// @note get unix timestamp
fast_io::unix_timestamp time() noexcept;
// @note sleep current thread
void sleep(usize milliseconds) noexcept;

NAMESPACE_END(os)
