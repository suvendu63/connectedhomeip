/*
 *
 *    Copyright (c) 2020-2022 Project CHIP Authors
 *    Copyright 2024 NXP
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <platform/nxp/common/factory_data/legacy/FactoryDataProvider.h>

#define FACTORY_DATA_MAX_SIZE 4096

#define EL2GO_MAX_BLOB_SIZE 3072U
#define EL2GO_MAX_CERT_SIZE 2048U
#define PUBLIC_KEY_SIZE 64U

namespace chip {
namespace DeviceLayer {

/**
 * @brief This class provides Commissionable data and Device Attestation Credentials.
 *
 * This implementation allows to use the ELS hardware module to load the Matter factory
 * dataset in RAM at the boot.
 *
 *
 */

class FactoryDataProviderImpl : public FactoryDataProvider
{
public:
    static FactoryDataProviderImpl sInstance;

    ~FactoryDataProviderImpl(){};

    CHIP_ERROR Init(void) override;
    CHIP_ERROR SignWithDacKey(const ByteSpan & digestToSign, MutableByteSpan & outSignBuffer) override;
    CHIP_ERROR SearchForId(uint8_t searchedType, uint8_t * pBuf, size_t bufLength, uint16_t & length,
                           uint32_t * contentAddr = NULL);

    CHIP_ERROR GetDeviceAttestationCert(MutableByteSpan & outBuffer) override;

private:
    struct Header
    {
        uint32_t hashId;
        uint32_t size;
        uint8_t hash[4];
    };
    uint8_t factoryDataRamBuffer[FACTORY_DATA_MAX_SIZE];
    Header mHeader;

    CHIP_ERROR ReadAndCheckFactoryDataInFlash(void);
};

FactoryDataProvider & FactoryDataPrvdImpl();

} // namespace DeviceLayer
} // namespace chip
