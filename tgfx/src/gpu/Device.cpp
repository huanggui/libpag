/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making libpag available.
//
//  Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
//  except in compliance with the License. You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "Device.h"
#include "Context.h"
#include "base/utils/UniqueID.h"

namespace pag {
Device::Device(std::unique_ptr<Context> ctx) : context(ctx.release()), _uniqueID(UniqueID::Next()) {
  context->device = this;
}

Device::~Device() {
  delete context;
}

Context* Device::lockContext() {
  locker.lock();
  contextLocked = onLockContext();
  if (!contextLocked) {
    locker.unlock();
    return nullptr;
  }
  context->onLocked();
  return context;
}

void Device::unlock() {
  if (contextLocked) {
    context->onUnlocked();
    contextLocked = false;
    onUnlockContext();
  }
  locker.unlock();
}

void Device::releaseAll() {
  std::lock_guard<std::mutex> autoLock(locker);
  contextLocked = onLockContext();
  context->releaseAll(contextLocked);
  if (contextLocked) {
    contextLocked = false;
    onUnlockContext();
  }
}

bool Device::onLockContext() {
  return true;
}

void Device::onUnlockContext() {
}
}  // namespace pag